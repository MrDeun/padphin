#pragma once
#include <imgui.h>

// Highlight bar drawn behind the rows of a Miller column. Instead of letting
// ImGui paint the header of the selected row, the bar is drawn on its own and
// tweened (via ImAnim) towards the rectangle of the selected row, so moving
// the selection slides the bar rather than jumping it.
//
// One instance per column. The bar is drawn before the rows so the row text
// stays on top of it; because the target rect is only known once the rows have
// been submitted, it is captured at the end of the frame and picked up on the
// next one.
class EntryHighlight {
  // (min.x, min.y, max.x, max.y) of the selected row, as measured last frame.
  ImVec4 target_{0.0f, 0.0f, 0.0f, 0.0f};
  bool has_target_ = false;
  bool tween_seeded_ = false;

public:
  // Slide duration in seconds. Short enough to keep up with repeated
  // navigation, long enough to read as motion.
  static constexpr float duration = 0.12f;

  // Draws the bar for the column owning `id`. Call right after BeginChild and
  // before the rows so the text is drawn on top of it.
  void draw(ImGuiID id, ImU32 color);
  // Sets the rectangle the bar animates towards; picked up by draw() on the
  // next frame.
  void set_target(const ImVec2 &row_min, const ImVec2 &row_max);
  void clear();
};
