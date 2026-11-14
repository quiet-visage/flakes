export module subsystems.spatial_telemetry.spatial_telemetry_player;
export import std.compat;
export import core.sys;
export import subsystems.spatial_telemetry.spatial_telemetry_hpp;
export import gui.config;
export import core.vec;
export import sdk.classes.player;
export import core.math;
export import subsystems.vector_targeting.vector_targeting_hpp;
export import sdk.interfaces.surface;
export import sdk.interfaces.render_view;
export import sdk.interfaces.entity_list;
export import sdk.interfaces.engine;
export import sdk.interfaces.convar_system;
export import core.print;

export extern "C++" {







inline constexpr auto WIDTH_FRACTION = 0.35;

void draw_player_bounding_box(Vec3 screen, Vec3 screen_offset, Player* player, Player* localplayer) {
  if (config.telemetry.player.box == true) {
    RGBA color = config.telemetry.player.enemy_color.to_RGBA();
    if (player->get_team() == localplayer->get_team()) color = config.telemetry.player.team_color.to_RGBA();
    if (config.telemetry.player.friends == true && player->is_friend() == true) color = config.telemetry.player.friend_color.to_RGBA();
    
    draw_outline_rectangle(screen, screen_offset, WIDTH_FRACTION, color);
  }
}

void draw_player_vitality_bar(Vec3 screen, Vec3 screen_offset, Player* player) {
  if (config.telemetry.player.health_bar == true) {
    float health_offset = (screen.y - screen_offset.y)*WIDTH_FRACTION;

    //shadow
    surface->set_rgba(0, 0, 0, 255);
    surface->draw_line(screen.x - health_offset - 5, screen.y + 1, screen.x - health_offset - 5, screen_offset.y - 2);
    surface->draw_line(screen.x - health_offset - 4, screen.y + 1, screen.x - health_offset - 4, screen_offset.y - 2);
    surface->draw_line(screen.x - health_offset - 3, screen.y + 1, screen.x - health_offset - 3, screen_offset.y - 2);

    surface->set_rgba(0, 255, 0, 255);
    int ydelta = (screen_offset.y - screen.y) * (1.f - (float(player->get_health()) / player->get_max_health()));

    if (player->get_health() > player->get_max_health()) { // over healed
      surface->set_rgba(0, 255, 255, 255);
      ydelta = 0;
    }
    else if (player->get_health() <= player->get_max_health() && player->get_health() >= (player->get_max_health()*.9))
      surface->set_rgba(0, 255, 0, 255);
    else if (player->get_health() < (player->get_max_health()*.9) && player->get_health() > (player->get_max_health()*.6))
      surface->set_rgba(90, 255, 0, 255);
    else if (player->get_health() <= (player->get_max_health()*.6) && player->get_health() > (player->get_max_health()*.35))
      surface->set_rgba(255, 100, 0, 255);
    else if (player->get_health() <= (player->get_max_health()*.35))
      surface->set_rgba(255, 0, 0, 255);
  
    surface->draw_line(screen.x - health_offset - 4, screen.y, screen.x - health_offset - 4, screen_offset.y - ydelta - 1);
  }
}

void draw_player_identifier(Vec3 screen, Vec3 screen_offset, Player* player, unsigned int i) {
  if (config.telemetry.player.name == true) {

    wchar_t name[32];
    size_t len = player->get_player_name(name);

    unsigned int name_width = surface->get_string_width(telemetry_player_font, name);
    
    surface->draw_set_text_color(255, 255, 255, 255);
    surface->draw_set_text_pos(screen.x - (name_width/2.f), screen_offset.y - surface->get_font_height(telemetry_player_font));  

    surface->draw_print_text(name, wcslen(name));
  }
}

void draw_player_annotations(Vec3 screen, Vec3 screen_offset, Player* player, unsigned int i) {
  float flags_x_offset = (screen.y - screen_offset.y)*WIDTH_FRACTION;
  float flags_y_offset = 0;
    
  if (config.telemetry.player.flags.target_indicator == true && player == target_player) {
    surface->draw_set_text_color(255, 0, 0, 255);
    surface->draw_set_text_pos(screen.x + flags_x_offset + surface->get_character_width(telemetry_player_font, L"TARGET"[0]), screen_offset.y + flags_y_offset);

    surface->draw_print_text(L"TARGET", wcslen(L"TARGET"));

    flags_y_offset += surface->get_font_height(telemetry_player_font);
  }

  if (config.telemetry.player.flags.friend_indicator == true && player->is_friend()) {
    surface->draw_set_text_color(0, 220, 80, 255);
    surface->draw_set_text_pos(screen.x + flags_x_offset + surface->get_character_width(telemetry_player_font, L"FRIEND"[0]), screen_offset.y + flags_y_offset);

    surface->draw_print_text(L"FRIEND", wcslen(L"FRIEND"));

    flags_y_offset += surface->get_font_height(telemetry_player_font);
  }

  if (config.telemetry.player.flags.scoped_indicator == true && player->is_scoped()) {
    surface->draw_set_text_color(0, 220, 80, 255);
    surface->draw_set_text_pos(screen.x + flags_x_offset + surface->get_character_width(telemetry_player_font, L"ZOOM"[0]), screen_offset.y + flags_y_offset);

    surface->draw_print_text(L"ZOOM", wcslen(L"ZOOM"));

    flags_y_offset += surface->get_font_height(telemetry_player_font);    
  }

  if (config.diagnostics.show_active_flag_ids_of_players == true) {

    for (unsigned int i = 0; i < TF_COND_LAST; ++i) { 
      if(player->in_cond((tf_cond)i) == true) {
	std::wstring cond_number = std::to_wstring(i);
	
	surface->draw_set_text_color(255, 225, 255, 255);
	surface->draw_set_text_pos(screen.x + flags_x_offset + surface->get_character_width(telemetry_player_font, cond_number[0]), screen_offset.y + flags_y_offset);

	surface->draw_print_text(cond_number.c_str(), wcslen(cond_number.c_str()));

	flags_y_offset += surface->get_font_height(telemetry_player_font);      
      }        
    }

  }  
}

void draw_player_skeleton(Player* player) {
  //todo
}

void render_player_telemetry(unsigned int i, Player* player) {

  bool friendlyfire = false;
  static Convar* friendlyfirevar = convar_system->find_var("mp_friendlyfire");
  if (friendlyfirevar != nullptr) {
    if (friendlyfirevar->get_int() != 0) {
      friendlyfire = true;
    }
  }
  
  Player* localplayer = entity_list->get_localplayer();
  
  if (player == localplayer) return; // Ignore Local Player
  if (player->is_dormant()) return; // Ignore Dormant
  if (player->get_lifestate() != 1) return; // Ignore Dead
  if (player->get_team() != localplayer->get_team() && config.telemetry.player.enemy == false && !player->is_friend()) return; // Ignore Enemy
  if (player->get_team() == localplayer->get_team() && config.telemetry.player.team == false && !player->is_friend()) return; // Ignore Team
  if (player->is_friend() && config.telemetry.player.friends == false && (config.telemetry.player.team == false && player->get_team() == localplayer->get_team())) return; // Ignore Friends 
  
  Vec3 location = player->get_origin();
  Vec3 screen;
  if (!render_view->world_to_screen(&location, &screen)) return;

  float distance = distance_3d(localplayer->get_origin(), player->get_origin());    

  Vec3 location_offset = {location.x, location.y, player->get_bone_pos(player->get_head_bone()).z + 10};
  Vec3 screen_offset;
  render_view->world_to_screen(&location_offset, &screen_offset);

  draw_player_bounding_box(screen, screen_offset, player, localplayer);
  draw_player_vitality_bar(screen, screen_offset, player);
  draw_player_annotations(screen, screen_offset, player, i);
  draw_player_identifier(screen, screen_offset, player, i);
  draw_player_skeleton(player);
}

}
