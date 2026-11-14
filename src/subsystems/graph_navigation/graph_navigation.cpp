#include "subsystems/graph_navigation/navmesh.hpp"

#include "micropather/micropather.h"
#include "subsystems/graph_navigation/nav_graph.hpp"

#include "core/math.hpp"

#include "gui/config.hpp"

#include "sdk/interfaces/client.hpp"
#include "sdk/interfaces/entity_list.hpp"

#include "sdk/classes/player.hpp"
#include "sdk/classes/capture_flag.hpp"
#include "sdk/classes/team_objective_resource.hpp"

#include "core/entity_cache.hpp"

bool make_path(unsigned int area_start_id, unsigned int area_end_id) {
  path.path_ids.clear();
  path.next_index = 0;
  if (!area_start_id || !area_end_id || area_start_id == area_end_id) return false;
  
  Area* a = mesh.id_to_area(area_start_id);
  Area* b = mesh.id_to_area(area_end_id);
  if (a == nullptr || b == nullptr) return false;
  
  static micropather::MicroPather* path_solver = new micropather::MicroPather(new NavGraph());
  MP_VECTOR<void*> states;
  float cost = 0.0f;
  int rc = path_solver->Solve((void*)a, (void*)b, &states, &cost);
  if (rc == micropather::MicroPather::SOLVED || rc == micropather::MicroPather::START_END_SAME) {
    path.path_ids.reserve(states.size());
    for (void* s : states) {
      path.path_ids.push_back(((Area*)s)->id);
    }
  }

  if (!path.path_ids.empty() && path.path_ids[0] == area_start_id) {
    path.next_index = 1;
  }
  
  return !path.path_ids.empty();
}

enum class game_mode {
  UNKNOWN,
  CTF,
  CP,
  PL,
  PLR,
  KOTH
};

enum game_mode determine_game_mode(const char* level_path) {
  if (level_path == nullptr) return game_mode::UNKNOWN;

  std::string game_mode_name = std::string(level_path);
  game_mode_name = game_mode_name.substr(game_mode_name.find_last_of('/')+1);
  game_mode_name.resize(game_mode_name.find('_'));    

  if (game_mode_name == "ctf") {
    return game_mode::CTF;
  } else if (game_mode_name == "cp") {
    return game_mode::CP;
  } else if (game_mode_name == "pl") {
    return game_mode::PL;
  } else if (game_mode_name == "plr") {
    return game_mode::PLR;
  } else if (game_mode_name == "koth") {
    return game_mode::KOTH;
  }

  return game_mode::UNKNOWN;
}

Vec3 ctf_target_location(Player* localplayer) {
  std::lock_guard<std::recursive_mutex> lock(cache_mutex);
  Vec3 target_location;

  // Locate which flag to travel to
  CaptureFlag* enemy_intelligence = nullptr;
  CaptureFlag* team_intelligence = nullptr;

  for (unsigned int i = 0; i < entity_cache[class_id::CAPTURE_FLAG].size(); ++i) {
    Entity* entity = entity_cache[class_id::CAPTURE_FLAG].at(i);
    if (entity == nullptr) continue;

    CaptureFlag* temp_flag = (CaptureFlag*)entity;
      
    // Target Enemy Flag
    if (entity->get_team() != localplayer->get_team())  {
      enemy_intelligence = temp_flag;
      continue;
    }

    // Returning the Flag to our own
    if (entity->get_team() == localplayer->get_team()) {
      team_intelligence = temp_flag;
      continue;
    }
      
  }

  CaptureFlag* target_intelligence = nullptr;
  
  if (enemy_intelligence != nullptr && team_intelligence != nullptr) {

    if (team_intelligence->get_owner_entity() != nullptr || team_intelligence->get_status() == flag_status::DROPPED) { // Defend allied objective if taken
      target_intelligence = team_intelligence;
    } else if (enemy_intelligence->get_owner_entity() != localplayer) { // Target opposing objective if not held
      target_intelligence = enemy_intelligence;
    } else if (enemy_intelligence->get_owner_entity() == localplayer && team_intelligence->get_status() == flag_status::HOME) { // Return opposing objective
      target_intelligence = team_intelligence;
    } 

    if (target_intelligence != nullptr)
      target_location = target_intelligence->get_origin();
  }

  return target_location;
}

Vec3 koth_target_location(Player* localplayer) {
  std::lock_guard<std::recursive_mutex> lock(cache_mutex);
  Vec3 target_location;
  
  TeamObjectiveResource* objective_resource = nullptr;
  
  for (unsigned int i = 0; i < entity_cache[class_id::OBJECTIVE_RESOURCE].size(); ++i) {
    Entity* entity = entity_cache[class_id::OBJECTIVE_RESOURCE].at(i);
    if (entity == nullptr) continue;

    objective_resource = (TeamObjectiveResource*)entity;
    break;
  }

  if (objective_resource == nullptr) {
    return target_location;
  }
  
  if (objective_resource->is_locked(0) == false && objective_resource->can_team_capture(0, localplayer->get_team()) == true) {
    target_location = objective_resource->get_origin(0);
  } 
  
  return target_location;
}

void run_graph_navigation(user_cmd* user_cmd, Vec3 original_view_angles) {
  if (config.navigation.master == false) {
    path = Path{};
    return;
  }
  
  Player* localplayer = entity_list->get_localplayer();
  if (localplayer == nullptr) {
    path = Path{};
    return;
  }
  
  if (localplayer->get_lifestate() != 1) {
    path = Path{};
    return;
  }
  
  Vec3 location = localplayer->get_origin();
  Vec3 target_location;
  
  Area* from_area = mesh.best_area_from_xyz(location);
  if (from_area == nullptr)
    from_area = mesh.find_nearest_area_2d(target_location); // Less accurate fallback
  
  // Determine a target world origin depending on game mode.
  switch (determine_game_mode(engine->get_level_name())) {
  case game_mode::CTF:
    target_location = ctf_target_location(localplayer);
    break;

  case game_mode::KOTH:
    target_location = koth_target_location(localplayer);
    break;
    
  default:
    if (from_area != nullptr) {
      int far_id = mesh.pick_far_goal_from_here(from_area, location.x, location.y, location.z);
      Area* to_area = mesh.id_to_area(far_id);
      if (to_area != nullptr)
	target_location = to_area->center();
    }
  }
  
  Area* new_target_area = mesh.best_area_from_xyz(target_location);
  if (new_target_area == nullptr)
    new_target_area = mesh.find_nearest_area_2d(target_location); // Less accurate fallback
  
  // Create path to target location if we don't have one
  if (from_area != nullptr && new_target_area != nullptr && (path.goal_id == 0 || path.goal_id != new_target_area->id)) {    
    if(make_path(from_area->id, new_target_area->id)) {  
      path.goal_id = new_target_area->id;
    }
  }
  
  // Follow path to target location
  if (path.goal_id > 0 && !path.path_ids.empty()) {  
    Area* current_area = mesh.best_area_from_xyz(location);
    Area* next_area = mesh.id_to_area(path.path_ids[path.next_index]);
    
    // Completed path
    if (current_area != nullptr && current_area->id == path.goal_id) {
      path.goal_id = 0;
    }

    // Reached next consecutive node
    if (current_area != nullptr && next_area == current_area) {
      path.next_index++;
    }

    // Actuate locomotion to next consecutive node
    if (config.navigation.walk == true && next_area != nullptr) {
      Vec3 area_center = next_area->center();
      float x_diff = area_center.x - location.x;
      float y_diff = area_center.y - location.y;
      float dist_squared = x_diff*x_diff + y_diff*y_diff;

      float desired_yaw = std::atan2(y_diff, x_diff) * radpi;
      float current_yaw = original_view_angles.y;
      float delta_yaw   = azimuth_to_signed(desired_yaw - current_yaw);

      float dist = sqrt(dist_squared);
      float step = dist > 120.0f ? 120.0f : (dist > 60.0f ? 60.0f : 40.0f);
      float dirx = std::cos(std::atan2(y_diff, x_diff));
      float diry = std::sin(std::atan2(y_diff, x_diff));
      Vec3 ahead = Vec3{ location.x + dirx * step, location.y + diry * step, location.z };
      
      float speed = 350.0f;
      float fwd = std::cos(delta_yaw * pideg) * speed;
      float side = -std::sin(delta_yaw * pideg) * speed;
      
      user_cmd->forwardmove = clampf(fwd, -450.0f, 450.0f);
      user_cmd->sidemove = clampf(side, -450.0f, 450.0f);

      // Jump to reach node, if necessary
      if (next_area->center().z - location.z > kStepHeight + kZSlop) {
	user_cmd->buttons |= IN_DUCK;  
	if (localplayer->get_ground_entity()) {
	  user_cmd->buttons |= IN_JUMP;
	}
      }

      // Make view angles follow movement direction
      if (config.navigation.look_at_path == true && !(user_cmd->buttons & IN_ATTACK)) {
	float linear_interp = config.navigation.look_smoothness;
	if (config.navigation.look_smoothness <= 0) {
	  linear_interp = 1;
	}

	float new_yaw = azimuth_to_signed((delta_yaw/linear_interp) + current_yaw);
	user_cmd->view_angles.y = new_yaw;
	user_cmd->view_angles.x = 0.0;
      }
      
    }
  }

}
