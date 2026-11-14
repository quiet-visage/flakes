export module subsystems.material_override.material_override_hpp;
export import std.compat;
export import core.sys;
export import core.assert;
export import core.vec;
export import sdk.classes.keyvalues;
export import sdk.interfaces.material_system;
export import sdk.interfaces.model_render;
export import sdk.interfaces.render_view;
export import sdk.classes.material;
export import sdk.classes.material_var;

export extern "C++" {

struct ModelRenderInfo;
inline void (*draw_model_execute_original)(void*, void*, ModelRenderInfo*, VMatrix*) = nullptr;

inline std::vector<Material*> materials;

inline Material* create_material_ex(const std::string_view name, const std::string_view shader, const std::string_view material) {
  KeyValues* values = new KeyValues(shader.data());
  values->load_from_buffer(name.data(), material.data());
  Material* mat = material_system->create_material(name.data(), values);
  if (mat != nullptr) {
    mat->increment_reference_count();
  }
  return mat;
}

inline Material* get_configured_material(int index) {
  if (index < 0 || (size_t)index >= materials.size()) {
    return nullptr;
  }
  return materials[(size_t)index];
}

inline void initialize_materials(void) {
  materials.push_back(nullptr);
  
  materials.push_back(create_material_ex("TfFlat",
					 "UnlitGeneric",
					 R"#(
"UnlitGeneric" {
   "$basetexture" "white"
}
)#"));
  
  materials.push_back(create_material_ex("TfShaded",
					 "VertexLitGeneric",
					 R"#(
"VertexLitGeneric" {
   "$basetexture" "white"
}
)#"));

  materials.push_back(create_material_ex("TfFresnel",
					 "VertexLitGeneric",
					 R"#(
"VertexLitGeneric" {
   "$basetexture" "white"
   "$bumpmap" "white"
   "$envmap" "skybox/sky_dust_hdr"
   "$envmapfresnel" "1"
   "$envmapfresnelminmaxexp" "[0 1 2]"
}
)#"));

}

inline void set_material_information(Material* material, RGBA_float color, bool wireframe = false, bool ignore_z = false, OverrideType override_type = OVERRIDE_NORMAL) {
  material->set_material_flag(MATERIAL_VAR_WIREFRAME, wireframe);
  material->set_material_flag(MATERIAL_VAR_IGNOREZ, ignore_z);
  
  material->color_modulate(color);
  material->alpha_modulate(color.a);

  MaterialVar* envmap = material->find_var("$envmaptint");
  if (envmap != nullptr) {
    envmap->set_vec_value(color);
  }
  
  model_render->forced_material_override(material, override_type);
}

}
