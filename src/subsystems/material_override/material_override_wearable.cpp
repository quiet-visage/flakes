#include "gui/config.hpp"

#include "sdk/interfaces/material_system.hpp"
#include "sdk/interfaces/render_view.hpp"
#include "sdk/interfaces/model_render.hpp"

#include "sdk/classes/entity.hpp"
#include "sdk/classes/player.hpp"
#include "subsystems/material_override/material_override.hpp"

void apply_wearable_material_override(Entity* entity, void* me, void* state, ModelRenderInfo* pinfo, VMatrix* bone_to_world) {

  Player* localplayer = entity_list->get_localplayer();
  if (localplayer == nullptr || entity == nullptr) DME_RETURN;

  Entity* owner_entity = entity->get_owner_entity();
  if (owner_entity == nullptr || owner_entity->get_class_id() != class_id::PLAYER) DME_RETURN;
  Player* owner_player = (Player*)owner_entity;
  
  
  if (owner_player == localplayer && config.materials.player.local == false) DME_RETURN; // Ignore Local Player
  if (entity->is_dormant() || owner_player->is_dormant()) DME_RETURN;
  if (owner_player->get_lifestate() != 1) DME_RETURN; // Ignore Dead Owner
  if (owner_player->get_team() != localplayer->get_team() && config.materials.player.enemy == false && !owner_player->is_friend()) DME_RETURN; // Ignore Enemy
  if (owner_player->get_team() == localplayer->get_team() && owner_player != localplayer && config.materials.player.team == false && !owner_player->is_friend()) DME_RETURN; // Ignore Team
  if (owner_player->is_friend() && config.materials.player.friends == false && (config.materials.player.team == false && owner_player->get_team() == localplayer->get_team())) DME_RETURN; // Ignore Friends

  
  RenderContext* render_context = material_system->get_render_context(); 
  
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

  if (owner_player->get_team() != localplayer->get_team()) {
    color = config.materials.player.enemy_color;
    color_z = config.materials.player.enemy_color_z;
    ignore_z = config.materials.player.enemy_flags.ignore_z;
    wireframe = config.materials.player.enemy_flags.wireframe;
    material = get_configured_material((int)config.materials.player.enemy_material_type);
    material_z = get_configured_material((int)config.materials.player.enemy_material_z_type);

    color_overlay = config.materials.player.enemy_overlay_color;
    color_z_overlay = config.materials.player.enemy_overlay_color_z;    
    ignore_z_overlay = config.materials.player.enemy_overlay_flags.ignore_z;    
    wireframe_overlay = config.materials.player.enemy_overlay_flags.wireframe;    
    material_overlay = get_configured_material((int)config.materials.player.enemy_overlay_material_type);
    material_z_overlay = get_configured_material((int)config.materials.player.enemy_overlay_material_z_type);
  }
  if (owner_player->get_team() == localplayer->get_team()) {
    color = config.materials.player.team_color;
    color_z = config.materials.player.team_color_z;
    ignore_z = config.materials.player.team_flags.ignore_z;
    wireframe = config.materials.player.team_flags.wireframe;
    material = get_configured_material((int)config.materials.player.team_material_type);
    material_z = get_configured_material((int)config.materials.player.team_material_z_type);
  }
  if (owner_player->is_friend() && config.materials.player.friends) {
    color = config.materials.player.friend_color;
    color_z = config.materials.player.friend_color_z;
    ignore_z = config.materials.player.friends_flags.ignore_z;
    wireframe = config.materials.player.friends_flags.wireframe;
    material = get_configured_material((int)config.materials.player.friend_material_type);
    material_z = get_configured_material((int)config.materials.player.friend_material_z_type);
  }
  if (owner_player == localplayer) {
    color = config.materials.player.local_color;
    color_z = RGBA_float{};
    ignore_z = false;
    wireframe = config.materials.player.local_flags.wireframe;
    material = get_configured_material((int)config.materials.player.local_material_type);
    material_z = material;
  }
  

  /* Primary Shader Pass */
  if (ignore_z == true && material_z != nullptr && render_context != nullptr) {
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
    set_material_information(material, color, wireframe, false, OVERRIDE_DEPTH_WRITE);
    draw_model_execute_original(me, state, pinfo, bone_to_world);
    set_material_information(material, color, wireframe, false, OVERRIDE_NORMAL);
    draw_model_execute_original(me, state, pinfo, bone_to_world);
  } else {
    draw_model_execute_original(me, state, pinfo, bone_to_world);
  }
  
  if (ignore_z == true && material_z != nullptr && render_context != nullptr) {
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
  if (ignore_z_overlay == true && material_z_overlay != nullptr && render_context != nullptr) {
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

  if (ignore_z_overlay == true && material_z_overlay != nullptr && render_context != nullptr) {
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

  
  model_render->forced_material_override(nullptr, OVERRIDE_NORMAL);

}
