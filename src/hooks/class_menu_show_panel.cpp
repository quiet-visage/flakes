export module hooks.class_menu_show_panel;
export import std.compat;
export import core.sys;
export import sdk.interfaces.entity_list;
export import gui.config;

export extern "C++" {



inline void (*class_menu_show_panel_original)(void*, bool) = nullptr;

inline void class_menu_show_panel_hook(void* me, bool show) {
  Player* localplayer = entity_list->get_localplayer();
  if (localplayer == nullptr) {
    class_menu_show_panel_original(me, show);
    return;
  }
  
  if (config.simulation.automation.auto_class_select == true && localplayer->get_tf_class() == tf_class::UNDEFINED) {
    class_menu_show_panel_original(me, false);
  } else {
    class_menu_show_panel_original(me, show);
  }
}

}
