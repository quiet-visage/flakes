export module hooks.team_menu_show_panel;
export import std.compat;
export import core.sys;
export import gui.config;
export import sdk.interfaces.engine;
export import core.print;

export extern "C++" {



inline void (*team_menu_show_panel_original)(void*, bool) = nullptr;

inline void team_menu_show_panel_hook(void* me, bool show) {
  Player* localplayer = entity_list->get_localplayer();
  if (localplayer == nullptr) {
    team_menu_show_panel_original(me, show);
    return;
  }

  if (config.simulation.automation.auto_class_select == true && localplayer->get_team() == tf_team::UNKNOWN) {
    team_menu_show_panel_original(me, false);    
  } else {
    team_menu_show_panel_original(me, show);
  }  
}

}
