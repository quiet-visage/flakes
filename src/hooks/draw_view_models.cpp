export module hooks.draw_view_models;
export import std.compat;
export import core.sys;
export import core.vec;
export import core.print;

export extern "C++" {


inline void (*draw_view_models_original)(void*, view_setup*, bool);

inline void draw_view_models_hook(void* me, view_setup* setup, bool draw_view_models) {
  draw_view_models_original(me, setup, draw_view_models);
}

}
