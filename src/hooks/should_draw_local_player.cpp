export module hooks.should_draw_local_player;
export import std.compat;
export import core.sys;
export import sdk.interfaces.entity_list;
export import sdk.interfaces.input;
export import sdk.classes.player;

export extern "C++" {



inline bool (*should_draw_local_player_original)(void*);

inline bool should_draw_local_player_hook(void* me) {
  Player* localplayer = entity_list->get_localplayer();

  //return true;
  /*
  if (localplayer != nullptr && me == localplayer)
    return true;
  else
  */
    return should_draw_local_player_original(me);
}

}
