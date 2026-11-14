export module hooks.text_window_show_panel;
export import std.compat;
export import core.sys;
export import gui.config;

export extern "C++" {
//55 48 8D 15 ? ? ? ? 48 89 E5 41 54 41 89 F4 53 48 8B 07



inline void (*text_window_show_panel_original)(void*, bool) = nullptr;

inline void text_window_show_panel_hook(void* me, bool show) {
  if (config.simulation.automation.auto_class_select == true) {
    text_window_show_panel_original(me, false);
  } else {
    text_window_show_panel_original(me, show);
  }    
}

}
