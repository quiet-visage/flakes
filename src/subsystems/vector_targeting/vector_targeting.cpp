#include <algorithm>

#include "subsystems/vector_targeting/vector_targeting.hpp"
#include "subsystems/observer_telemetry/observer_telemetry.hpp"

#include "core/math.hpp"

#include "sdk/interfaces/client.hpp"
#include "sdk/interfaces/entity_list.hpp"
#include "sdk/interfaces/engine.hpp"
#include "sdk/interfaces/engine_trace.hpp"
#include "sdk/interfaces/convar_system.hpp"

#include "gui/config.hpp"
#include "sdk/classes/player.hpp"

#include "core/entity_cache.hpp"
#include "core/print.hpp"

bool is_player_visible(Player* localplayer, Player* entity, int bone) {
  Vec3 target_pos = entity->get_bone_pos(bone);
  Vec3 start_pos  = localplayer->get_shoot_pos();
  
  struct ray_t ray = engine_trace->init_ray(&start_pos, &target_pos);
  struct trace_filter filter;
  engine_trace->init_trace_filter(&filter, localplayer);

  struct trace_t trace_world;
  engine_trace->trace_ray(&ray, MASK_SHOT | CONTENTS_GRATE, &filter, &trace_world);

  wchar_t target_wname[32];
  entity->get_player_name(target_wname);
  char target_name[32];
  std::wcstombs(target_name, target_wname, 32);
  
  if (trace_world.entity == entity || trace_world.fraction > 0.97f /* magic number */) {
    return true;
  }

  return false;
}

void run_vector_targeting(user_cmd* user_cmd, Vec3 original_view_angles) {  
  if (config.targeting.master == false || (config.targeting.ignore_spectated == true && spectated_1st == true)) {
    target_player = nullptr;
    return;
  }

  std::lock_guard<std::recursive_mutex> lock(cache_mutex);

  static int dbg_last_aim = 0; // TEMP DEBUG
  bool dbg = debug_every(&dbg_last_aim);

  if (entity_cache[class_id::PLAYER].empty()) {
    if (dbg) print("[DBG targeting] bail: entity_cache[PLAYER] empty\n");
    target_player = nullptr;
    return;
  }
  
  Player* localplayer = entity_list->get_localplayer();
  if (localplayer->get_lifestate() != 1) {
    if (dbg) print("[DBG targeting] bail: localplayer lifestate=%d\n", (int)localplayer->get_lifestate());
    target_player = nullptr;
    return;
  }

  Weapon* weapon = localplayer->get_weapon();
  if (weapon == nullptr) {
    if (dbg) print("[DBG targeting] bail: weapon == nullptr\n");
    target_player = nullptr;
    return;    
  }

  if (dbg) print("[DBG targeting] running: cached_players=%zu use_key=%d key=%d\n",
                 entity_cache[class_id::PLAYER].size(), config.targeting.use_key, config.targeting.key.button);

  // Check if the target player is still even in the entity list
  if (std::find(entity_cache[class_id::PLAYER].begin(), entity_cache[class_id::PLAYER].end(), target_player) == entity_cache[class_id::PLAYER].end()) {
    target_player = nullptr;
  }
    
    
  if (target_player != nullptr && target_player->is_friend() && config.targeting.ignore_friends == true) {
    target_player = nullptr;
  }
  
  bool friendlyfire = false;
  static Convar* friendlyfirevar = convar_system->find_var("mp_friendlyfire");
  if (friendlyfirevar != nullptr) {
    if (friendlyfirevar->get_int() != 0) {
      friendlyfire = true;
    }
  }

  float smallest_fov_angle = __FLT_MAX__;
  float smallest_distance  = __FLT_MAX__;
  int   smallest_health    = INT_MAX;

  int   largest_health = INT_MIN;
  
  for (unsigned int i = 0; i < entity_cache[class_id::PLAYER].size(); ++i) {
    Player* player = (Player*)entity_cache[class_id::PLAYER].at(i);
    
    if (player == nullptr                                                        ||
	player == localplayer                                                    ||
	player->is_dormant() == true                                             || 
	(player->get_team() == localplayer->get_team() && friendlyfire == false) ||
	player->get_lifestate() != 1                                             ||
	player->is_invulnerable() == true                                        ||
	(config.targeting.ignore_friends == true && player->is_friend()))
      {
	continue;
      }
    
    int bone = player->get_tf_class() == tf_class::ENGINEER ? 5 : 2; // Target body by default

    // Target head if it yields higher damage
    if (localplayer->get_tf_class() == tf_class::SNIPER) {
      if (localplayer->is_scoped() && player->get_health() > 50)
	bone = player->get_head_bone();
    } else if (localplayer->get_tf_class() == tf_class::SPY) {
      if (weapon->is_headshot_weapon())
	bone = player->get_head_bone();
    }
    
    Vec3 diff = {player->get_bone_pos(bone).x - localplayer->get_shoot_pos().x,
		 player->get_bone_pos(bone).y - localplayer->get_shoot_pos().y,
		 player->get_bone_pos(bone).z - localplayer->get_shoot_pos().z};
      
    float yaw_hyp = sqrt((diff.x * diff.x) + (diff.y * diff.y));

    float pitch_angle = atan2(diff.z, yaw_hyp) * radpi;
    float yaw_angle = atan2(diff.y, diff.x) * radpi;

    Vec3 view_angles = {
      .x = -pitch_angle,
      .y = yaw_angle,
      .z = 0
    };
      
    float x_diff = view_angles.x - original_view_angles.x;
    float y_diff = view_angles.y - original_view_angles.y;

    float x = remainderf(x_diff, 360.0f);
    float y = remainderf(y_diff, 360.0f);

    float clamped_x = x > 89.0f ? 89.0f : x < -89.0f ? -89.0f : x;
    float clamped_y = y > 180.0f ? 180.0f : y < -180.0f ? -180.0f : y;

    bool visible = is_player_visible(localplayer, player, bone);

    
    float fov = hypotf(clamped_x, clamped_y);
    float distance = distance_3d(localplayer->get_origin(), player->get_origin());
    int   health = player->get_health();

    if (config.targeting.target_type == VectorTargetingConfig::TargetType::FOV) {
      if (visible == true && fov <= config.targeting.fov && fov < smallest_fov_angle) {
	target_player = player;
	smallest_fov_angle = fov;
      }
    } else if (config.targeting.target_type == VectorTargetingConfig::TargetType::DISTANCE) {
      if (visible == true && fov <= config.targeting.fov && distance < smallest_distance) {
	target_player = player;
	smallest_fov_angle = fov;
	smallest_distance = distance;
      }
    } else if (config.targeting.target_type == VectorTargetingConfig::TargetType::LEAST_HEALTH) {
      if (visible == true && fov <= config.targeting.fov && health < smallest_health) {
	target_player = player;
	smallest_fov_angle = fov;
	smallest_health = health;
      }
    } else if (config.targeting.target_type == VectorTargetingConfig::TargetType::MOST_HEALTH) {
      if (visible == true && fov <= config.targeting.fov && health > largest_health) {
	target_player = player;
	smallest_fov_angle = fov;
        largest_health = health;
      }    
    }
    
    if (target_player == player && (fov > config.targeting.fov || visible == false))
      target_player = nullptr;


    bool scoped_only = ((config.targeting.scoped_only == true && weapon->is_sniper_rifle() && localplayer->is_scoped()) || config.targeting.scoped_only == false || !weapon->is_sniper_rifle());
    bool use_key = ((is_button_down(config.targeting.key) && config.targeting.use_key == true) || config.targeting.use_key == false);

    if (use_key == true && config.targeting.auto_shoot == true && target_player == player && localplayer->can_shoot(target_player)) {
      
      if (config.targeting.auto_scope == true && localplayer->get_tf_class() == tf_class::SNIPER && weapon->is_sniper_rifle() && !localplayer->is_scoped() && weapon->can_primary_attack() && localplayer->get_ground_entity() != nullptr)
	user_cmd->buttons |= IN_ATTACK2;

      if (!(user_cmd->buttons & IN_ATTACK2) && scoped_only == true)
	user_cmd->buttons |= IN_ATTACK;
    }

    if (config.targeting.auto_unscope == true && (localplayer->get_tickbase() * TICK_INTERVAL) - localplayer->get_fov_time() >= 1 && localplayer->get_tf_class() == tf_class::SNIPER && localplayer->is_scoped()) {
      user_cmd->buttons |= IN_ATTACK2;
    }
    
    if (use_key == true && weapon->can_primary_attack() && scoped_only == true && target_player == player)
      user_cmd->view_angles = (view_angles - localplayer->get_punch_angles());

  }
}
