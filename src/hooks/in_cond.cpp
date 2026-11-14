export module hooks.in_cond;
export import std.compat;
export import core.sys;
export import gui.config;
export import sdk.classes.player;

export extern "C++" {


inline bool in_cond_hook(void* me, int mask) {

  if (mask == TF_COND_ZOOMED && config.viewport.filters.scope == true) { //if the player is scoped
    return false;
  }

  bool re = in_cond_original(me, mask);
  
  return re;
}

}
