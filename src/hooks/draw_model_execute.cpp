export module hooks.draw_model_execute;
export import std.compat;
export import core.sys;
export import subsystems.material_override.material_override_hpp;
export import core.vec;
export import core.assert;
export import gui.config;
export import sdk.interfaces.entity_list;
export import sdk.interfaces.material_system;
export import sdk.interfaces.model_render;
export import sdk.interfaces.render_view;
export import sdk.classes.material;
export import sdk.classes.player;
export import subsystems.material_override.material_override;
export import core.print;

export extern "C++" {
struct ModelRenderInfo;








struct model_t {
  void* handle;
  char* name;
  int load_flags;
  int server_count;
  int type;
  int flags;
  Vec3 vec_mins;
  Vec3 vec_maxs;
  float radius;
};


struct ModelRenderInfo {
  Vec3 origin;
  Vec3 angles;
  void* pRenderable;
  const model_t* model;
  const VMatrix* model_to_world;
  const VMatrix* lighting_offset;
  const Vec3* lighting_origin;
  int flags;
  int entity_index;
  int skin;
  int body;
  int hitboxset;
  short instance;
};

inline void draw_model_execute_hook(void* me, void* state, ModelRenderInfo* pinfo, VMatrix* bone_to_world) {  
  if (pinfo == nullptr || pinfo->entity_index <= 0) {
    { draw_model_execute_original(me, state, pinfo, bone_to_world); return; }
  }

  Entity* entity = entity_list->entity_from_index(pinfo->entity_index);
  if (entity == nullptr) {
    { draw_model_execute_original(me, state, pinfo, bone_to_world); return; }
  }

  apply_material_override(entity, me, state, pinfo, bone_to_world);
}

}
