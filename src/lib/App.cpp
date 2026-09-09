#include "../include/App.hpp"
#include "fmt/core.h"
#include "imgui.h"
#include <algorithm>
#include <filesystem>
#include <string>

void App::populate_entries() {
  entries.clear();
  for (const auto &e : fs::directory_iterator(current_path))
    entries.push_back(e);
  std::sort(
      std::begin(entries), std::end(entries),
      [](const fs::directory_entry &first, const fs::directory_entry &second) {
        auto first_result = static_cast<uint8_t>(first.is_directory());
        auto second_result = static_cast<uint8_t>(second.is_directory());
        return first_result > second_result;
      });
  entries.shrink_to_fit();
  if (!entries.empty())
    selected_index = std::min(selected_index, entries.size() - 1);
}

void App::go_to(fs::path new_path) {
  current_path = new_path;
  populate_entries();
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
    current_direction = Dir::None;
    return;
  }

  switch (current_direction) {
  case Dir::None:
  case Dir::Up:
  case Dir::Down:
    break;
  case Dir::Left:
    if (selected_index == 0) {
      selected_index = entries.size() - 1;
    } else {
      selected_index = static_cast<int>(selected_index - 1) % entries.size();
    }
    break;
  case Dir::Right:
    selected_index = static_cast<int>(selected_index + 1) % entries.size();
    break;
  }
  if (current_direction != Dir::None) {
    fmt::println("Current index: {}", selected_index);
  }
  current_direction = Dir::None;
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

  ImDrawList *draw = ImGui::GetWindowDrawList();
  const ImVec2 window_pos = ImGui::GetWindowPos();
  const ImVec2 window_size = ImGui::GetWindowSize();

  ImVec2 header_min = window_pos;
  ImVec2 header_max(window_pos.x + window_size.x, window_pos.y + header_height);
  draw->AddRectFilled(header_min, header_max, IM_COL32(38, 38, 38, 255));
  ImVec2 header_text_pos(window_pos.x + padding,
                         window_pos.y +
                             (header_height - ImGui::GetFontSize()) / 2.0f);
  draw->AddText(header_text_pos, IM_COL32(220, 220, 220, 255),
                fs::canonical(current_path).c_str());
  int columns =
      static_cast<int>((window_size.x - 2 * padding) / (cell_width + padding));
  if (columns < 1)
    columns = 1;

  const ImVec2 grid_origin(window_pos.x + padding,
                           window_pos.y + header_height + padding);
  constexpr float icon_size = 40.0f;
  constexpr ImU32 selected_color = IM_COL32(0, 160, 200, 255);
  constexpr ImU32 cell_color = IM_COL32(50, 50, 50, 255);
  constexpr ImU32 cell_outline = IM_COL32(80, 80, 80, 255);
  constexpr ImU32 dir_icon = IM_COL32(230, 200, 80, 255);
  constexpr ImU32 file_icon = IM_COL32(180, 180, 180, 255);

  for (size_t i = 0; i < entries.size(); ++i) {
    const auto &e = entries[i];
    int col = static_cast<int>(i) % columns;
    int row = static_cast<int>(i) / columns;
    float x = grid_origin.x + col * (cell_width + padding);
    float y = grid_origin.y + row * (cell_height + padding);

    ImVec2 cell_min(x, y);
    ImVec2 cell_max(x + cell_width, y + cell_height);
    bool is_selected = (i == selected_index);
    draw->AddRectFilled(cell_min, cell_max, cell_color);
    draw->AddRect(cell_min, cell_max,
                  is_selected ? selected_color : cell_outline, 0.0f,
                  ImDrawFlags_None, is_selected ? 3.0f : 1.0f);

    ImVec2 icon_min(x + (cell_width - icon_size) / 2.0f,
                    y + (cell_height - icon_size) / 2.0f - 10.0f);
    ImVec2 icon_max(icon_min.x + icon_size, icon_min.y + icon_size);
    if (e.is_directory()) {
      draw->AddRectFilled(icon_min, icon_max, dir_icon);
    } else {
      draw->AddRect(icon_min, icon_max, file_icon, 0.0f, ImDrawFlags_None,
                    2.0f);
    }

    draw->AddText(ImVec2(x, y + cell_height), IM_COL32(240, 240, 240, 255),
                  e.path().stem().string().c_str());
  }
  draw->AddText(ImVec2(window_size.x, window_size.y + 20),
                IM_COL32(240, 240, 240, 255),
                fmt::format("Current index: {}", this->selected_index).c_str());

  ImGui::End();
}