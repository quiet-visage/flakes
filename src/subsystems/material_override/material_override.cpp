export module subsystems.material_override.material_override;
export import std.compat;
export import core.sys;
export import subsystems.material_override.material_override_hpp;
export import subsystems.material_override.material_override_player;
export import subsystems.material_override.material_override_wearable;
export import core.assert;
export import gui.config;
export import sdk.interfaces.entity_list;
export import sdk.interfaces.material_system;
export import sdk.classes.keyvalues;
export import sdk.classes.player;

export extern "C++" {
struct ModelRenderInfo;







void apply_material_override(Entity* entity, void* me, void* state, ModelRenderInfo* pinfo, VMatrix* bone_to_world) {
  if (config.materials.master == false) { draw_model_execute_original(me, state, pinfo, bone_to_world); return; }

  Player* localplayer = entity_list->get_localplayer();
  if (localplayer == nullptr) {
    { draw_model_execute_original(me, state, pinfo, bone_to_world); return; }
  }


  if (materials.empty()) {
    initialize_materials();
  }  

  // If we're still empty, something has gone very wrong: draw normally instead
  // of killing the game.
  if (materials.empty()) {
    static bool logged = false;
    if (!logged) { print("[material_override] materials failed to initialize, overrides disabled\n"); logged = true; }
    { draw_model_execute_original(me, state, pinfo, bone_to_world); return; }
  }

    
  class_id id = entity->get_class_id();
  if (id == class_id::PLAYER) {
    apply_player_material_override((Player*)entity, me, state, pinfo, bone_to_world);
  } else if ((entity->is_wearable() || (entity->is_base_combat_weapon() && !entity->get_pickup_type())) && entity->get_owner_entity() != nullptr) {
    apply_wearable_material_override(entity, me, state, pinfo, bone_to_world); // Attachments on the player model like weapons and cosmetics
  } else {
    draw_model_execute_original(me, state, pinfo, bone_to_world);
  }

  model_render->forced_material_override(nullptr, OVERRIDE_NORMAL);
  RGBA_float white = {1,1,1,1};
  render_view->set_color_modulation(&white);
  render_view->set_blend(1);
}

}
