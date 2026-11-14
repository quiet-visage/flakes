export module core.random_seed;
export import std.compat;

export extern "C++" {
  inline uint32_t* random_seed = nullptr;
}
