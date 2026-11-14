export module hooks.draw_view_model;
export import std.compat;
export import core.sys;
export import gui.config;

export extern "C++" {

inline bool (*draw_view_model_original)(void*);

inline bool draw_view_model_hook(void* me) {
  if (config.viewport.filters.scope == true)
    return true;
  else
    return draw_view_model_original(me);
}

}
