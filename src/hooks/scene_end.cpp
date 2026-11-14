export module hooks.scene_end;
export import std.compat;
export import core.sys;

export extern "C++" {

inline void (*scene_end_original)(void*, void*);

inline void scene_end_hook(void* me, void* b8) {
  scene_end_original(me, b8);
}

}
