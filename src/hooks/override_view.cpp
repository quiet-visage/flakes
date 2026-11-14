export module hooks.override_view;
export import std.compat;
export import core.sys;
export import core.vec;
export import sdk.interfaces.entity_list;
export import sdk.interfaces.convar_system;
export import gui.config;
export import sdk.classes.player;
export import subsystems.camera_frustum.camera_frustum;

export extern "C++" {





inline void (*override_view_original)(void*, view_setup*);

inline void override_view_hook(void* me, view_setup* setup) {
  override_view_original(me, setup);

  Player* localplayer = entity_list->get_localplayer();
  if (localplayer == nullptr) return;
  
  if (config.viewport.filters.zoom == true) {
    setup->fov = config.viewport.override_fov ? config.viewport.custom_fov : localplayer->get_default_fov();
  } else {
    if (config.viewport.override_fov == true && localplayer->is_scoped() == false)
      setup->fov = config.viewport.custom_fov;
  }
  
  static Convar* viewmodel_fov = convar_system->find_var("viewmodel_fov");
  if (viewmodel_fov != nullptr && config.viewport.override_viewmodel_fov == true) {
    viewmodel_fov->set_float(config.viewport.custom_viewmodel_fov);
  }
  
  update_camera_frustum(setup);
}

}
