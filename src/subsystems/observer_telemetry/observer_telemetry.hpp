export module subsystems.observer_telemetry.observer_telemetry;
export import std.compat;
export import core.sys;
export import gui.config;
export import sdk.interfaces.engine;
export import sdk.interfaces.entity_list;
export import sdk.interfaces.global_vars;
export import sdk.classes.player;

export extern "C++" {

inline bool spectated_1st = false;

inline void draw_observer_telemetry(void) {
  spectated_1st = false;

  if (config.telemetry.observer_overlay == false) {
    return;
  }

  if (engine == nullptr || !engine->is_in_game()) {
    return;
  }

  Player* localplayer = entity_list->get_localplayer();
  if (localplayer == nullptr) {
    return;
  }

  Entity* target = (localplayer->get_lifestate() == 1)
                     ? localplayer->to_entity()
                     : localplayer->get_observer_target();
  if (target == nullptr) {
    return;
  }

  Vec2 screen_size = engine->get_screen_size();
  float x = 10.0f;
  float y = screen_size.y * 0.5f;
  float line_height = ImGui::GetTextLineHeightWithSpacing();

  ImDrawList* draw_list = ImGui::GetBackgroundDrawList();

  int max_clients = (global_vars != nullptr && global_vars->max_clients > 0)
                      ? std::min(global_vars->max_clients, 64)
                      : 64;

  for (int i = 1; i <= max_clients; ++i) {
    Player* player = entity_list->player_from_index(i);
    if (player == nullptr) {
      continue;
    }

    if (player == localplayer || player->to_entity() == target) {
      continue;
    }

    if (player->is_dormant()) {
      continue;
    }

    if (player->get_lifestate() == 1) {
      continue;
    }

    observer_mode_t obs_mode = player->get_observer_mode();
    if (obs_mode == OBS_MODE_NONE) {
      continue;
    }

    if (player->get_observer_target() != target) {
      continue;
    }

    const char* prefix = "[1st]";
    switch (obs_mode) {
    case OBS_MODE_IN_EYE:
    default:
      prefix = "[1st]";
      spectated_1st = true;
      break;
    case OBS_MODE_CHASE:
      prefix = "[3rd]";
      break;
    case OBS_MODE_ROAMING:
      prefix = "[Free]";
      break;
    case OBS_MODE_FREEZECAM:
      prefix = "[Freeze]";
      break;
    case OBS_MODE_DEATHCAM:
      prefix = "[Death]";
      break;
    case OBS_MODE_FIXED:
      prefix = "[Fixed]";
      break;
    case OBS_MODE_POI:
      prefix = "[Poi]";
      break;
    }

    player_info pinfo;
    if (!engine->get_player_info(i, &pinfo)) {
      continue;
    }

    char line_buf[96];
    std::snprintf(line_buf, sizeof(line_buf), "%s %s", prefix, pinfo.name);

    ImU32 text_color = (obs_mode == OBS_MODE_IN_EYE)
                         ? IM_COL32(255, 110, 110, 255)
                         : IM_COL32(235, 240, 245, 255);

    draw_list->AddText(ImVec2(x + 1.0f, y + 1.0f), IM_COL32(0, 0, 0, 220), line_buf);
    draw_list->AddText(ImVec2(x, y), text_color, line_buf);
    y += line_height;
  }
}

}
