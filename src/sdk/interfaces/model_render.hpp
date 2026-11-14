export module sdk.interfaces.model_render;
export import std.compat;
export import core.sys;
export import core.memory;
export import sdk.classes.material;

export extern "C++" {



enum OverrideType {
  OVERRIDE_NORMAL = 0,
  OVERRIDE_BUILD_SHADOWS,
  OVERRIDE_DEPTH_WRITE,
  OVERRIDE_SSAO_DEPTH_WRITE,
};

class ModelRender {
public:
  void forced_material_override(Material* new_material, OverrideType override_type = OVERRIDE_NORMAL) {
    mem::call_vfunc<void(Material*, int)>(this, 1, new_material, override_type);
  }

  
};

inline ModelRender* model_render;


}
