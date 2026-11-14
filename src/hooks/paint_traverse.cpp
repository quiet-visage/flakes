#include "core/memory.hpp"
#include "subsystems/spatial_telemetry/spatial_telemetry.hpp"
#include "subsystems/spatial_telemetry/spatial_telemetry_player.cpp"
#include "subsystems/spatial_telemetry/spatial_telemetry_entity.cpp"

#include "subsystems/graph_navigation/draw_navmesh.cpp"

#include <cstring>
#include <unistd.h>
#include <wchar.h>
#include <string>

#include "gui/config.hpp"

#include "sdk/interfaces/engine.hpp"
#include "sdk/interfaces/surface.hpp"
#include "sdk/interfaces/entity_list.hpp"

#include "sdk/classes/player.hpp"
 
void (*paint_traverse_original)(void*, void*, bool, bool) = NULL;

void* vgui;
const char* get_panel_name(void* panel) {
    return mem::call_vfunc<const char*(void*)>(vgui, 37, panel);
}


void paint_traverse_hook(void* me, void* panel, bool force_repaint, bool allow_force) {
  std::string panel_name = get_panel_name(panel);

  // TEMP DEBUG
  static unsigned long dbg_calls = 0, dbg_top_hits = 0;
  static int dbg_last_alive = 0;
  ++dbg_calls;
  if (panel_name == "MatSystemTopPanel") ++dbg_top_hits;
  if (debug_every(&dbg_last_alive)) {
    print("[DBG paint] calls=%lu top_panel_hits=%lu last_panel=%s\n", dbg_calls, dbg_top_hits, panel_name.c_str());
  }

  // skip the original function to hide elements
  if (config.viewport.filters.scope == true && panel_name == "HudScope") {
    return;
  }


  /*
  if (panel_name == "ClassMenuSelect" ) {
    
    surface->set_cursor_visible(false);
    return;
  }
  */
  
  paint_traverse_original(me, panel, force_repaint, allow_force);

  //print("%s\n", panel_name.c_str());
  
  if (panel_name != "MatSystemTopPanel") {
    return;
  }
  
  if (!engine->is_in_game()) {
    return;
  }

  Vec2 screen_size = engine->get_screen_size();

  // TEMP DEBUG
  static int dbg_last_state = 0;
  if (debug_every(&dbg_last_state)) {
    Player* lp = entity_list->get_localplayer();
    int players = 0;
    for (unsigned int i = 1; i <= entity_list->get_max_entities(); ++i) {
      Player* p = entity_list->player_from_index(i);
      if (p != nullptr && p->get_class_id() == class_id::PLAYER) ++players;
    }
    print("[DBG paint] in_game screen=%dx%d targeting.master=%d draw_fov=%d fov=%.1f telemetry.master=%d enemy=%d box=%d name=%d\n",
          (int)screen_size.x, (int)screen_size.y, config.targeting.master, config.targeting.draw_fov, config.targeting.fov,
          config.telemetry.master, config.telemetry.player.enemy, config.telemetry.player.box, config.telemetry.player.name);
    print("[DBG paint] localplayer=%p max_ents=%d players=%d", (void*)lp, entity_list->get_max_entities(), players);
    if (lp != nullptr)
      print(" lp.lifestate=%d lp.team=%d lp.fov=%d lp.default_fov=%d lp.origin=(%.0f %.0f %.0f)",
            (int)lp->get_lifestate(), (int)lp->get_team(), lp->get_fov(), lp->get_default_fov(),
            lp->get_origin().x, lp->get_origin().y, lp->get_origin().z);
    print("\n");
  }

  static int old_font_height = config.diagnostics.font_height;
  static int old_font_weight = config.diagnostics.font_weight;
  
  if (telemetry_player_font == 0 || old_font_height != config.diagnostics.font_height || old_font_weight != config.diagnostics.font_weight) {
    telemetry_player_font = surface->text_create_font();
    surface->text_set_font_glyph_set(telemetry_player_font, "ProggySquare", config.diagnostics.font_height, config.diagnostics.font_weight, 0, 0, 0x0);
  }
  
  if (telemetry_entity_font == 0 || old_font_height != config.diagnostics.font_height || old_font_weight != config.diagnostics.font_weight) {
    old_font_height = config.diagnostics.font_height;
    old_font_weight = config.diagnostics.font_weight;
    telemetry_entity_font = surface->text_create_font();
    surface->text_set_font_glyph_set(telemetry_entity_font, "ProggySquare", config.diagnostics.font_height, config.diagnostics.font_weight, 0, 0, 0x0);
  }
  

  // Vector targeting FOV cone
  if (config.targeting.draw_fov == true && config.targeting.master == true && config.targeting.fov < 90) {
    Player* localplayer = entity_list->get_localplayer();

    //very poor practice.
    float local_fov = localplayer->get_fov();
    if (config.viewport.override_fov == true && (localplayer->is_scoped()) == false) {
      local_fov = config.viewport.custom_fov;
    }
    if (config.viewport.filters.zoom == true) {
      local_fov = localplayer->get_default_fov();
    }
    if (config.viewport.override_fov == true && config.viewport.filters.zoom == true) {
      local_fov = config.viewport.custom_fov;
    }
      
    
    int radius = (tan(config.targeting.fov / 180 * M_PI) / tan((local_fov / 2) / 180 * M_PI) * (float(screen_size.x) / 2))/1.35;

    surface->set_rgba(255, 255, 255, 255);
    surface->draw_circle(screen_size.x / 2, screen_size.y /2, radius, 55);
  }

  // Navmesh debug visual
  draw_navmesh();
  
  // Entity and Player spatial telemetry
  for (unsigned int i = 1; i <= entity_list->get_max_entities(); ++i) {
    if (config.telemetry.master == false) continue;

    Player* player = entity_list->player_from_index(i);
    if (player == nullptr) continue;

    if (player->get_class_id() == class_id::PLAYER) {
      surface->draw_set_text_font(telemetry_player_font);
      render_player_telemetry(i, player);
    } else {
      surface->draw_set_text_font(telemetry_entity_font);
      render_entity_telemetry(i, player->to_entity());
    }
  }

}
