#pragma once
#include "IconLoader.hpp"
#include "IControl.hpp"
#include "Clipboard.hpp"
#include "SoundPlayer.hpp"
#include <cstddef>
#include <filesystem>
#include <memory>
#include <vector>

struct ImVec2;

std::string find_resource_path(const std::string&);

namespace fs = std::filesystem;

class App {
  std::unique_ptr<IControl> control_;
  Input last_input{};
  fs::path current_path = ".";
  std::vector<fs::directory_entry> entries{};         // current directory
  std::vector<fs::directory_entry> parent_entries{};  // parent directory
  std::vector<fs::directory_entry> preview_entries{}; // children of selection
  Clipboard _clipboard{};
  SoundPlayer player{};
  bool preview_is_dir = false;
  size_t selected_index = 0;
  size_t parent_highlight = 0; // index in parent_entries matching current_path
  bool selection_changed = true;

  static constexpr float padding = 10.0f;
  static constexpr float header_height = 48.0f;
  static constexpr float footer_height = 32.0f;

  // Big gamepad-friendly rows (baseline values at 1920x1080; scaled by
  // DisplaySettings::ui_scale_for() of the working resolution at render).
  static constexpr float item_height = 54.0f;
  static constexpr float item_font_scale = 1.4f;
  static constexpr float item_spacing_y = 6.0f;

  // Miller-column width ratios: parent | current | preview (ranger defaults
  // to a narrow parent, wider current + preview).
  static constexpr float parent_ratio = 0.22f;
  static constexpr float current_ratio = 0.38f;
  static constexpr float preview_ratio = 0.40f;

  // Footer icon scale factor.
  static constexpr float icon_scale = 2.0f;

  IconLoader icon_loader_;

  IconSet keyboard_set;
  IconSet playstation_set;
  IconSet switch_set;
  IconSet xbox_set;

  // UI scale derived from the working resolution (viewport WorkSize)
  // relative to the 1080p baseline. Updated every frame in render().
  float ui_scale_ = 1.0f;

  float scaled_padding() const { return padding * ui_scale_; }
  float scaled_header_height() const { return header_height * ui_scale_; }
  float scaled_footer_height() const { return footer_height * ui_scale_; }
  float scaled_item_height() const { return item_height * ui_scale_; }
  float scaled_item_spacing_y() const { return item_spacing_y * ui_scale_; }

  // Loaded icon textures for the footer hints.

  void load_icons();
  void load_sounds();
  void populate_entries();
  void refresh_preview();
  static std::vector<fs::directory_entry> list_dir(const fs::path &path);

  void render_header(const ImVec2 &window_pos, float window_width);
  void render_miller_columns(const ImVec2 &origin, const ImVec2 &avail);
  void render_entry_list(const char *child_id,
                         const std::vector<fs::directory_entry> &list,
                         int highlight_index, bool is_active,
                         bool *scrolled_to_highlight);
  void render_file_preview(const char *child_id, const fs::directory_entry &e);
  void render_footer(const ImVec2 &window_pos, const ImVec2 &window_size);

  using SoundEffectId = size_t;

  SoundEffectId hover_id;

public:
  void go_to(fs::path new_path);
  const fs::path &get_current_path() const { return current_path; }
  void poll_input();
  void update_state();
  void render();

  explicit App(std::unique_ptr<IControl> control)
      : control_(std::move(control)) {
    load_icons();
    load_sounds();
  }
};