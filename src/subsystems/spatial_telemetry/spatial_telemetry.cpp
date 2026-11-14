#include "imgui/imgui.h"

#include "core/entity_cache.hpp"

#include "gui/config.hpp"

#include "sdk/interfaces/debug_overlay.hpp"

#include "sdk/classes/player.hpp"

void draw_players_imgui() {
  std::lock_guard<std::recursive_mutex> lock(cache_mutex);
}
