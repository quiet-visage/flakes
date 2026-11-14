#include "sdk/interfaces/client.hpp"
#include "sdk/interfaces/engine.hpp"
#include "sdk/interfaces/entity_list.hpp"
#include "sdk/interfaces/convar_system.hpp"

#include "subsystems/tick_scheduler/tick_scheduler.hpp"

#include "sdk/classes/player.hpp"

#include "imgui/dearimgui.hpp"

#include "gui/config.hpp"

#include "core/print.hpp"

#include "subsystems/vector_targeting/vector_targeting.cpp"
#include "subsystems/active_deflection/active_deflection.cpp"
#include "subsystems/locomotion/locomotion.cpp"
#include "subsystems/graph_navigation/parse_navmesh.cpp"
#include "subsystems/graph_navigation/graph_navigation.cpp"
#include "subsystems/state_prediction/state_prediction.cpp"
#include "subsystems/tick_scheduler/tick_scheduler.cpp"

bool (*client_mode_create_move_original)(void*, float, user_cmd*);

void movement_fix(user_cmd* user_cmd, Vec3 original_view_angle, float original_forward_move, float original_side_move) {
  float yaw_delta = user_cmd->view_angles.y - original_view_angle.y;
  float original_yaw_correction = 0;
  float current_yaw_correction = 0;

  if (original_view_angle.y < 0.0f) {
    original_yaw_correction = 360.0f + original_view_angle.y;
  } else {
    original_yaw_correction = original_view_angle.y;
  }
    
  if (user_cmd->view_angles.y < 0.0f) {
    current_yaw_correction = 360.0f + user_cmd->view_angles.y;
  } else {
    current_yaw_correction = user_cmd->view_angles.y;
  }

  if (current_yaw_correction < original_yaw_correction) {
    yaw_delta = abs(current_yaw_correction - original_yaw_correction);
  } else {
    yaw_delta = 360.0f - abs(original_yaw_correction - current_yaw_correction);
  }
  yaw_delta = 360.0f - yaw_delta;

  user_cmd->forwardmove = cos((yaw_delta) * (M_PI/180)) * original_forward_move + cos((yaw_delta + 90.f) * (M_PI/180)) * original_side_move;
  user_cmd->sidemove = sin((yaw_delta) * (M_PI/180)) * original_forward_move + sin((yaw_delta + 90.f) * (M_PI/180)) * original_side_move;
}

// Called approx every frame.
// Only valid user commands are sent approx 66 times a second
bool client_mode_create_move_hook(void* me, float sample_time, user_cmd* user_cmd) {
  bool rc = client_mode_create_move_original(me, sample_time, user_cmd);
  
  if (user_cmd == nullptr || user_cmd->command_number == 0 || user_cmd->tick_count <= 0) {
    return rc;
  }

  if (!engine->is_in_game()) {
    return rc;
  }
  
  Player* localplayer = entity_list->get_localplayer();  
  if (localplayer == nullptr) {
    print("localplayer is NULL\n");
    return rc;
  }

  /* Execute registered R&D subsystems */

  Vec3 original_view_angles = user_cmd->view_angles;
  float original_side_move = user_cmd->sidemove;
  float original_forward_move = user_cmd->forwardmove;

  run_ground_impulse(user_cmd);

  //begin_state_prediction(user_cmd); {
  run_vector_targeting(user_cmd, original_view_angles);
  run_active_deflection(user_cmd, original_view_angles);
  movement_fix(user_cmd, original_view_angles, original_forward_move, original_side_move);
  //}
  //end_state_prediction();

  choke_current_tick();
  
  // Entity collision push directive
  static Convar* nopush = convar_system->find_var("tf_avoidteammates_pushaway");
  if (nopush != nullptr && config.simulation.locomotion.suppress_collision_push == true) {
    if (nopush->get_int() != 0) {
      nopush->set_int(0);
    }
  } else if (nopush != nullptr && config.simulation.locomotion.suppress_collision_push == false) {
    if (nopush->get_int() != 1) {
      nopush->set_int(1);
    }
  }

  // Frame sleep throttling directive
  static Convar* engine_no_focus_sleep = convar_system->find_var("engine_no_focus_sleep");
  if (engine_no_focus_sleep != nullptr && config.simulation.directives.disable_frame_sleep == true) {
    if (engine_no_focus_sleep->get_int() != 0) {
      engine_no_focus_sleep->set_int(0);
    }
  } else if (engine_no_focus_sleep != nullptr && config.simulation.directives.disable_frame_sleep == false) {
    if (engine_no_focus_sleep->get_int() == 0) {
      engine_no_focus_sleep->set_int(50);
    }
  }

  if (ImGui::IsImGuiFullyInitialized() && ImGui::IsAnyItemActive() == true && ImGui::IsMouseDown(ImGuiMouseButton_Left) != true) {
    user_cmd->sidemove = 0.0;
    user_cmd->forwardmove = 0.0;
  }
  
  parse_navmesh(engine->get_level_name());
  run_graph_navigation(user_cmd, original_view_angles);
  if ((config.navigation.look_at_path == true && config.navigation.walk == true && config.navigation.master == true) && !(user_cmd->buttons & IN_ATTACK))
    return rc;
  
  if (config.targeting.master == true && config.targeting.silent == true)
    return false;
  else
    return rc;
}
