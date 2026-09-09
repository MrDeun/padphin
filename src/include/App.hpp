#pragma once
#include <cstdint>
#include <filesystem>
#include <vector>

namespace fs = std::filesystem;
class App {
  enum class Dir : uint8_t { None, Up, Down, Left, Right };

  Dir current_direction = Dir::None;
  fs::path current_path = ".";
  std::vector<fs::directory_entry> entries;
  size_t selected_index = 0;

  static constexpr float cell_width = 120.0f;
  static constexpr float cell_height = 120.0f;
  static constexpr float padding = 10.0f;
  static constexpr float header_height = 48.0f;

  void populate_entries();

public:
  void go_to(fs::path new_path);
  const fs::path &get_current_path() const { return current_path; }
  void poll_input();
  void update_state();
  void render();
};