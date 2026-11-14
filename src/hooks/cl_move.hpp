export module hooks.cl_move_hpp;
export import std.compat;
export import core.sys;

export extern "C++" {

inline void (*cl_move_original)(float, bool) = nullptr;

}
