#include "subsystems/material_override/material_override.hpp"

#include "gui/config.hpp"

#include "sdk/interfaces/entity_list.hpp"
#include "sdk/interfaces/material_system.hpp"
#include "sdk/interfaces/render_view.hpp"
#include "sdk/interfaces/model_render.hpp"

#include "sdk/classes/player.hpp"

void override_player_material(Player* player, void* me, void* state, ModelRenderInfo* pinfo, VMatrix* bone_to_world) {

  Player* localplayer = entity_list->get_localplayer();
  
  if (player == localplayer && config.materials.player.local == false) DME_RETURN; // Ignore Local Player
  if (player->is_dormant()) DME_RETURN;
  if (player->get_lifestate() != 1) DME_RETURN; // Ignore Dead
  if (player->get_team() != localplayer->get_team() && config.materials.player.enemy == false && !player->is_friend()) DME_RETURN; // Ignore Enemy
  if (player->get_team() == localplayer->get_team() && player != localplayer && config.materials.player.team == false && !player->is_friend()) DME_RETURN; // Ignore Team
  if (player->is_friend() && config.materials.player.friends == false && (config.materials.player.team == false && player->get_team() == localplayer->get_team())) DME_RETURN; // Ignore Friends

  RenderContext* render_context = material_system->get_render_context();

  float old_invis = player->get_invisibility();  
    
  RGBA_float color;
  RGBA_float color_z;
  RGBA_float color_overlay;  
  RGBA_float color_z_overlay;
  bool ignore_z = false;
  bool ignore_z_overlay = false;
  bool wireframe = false;
  bool wireframe_overlay = false;
  Material* material = nullptr;
  Material* material_z = nullptr;
  Material* material_overlay = nullptr;
  Material* material_z_overlay = nullptr;
  
  if (player->get_team() != localplayer->get_team()) {
    color = config.materials.player.enemy_color;
    color_z = config.materials.player.enemy_color_z;
    ignore_z = config.materials.player.enemy_flags.ignore_z;
    wireframe = config.materials.player.enemy_flags.wireframe;
    material = materials.at((int)config.materials.player.enemy_material_type);
    material_z = materials.at((int)config.materials.player.enemy_material_z_type);

    color_overlay = config.materials.player.enemy_overlay_color;
    color_z_overlay = config.materials.player.enemy_overlay_color_z;    
    ignore_z_overlay = config.materials.player.enemy_overlay_flags.ignore_z;    
    wireframe_overlay = config.materials.player.enemy_overlay_flags.wireframe;    
    material_overlay = materials.at((int)config.materials.player.enemy_overlay_material_type);
    material_z_overlay = materials.at((int)config.materials.player.enemy_overlay_material_z_type);
  }
  if (player->get_team() == localplayer->get_team()) {
    color = config.materials.player.team_color;
    color_z = config.materials.player.team_color_z;
    ignore_z = config.materials.player.team_flags.ignore_z;
    wireframe = config.materials.player.team_flags.wireframe;
    material = materials.at((int)config.materials.player.team_material_type);
    material_z = materials.at((int)config.materials.player.team_material_z_type);
  }
  if (player->is_friend()) {
    color = config.materials.player.friend_color;
    color_z = config.materials.player.friend_color_z;
    ignore_z = config.materials.player.friends_flags.ignore_z;
    wireframe = config.materials.player.friends_flags.wireframe;
    material = materials.at((int)config.materials.player.friend_material_type);
    material_z = materials.at((int)config.materials.player.friend_material_z_type);
  }
  if (player == localplayer) {
    color = config.materials.player.local_color;
    color_z = RGBA_float{};
    ignore_z = false;
    wireframe = config.materials.player.local_flags.wireframe;
    material = materials.at((int)config.materials.player.local_material_type);
    material_z = material;
  }


  /* Primary Shader Pass */
  if (ignore_z == true && material_z != nullptr) {
    render_context->set_stencil_enable(true);

    render_context->clear_buffers(false, false, false);
    render_context->set_stencil_compare_mode(STENCILCOMPARISONFUNCTION_ALWAYS);
    render_context->set_stencil_pass_mode(STENCILOPERATION_REPLACE);
    render_context->set_stencil_fail_mode(STENCILOPERATION_KEEP);
    render_context->set_stencil_zfail_mode(STENCILOPERATION_KEEP);
    render_context->set_stencil_reference_count(1);
    render_context->set_stencil_write_mask(0xFF);
    render_context->set_stencil_test_mask(0x0);
  }

  if (material != nullptr) {
    // Two-pass depth write + normal pass so translucent materials occlude props cleanly.
    set_material_information(material, color, wireframe, false, OVERRIDE_DEPTH_WRITE);
    draw_model_execute_original(me, state, pinfo, bone_to_world);
    set_material_information(material, color, wireframe, false, OVERRIDE_NORMAL);
    draw_model_execute_original(me, state, pinfo, bone_to_world);
  } else {
    draw_model_execute_original(me, state, pinfo, bone_to_world);
  }
  
  if (ignore_z == true && material_z != nullptr) {
    render_context->clear_buffers(false, false, false);
    render_context->set_stencil_compare_mode(STENCILCOMPARISONFUNCTION_EQUAL);
    render_context->set_stencil_pass_mode(STENCILOPERATION_KEEP);
    render_context->set_stencil_fail_mode(STENCILOPERATION_KEEP);
    render_context->set_stencil_zfail_mode(STENCILOPERATION_KEEP);
    render_context->set_stencil_reference_count(0);
    render_context->set_stencil_write_mask(0x0);
    render_context->set_stencil_test_mask(0xFF);
    render_context->set_depth_range(0, 0.2);

    set_material_information(material_z, color_z, wireframe, true);
    draw_model_execute_original(me, state, pinfo, bone_to_world);
    
    render_context->set_stencil_enable(false);
    render_context->set_depth_range(0, 1);
  }

  /* Secondary Overlay Pass */
  if (ignore_z_overlay == true && material_z_overlay != nullptr) {
    render_context->set_stencil_enable(true);

    render_context->clear_buffers(false, false, false);
    render_context->set_stencil_compare_mode(STENCILCOMPARISONFUNCTION_ALWAYS);
    render_context->set_stencil_pass_mode(STENCILOPERATION_REPLACE);
    render_context->set_stencil_fail_mode(STENCILOPERATION_KEEP);
    render_context->set_stencil_zfail_mode(STENCILOPERATION_KEEP);
    render_context->set_stencil_reference_count(1);
    render_context->set_stencil_write_mask(0xFF);
    render_context->set_stencil_test_mask(0x0);
  }
  
  if (material_overlay != nullptr) {
    set_material_information(material_overlay, color_overlay, wireframe_overlay);
    draw_model_execute_original(me, state, pinfo, bone_to_world); 
  }

  if (ignore_z_overlay == true && material_z_overlay != nullptr) {
    render_context->clear_buffers(false, false, false);
    render_context->set_stencil_compare_mode(STENCILCOMPARISONFUNCTION_EQUAL);
    render_context->set_stencil_pass_mode(STENCILOPERATION_KEEP);
    render_context->set_stencil_fail_mode(STENCILOPERATION_KEEP);
    render_context->set_stencil_zfail_mode(STENCILOPERATION_KEEP);
    render_context->set_stencil_reference_count(0);
    render_context->set_stencil_write_mask(0x0);
    render_context->set_stencil_test_mask(0xFF);
    render_context->set_depth_range(0, 0.2);
    
    set_material_information(material_z_overlay, color_z_overlay, wireframe_overlay, true);
    draw_model_execute_original(me, state, pinfo, bone_to_world);
      
    render_context->set_stencil_enable(false);
    render_context->set_depth_range(0, 1);
  }
  
  player->set_invisibility(old_invis);
}
