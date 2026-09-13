#include "../include/App.hpp"
#include "Clipboard.hpp"
#include <imgui.h>
#include <algorithm>
#include <cctype>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <string>
#include <unistd.h>
#include <utility>
// TODO - get OS-agnostic version of this function
std::string get_exe_dir() {
  char buf[4096];
  ssize_t len = readlink("/proc/self/exe", buf, sizeof(buf) - 1);
  if (len == -1)
    return ".";
  buf[len] = '\0';
  return fs::path(buf).parent_path().string();
}

auto display_name = [](const fs::directory_entry &e, bool &out_is_dir) {
  std::error_code ec;
  out_is_dir = e.is_directory(ec);
  std::string name = e.path().filename().string();
  if (name.empty())
    name = e.path().string();
  if (out_is_dir)
    name += "/";
  return name;
};

auto name_less = [](const fs::directory_entry &a,
                    const fs::directory_entry &b) -> bool {
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
};

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

std::string find_resource_path(const std::string &relative) {
  // Try common locations relative to the executable.
  std::string exe_dir = get_exe_dir();

  std::string candidates[] = {
      exe_dir + "/resources/" + relative,
      exe_dir + "/../resources/" + relative,
      exe_dir + "/../../resources/" + relative,
  };
  for (const auto &c : candidates) {
    std::error_code ec;
    if (fs::is_regular_file(c, ec))
      return c;
  }
  return "";
}

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
  return std::move(out);
}

void App::load_icons() {
  auto load = [this](const std::string &rel) -> Icon {
    std::string path = find_resource_path(rel);
    if (path.empty()) {
      fmt::println("File not found for -> {}", rel);
      return {};
    }

    return icon_loader_.load(path, icon_scale);
  };

  // Keyboard icons.
  const std::string playstation_base = "icon/kenney/playstation/";
  playstation_set = {
      .up = load(playstation_base + "playstation_dpad_up.svg"),
      .down = load(playstation_base + "playstation_dpad_down.svg"),
      .left = load(playstation_base + "playstation_dpad_left.svg"),
      .right = load(playstation_base + "playstation_dpad_right.svg"),
      .accept = load(playstation_base + "playstation_button_cross.svg"),
      .deny = load(playstation_base + "playstation_button_circle.svg"),
      .add_to_clipboard =
          load(playstation_base + "playstation_button_triangle.svg"),
      .open_menubar = load(playstation_base + "playstation_button_square.svg"),
  };
  const std::string xbox_base = "icon/kenney/xbox/";
  xbox_set = {
      .up = load(xbox_base + "xbox_dpad_up.svg"),
      .down = load(xbox_base + "xbox_dpad_down.svg"),
      .left = load(xbox_base + "xbox_dpad_left.svg"),
      .right = load(xbox_base + "xbox_dpad_right.svg"),
      .accept = load(xbox_base + "xbox_button_a.svg"),
      .deny = load(xbox_base + "xbox_button_b.svg"),
      .add_to_clipboard = load(xbox_base + "xbox_button_y.svg"),
      .open_menubar = load(xbox_base + "xbox_button_x.svg"),
  };
  const std::string switch_base = "icon/kenney/switch/";
  switch_set = {
      .up = load(switch_base + "switch_dpad_up.svg"),
      .down = load(switch_base + "switch_dpad_down.svg"),
      .left = load(switch_base + "switch_dpad_left.svg"),
      .right = load(switch_base + "switch_dpad_right.svg"),
      .accept = load(switch_base + "switch_button_b.svg"),
      .deny = load(switch_base + "switch_button_a.svg"),
      .add_to_clipboard = load(switch_base + "switch_button_x.svg"),
      .open_menubar = load(switch_base + "switch_button_y.svg"),
  };
  const std::string keyboard_base = "icon/kenney/keyboard/";
  switch_set = {
      .up = load(keyboard_base + "keyboard_arrow_up.svg"),
      .down = load(keyboard_base + "keyboard_arrow_down.svg"),
      .left = load(keyboard_base + "keyboard_arrow_left.svg"),
      .right = load(keyboard_base + "keyboard_arrow_right.svg"),
      .accept = load(keyboard_base + "keyboard_enter.svg"),
      .deny = load(keyboard_base + "keyboard_escape.svg"),
      .add_to_clipboard = load(keyboard_base + "keyboard_c.svg"),
      .open_menubar = load(keyboard_base + "keyboard_alt.svg"),
  };
}

void App::refresh_preview() {
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
  if (!entries.empty()) {
    if (selected_index >= entries.size())
      selected_index = entries.size() - 1;
    refresh_preview();
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
    refresh_preview();
    selection_changed = true;
  }
}

void App::poll_input() { last_input = control_->poll(); }

void App::update_state() {
  Dir dir = last_input.dir;
  uint8_t btn = last_input.button;

  if (entries.empty()) {
    if (dir == Dir::Left || (btn & Input::DENY)) {
      fs::path parent = current_path.parent_path();
      if (!parent.empty() && parent != current_path)
        go_to(parent);
    }
    return;
  }

  switch (dir) {
  case Dir::None:
    break;
  case Dir::Up:
    selected_index =
        (selected_index == 0) ? entries.size() - 1 : selected_index - 1;
    selection_changed = true;
    if (selected_index >= entries.size() && !entries.empty())
      selected_index = entries.size() - 1;
    refresh_preview();
    break;
  case Dir::Down:
    selected_index = (selected_index + 1) % entries.size();
    selection_changed = true;
    if (selected_index >= entries.size() && !entries.empty())
      selected_index = entries.size() - 1;
    refresh_preview();
    break;
  case Dir::Left: {
    fs::path parent = current_path.parent_path();
    if (!parent.empty() && parent != current_path)
      go_to(parent);
    break;
  }
  case Dir::Right: {
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

  if (btn & Input::ACCEPT) {
    const auto &sel = entries[selected_index];
    std::error_code ec;
    if (sel.is_directory(ec) && !ec) {
      current_path = sel.path();
      selected_index = 0;
      populate_entries();
    }
  }
  if (btn & Input::DENY) {
    fs::path parent = current_path.parent_path();
    if (!parent.empty() && parent != current_path)
      go_to(parent);
  }

  if (btn & Input::ADD_TO_CLIPBOARD) {
    fs::path abs = fs::absolute(entries.at(selected_index));
    _clipboard.append(std::move(abs));
  }

  last_input = {};
}

void App::render_header(const ImVec2 &window_pos, float window_width) {
  const float header_h = scaled_header_height();
  const float pad = scaled_padding();
  ImDrawList *draw = ImGui::GetWindowDrawList();
  ImVec2 header_min = window_pos;
  ImVec2 header_max(window_pos.x + window_width, window_pos.y + header_h);
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
  draw->AddText(ImVec2(window_pos.x + pad,
                       window_pos.y + (header_h - ImGui::GetFontSize()) / 2.0f),
                IM_COL32(220, 220, 220, 255), path_str.c_str());
  ImGui::NextColumn();
  std::string text = fmt::format("Clipboard size: {}", _clipboard.items.size());
  auto posX = (ImGui::GetCursorPosX() + ImGui::GetColumnWidth() -
               ImGui::CalcTextSize(text.c_str()).x - ImGui::GetScrollX() -
               2 * ImGui::GetStyle().ItemSpacing.x);
  if (posX > ImGui::GetCursorPosX())
    ImGui::SetCursorPos(ImVec2(posX,window_pos.y + (header_h - ImGui::GetFontSize()) / 2.0f));
  ImGui::Text("%s", text.c_str());
}

void App::render_entry_list(const char *child_id,
                            const std::vector<fs::directory_entry> &list,
                            int highlight_index, bool is_active,
                            bool *scrolled_to_highlight) {
  // Bound the child to the remaining cell space so tall rows scroll
  // instead of growing the table.
  // Row metrics scale with the working resolution so entries keep the same
  // relative size on 720p / 1080p / 1440p / 4K. Font scaling comes from
  // io.FontGlobalScale (set in render()); pixel sizes are scaled here.
  const float row_h = scaled_item_height();
  const float spacing_y = scaled_item_spacing_y();
  ImVec2 avail = ImGui::GetContentRegionAvail();
  if (avail.y < 1.0f)
    avail.y = 1.0f;
  ImGui::BeginChild(child_id, avail, false,
                    ImGuiWindowFlags_HorizontalScrollbar |
                        ImGuiWindowFlags_AlwaysVerticalScrollbar);
  ImGui::SetWindowFontScale(item_font_scale);
  ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, spacing_y));
  ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, ImVec2(0.0f, 0.5f));
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
      ImGui::PushStyleColor(ImGuiCol_Header, is_active
                                                 ? IM_COL32(0, 120, 160, 255)
                                                 : IM_COL32(60, 60, 60, 255));
    ImGui::Selectable(name.c_str(), highlighted, 0, ImVec2(0.0f, row_h));
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
  ImGui::PopStyleVar(2);
  ImGui::EndChild();
}

void App::render_file_preview(const char *child_id,
                              const fs::directory_entry &e) {
  ImVec2 avail = ImGui::GetContentRegionAvail();
  if (avail.y < 1.0f)
    avail.y = 1.0f;
  ImGui::BeginChild(child_id, avail, false,
                    ImGuiWindowFlags_AlwaysVerticalScrollbar |
                        ImGuiWindowFlags_HorizontalScrollbar);
  ImGui::SetWindowFontScale(item_font_scale);
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
        ImGui::TextUnformatted(content.data(), content.data() + cut);
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
    ImGui::TableSetupColumn("Parent", ImGuiTableColumnFlags_None, parent_ratio);
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
                   : static_cast<int>(
                         std::min(parent_highlight, parent_entries.size() - 1));
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
          ImGui::TextDisabled("%s", sel.path().filename().string().c_str());
          ImGui::Separator();
          bool scrolled = true; // preview list starts at top, like ranger
          render_entry_list("##preview_list", preview_entries, selected_index,
                            false, &scrolled);
        } else {
          render_file_preview("##file_preview", sel);
        }
      }
    }
    ImGui::EndTable();
  }
}

void App::render_footer(const ImVec2 &window_pos, const ImVec2 &window_size) {
  const float footer_h = scaled_footer_height();
  const float pad = scaled_padding();
  ImDrawList *draw = ImGui::GetWindowDrawList();
  float footer_y = window_pos.y + window_size.y - footer_h;
  draw->AddRectFilled(
      ImVec2(window_pos.x, footer_y),
      ImVec2(window_pos.x + window_size.x, window_pos.y + window_size.y),
      IM_COL32(38, 38, 38, 255));
  draw->AddLine(ImVec2(window_pos.x, footer_y),
                ImVec2(window_pos.x + window_size.x, footer_y),
                IM_COL32(70, 70, 70, 255));

  const ImU32 tint = IM_COL32_WHITE;
  float icon_size = footer_h - 6.0f * ui_scale_;
  float x = window_pos.x + pad;
  float text_y = footer_y + (footer_h - ImGui::GetFontSize()) / 2.0f;
  float icon_y = footer_y + (footer_h - icon_size) / 2.0f;
  float spacing = 5.0f * ui_scale_;
  float sep_color = IM_COL32(90, 90, 90, 255);

  // Left section: file index and name.
  if (!entries.empty()) {
    bool is_dir = false;
    std::string name = display_name(entries[selected_index], is_dir);
    std::string pos_str =
        fmt::format("{} / {}  {}", selected_index + 1, entries.size(), name);
    draw->AddText(ImVec2(x, text_y), IM_COL32(180, 180, 180, 255),
                  pos_str.c_str());
    x += ImGui::CalcTextSize(pos_str.c_str()).x + pad;
  }

  // Separator.
  draw->AddLine(ImVec2(x, footer_y + 6.0f * ui_scale_),
                ImVec2(x, footer_y + footer_h - 6.0f * ui_scale_), sep_color);
  x += pad;

  // Determine which icon set to use based on last input device.
  DeviceType device = control_->get_device_type();
  bool use_gamepad = (device == DeviceType::Xbox);

  // Navigation hints with icons.
  auto draw_icon_hint = [&](Icon &keyboard_icon, Icon &gamepad_icon,
                            const char *label) {
    Icon &icon = use_gamepad ? gamepad_icon : keyboard_icon;
    if (icon.texture_id) {
      draw->AddImage((ImTextureID)(intptr_t)icon.texture_id, ImVec2(x, icon_y),
                     ImVec2(x + icon_size, icon_y + icon_size), ImVec2(0, 0),
                     ImVec2(1, 1), tint);
      x += icon_size + spacing;
    }
    draw->AddText(ImVec2(x, text_y), IM_COL32(160, 160, 160, 255), label);
    x += ImGui::CalcTextSize(label).x + pad * 2;
  };

  draw_icon_hint(keyboard_set.up, xbox_set.up, "Up");
  draw_icon_hint(keyboard_set.down, xbox_set.down, "Down");
  draw_icon_hint(keyboard_set.left, xbox_set.left, "Parent");
  draw_icon_hint(keyboard_set.right, xbox_set.right, "Open");

  // Separator.
  draw->AddLine(ImVec2(x, footer_y + 6.0f * ui_scale_),
                ImVec2(x, footer_y + footer_h - 6.0f * ui_scale_), sep_color);
  x += pad;

  // Action hints.
  draw_icon_hint(keyboard_set.accept, xbox_set.accept, "Accept");
  draw_icon_hint(keyboard_set.deny, xbox_set.deny, "Back");

  // Right section: device info.
  std::string device_name = control_->get_device_name();
  if (!device_name.empty()) {
    float text_width = ImGui::CalcTextSize(device_name.c_str()).x;
    draw->AddText(
        ImVec2(window_pos.x + window_size.x - text_width - pad, text_y),
        IM_COL32(120, 120, 120, 255), device_name.c_str());
  }
}

void App::render() {
  const ImGuiViewport *viewport = ImGui::GetMainViewport();
  // Scale entries (and chrome) from the working resolution so rows keep the
  // same relative size across 720p / 1080p / 1440p / 4K. Font scaling is
  // applied globally; pixel sizes use the scaled_*() helpers.
  ui_scale_ =
      DisplaySettings::ui_scale_for(static_cast<int>(viewport->WorkSize.x),
                                    static_cast<int>(viewport->WorkSize.y));
  ImGui::GetIO().FontGlobalScale = ui_scale_;

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

  const float header_h = scaled_header_height();
  const float footer_h = scaled_footer_height();
  ImVec2 columns_origin(window_pos.x, window_pos.y + header_h);
  ImVec2 columns_avail(window_size.x, window_size.y - header_h - footer_h);
  ImGui::GetWindowDrawList()->PushClipRect(
      columns_origin, ImVec2(columns_origin.x + columns_avail.x,
                             columns_origin.y + columns_avail.y));
  render_miller_columns(columns_origin, columns_avail);
  ImGui::GetWindowDrawList()->PopClipRect();

  render_footer(window_pos, window_size);

  selection_changed = false;
  ImGui::End();
}
