#include "subsystems/active_deflection/active_deflection.hpp"

#include <cmath>

#include "core/math.hpp"

#include "sdk/interfaces/client.hpp"
#include "sdk/interfaces/entity_list.hpp"
#include "sdk/interfaces/engine_trace.hpp"
#include "sdk/interfaces/convar_system.hpp"

#include "gui/config.hpp"
#include "sdk/classes/player.hpp"
#include "sdk/classes/weapon.hpp"

#include "core/entity_cache.hpp"

static bool is_projectile_visible(Player* localplayer, Entity* projectile) {
  Vec3 start_pos = localplayer->get_shoot_pos();
  Vec3 target_pos = projectile->get_origin();

  struct ray_t ray = engine_trace->init_ray(&start_pos, &target_pos);
  struct trace_filter filter;
  engine_trace->init_trace_filter(&filter, localplayer);

  struct trace_t trace_world;
  unsigned int mask = CONTENTS_SOLID | CONTENTS_MOVEABLE | CONTENTS_WINDOW | CONTENTS_GRATE;
  engine_trace->trace_ray(&ray, mask, &filter, &trace_world);

  if (trace_world.entity == projectile || trace_world.fraction > 0.97f) {
    return true;
  }

  return false;
}

static bool is_projectile_type_enabled(projectile_type type) {
  switch (type) {
  case projectile_type::ROCKET:
    return config.deflection.projectiles.rockets;
  case projectile_type::GRENADE:
    return config.deflection.projectiles.grenades;
  case projectile_type::STICKY:
    return config.deflection.projectiles.stickies;
  case projectile_type::ARROW:
    return config.deflection.projectiles.arrows;
  case projectile_type::FLARE:
    return config.deflection.projectiles.flares;
  case projectile_type::THROWABLE:
    return config.deflection.projectiles.throwables;
  default:
    return false;
  }
}

void run_active_deflection(user_cmd* user_cmd, Vec3 original_view_angles) {
  if (config.deflection.master == false) {
    return;
  }

  bool use_key = ((is_button_down(config.deflection.key) && config.deflection.use_key == true) ||
                  config.deflection.use_key == false);
  if (!use_key) {
    return;
  }

  Player* localplayer = entity_list->get_localplayer();
  if (localplayer == nullptr || localplayer->get_lifestate() != 1) {
    return;
  }

  if (localplayer->get_tf_class() != tf_class::PYRO) {
    return;
  }

  Weapon* weapon = localplayer->get_weapon();
  if (weapon == nullptr || !weapon->can_deflect()) {
    return;
  }

  std::lock_guard<std::recursive_mutex> lock(cache_mutex);

  if (entity_cache[class_id::PROJECTILE].empty()) {
    return;
  }

  bool friendlyfire = false;
  static Convar* friendlyfirevar = convar_system->find_var("mp_friendlyfire");
  if (friendlyfirevar != nullptr && friendlyfirevar->get_int() != 0) {
    friendlyfire = true;
  }

  Vec3 eye_pos = localplayer->get_shoot_pos();
  Vec3 forward;
  angle_vectors(original_view_angles, &forward, nullptr, nullptr);

  // Full cone width is config.deflection.fov (default 90 deg -> 45 deg half-angle from forward axis)
  float half_cone_rad = (config.deflection.fov * 0.5f) * pideg;
  float min_cos = cosf(half_cone_rad);

  for (Entity* projectile : entity_cache[class_id::PROJECTILE]) {
    if (projectile == nullptr || projectile->is_dormant()) {
      continue;
    }

    tf_team proj_team = projectile->get_team();
    if (proj_team != tf_team::RED && proj_team != tf_team::BLU) {
      continue;
    }

    if (proj_team == localplayer->get_team() && !friendlyfire) {
      continue;
    }

    if (projectile->get_owner_entity() == localplayer) {
      continue;
    }

    projectile_type type = projectile->get_projectile_type();
    if (!is_projectile_type_enabled(type)) {
      continue;
    }

    Vec3 proj_pos = projectile->get_origin();
    float dist = distance_3d(eye_pos, proj_pos);
    if (dist > config.deflection.range) {
      continue;
    }

    if (dist > 0.001f) {
      Vec3 delta = proj_pos - eye_pos;
      float cos_angle = (forward.x * delta.x + forward.y * delta.y + forward.z * delta.z) / dist;
      if (cos_angle < min_cos) {
        continue;
      }
    }

    if (!is_projectile_visible(localplayer, projectile)) {
      continue;
    }

    user_cmd->buttons |= IN_ATTACK2;
    break;
  }
}
