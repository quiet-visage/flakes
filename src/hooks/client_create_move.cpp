export module hooks.client_create_move;
export import std.compat;
export import core.sys;
export import sdk.interfaces.global_vars;
export import sdk.interfaces.steam_friends;
export import sdk.classes.player;
export import core.entity_cache;

export extern "C++" {



inline void (*client_create_move_original)(void*, int, float, bool);

inline void client_create_move_hook(void* me, int sequence_number, float input_sample_frametime, bool active) {
  client_create_move_original(me, sequence_number, input_sample_frametime, active);
}

}
