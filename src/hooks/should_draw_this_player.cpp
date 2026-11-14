export module hooks.should_draw_this_player;
export import std.compat;
export import core.sys;
export import sdk.interfaces.entity_list;
export import sdk.interfaces.input;
export import sdk.classes.player;
export import core.print;

export extern "C++" {



inline bool (*should_draw_this_player_original)(void*);

inline bool should_draw_this_player_hook(void* me) {
  //if (me == entity_list->get_localplayer()) return true;
  
  return should_draw_this_player_original(me);
}

}
