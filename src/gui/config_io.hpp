export module gui.config_io;
export import std.compat;
export import core.sys;
export import core.json;
export import core.vec;
export import gui.config;
export import core.print;

export extern "C++" {

inline JsonValue rgba_to_json(const RGBA_float& col) {
  JsonValue j;
  j.type = JsonType::Object;
  j["r"] = col.r;
  j["g"] = col.g;
  j["b"] = col.b;
  j["a"] = col.a;
  return j;
}

inline void rgba_from_json(const JsonValue& j, RGBA_float& col) {
  if (!j.is_object()) return;
  col.r = j["r"].as_float(col.r);
  col.g = j["g"].as_float(col.g);
  col.b = j["b"].as_float(col.b);
  col.a = j["a"].as_float(col.a);
}

inline JsonValue config_to_json(const Config& cfg) {
  JsonValue root;
  root.type = JsonType::Object;

  // Vector Targeting
  {
    JsonValue& t = root["targeting"];
    t["master"] = cfg.targeting.master;
    t["auto_shoot"] = cfg.targeting.auto_shoot;
    t["target_type"] = (int)cfg.targeting.target_type;
    t["silent"] = cfg.targeting.silent;
    t["key"] = cfg.targeting.key.button;
    t["use_key"] = cfg.targeting.use_key;
    t["fov"] = cfg.targeting.fov;
    t["draw_fov"] = cfg.targeting.draw_fov;
    t["auto_scope"] = cfg.targeting.auto_scope;
    t["auto_unscope"] = cfg.targeting.auto_unscope;
    t["scoped_only"] = cfg.targeting.scoped_only;
    t["ignore_friends"] = cfg.targeting.ignore_friends;
    t["ignore_spectated"] = cfg.targeting.ignore_spectated;
    t["bio_model"] = (int)cfg.targeting.bio_model;
    t["spring_omega_n"] = cfg.targeting.spring_omega_n;
    t["spring_zeta"] = cfg.targeting.spring_zeta;
    t["jerk_duration"] = cfg.targeting.jerk_duration;
    t["jerk_replan_accel"] = cfg.targeting.jerk_replan_accel;
    t["ou_noise_enabled"] = cfg.targeting.ou_noise_enabled;
    t["ou_theta"] = cfg.targeting.ou_theta;
    t["ou_sigma"] = cfg.targeting.ou_sigma;
    t["ou_scale"] = cfg.targeting.ou_scale;
    t["kalman_enabled"] = cfg.targeting.kalman_enabled;
    t["kalman_latency"] = cfg.targeting.kalman_latency;
    t["kalman_q"] = cfg.targeting.kalman_q;
    t["kalman_r"] = cfg.targeting.kalman_r;
    t["one_euro_enabled"] = cfg.targeting.one_euro_enabled;
    t["one_euro_min_cutoff"] = cfg.targeting.one_euro_min_cutoff;
    t["one_euro_beta"] = cfg.targeting.one_euro_beta;
  }

  // Active Deflection
  {
    JsonValue& d = root["deflection"];
    d["master"] = cfg.deflection.master;
    d["key"] = cfg.deflection.key.button;
    d["use_key"] = cfg.deflection.use_key;
    d["fov"] = cfg.deflection.fov;
    d["range"] = cfg.deflection.range;
    d["rockets"] = cfg.deflection.projectiles.rockets;
    d["grenades"] = cfg.deflection.projectiles.grenades;
    d["stickies"] = cfg.deflection.projectiles.stickies;
    d["arrows"] = cfg.deflection.projectiles.arrows;
    d["flares"] = cfg.deflection.projectiles.flares;
    d["throwables"] = cfg.deflection.projectiles.throwables;
  }

  // Demoman Proximity Fuze
  {
    JsonValue& f = root["proximity_fuze"];
    f["master"] = cfg.proximity_fuze.master;
    f["key"] = cfg.proximity_fuze.key.button;
    f["use_key"] = cfg.proximity_fuze.use_key;
    f["radius"] = cfg.proximity_fuze.radius;
    f["check_visibility"] = cfg.proximity_fuze.check_visibility;
    f["check_arm_time"] = cfg.proximity_fuze.check_arm_time;
    f["target_players"] = cfg.proximity_fuze.target_players;
    f["target_buildings"] = cfg.proximity_fuze.target_buildings;
    f["ignore_friends"] = cfg.proximity_fuze.ignore_friends;
    f["ignore_invulnerable"] = cfg.proximity_fuze.ignore_invulnerable;
    f["ignore_cloaked"] = cfg.proximity_fuze.ignore_cloaked;
    f["ignore_disguised"] = cfg.proximity_fuze.ignore_disguised;
  }

  // Spatial Telemetry
  {
    JsonValue& t = root["telemetry"];
    t["master"] = cfg.telemetry.master;
    t["observer_overlay"] = cfg.telemetry.observer_overlay;

    // Player telemetry
    t["player_enemy_color"] = rgba_to_json(cfg.telemetry.player.enemy_color);
    t["player_team_color"] = rgba_to_json(cfg.telemetry.player.team_color);
    t["player_friend_color"] = rgba_to_json(cfg.telemetry.player.friend_color);
    t["player_box"] = cfg.telemetry.player.box;
    t["player_health_bar"] = cfg.telemetry.player.health_bar;
    t["player_name"] = cfg.telemetry.player.name;
    t["player_target_indicator"] = cfg.telemetry.player.flags.target_indicator;
    t["player_friend_indicator"] = cfg.telemetry.player.flags.friend_indicator;
    t["player_scoped_indicator"] = cfg.telemetry.player.flags.scoped_indicator;
    t["player_enemy"] = cfg.telemetry.player.enemy;
    t["player_team"] = cfg.telemetry.player.team;
    t["player_friends"] = cfg.telemetry.player.friends;

    // Pickups & Intel
    t["pickup_box"] = cfg.telemetry.pickup.box;
    t["pickup_name"] = cfg.telemetry.pickup.name;
    t["intel_box"] = cfg.telemetry.intelligence.box;
    t["intel_name"] = cfg.telemetry.intelligence.name;

    // Buildings
    t["buildings_box"] = cfg.telemetry.buildings.box;
    t["buildings_health_bar"] = cfg.telemetry.buildings.health_bar;
    t["buildings_name"] = cfg.telemetry.buildings.name;
    t["buildings_team"] = cfg.telemetry.buildings.team;
  }

  // Material Override
  {
    JsonValue& m = root["materials"];
    m["master"] = cfg.materials.master;

    // Enemy
    m["enemy"] = cfg.materials.player.enemy;
    m["enemy_color"] = rgba_to_json(cfg.materials.player.enemy_color);
    m["enemy_material_type"] = (int)cfg.materials.player.enemy_material_type;
    m["enemy_color_z"] = rgba_to_json(cfg.materials.player.enemy_color_z);
    m["enemy_material_z_type"] = (int)cfg.materials.player.enemy_material_z_type;
    m["enemy_ignore_z"] = cfg.materials.player.enemy_flags.ignore_z;
    m["enemy_wireframe"] = cfg.materials.player.enemy_flags.wireframe;

    // Enemy Overlay
    m["enemy_overlay_color"] = rgba_to_json(cfg.materials.player.enemy_overlay_color);
    m["enemy_overlay_material_type"] = (int)cfg.materials.player.enemy_overlay_material_type;
    m["enemy_overlay_color_z"] = rgba_to_json(cfg.materials.player.enemy_overlay_color_z);
    m["enemy_overlay_material_z_type"] = (int)cfg.materials.player.enemy_overlay_material_z_type;
    m["enemy_overlay_ignore_z"] = cfg.materials.player.enemy_overlay_flags.ignore_z;
    m["enemy_overlay_wireframe"] = cfg.materials.player.enemy_overlay_flags.wireframe;

    // Team
    m["team"] = cfg.materials.player.team;
    m["team_color"] = rgba_to_json(cfg.materials.player.team_color);
    m["team_material_type"] = (int)cfg.materials.player.team_material_type;
    m["team_color_z"] = rgba_to_json(cfg.materials.player.team_color_z);
    m["team_material_z_type"] = (int)cfg.materials.player.team_material_z_type;
    m["team_ignore_z"] = cfg.materials.player.team_flags.ignore_z;
    m["team_wireframe"] = cfg.materials.player.team_flags.wireframe;

    // Friends
    m["friends"] = cfg.materials.player.friends;
    m["friend_color"] = rgba_to_json(cfg.materials.player.friend_color);
    m["friend_material_type"] = (int)cfg.materials.player.friend_material_type;
    m["friend_color_z"] = rgba_to_json(cfg.materials.player.friend_color_z);
    m["friend_material_z_type"] = (int)cfg.materials.player.friend_material_z_type;
    m["friend_ignore_z"] = cfg.materials.player.friends_flags.ignore_z;
    m["friend_wireframe"] = cfg.materials.player.friends_flags.wireframe;

    // Local
    m["local"] = cfg.materials.player.local;
    m["local_color"] = rgba_to_json(cfg.materials.player.local_color);
    m["local_material_type"] = (int)cfg.materials.player.local_material_type;
    m["local_wireframe"] = cfg.materials.player.local_flags.wireframe;
  }

  // Viewport
  {
    JsonValue& v = root["viewport"];
    v["scope_filter"] = cfg.viewport.filters.scope;
    v["zoom_filter"] = cfg.viewport.filters.zoom;
    v["camera_frustum_key"] = cfg.viewport.camera_frustum.key.button;
    v["camera_frustum_enabled"] = cfg.viewport.camera_frustum.enabled;
    v["camera_frustum_x"] = cfg.viewport.camera_frustum.x;
    v["camera_frustum_y"] = cfg.viewport.camera_frustum.y;
    v["camera_frustum_z"] = cfg.viewport.camera_frustum.z;
    v["override_fov"] = cfg.viewport.override_fov;
    v["custom_fov"] = cfg.viewport.custom_fov;
    v["override_viewmodel_fov"] = cfg.viewport.override_viewmodel_fov;
    v["custom_viewmodel_fov"] = cfg.viewport.custom_viewmodel_fov;
  }

  // Simulation
  {
    JsonValue& s = root["simulation"];
    s["ground_impulse"] = cfg.simulation.locomotion.ground_impulse;
    s["suppress_collision_push"] = cfg.simulation.locomotion.suppress_collision_push;
    s["bypass_sv_pure"] = cfg.simulation.directives.bypass_sv_pure;
    s["disable_frame_sleep"] = cfg.simulation.directives.disable_frame_sleep;
    s["tick_scheduler"] = cfg.simulation.directives.tick_scheduler;
    s["tick_telemetry_widget"] = cfg.simulation.directives.tick_telemetry_widget;
    s["recharge_button"] = cfg.simulation.directives.recharge_button.button;
    s["auto_class_select"] = cfg.simulation.automation.auto_class_select;
    s["class_selected"] = (int)cfg.simulation.automation.class_selected;
  }

  // Navigation
  {
    JsonValue& n = root["navigation"];
    n["master"] = cfg.navigation.master;
    n["walk"] = cfg.navigation.walk;
    n["look_at_path"] = cfg.navigation.look_at_path;
    n["look_smoothness"] = cfg.navigation.look_smoothness;
    n["do_objective"] = cfg.navigation.do_objective;
  }

  // Diagnostics
  {
    JsonValue& diag = root["diagnostics"];
    diag["font_height"] = cfg.diagnostics.font_height;
    diag["font_weight"] = cfg.diagnostics.font_weight;
    diag["render_all_entities"] = cfg.diagnostics.render_all_entities;
    diag["draw_navigation_path"] = cfg.diagnostics.draw_navigation_path;
    diag["draw_navmesh"] = cfg.diagnostics.draw_navmesh;
    diag["draw_navigation_goal"] = cfg.diagnostics.draw_navigation_goal;
    diag["draw_navigation_current_area"] = cfg.diagnostics.draw_navigation_current_area;
    diag["draw_navigation_next_area"] = cfg.diagnostics.draw_navigation_next_area;
    diag["show_active_flag_ids_of_players"] = cfg.diagnostics.show_active_flag_ids_of_players;
    diag["disable_friend_checks"] = cfg.diagnostics.disable_friend_checks;

    diag["bridge_master"] = cfg.diagnostics.bridge_master;
    diag["bridge_on_kill"] = cfg.diagnostics.bridge_on_kill;
    diag["bridge_on_death"] = cfg.diagnostics.bridge_on_death;
    diag["username"] = cfg.diagnostics.username;
    diag["sharecode"] = cfg.diagnostics.sharecode;
    diag["apikey"] = cfg.diagnostics.apikey;
    diag["intensity"] = cfg.diagnostics.intensity;
    diag["duration"] = cfg.diagnostics.duration;
    diag["operation"] = (int)cfg.diagnostics.operation;
  }

  return root;
}

inline bool config_from_json(Config& cfg, const JsonValue& root) {
  if (!root.is_object()) return false;

  // Vector Targeting
  if (root.has("targeting")) {
    const JsonValue& t = root["targeting"];
    cfg.targeting.master = t["master"].as_bool(cfg.targeting.master);
    cfg.targeting.auto_shoot = t["auto_shoot"].as_bool(cfg.targeting.auto_shoot);
    cfg.targeting.target_type = (VectorTargetingConfig::TargetType)t["target_type"].as_int((int)cfg.targeting.target_type);
    cfg.targeting.silent = t["silent"].as_bool(cfg.targeting.silent);
    cfg.targeting.key.button = t["key"].as_int(cfg.targeting.key.button);
    cfg.targeting.use_key = t["use_key"].as_bool(cfg.targeting.use_key);
    cfg.targeting.fov = t["fov"].as_float(cfg.targeting.fov);
    cfg.targeting.draw_fov = t["draw_fov"].as_bool(cfg.targeting.draw_fov);
    cfg.targeting.auto_scope = t["auto_scope"].as_bool(cfg.targeting.auto_scope);
    cfg.targeting.auto_unscope = t["auto_unscope"].as_bool(cfg.targeting.auto_unscope);
    cfg.targeting.scoped_only = t["scoped_only"].as_bool(cfg.targeting.scoped_only);
    cfg.targeting.ignore_friends = t["ignore_friends"].as_bool(cfg.targeting.ignore_friends);
    cfg.targeting.ignore_spectated = t["ignore_spectated"].as_bool(cfg.targeting.ignore_spectated);
    cfg.targeting.bio_model = (VectorTargetingConfig::BiomechanicalModel)t["bio_model"].as_int((int)cfg.targeting.bio_model);
    cfg.targeting.spring_omega_n = t["spring_omega_n"].as_float(cfg.targeting.spring_omega_n);
    cfg.targeting.spring_zeta = t["spring_zeta"].as_float(cfg.targeting.spring_zeta);
    cfg.targeting.jerk_duration = t["jerk_duration"].as_float(cfg.targeting.jerk_duration);
    cfg.targeting.jerk_replan_accel = t["jerk_replan_accel"].as_float(cfg.targeting.jerk_replan_accel);
    cfg.targeting.ou_noise_enabled = t["ou_noise_enabled"].as_bool(cfg.targeting.ou_noise_enabled);
    cfg.targeting.ou_theta = t["ou_theta"].as_float(cfg.targeting.ou_theta);
    cfg.targeting.ou_sigma = t["ou_sigma"].as_float(cfg.targeting.ou_sigma);
    cfg.targeting.ou_scale = t["ou_scale"].as_float(cfg.targeting.ou_scale);
    cfg.targeting.kalman_enabled = t["kalman_enabled"].as_bool(cfg.targeting.kalman_enabled);
    cfg.targeting.kalman_latency = t["kalman_latency"].as_float(cfg.targeting.kalman_latency);
    cfg.targeting.kalman_q = t["kalman_q"].as_float(cfg.targeting.kalman_q);
    cfg.targeting.kalman_r = t["kalman_r"].as_float(cfg.targeting.kalman_r);
    cfg.targeting.one_euro_enabled = t["one_euro_enabled"].as_bool(cfg.targeting.one_euro_enabled);
    cfg.targeting.one_euro_min_cutoff = t["one_euro_min_cutoff"].as_float(cfg.targeting.one_euro_min_cutoff);
    cfg.targeting.one_euro_beta = t["one_euro_beta"].as_float(cfg.targeting.one_euro_beta);
  }

  // Active Deflection
  if (root.has("deflection")) {
    const JsonValue& d = root["deflection"];
    cfg.deflection.master = d["master"].as_bool(cfg.deflection.master);
    cfg.deflection.key.button = d["key"].as_int(cfg.deflection.key.button);
    cfg.deflection.use_key = d["use_key"].as_bool(cfg.deflection.use_key);
    cfg.deflection.fov = d["fov"].as_float(cfg.deflection.fov);
    cfg.deflection.range = d["range"].as_float(cfg.deflection.range);
    cfg.deflection.projectiles.rockets = d["rockets"].as_bool(cfg.deflection.projectiles.rockets);
    cfg.deflection.projectiles.grenades = d["grenades"].as_bool(cfg.deflection.projectiles.grenades);
    cfg.deflection.projectiles.stickies = d["stickies"].as_bool(cfg.deflection.projectiles.stickies);
    cfg.deflection.projectiles.arrows = d["arrows"].as_bool(cfg.deflection.projectiles.arrows);
    cfg.deflection.projectiles.flares = d["flares"].as_bool(cfg.deflection.projectiles.flares);
    cfg.deflection.projectiles.throwables = d["throwables"].as_bool(cfg.deflection.projectiles.throwables);
  }

  // Demoman Proximity Fuze
  if (root.has("proximity_fuze")) {
    const JsonValue& f = root["proximity_fuze"];
    cfg.proximity_fuze.master = f["master"].as_bool(cfg.proximity_fuze.master);
    cfg.proximity_fuze.key.button = f["key"].as_int(cfg.proximity_fuze.key.button);
    cfg.proximity_fuze.use_key = f["use_key"].as_bool(cfg.proximity_fuze.use_key);
    cfg.proximity_fuze.radius = f["radius"].as_float(cfg.proximity_fuze.radius);
    cfg.proximity_fuze.check_visibility = f["check_visibility"].as_bool(cfg.proximity_fuze.check_visibility);
    cfg.proximity_fuze.check_arm_time = f["check_arm_time"].as_bool(cfg.proximity_fuze.check_arm_time);
    cfg.proximity_fuze.target_players = f["target_players"].as_bool(cfg.proximity_fuze.target_players);
    cfg.proximity_fuze.target_buildings = f["target_buildings"].as_bool(cfg.proximity_fuze.target_buildings);
    cfg.proximity_fuze.ignore_friends = f["ignore_friends"].as_bool(cfg.proximity_fuze.ignore_friends);
    cfg.proximity_fuze.ignore_invulnerable = f["ignore_invulnerable"].as_bool(cfg.proximity_fuze.ignore_invulnerable);
    cfg.proximity_fuze.ignore_cloaked = f["ignore_cloaked"].as_bool(cfg.proximity_fuze.ignore_cloaked);
    cfg.proximity_fuze.ignore_disguised = f["ignore_disguised"].as_bool(cfg.proximity_fuze.ignore_disguised);
  }

  // Spatial Telemetry
  if (root.has("telemetry")) {
    const JsonValue& t = root["telemetry"];
    cfg.telemetry.master = t["master"].as_bool(cfg.telemetry.master);
    cfg.telemetry.observer_overlay = t["observer_overlay"].as_bool(cfg.telemetry.observer_overlay);

    rgba_from_json(t["player_enemy_color"], cfg.telemetry.player.enemy_color);
    rgba_from_json(t["player_team_color"], cfg.telemetry.player.team_color);
    rgba_from_json(t["player_friend_color"], cfg.telemetry.player.friend_color);

    cfg.telemetry.player.box = t["player_box"].as_bool(cfg.telemetry.player.box);
    cfg.telemetry.player.health_bar = t["player_health_bar"].as_bool(cfg.telemetry.player.health_bar);
    cfg.telemetry.player.name = t["player_name"].as_bool(cfg.telemetry.player.name);
    cfg.telemetry.player.flags.target_indicator = t["player_target_indicator"].as_bool(cfg.telemetry.player.flags.target_indicator);
    cfg.telemetry.player.flags.friend_indicator = t["player_friend_indicator"].as_bool(cfg.telemetry.player.flags.friend_indicator);
    cfg.telemetry.player.flags.scoped_indicator = t["player_scoped_indicator"].as_bool(cfg.telemetry.player.flags.scoped_indicator);
    cfg.telemetry.player.enemy = t["player_enemy"].as_bool(cfg.telemetry.player.enemy);
    cfg.telemetry.player.team = t["player_team"].as_bool(cfg.telemetry.player.team);
    cfg.telemetry.player.friends = t["player_friends"].as_bool(cfg.telemetry.player.friends);

    cfg.telemetry.pickup.box = t["pickup_box"].as_bool(cfg.telemetry.pickup.box);
    cfg.telemetry.pickup.name = t["pickup_name"].as_bool(cfg.telemetry.pickup.name);
    cfg.telemetry.intelligence.box = t["intel_box"].as_bool(cfg.telemetry.intelligence.box);
    cfg.telemetry.intelligence.name = t["intel_name"].as_bool(cfg.telemetry.intelligence.name);

    cfg.telemetry.buildings.box = t["buildings_box"].as_bool(cfg.telemetry.buildings.box);
    cfg.telemetry.buildings.health_bar = t["buildings_health_bar"].as_bool(cfg.telemetry.buildings.health_bar);
    cfg.telemetry.buildings.name = t["buildings_name"].as_bool(cfg.telemetry.buildings.name);
    cfg.telemetry.buildings.team = t["buildings_team"].as_bool(cfg.telemetry.buildings.team);
  }

  // Material Override
  if (root.has("materials")) {
    const JsonValue& m = root["materials"];
    cfg.materials.master = m["master"].as_bool(cfg.materials.master);

    // Enemy
    cfg.materials.player.enemy = m["enemy"].as_bool(cfg.materials.player.enemy);
    rgba_from_json(m["enemy_color"], cfg.materials.player.enemy_color);
    cfg.materials.player.enemy_material_type = (MaterialOverrideConfig::PlayerConfig::MaterialType)m["enemy_material_type"].as_int((int)cfg.materials.player.enemy_material_type);
    rgba_from_json(m["enemy_color_z"], cfg.materials.player.enemy_color_z);
    cfg.materials.player.enemy_material_z_type = (MaterialOverrideConfig::PlayerConfig::MaterialType)m["enemy_material_z_type"].as_int((int)cfg.materials.player.enemy_material_z_type);
    cfg.materials.player.enemy_flags.ignore_z = m["enemy_ignore_z"].as_bool(cfg.materials.player.enemy_flags.ignore_z);
    cfg.materials.player.enemy_flags.wireframe = m["enemy_wireframe"].as_bool(cfg.materials.player.enemy_flags.wireframe);

    // Enemy Overlay
    rgba_from_json(m["enemy_overlay_color"], cfg.materials.player.enemy_overlay_color);
    cfg.materials.player.enemy_overlay_material_type = (MaterialOverrideConfig::PlayerConfig::MaterialType)m["enemy_overlay_material_type"].as_int((int)cfg.materials.player.enemy_overlay_material_type);
    rgba_from_json(m["enemy_overlay_color_z"], cfg.materials.player.enemy_overlay_color_z);
    cfg.materials.player.enemy_overlay_material_z_type = (MaterialOverrideConfig::PlayerConfig::MaterialType)m["enemy_overlay_material_z_type"].as_int((int)cfg.materials.player.enemy_overlay_material_z_type);
    cfg.materials.player.enemy_overlay_flags.ignore_z = m["enemy_overlay_ignore_z"].as_bool(cfg.materials.player.enemy_overlay_flags.ignore_z);
    cfg.materials.player.enemy_overlay_flags.wireframe = m["enemy_overlay_wireframe"].as_bool(cfg.materials.player.enemy_overlay_flags.wireframe);

    // Team
    cfg.materials.player.team = m["team"].as_bool(cfg.materials.player.team);
    rgba_from_json(m["team_color"], cfg.materials.player.team_color);
    cfg.materials.player.team_material_type = (MaterialOverrideConfig::PlayerConfig::MaterialType)m["team_material_type"].as_int((int)cfg.materials.player.team_material_type);
    rgba_from_json(m["team_color_z"], cfg.materials.player.team_color_z);
    cfg.materials.player.team_material_z_type = (MaterialOverrideConfig::PlayerConfig::MaterialType)m["team_material_z_type"].as_int((int)cfg.materials.player.team_material_z_type);
    cfg.materials.player.team_flags.ignore_z = m["team_ignore_z"].as_bool(cfg.materials.player.team_flags.ignore_z);
    cfg.materials.player.team_flags.wireframe = m["team_wireframe"].as_bool(cfg.materials.player.team_flags.wireframe);

    // Friends
    cfg.materials.player.friends = m["friends"].as_bool(cfg.materials.player.friends);
    rgba_from_json(m["friend_color"], cfg.materials.player.friend_color);
    cfg.materials.player.friend_material_type = (MaterialOverrideConfig::PlayerConfig::MaterialType)m["friend_material_type"].as_int((int)cfg.materials.player.friend_material_type);
    rgba_from_json(m["friend_color_z"], cfg.materials.player.friend_color_z);
    cfg.materials.player.friend_material_z_type = (MaterialOverrideConfig::PlayerConfig::MaterialType)m["friend_material_z_type"].as_int((int)cfg.materials.player.friend_material_z_type);
    cfg.materials.player.friends_flags.ignore_z = m["friend_ignore_z"].as_bool(cfg.materials.player.friends_flags.ignore_z);
    cfg.materials.player.friends_flags.wireframe = m["friend_wireframe"].as_bool(cfg.materials.player.friends_flags.wireframe);

    // Local
    cfg.materials.player.local = m["local"].as_bool(cfg.materials.player.local);
    rgba_from_json(m["local_color"], cfg.materials.player.local_color);
    cfg.materials.player.local_material_type = (MaterialOverrideConfig::PlayerConfig::MaterialType)m["local_material_type"].as_int((int)cfg.materials.player.local_material_type);
    cfg.materials.player.local_flags.wireframe = m["local_wireframe"].as_bool(cfg.materials.player.local_flags.wireframe);
  }

  // Viewport
  if (root.has("viewport")) {
    const JsonValue& v = root["viewport"];
    cfg.viewport.filters.scope = v["scope_filter"].as_bool(cfg.viewport.filters.scope);
    cfg.viewport.filters.zoom = v["zoom_filter"].as_bool(cfg.viewport.filters.zoom);
    cfg.viewport.camera_frustum.key.button = v["camera_frustum_key"].as_int(cfg.viewport.camera_frustum.key.button);
    cfg.viewport.camera_frustum.enabled = v["camera_frustum_enabled"].as_bool(cfg.viewport.camera_frustum.enabled);
    cfg.viewport.camera_frustum.x = v["camera_frustum_x"].as_float(cfg.viewport.camera_frustum.x);
    cfg.viewport.camera_frustum.y = v["camera_frustum_y"].as_float(cfg.viewport.camera_frustum.y);
    cfg.viewport.camera_frustum.z = v["camera_frustum_z"].as_float(cfg.viewport.camera_frustum.z);
    cfg.viewport.override_fov = v["override_fov"].as_bool(cfg.viewport.override_fov);
    cfg.viewport.custom_fov = v["custom_fov"].as_float(cfg.viewport.custom_fov);
    cfg.viewport.override_viewmodel_fov = v["override_viewmodel_fov"].as_bool(cfg.viewport.override_viewmodel_fov);
    cfg.viewport.custom_viewmodel_fov = v["custom_viewmodel_fov"].as_float(cfg.viewport.custom_viewmodel_fov);
  }

  // Simulation
  if (root.has("simulation")) {
    const JsonValue& s = root["simulation"];
    cfg.simulation.locomotion.ground_impulse = s["ground_impulse"].as_bool(cfg.simulation.locomotion.ground_impulse);
    cfg.simulation.locomotion.suppress_collision_push = s["suppress_collision_push"].as_bool(cfg.simulation.locomotion.suppress_collision_push);
    cfg.simulation.directives.bypass_sv_pure = s["bypass_sv_pure"].as_bool(cfg.simulation.directives.bypass_sv_pure);
    cfg.simulation.directives.disable_frame_sleep = s["disable_frame_sleep"].as_bool(cfg.simulation.directives.disable_frame_sleep);
    cfg.simulation.directives.tick_scheduler = s["tick_scheduler"].as_bool(cfg.simulation.directives.tick_scheduler);
    cfg.simulation.directives.tick_telemetry_widget = s["tick_telemetry_widget"].as_bool(cfg.simulation.directives.tick_telemetry_widget);
    cfg.simulation.directives.recharge_button.button = s["recharge_button"].as_int(cfg.simulation.directives.recharge_button.button);
    cfg.simulation.automation.auto_class_select = s["auto_class_select"].as_bool(cfg.simulation.automation.auto_class_select);
    cfg.simulation.automation.class_selected = (enum tf_class)s["class_selected"].as_int((int)cfg.simulation.automation.class_selected);
  }

  // Navigation
  if (root.has("navigation")) {
    const JsonValue& n = root["navigation"];
    cfg.navigation.master = n["master"].as_bool(cfg.navigation.master);
    cfg.navigation.walk = n["walk"].as_bool(cfg.navigation.walk);
    cfg.navigation.look_at_path = n["look_at_path"].as_bool(cfg.navigation.look_at_path);
    cfg.navigation.look_smoothness = n["look_smoothness"].as_float(cfg.navigation.look_smoothness);
    cfg.navigation.do_objective = n["do_objective"].as_bool(cfg.navigation.do_objective);
  }

  // Diagnostics
  if (root.has("diagnostics")) {
    const JsonValue& diag = root["diagnostics"];
    cfg.diagnostics.font_height = diag["font_height"].as_int(cfg.diagnostics.font_height);
    cfg.diagnostics.font_weight = diag["font_weight"].as_int(cfg.diagnostics.font_weight);
    cfg.diagnostics.render_all_entities = diag["render_all_entities"].as_bool(cfg.diagnostics.render_all_entities);
    cfg.diagnostics.draw_navigation_path = diag["draw_navigation_path"].as_bool(cfg.diagnostics.draw_navigation_path);
    cfg.diagnostics.draw_navmesh = diag["draw_navmesh"].as_bool(cfg.diagnostics.draw_navmesh);
    cfg.diagnostics.draw_navigation_goal = diag["draw_navigation_goal"].as_bool(cfg.diagnostics.draw_navigation_goal);
    cfg.diagnostics.draw_navigation_current_area = diag["draw_navigation_current_area"].as_bool(cfg.diagnostics.draw_navigation_current_area);
    cfg.diagnostics.draw_navigation_next_area = diag["draw_navigation_next_area"].as_bool(cfg.diagnostics.draw_navigation_next_area);
    cfg.diagnostics.show_active_flag_ids_of_players = diag["show_active_flag_ids_of_players"].as_bool(cfg.diagnostics.show_active_flag_ids_of_players);
    cfg.diagnostics.disable_friend_checks = diag["disable_friend_checks"].as_bool(cfg.diagnostics.disable_friend_checks);

    cfg.diagnostics.bridge_master = diag["bridge_master"].as_bool(cfg.diagnostics.bridge_master);
    cfg.diagnostics.bridge_on_kill = diag["bridge_on_kill"].as_bool(cfg.diagnostics.bridge_on_kill);
    cfg.diagnostics.bridge_on_death = diag["bridge_on_death"].as_bool(cfg.diagnostics.bridge_on_death);
    cfg.diagnostics.username = diag["username"].as_string(cfg.diagnostics.username);
    cfg.diagnostics.sharecode = diag["sharecode"].as_string(cfg.diagnostics.sharecode);
    cfg.diagnostics.apikey = diag["apikey"].as_string(cfg.diagnostics.apikey);
    cfg.diagnostics.intensity = diag["intensity"].as_int(cfg.diagnostics.intensity);
    cfg.diagnostics.duration = diag["duration"].as_int(cfg.diagnostics.duration);
    cfg.diagnostics.operation = (DiagnosticsConfig::BridgeActuationMode)diag["operation"].as_int((int)cfg.diagnostics.operation);
  }

  return true;
}

inline std::string get_config_directory() {
  const char* home = std::getenv("HOME");
  std::filesystem::path dir;
  if (home != nullptr && home[0] != '\0') {
    dir = std::filesystem::path(home) / ".config" / "flakes";
  } else {
    dir = std::filesystem::path("/tmp/flakes");
  }

  std::error_code ec;
  std::filesystem::create_directories(dir, ec);
  if (ec) {
    dir = "/tmp";
  }
  return dir.string();
}

inline std::string sanitize_profile_name(std::string_view name) {
  std::string clean;
  for (char c : name) {
    if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_' || c == '-') {
      clean += c;
    }
  }
  return clean.empty() ? "default" : clean;
}

inline std::string get_profile_path(const std::string& profile_name) {
  std::string clean = sanitize_profile_name(profile_name);
  std::filesystem::path dir = get_config_directory();
  return (dir / (clean + ".json")).string();
}

inline bool save_config_file(const std::string& profile_name = "default", const Config& cfg = config) {
  std::string filepath = get_profile_path(profile_name);
  JsonValue j = config_to_json(cfg);
  std::string json_str = j.dump(2);

  std::ofstream file(filepath, std::ios::out | std::ios::trunc);
  if (!file.is_open()) {
    print("[Config] Failed to open %s for writing\n", filepath.c_str());
    return false;
  }

  file << json_str << "\n";
  file.close();
  print("[Config] Successfully saved profile '%s' to %s\n", profile_name.c_str(), filepath.c_str());
  return true;
}

inline bool load_config_file(const std::string& profile_name = "default", Config& cfg = config) {
  std::string filepath = get_profile_path(profile_name);
  std::ifstream file(filepath, std::ios::in | std::ios::binary);
  if (!file.is_open()) {
    print("[Config] Profile '%s' not found at %s\n", profile_name.c_str(), filepath.c_str());
    return false;
  }

  std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
  file.close();

  if (content.empty()) {
    print("[Config] File %s is empty\n", filepath.c_str());
    return false;
  }

  JsonValue j = JsonValue::parse(content);
  if (!j.is_object()) {
    print("[Config] Failed to parse JSON from %s\n", filepath.c_str());
    return false;
  }

  bool ok = config_from_json(cfg, j);
  if (ok) {
    print("[Config] Successfully loaded profile '%s' from %s\n", profile_name.c_str(), filepath.c_str());
  }
  return ok;
}

inline std::vector<std::string> get_profile_list() {
  std::vector<std::string> profiles;
  std::filesystem::path dir = get_config_directory();
  std::error_code ec;

  if (std::filesystem::exists(dir, ec) && std::filesystem::is_directory(dir, ec)) {
    for (const auto& entry : std::filesystem::directory_iterator(dir, ec)) {
      if (entry.is_regular_file(ec) && entry.path().extension() == ".json") {
        profiles.push_back(entry.path().stem().string());
      }
    }
  }

  if (profiles.empty()) {
    profiles.push_back("default");
  } else {
    std::sort(profiles.begin(), profiles.end());
  }

  return profiles;
}

inline void reset_config_defaults(Config& cfg = config) {
  cfg = Config();
  print("[Config] Reset configuration to factory defaults\n");
}

}
