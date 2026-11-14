#ifndef ENTITY_CACHE_HPP
#define ENTITY_CACHE_HPP

#include <vector>
#include <unordered_map>
#include <mutex>

#include "core/vec.hpp"

#include "sdk/classes/entity.hpp"

// These are populated in src/hooks/frame_stage_notify.cpp
inline static std::recursive_mutex cache_mutex;
inline static std::unordered_map<enum class_id, std::vector<Entity*>> entity_cache;
inline static std::unordered_map<unsigned long, bool> friend_cache;

struct PickupItem {
  Vec3 location;
  float time;
};
inline static std::vector<PickupItem> pickup_item_cache;

#endif
