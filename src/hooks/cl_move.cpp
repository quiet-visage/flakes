export module hooks.cl_move;
export import std.compat;
export import core.sys;
export import hooks.cl_move_hpp;
export import subsystems.tick_scheduler.tick_scheduler_hpp;
export import imgui.dearimgui;
export import gui.config;
export import core.print;

export extern "C++" {





// Called 66 times a second by the game
inline void cl_move_hook(float accumulated_extra_samples, bool final_tick) {
  dispatch_buffered_ticks(accumulated_extra_samples);

  cl_move_original(accumulated_extra_samples, final_tick);      
}

}
