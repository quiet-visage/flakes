export module subsystems.proximity_fuze.proximity_fuze;
export import std.compat;
export import core.sys;
export import subsystems.proximity_fuze.proximity_fuze_hpp;
export import core.math;
export import sdk.interfaces.client;
export import sdk.interfaces.entity_list;
export import sdk.interfaces.engine_trace;
export import sdk.interfaces.convar_system;
export import sdk.interfaces.global_vars;
export import gui.config;
export import sdk.classes.player;
export import sdk.classes.building;
export import sdk.classes.weapon;
export import core.entity_cache;

export extern "C++" {

inline Vec3 calc_angle_to(Vec3 start, Vec3 target) {
  Vec3 diff = {target.x - start.x, target.y - start.y, target.z - start.z};
  float yaw_hyp = sqrt((diff.x * diff.x) + (diff.y * diff.y));

  float pitch_angle = atan2(diff.z, yaw_hyp) * radpi;
  float yaw_angle = atan2(diff.y, diff.x) * radpi;

  return Vec3{
    .x = -pitch_angle,
    .y = yaw_angle,
    .z = 0.0f
  };
}

inline bool is_entity_visible_from_sticky(Entity* sticky, Entity* target, Vec3 target_pos) {
  Vec3 sticky_pos = sticky->get_origin();
  Vec3 start_pos = {sticky_pos.x, sticky_pos.y, sticky_pos.z + 4.0f};

  struct ray_t ray = engine_trace->init_ray(&start_pos, &target_pos);
  struct trace_filter filter;
  engine_trace->init_trace_filter(&filter, sticky);

  struct trace_t trace_world;
  unsigned int mask = CONTENTS_SOLID | CONTENTS_MOVEABLE | CONTENTS_WINDOW | CONTENTS_GRATE;
  engine_trace->trace_ray(&ray, mask, &filter, &trace_world);

  if (trace_world.entity == target || trace_world.fraction > 0.97f) {
    return true;
  }

  return false;
}

void run_proximity_fuze(user_cmd* user_cmd, Vec3 original_view_angles) {
  if (config.proximity_fuze.master == false) {
    return;
  }

  bool use_key = ((is_button_down(config.proximity_fuze.key) && config.proximity_fuze.use_key == true) ||
                  config.proximity_fuze.use_key == false);
  if (!use_key) {
    return;
  }

  Player* localplayer = entity_list->get_localplayer();
  if (localplayer == nullptr || localplayer->get_lifestate() != 1) {
    return;
  }

  if (localplayer->get_tf_class() != tf_class::DEMOMAN) {
    return;
  }

  std::lock_guard<std::recursive_mutex> lock(cache_mutex);

  if (entity_cache[class_id::PROJECTILE].empty()) {
    return;
  }

  bool friendlyfire = false;
  static Convar* ff_cvar = convar_system->find_var("mp_friendlyfire");
  if (ff_cvar != nullptr && ff_cvar->get_int() != 0) {
    friendlyfire = true;
  }

  for (Entity* projectile : entity_cache[class_id::PROJECTILE]) {
    if (projectile == nullptr || projectile->is_dormant()) {
      continue;
    }

    if (projectile->get_class_id() != class_id::PILL_OR_STICKY) {
      continue;
    }

    // Type 1 is remote-detonate sticky; Type 2 is Sticky Jumper (0 damage)
    if (projectile->get_pipebomb_type() != 1) {
      continue;
    }

    // Must belong to local player
    if (!projectile->is_owned_by(localplayer)) {
      continue;
    }

    // Arming delay validation (standard sticky: 0.70s; Scottish Resistance: 1.52s)
    if (config.proximity_fuze.check_arm_time) {
      float creation_time = projectile->get_pipebomb_creation_time();
      if (creation_time > 0.0f) {
        float arm_delay = projectile->is_pipebomb_defensive() ? 1.52f : 0.70f;
        if (global_vars->curtime - creation_time < arm_delay) {
          continue;
        }
      }
    }

    Vec3 sticky_pos = projectile->get_origin();
    bool should_detonate = false;

    // Check opposing players in range
    if (config.proximity_fuze.target_players && !entity_cache[class_id::PLAYER].empty()) {
      for (Entity* ent : entity_cache[class_id::PLAYER]) {
        Player* player = (Player*)ent;
        if (player == nullptr || player == localplayer || player->is_dormant()) {
          continue;
        }

        if (player->get_lifestate() != 1) {
          continue;
        }

        if (!friendlyfire && player->get_team() == localplayer->get_team()) {
          continue;
        }

        if (config.proximity_fuze.ignore_friends && player->is_friend()) {
          continue;
        }

        if (config.proximity_fuze.ignore_invulnerable && player->is_invulnerable()) {
          continue;
        }

        if (config.proximity_fuze.ignore_cloaked && player->is_cloaked()) {
          continue;
        }

        if (config.proximity_fuze.ignore_disguised && player->is_disguised()) {
          continue;
        }

        Vec3 player_center = player->get_origin() + Vec3{0.0f, 0.0f, 36.0f};
        float dist = distance_3d(sticky_pos, player_center);
        if (dist <= config.proximity_fuze.radius) {
          if (!config.proximity_fuze.check_visibility ||
              is_entity_visible_from_sticky(projectile, player, player_center) ||
              is_entity_visible_from_sticky(projectile, player, player->get_shoot_pos())) {
            should_detonate = true;
            break;
          }
        }
      }
    }

    // Check opposing buildings in range
    if (!should_detonate && config.proximity_fuze.target_buildings && !entity_cache[class_id::BUILDING].empty()) {
      for (Entity* ent : entity_cache[class_id::BUILDING]) {
        if (ent == nullptr || ent->is_dormant()) {
          continue;
        }

        if (!friendlyfire && ent->get_team() == localplayer->get_team()) {
          continue;
        }

        Building* building = (Building*)ent;
        if (building->is_carried() || building->get_health() <= 0) {
          continue;
        }

        Vec3 building_center = building->get_origin() + Vec3{0.0f, 0.0f, 30.0f};
        float dist = distance_3d(sticky_pos, building_center);
        if (dist <= config.proximity_fuze.radius) {
          if (!config.proximity_fuze.check_visibility ||
              is_entity_visible_from_sticky(projectile, building, building_center)) {
            should_detonate = true;
            break;
          }
        }
      }
    }

    if (should_detonate) {
      if (projectile->is_pipebomb_defensive()) {
        user_cmd->view_angles = calc_angle_to(localplayer->get_shoot_pos(), sticky_pos);
      }
      user_cmd->buttons |= IN_ATTACK2;
      return;
    }
  }
}

}
