#pragma once
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <vector>

struct ImVec2;

namespace fs = std::filesystem;
class App {
  enum class Dir : uint8_t { None, Up, Down, Left, Right };

  Dir current_direction = Dir::None;
  fs::path current_path = ".";
  std::vector<fs::directory_entry> entries;         // current directory
  std::vector<fs::directory_entry> parent_entries;  // parent directory
  std::vector<fs::directory_entry> preview_entries; // children of selection
  bool preview_is_dir = false;
  size_t selected_index = 0;
  size_t parent_highlight = 0; // index in parent_entries matching current_path
  bool selection_changed = true;

  static constexpr float padding = 10.0f;
  static constexpr float header_height = 48.0f;
  static constexpr float footer_height = 32.0f;

  // Miller-column width ratios: parent | current | preview (ranger defaults
  // to a narrow parent, wider current + preview).
  static constexpr float parent_ratio = 0.22f;
  static constexpr float current_ratio = 0.38f;
  static constexpr float preview_ratio = 0.40f;

  void populate_entries();
  static std::vector<fs::directory_entry> list_dir(const fs::path &path);

  void render_header(const ImVec2 &window_pos, float window_width);
  void render_miller_columns(const ImVec2 &origin, const ImVec2 &avail);
  void render_entry_list(const char *child_id,
                         const std::vector<fs::directory_entry> &list,
                         int highlight_index, bool is_active,
                         bool *scrolled_to_highlight);
  void render_file_preview(const char *child_id, const fs::directory_entry &e);
  void render_footer(const ImVec2 &window_pos, const ImVec2 &window_size);

public:
  void go_to(fs::path new_path);
  const fs::path &get_current_path() const { return current_path; }
  void poll_input();
  void update_state();
  void render();
};