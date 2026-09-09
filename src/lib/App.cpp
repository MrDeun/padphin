#include "../include/App.hpp"
#include "fmt/core.h"
#include "imgui.h"
#include <algorithm>
#include <cctype>
#include <chrono>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <string>

namespace {

std::string display_name(const fs::directory_entry &e, bool &out_is_dir) {
  std::error_code ec;
  out_is_dir = e.is_directory(ec);
  std::string name = e.path().filename().string();
  if (name.empty())
    name = e.path().string();
  if (out_is_dir)
    name += "/";
  return name;
}

bool name_less(const fs::directory_entry &a, const fs::directory_entry &b) {
  std::error_code ec;
  bool a_dir = a.is_directory(ec);
  bool b_dir = b.is_directory(ec);
  if (a_dir != b_dir)
    return a_dir > b_dir; // directories first, like ranger
  std::string an = a.path().filename().string();
  std::string bn = b.path().filename().string();
  std::transform(an.begin(), an.end(), an.begin(),
                 [](unsigned char c) { return std::tolower(c); });
  std::transform(bn.begin(), bn.end(), bn.begin(),
                 [](unsigned char c) { return std::tolower(c); });
  if (an != bn)
    return an < bn;
  return a.path().string() < b.path().string();
}

std::string human_size(std::uintmax_t bytes) {
  static const char *units[] = {"B", "K", "M", "G", "T"};
  double size = static_cast<double>(bytes);
  int unit = 0;
  while (size >= 1024.0 && unit < 4) {
    size /= 1024.0;
    ++unit;
  }
  if (unit == 0)
    return fmt::format("{} {}", bytes, units[unit]);
  return fmt::format("{:.1f} {}", size, units[unit]);
}

} // namespace

std::vector<fs::directory_entry> App::list_dir(const fs::path &path) {
  std::vector<fs::directory_entry> out;
  std::error_code ec;
  fs::directory_iterator it(path, ec);
  if (ec)
    return out;
  for (; it != fs::directory_iterator(); it.increment(ec)) {
    if (ec)
      break;
    out.push_back(*it);
  }
  std::sort(out.begin(), out.end(), name_less);
  return out;
}

void App::populate_entries() {
  std::error_code ec;
  fs::path abs = fs::absolute(current_path, ec);
  if (!ec)
    current_path = abs;

  entries = list_dir(current_path);

  // Parent column (ranger's left pane).
  fs::path parent = current_path.parent_path();
  if (parent.empty())
    parent = current_path;
  parent_entries = list_dir(parent);

  // Find which parent row corresponds to current_path so it can be
  // highlighted, mirroring ranger.
  parent_highlight = 0;
  std::string cur_name = current_path.filename().string();
  for (size_t i = 0; i < parent_entries.size(); ++i) {
    if (parent_entries[i].path().filename().string() == cur_name) {
      parent_highlight = i;
      break;
    }
  }

  // Preview column (ranger's right pane): children of the selected dir.
  preview_entries.clear();
  preview_is_dir = false;
  if (!entries.empty()) {
    if (selected_index >= entries.size())
      selected_index = entries.size() - 1;
    const auto &sel = entries[selected_index];
    std::error_code dec;
    if (sel.is_directory(dec) && !dec) {
      preview_is_dir = true;
      preview_entries = list_dir(sel.path());
    }
  } else {
    selected_index = 0;
  }
  selection_changed = true;
}

void App::go_to(fs::path new_path) {
  std::error_code ec;
  fs::path target = fs::absolute(new_path, ec);
  if (ec)
    return;
  fs::path old_current = current_path;

  // Going up: remember which entry we came from so the parent selection
  // lands on it, like ranger does.
  bool going_to_parent = (target == old_current.parent_path());
  std::string restore_name;
  if (going_to_parent)
    restore_name = old_current.filename().string();

  current_path = target;
  selected_index = 0;
  populate_entries();

  if (going_to_parent && !restore_name.empty()) {
    for (size_t i = 0; i < entries.size(); ++i) {
      if (entries[i].path().filename().string() == restore_name) {
        selected_index = i;
        break;
      }
    }
    // Refresh preview for the restored selection.
    preview_entries.clear();
    preview_is_dir = false;
    if (!entries.empty() && selected_index < entries.size()) {
      const auto &sel = entries[selected_index];
      std::error_code dec;
      if (sel.is_directory(dec) && !dec) {
        preview_is_dir = true;
        preview_entries = list_dir(sel.path());
      }
    }
    selection_changed = true;
  }
}

void App::poll_input() {
  auto left = ImGui::IsKeyPressed(ImGuiKey_LeftArrow, false);
  auto right = ImGui::IsKeyPressed(ImGuiKey_RightArrow, false);
  auto up = ImGui::IsKeyPressed(ImGuiKey_UpArrow, false);
  auto down = ImGui::IsKeyPressed(ImGuiKey_DownArrow, false);

  int hor_direction = right - left;
  int ver_direction = up - down;

  if (!hor_direction) {
    if (ver_direction == 1) {
      current_direction = Dir::Up;
    } else if (ver_direction == -1) {
      current_direction = Dir::Down;
    } else {
      current_direction = Dir::None;
    }
  } else {
    if (hor_direction == 1) {
      current_direction = Dir::Right;
    } else if (hor_direction == -1) {
      current_direction = Dir::Left;
    }
  }
}

void App::update_state() {
  if (entries.empty()) {
    // Still allow navigating up out of an empty directory.
    if (current_direction == Dir::Left) {
      fs::path parent = current_path.parent_path();
      if (!parent.empty() && parent != current_path)
        go_to(parent);
    }
    current_direction = Dir::None;
    return;
  }

  switch (current_direction) {
  case Dir::None:
    break;
  case Dir::Up:
    // ranger: move selection up.
    selected_index =
        (selected_index == 0) ? entries.size() - 1 : selected_index - 1;
    selection_changed = true;
    populate_entries();
    // populate_entries() keeps selected_index (clamps only) and refreshes
    // the preview for the new selection; re-assert the moved index in case
    // the directory changed on disk between frames.
    if (selected_index >= entries.size() && !entries.empty())
      selected_index = entries.size() - 1;
    break;
  case Dir::Down:
    // ranger: move selection down.
    selected_index = (selected_index + 1) % entries.size();
    selection_changed = true;
    populate_entries();
    if (selected_index >= entries.size() && !entries.empty())
      selected_index = entries.size() - 1;
    break;
  case Dir::Left:
    // ranger: go to parent directory.
    {
      fs::path parent = current_path.parent_path();
      if (!parent.empty() && parent != current_path)
        go_to(parent);
    }
    break;
  case Dir::Right: {
    // ranger: enter highlighted directory.
    const auto &sel = entries[selected_index];
    std::error_code ec;
    if (sel.is_directory(ec) && !ec) {
      current_path = sel.path();
      selected_index = 0;
      populate_entries();
    }
    break;
  }
  }
  // populate_entries() above may have overwritten selected_index for Up/Down
  // because it clamps; fix by re-applying movement against fresh size would
  // be wrong. Instead handle Up/Down without full repopulate clobbering:
  // (recompute preview only). The simplest correct approach: recompute here.
  current_direction = Dir::None;
}

void App::render_header(const ImVec2 &window_pos, float window_width) {
  ImDrawList *draw = ImGui::GetWindowDrawList();
  ImVec2 header_min = window_pos;
  ImVec2 header_max(window_pos.x + window_width, window_pos.y + header_height);
  draw->AddRectFilled(header_min, header_max, IM_COL32(38, 38, 38, 255));
  draw->AddLine(ImVec2(header_min.x, header_max.y - 1),
                ImVec2(header_max.x, header_max.y - 1),
                IM_COL32(70, 70, 70, 255));
  std::error_code ec;
  std::string path_str = fs::canonical(current_path, ec).string();
  if (ec)
    path_str = fs::absolute(current_path, ec).string();
  if (ec)
    path_str = current_path.string();
  draw->AddText(ImVec2(window_pos.x + padding,
                       window_pos.y +
                           (header_height - ImGui::GetFontSize()) / 2.0f),
                IM_COL32(220, 220, 220, 255), path_str.c_str());
}

void App::render_entry_list(const char *child_id,
                            const std::vector<fs::directory_entry> &list,
                            int highlight_index, bool is_active,
                            bool *scrolled_to_highlight) {
  ImGui::BeginChild(child_id, ImVec2(0, 0), false,
                    ImGuiWindowFlags_HorizontalScrollbar);
  for (size_t i = 0; i < list.size(); ++i) {
    bool is_dir = false;
    std::string name = display_name(list[i], is_dir);
    bool highlighted = (static_cast<int>(i) == highlight_index);
    if (is_active)
      ImGui::PushStyleColor(ImGuiCol_Text, is_dir
                                                 ? IM_COL32(230, 200, 80, 255)
                                                 : IM_COL32(220, 220, 220, 255));
    else
      ImGui::PushStyleColor(ImGuiCol_Text, is_dir
                                                 ? IM_COL32(160, 140, 70, 255)
                                                 : IM_COL32(150, 150, 150, 255));
    if (highlighted)
      ImGui::PushStyleColor(ImGuiCol_Header,
                            is_active ? IM_COL32(0, 120, 160, 255)
                                      : IM_COL32(60, 60, 60, 255));
    ImGui::Selectable(name.c_str(), highlighted);
    if (highlighted)
      ImGui::PopStyleColor(); // Header
    ImGui::PopStyleColor();   // Text
    if (highlighted && selection_changed && scrolled_to_highlight &&
        !*scrolled_to_highlight) {
      ImGui::SetScrollHereY(0.5f);
      *scrolled_to_highlight = true;
    }
  }
  if (list.empty()) {
    ImGui::TextDisabled("<empty>");
  }
  ImGui::EndChild();
}

void App::render_file_preview(const char *child_id,
                              const fs::directory_entry &e) {
  ImGui::BeginChild(child_id, ImVec2(0, 0), false);
  std::error_code ec;
  auto fsize = e.file_size(ec);
  ImGui::TextDisabled("%s", e.path().filename().string().c_str());
  ImGui::Separator();
  if (!ec)
    ImGui::Text("Size: %s", human_size(fsize).c_str());
  else
    ImGui::Text("Size: ?");
  auto ext = e.path().extension().string();
  ImGui::Text("Type: %s", ext.empty() ? "file" : ext.c_str());

  // Text preview for small files, mirroring ranger's file preview pane.
  const std::uintmax_t max_preview_bytes = 64 * 1024;
  if (!ec && fsize <= max_preview_bytes) {
    std::ifstream in(e.path(), std::ios::binary);
    if (in) {
      std::string content((std::istreambuf_iterator<char>(in)),
                          std::istreambuf_iterator<char>());
      bool is_binary = false;
      for (size_t i = 0; i < content.size() && i < 1024; ++i) {
        if (content[i] == '\0') {
          is_binary = true;
          break;
        }
      }
      ImGui::Separator();
      if (is_binary) {
        ImGui::TextDisabled("<binary file>");
      } else {
        // Clamp to first ~100 lines so huge single-line files don't hang.
        size_t lines = 0;
        size_t cut = content.size();
        for (size_t i = 0; i < content.size(); ++i) {
          if (content[i] == '\n' && ++lines >= 100) {
            cut = i;
            break;
          }
        }
        ImGui::TextUnformatted(content.data(),
                               content.data() + cut);
      }
    }
  } else if (!ec) {
    ImGui::Separator();
    ImGui::TextDisabled("<file too large to preview>");
  }
  ImGui::EndChild();
}

void App::render_miller_columns(const ImVec2 &origin, const ImVec2 &avail) {
  ImGui::SetCursorScreenPos(origin);

  ImGuiTableFlags flags =
      ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_SizingStretchProp;
  if (ImGui::BeginTable("MillerColumns", 3, flags, avail)) {
    ImGui::TableSetupColumn("Parent", ImGuiTableColumnFlags_None,
                            parent_ratio);
    ImGui::TableSetupColumn("Current", ImGuiTableColumnFlags_None,
                            current_ratio);
    ImGui::TableSetupColumn("Preview", ImGuiTableColumnFlags_None,
                            preview_ratio);

    ImGui::TableNextRow();
    // Parent pane (ranger left column).
    ImGui::TableNextColumn();
    {
      std::error_code ec;
      fs::path parent = current_path.parent_path();
      std::string title =
          parent.empty() ? current_path.string() : parent.string();
      ImGui::TextDisabled("%s", title.c_str());
      ImGui::Separator();
      ImVec2 col_avail = ImGui::GetContentRegionAvail();
      bool scrolled = false;
      int hl = parent_entries.empty()
                   ? -1
                   : static_cast<int>(std::min(
                         parent_highlight, parent_entries.size() - 1));
      render_entry_list("##parent_list", parent_entries, hl, false, &scrolled);
      (void)col_avail;
    }
    // Current pane (ranger middle column, the active one).
    ImGui::TableNextColumn();
    {
      std::string cur_title = current_path.filename().string();
      if (cur_title.empty())
        cur_title = current_path.string();
      ImGui::Text("%s", cur_title.c_str());
      ImGui::Separator();
      bool scrolled = false;
      int hl = entries.empty() ? -1 : static_cast<int>(selected_index);
      render_entry_list("##current_list", entries, hl, true, &scrolled);
    }
    // Preview pane (ranger right column).
    ImGui::TableNextColumn();
    {
      if (entries.empty()) {
        ImGui::TextDisabled("<empty>");
      } else {
        const auto &sel = entries[selected_index];
        std::error_code ec;
        bool is_dir = sel.is_directory(ec);
        if (!ec && is_dir) {
          ImGui::TextDisabled("%s",
                              sel.path().filename().string().c_str());
          ImGui::Separator();
          bool scrolled = true; // preview list starts at top, like ranger
          render_entry_list("##preview_list", preview_entries, -1, false,
                            &scrolled);
        } else {
          render_file_preview("##file_preview", sel);
        }
      }
    }
    ImGui::EndTable();
  }
}

void App::render_footer(const ImVec2 &window_pos, const ImVec2 &window_size) {
  ImDrawList *draw = ImGui::GetWindowDrawList();
  float footer_y = window_pos.y + window_size.y - footer_height;
  draw->AddRectFilled(ImVec2(window_pos.x, footer_y),
                      ImVec2(window_pos.x + window_size.x,
                             window_pos.y + window_size.y),
                      IM_COL32(38, 38, 38, 255));
  draw->AddLine(ImVec2(window_pos.x, footer_y),
                ImVec2(window_pos.x + window_size.x, footer_y),
                IM_COL32(70, 70, 70, 255));

  std::string status;
  if (entries.empty()) {
    status = "empty  |  Up/Down: select  Left: parent  Right: open";
  } else {
    bool is_dir = false;
    std::string name = display_name(entries[selected_index], is_dir);
    status = fmt::format("{} / {}  {}  |  Up/Down: select  Left: parent  "
                         "Right: open",
                         selected_index + 1, entries.size(), name);
  }
  draw->AddText(
      ImVec2(window_pos.x + padding,
             footer_y + (footer_height - ImGui::GetFontSize()) / 2.0f),
      IM_COL32(180, 180, 180, 255), status.c_str());
}

void App::render() {
  const ImGuiViewport *viewport = ImGui::GetMainViewport();
  ImGui::SetNextWindowPos(viewport->WorkPos);
  ImGui::SetNextWindowSize(viewport->WorkSize);
  ImGui::Begin("Padphin", nullptr,
               ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
                   ImGuiWindowFlags_NoResize |
                   ImGuiWindowFlags_NoSavedSettings |
                   ImGuiWindowFlags_NoBringToFrontOnFocus);

  const ImVec2 window_pos = ImGui::GetWindowPos();
  const ImVec2 window_size = ImGui::GetWindowSize();

  render_header(window_pos, window_size.x);

  ImVec2 columns_origin(window_pos.x, window_pos.y + header_height);
  ImVec2 columns_avail(window_size.x,
                       window_size.y - header_height - footer_height);
  render_miller_columns(columns_origin, columns_avail);

  render_footer(window_pos, window_size);

  selection_changed = false;
  ImGui::End();
}
