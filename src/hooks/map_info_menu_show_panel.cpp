export module hooks.map_info_menu_show_panel;
export import std.compat;
export import core.sys;
export import gui.config;

export extern "C++" {


inline void (*map_info_menu_show_panel_original)(void*, bool) = nullptr;

inline void map_info_menu_show_panel_hook(void* me, bool show) {
  if (config.simulation.automation.auto_class_select == true) {
    map_info_menu_show_panel_original(me, false);
  } else {
    map_info_menu_show_panel_original(me, show);
  }
}

}
