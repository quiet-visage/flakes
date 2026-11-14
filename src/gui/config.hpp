export module gui.config;
export import std.compat;
export import core.sys;
export import sdk.classes.player;
export import core.vec;

export extern "C++" {



struct key_button {
  int button;
  bool waiting = false;
};
using button = key_button;

struct VectorTargetingConfig {
  bool master = false;

  bool auto_shoot = false;

  enum class TargetType {
    FOV,
    DISTANCE,
    LEAST_HEALTH,
    MOST_HEALTH
  } target_type = TargetType::FOV;
  
  bool silent = false;
  
  key_button key = {-SDL_BUTTON_X1, false};
  bool use_key = true;
  
  float fov = 45;
  bool draw_fov = false;
  
  bool auto_scope = false;
  bool auto_unscope = false;
  bool scoped_only = false;
  
  bool ignore_friends = true;
  bool ignore_spectated = true;
};

struct ActiveDeflectionConfig {
  bool master = false;

  key_button key = {-SDL_BUTTON_X1, false};
  bool use_key = true;

  float fov = 90.0f;
  float range = 256.0f;

  struct Projectiles {
    bool rockets = true;
    bool grenades = true;
    bool stickies = true;
    bool arrows = true;
    bool flares = true;
    bool throwables = true;
  } projectiles;
};

struct SpatialTelemetryConfig {
  bool master = false;
  bool observer_overlay = false;

  struct PlayerConfig {
    RGBA_float enemy_color = {1.0f, 0.501960784f, 0.0f, 1.0f};
    RGBA_float team_color = {1.0f, 1.0f, 1.0f, 1.0f};
    RGBA_float friend_color = {0.0f, 0.862745098f, 0.31372549f, 1.0f};
    
    bool box = false;
    bool health_bar = false;    
    bool name = false;
    
    struct Flags {
      bool target_indicator = false;
      bool friend_indicator = false;
      bool scoped_indicator = false;
    } flags;

    bool enemy = false;
    bool team = false;
    bool friends = false;
  } player;

  struct Pickup {
    bool box = false;    
    bool name = false;
  } pickup;

  struct Intelligence {
    bool box = false;    
    bool name = false;
  } intelligence;
  
  struct Buildings {
    bool box = false;
    bool health_bar = false;    
    bool name = false;

    bool team = false;
  } buildings;
};

struct MaterialOverrideConfig {
  bool master = false;

  struct PlayerConfig {
    bool enemy = false;

    enum class MaterialType {
      NONE,
      FLAT,
      SHADED,
      FRESNEL
    };

    struct PassFlags {      
      bool ignore_z = false;
      bool wireframe = false;
    };
    
    RGBA_float enemy_color = {.8f, 0.701960784f, .1f, 1.0f};
    MaterialType enemy_material_type = MaterialType::FLAT;
    RGBA_float enemy_color_z = {1.0f, 0.2f, .2f, 1.0f};
    MaterialType enemy_material_z_type = MaterialType::FLAT;
    PassFlags enemy_flags;
    RGBA_float enemy_overlay_color = {.8f, 0.701960784f, .1f, 1.0f};
    MaterialType enemy_overlay_material_type = MaterialType::NONE;
    RGBA_float enemy_overlay_color_z = {1.0f, 0.2f, .2f, 1.0f};
    MaterialType enemy_overlay_material_z_type = MaterialType::NONE;
    PassFlags enemy_overlay_flags;

    bool team = false;    
    RGBA_float team_color = {0.0f, 1.0f, 0.0f, 1.0f};
    MaterialType team_material_type = MaterialType::SHADED;
    RGBA_float team_color_z = {0.0f, 0.25f, 1.0f, 1.0f};    
    MaterialType team_material_z_type = MaterialType::SHADED;
    PassFlags team_flags;
    
    bool friends = false;
    RGBA_float friend_color = {0.0f, 0.632745098f, 0.31372549f, 1.0f};
    MaterialType friend_material_type = MaterialType::FLAT;
    RGBA_float friend_color_z = {0.0f, 1.0f, 0.20272549f, 1.0f};
    MaterialType friend_material_z_type = MaterialType::FLAT;
    PassFlags friends_flags;

    bool local = false;
    RGBA_float local_color = {0.0f, 0.8f, 0.35f, 1.0f};
    MaterialType local_material_type = MaterialType::SHADED;
    PassFlags local_flags;
  } player;
};

struct ViewportConfig {

  struct PostProcessFilters {
    bool scope = false;
    bool zoom = false;
  } filters;
  
  struct CameraFrustum {
    key_button key = {SDL_SCANCODE_LALT, false};
    bool enabled = false;
    float z = 150.0f;
    float y = 20.0f;
    float x = 0; 
  } camera_frustum;
  
  bool override_fov = false;
  float custom_fov = 90;

  bool override_viewmodel_fov = false;
  float custom_viewmodel_fov = 70;
};

struct SimulationConfig {

  struct Locomotion {
    bool ground_impulse = false;
    bool suppress_collision_push = false;
  } locomotion;

  struct RuntimeDirectives {
    bool bypass_sv_pure = false;
    bool disable_frame_sleep = false;

    bool tick_scheduler = false;
    bool tick_telemetry_widget = false;
    key_button recharge_button = {SDL_SCANCODE_LSHIFT, false};
  } directives;
  
  struct Automation {
    bool auto_class_select = false;
    enum tf_class class_selected = tf_class::SNIPER;
  } automation;
};

struct GraphNavigationConfig {
  bool master = false;

  bool walk = false;
  
  bool look_at_path = false;
  float look_smoothness = 50;

  bool do_objective = false;
};

struct DiagnosticsConfig {
  int font_height = 14;
  int font_weight = 400;
  bool render_all_entities = false;
  bool draw_navigation_path = false;
  bool draw_navmesh = false;
  bool draw_navigation_goal = false;
  bool draw_navigation_current_area = false;
  bool draw_navigation_next_area = false;
  bool show_active_flag_ids_of_players = false;
  bool disable_friend_checks = true;
  
  // Hardware Telemetry Bridge
  bool bridge_master = false;
  bool bridge_on_kill = false;
  bool bridge_on_death = false;
  std::string username = "";
  std::string sharecode = "";
  std::string apikey = "";
  int intensity = 1;
  int duration = 1;
  enum BridgeActuationMode {
    SHOCK,
    VIBRATE,
    BEEP
  } operation;
};

struct Config {
  VectorTargetingConfig targeting;
  ActiveDeflectionConfig deflection;
  SpatialTelemetryConfig telemetry;
  MaterialOverrideConfig materials;
  ViewportConfig viewport;
  SimulationConfig simulation;
  GraphNavigationConfig navigation;
  DiagnosticsConfig diagnostics;
};

inline Config config;


inline bool is_button_down(key_button button) {
  if (button.button >= 0) {
    const uint8_t* keys = SDL_GetKeyboardState(nullptr);
  
    if (keys[button.button] == 1)
      return true;

    return false;
  } else {
    Uint32 mouse_state = SDL_GetMouseState(nullptr, nullptr);

    if (mouse_state & SDL_BUTTON(-button.button))
      return true;

    return false;
  }  

  return false;
}


}
