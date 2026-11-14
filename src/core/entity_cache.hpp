export module core.entity_cache;
export import std.compat;
export import core.sys;
export import core.vec;
export import sdk.classes.entity;

export extern "C++" {




// These are populated in src/hooks/frame_stage_notify.cpp
inline std::recursive_mutex cache_mutex;
inline std::unordered_map<enum class_id, std::vector<Entity*>> entity_cache;
inline std::unordered_map<unsigned long, bool> friend_cache;

struct PickupItem {
  Vec3 location;
  float time;
};
inline std::vector<PickupItem> pickup_item_cache;


}
