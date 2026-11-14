export module hooks.fire_event_client_side;
export import std.compat;
export import core.sys;
export import sdk.interfaces.game_event_manager;
export import sdk.classes.player;
export import subsystems.graph_navigation.navmesh;
export import subsystems.hardware_bridge.hardware_bridge_hpp;
export import core.print;

export extern "C++" {







enum BonusEffect {
  CRIT,
  MINI_CRIT,
  DOUBLE_DONK,
  WATER_BALLOON_SPLOOSH,
  NONE,
  DRAGONS_FURY,
  STOMP,
  COUNT
};

inline bool (*fire_event_client_side_original)(void*, GameEvent*) = nullptr;

inline bool fire_event_client_side_hook(void* me, GameEvent* event) {

  std::string event_name = std::string(event->get_name());

  // Force repath if we respawn
  if (event_name == "localplayer_respawn") {
    path = Path{};
  }
  
  if (event_name == "teamplay_round_win") {
    
  }

  if (event_name == "item_pickup") {
    Player* obtainer = entity_list->get_player_from_id(event->get_int("userid"));
    if (obtainer != nullptr && !obtainer->is_dormant()) {
      const char* item_name = event->get_string("item");
      if (strstr(item_name, "medkit") || strstr(item_name, "ammopack")) {
	std::lock_guard<std::recursive_mutex> lock(cache_mutex);
	float previous = FLT_MAX;
	Entity* obtained_entity = nullptr;
	if (strstr(item_name, "medkit")) {
	  for (Entity* pickup : entity_cache[class_id::HEALTH_PACK]) {
	    float distance = distance_3d(obtainer->get_origin(), pickup->get_origin());
	    if (distance < previous) {
	      previous = distance;
	      obtained_entity = pickup;
	    }
	  }
	} else if (strstr(item_name, "ammopack")) {
	  for (Entity* pickup : entity_cache[class_id::AMMO]) {
	    float distance = distance_3d(obtainer->get_origin(), pickup->get_origin());
	    if (distance < previous) {
	      previous = distance;
	      obtained_entity = pickup;
	    }
	  }
	}

	if (obtained_entity != nullptr)
	  pickup_item_cache.push_back(PickupItem{obtained_entity->get_origin(), global_vars->curtime + 10});
      }
    }
  }

  if (event_name == "player_hurt") {
    Player* victim = entity_list->get_player_from_id(event->get_int("userid"));
    Player* attacker = entity_list->get_player_from_id(event->get_int("attacker"));
    Player* localplayer = entity_list->get_localplayer();
    
    if (victim != nullptr && attacker != nullptr) {
      if (config.diagnostics.bridge_on_kill == true && event->get_int("health") <= 0 && attacker == localplayer) {
	if (event->get_int("bonuseffect") == CRIT)
	  dispatch_hardware_telemetry(config.diagnostics.intensity + 20);
	else 
	  dispatch_hardware_telemetry();
      }

      if (config.diagnostics.bridge_on_death == true && event->get_int("health") <= 0 && victim == localplayer) {
	if (event->get_int("bonuseffect") == CRIT)
	  dispatch_hardware_telemetry(config.diagnostics.intensity + 30);
	else 
	  dispatch_hardware_telemetry();
      }
    }
  }
  
  return fire_event_client_side_original(me, event);
}

}
