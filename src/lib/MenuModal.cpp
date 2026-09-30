#include "imgui.h"
#include <App.hpp>

void App::render_modal(const std::vector<fs::path> &clipboard_view) {
  // ImGui keeps popup state across frames, so request the open exactly once
  // per OPEN_MENU_BAR press. Calling OpenPopup() every frame would keep the
  // popup latched open and make it impossible to close.
  if (menu_open_request) {
    menu_open_request = false;
    ImGui::OpenPopup(menu_popup_id);
  }

  // BeginPopupModal renders nothing and returns false unless the popup is
  // open. EndPopup() must only be called when it returns true, otherwise the
  // window stack ends up unbalanced.
  if (!ImGui::BeginPopupModal(menu_popup_id)) {
    // io->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    // io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    return;
  }

  // Mouse input is disabled (ImGuiConfigFlags_NoMouse), so the button is not a
  // usable dismiss path on its own. DENY closes the menu from update_state(),
  // which clears active_menu; honour that here so the popup actually closes.
  if (ImGui::Button("Close Menu") || !active_menu) {
    active_menu = false;
    // io->ConfigFlags |= !ImGuiConfigFlags_NavEnableGamepad;
    // io->ConfigFlags |= !ImGuiConfigFlags_NavEnableKeyboard;
    ImGui::CloseCurrentPopup();
  }

  ImGui::EndPopup();
}
