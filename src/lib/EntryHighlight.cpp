#include "../include/EntryHighlight.hpp"
#include "im_anim.h"

void EntryHighlight::draw(ImGuiID id, ImU32 color) {
  if (!has_target_)
    return;

  // The first tween of a column is cut, otherwise it would ease in from the
  // zeroed channel a fresh tween starts with.
  const int policy = tween_seeded_ ? iam_policy_crossfade : iam_policy_cut;
  tween_seeded_ = true;
  ImVec4 rect =
      iam_tween_vec4(id, ImGui::GetID("##rect"), target_, duration,
                     iam_ease_preset(iam_ease_out_cubic), policy,
                     ImGui::GetIO().DeltaTime);
  if (rect.z <= rect.x || rect.w <= rect.y)
    return;
  ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(rect.x, rect.y),
                                            ImVec2(rect.z, rect.w), color);
}

void EntryHighlight::set_target(const ImVec2 &row_min, const ImVec2 &row_max) {
  target_ = ImVec4(row_min.x, row_min.y, row_max.x, row_max.y);
  has_target_ = true;
}

void EntryHighlight::clear() {
  has_target_ = false;
  tween_seeded_ = false;
}
