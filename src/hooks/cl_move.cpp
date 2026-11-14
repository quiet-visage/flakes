#include "hooks/cl_move.hpp"

#include "subsystems/tick_scheduler/tick_scheduler.hpp"

#include "imgui/dearimgui.hpp"

#include "gui/config.hpp"

#include "core/print.hpp"

// Called 66 times a second by the game
void cl_move_hook(float accumulated_extra_samples, bool final_tick) {
  dispatch_buffered_ticks(accumulated_extra_samples);

  cl_move_original(accumulated_extra_samples, final_tick);      
}
