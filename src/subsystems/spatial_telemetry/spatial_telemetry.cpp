export module subsystems.spatial_telemetry.spatial_telemetry;
export import std.compat;
export import core.sys;
export import core.entity_cache;
export import gui.config;
export import sdk.interfaces.debug_overlay;
export import sdk.classes.player;

export extern "C++" {





void draw_players_imgui() {
  std::lock_guard<std::recursive_mutex> lock(cache_mutex);
}

}
