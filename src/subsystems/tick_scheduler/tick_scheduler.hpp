#ifndef TICK_SCHEDULER_HPP
#define TICK_SCHEDULER_HPP

#include "imgui/dearimgui.hpp"

#include "gui/config.hpp"
#include "gui/menu.hpp"

#include "hooks/cl_move.hpp"

static bool choke_is_reached = false;
static int choked_ticks = 0;

static void choke_current_tick(void) {
  if (config.simulation.directives.tick_scheduler == false) return;

  // https://8dcc.github.io/reversing/reversing-tf2-bsendpacket.html
  void* current_frame_address = __builtin_frame_address(1);
  void* current_stack_address = (void*)((unsigned long)current_frame_address + 0x8);
  bool* send_packet = (bool*)((unsigned long)current_stack_address + 0xF8);

  if (is_button_down(config.simulation.directives.recharge_button) && choked_ticks < 24 && choke_is_reached == false) {
    *send_packet = false;
    choked_ticks++;
  }

  if (choke_is_reached == true && choked_ticks == 0) {
    choke_is_reached = false;
  }  
}

static void dispatch_buffered_ticks(float accumulated_extra_samples) {
  if (config.simulation.directives.tick_scheduler == false) return;
  
  if (menu_focused == false && ImGui::IsImGuiFullyInitialized() &&
      is_button_down(config.targeting.key) ||
      (ImGui::IsImGuiFullyInitialized() && ImGui::IsMouseDown(ImGuiMouseButton_Left)) &&
      choked_ticks == 24) {

    choke_is_reached = true;
    while (choked_ticks) {
      choked_ticks--;
      cl_move_original(accumulated_extra_samples, choked_ticks == 0);
    }
    
  }

}

#endif
