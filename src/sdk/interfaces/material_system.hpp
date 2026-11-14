export module sdk.interfaces.material_system;
export import std.compat;
export import core.sys;
export import core.memory;
export import sdk.interfaces.render_context;
export import sdk.classes.material;
export import sdk.classes.keyvalues;

export extern "C++" {




class MaterialSystem {
public:
  Material* find_material(char const* material_name, const char* texture_group_name, bool complain, const char* complain_prefix) {
    return mem::call_vfunc<Material*(const char*, const char*, bool, const char*)>(this, 73, material_name, texture_group_name, complain, complain_prefix);
  }

  Material* create_material(const char* name, KeyValues* key_value) {
    return mem::call_vfunc<Material*(const char*, void*)>(this, 70, name, key_value);
  }


  RenderContext* get_render_context(void) {
    return mem::call_vfunc<RenderContext*()>(this, 98);
  }
  
};

inline MaterialSystem* material_system;


}
