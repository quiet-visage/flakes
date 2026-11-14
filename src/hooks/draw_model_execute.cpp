#include "subsystems/material_override/material_override.hpp"

#include <string.h>

#include "core/vec.hpp"
#include "core/assert.hpp"

#include "gui/config.hpp"

#include "sdk/interfaces/entity_list.hpp"
#include "sdk/interfaces/material_system.hpp"
#include "sdk/interfaces/model_render.hpp"
#include "sdk/interfaces/render_view.hpp"

#include "sdk/classes/material.hpp"
#include "sdk/classes/player.hpp"

#include "subsystems/material_override/material_override.cpp"

#include "core/print.hpp"

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

void draw_model_execute_hook(void* me, void* state, ModelRenderInfo* pinfo, VMatrix* bone_to_world) {  
  if (pinfo == nullptr || pinfo->entity_index <= 0) {
    DME_RETURN;
  }

  Entity* entity = entity_list->entity_from_index(pinfo->entity_index);
  if (entity == nullptr) {
    DME_RETURN;
  }

  apply_material_override(entity, me, state, pinfo, bone_to_world);
}
