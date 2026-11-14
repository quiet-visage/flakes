export module gui.menu;
export import std.compat;
export import core.sys;
export import gui.config;
export import subsystems.graph_navigation.navmesh;
export import subsystems.hardware_bridge.hardware_bridge_hpp;
export import core.print;

export extern "C++" {





inline SDL_Window* sdl_window = nullptr;
inline bool menu_focused = false;
inline ImFont* ui_font = nullptr;

alignas(8) inline constexpr unsigned char google_sans_code_nerd_font_ttf[] = {
#embed "GoogleSansCodeNerdFont-Medium.ttf"
};

inline void get_input(SDL_Event* event) {
  ImGui::KeybindEvent(event, &config.targeting.key.waiting, &config.targeting.key.button);
  ImGui::KeybindEvent(event, &config.deflection.key.waiting, &config.deflection.key.button);
  ImGui::KeybindEvent(event, &config.viewport.camera_frustum.key.waiting, &config.viewport.camera_frustum.key.button);
  ImGui::KeybindEvent(event, &config.simulation.directives.recharge_button.waiting, &config.simulation.directives.recharge_button.button);
}

inline void init_imgui_fonts(void) {
  ImGuiIO& io = ImGui::GetIO();
  if (!io.Fonts->Fonts.empty()) {
    return;
  }

  static const ImWchar glyph_ranges[] = {
    0x0020, 0x00FF, // Basic Latin + Latin Supplement
    0x2000, 0x206F, // General Punctuation
    0x2190, 0x21FF, // Arrows
    0x2500, 0x25FF, // Box Drawing & Geometric Shapes
    0xE0A0, 0xE0D4, // Powerline symbols
    0xE5FA, 0xE6B5, // Seti-UI + Custom
    0xE700, 0xE7C5, // Devicons
    0xEA60, 0xEC1E, // Codicons
    0xF000, 0xF2E0, // Font Awesome
    0,
  };

  ImFontConfig font_cfg;
  font_cfg.FontDataOwnedByAtlas = false;
  font_cfg.OversampleH = 2;
  font_cfg.OversampleV = 2;
  font_cfg.PixelSnapH = true;

  ui_font = io.Fonts->AddFontFromMemoryTTF(
    (void*)google_sans_code_nerd_font_ttf,
    (int)sizeof(google_sans_code_nerd_font_ttf),
    15.0f,
    &font_cfg,
    glyph_ranges
  );

  if (ui_font != nullptr) {
    io.FontDefault = ui_font;
    print("Loaded GoogleSansCodeNerdFont-Medium.ttf (%zu bytes)\n", sizeof(google_sans_code_nerd_font_ttf));
  } else {
    print("Failed to load embedded GoogleSansCodeNerdFont-Medium.ttf, falling back to default font\n");
    io.Fonts->AddFontDefault();
  }
}

inline void set_imgui_theme(void) {
  init_imgui_fonts();

  // Framecraft · Obsidian & Mint theme
  ImGuiStyle& style = ImGui::GetStyle();
  ImVec4* colors = style.Colors;

  style.WindowRounding    = 12.0f;
  style.ChildRounding     = 10.0f;
  style.FrameRounding     = 7.0f;
  style.PopupRounding     = 10.0f;
  style.ScrollbarRounding = 9.0f;
  style.GrabRounding      = 6.0f;
  style.TabRounding       = 7.0f;

  style.WindowBorderSize  = 1.0f;
  style.ChildBorderSize   = 1.0f;
  style.PopupBorderSize   = 1.0f;
  style.FrameBorderSize   = 1.0f;

  style.WindowPadding     = ImVec2(14.0f, 14.0f);
  style.FramePadding      = ImVec2(10.0f, 5.0f);
  style.ItemSpacing       = ImVec2(10.0f, 8.0f);
  style.ItemInnerSpacing  = ImVec2(8.0f, 6.0f);
  style.ScrollbarSize     = 11.0f;
  style.GrabMinSize       = 10.0f;

  // Distinct surface elevation hierarchy
  colors[ImGuiCol_WindowBg]             = ImVec4(0.08f, 0.10f, 0.12f, 0.98f);
  colors[ImGuiCol_ChildBg]              = ImVec4(0.11f, 0.14f, 0.17f, 1.00f);
  colors[ImGuiCol_PopupBg]              = ImVec4(0.10f, 0.13f, 0.16f, 0.98f);
  colors[ImGuiCol_Border]               = ImVec4(0.20f, 0.25f, 0.30f, 0.85f);
  colors[ImGuiCol_BorderShadow]         = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);

  // Interactive controls (contrasting against ChildBg)
  colors[ImGuiCol_FrameBg]              = ImVec4(0.16f, 0.20f, 0.25f, 1.00f);
  colors[ImGuiCol_FrameBgHovered]       = ImVec4(0.21f, 0.26f, 0.32f, 1.00f);
  colors[ImGuiCol_FrameBgActive]        = ImVec4(0.25f, 0.31f, 0.38f, 1.00f);

  // Titlebar
  colors[ImGuiCol_TitleBg]              = ImVec4(0.07f, 0.09f, 0.11f, 1.00f);
  colors[ImGuiCol_TitleBgActive]        = ImVec4(0.09f, 0.12f, 0.15f, 1.00f);
  colors[ImGuiCol_TitleBgCollapsed]     = ImVec4(0.07f, 0.09f, 0.11f, 0.75f);

  // Typography
  colors[ImGuiCol_Text]                 = ImVec4(0.92f, 0.94f, 0.96f, 1.00f);
  colors[ImGuiCol_TextDisabled]         = ImVec4(0.50f, 0.56f, 0.63f, 1.00f);

  // Mint Accent
  colors[ImGuiCol_CheckMark]            = ImVec4(0.52f, 0.88f, 0.76f, 1.00f);
  colors[ImGuiCol_SliderGrab]           = ImVec4(0.52f, 0.88f, 0.76f, 0.90f);
  colors[ImGuiCol_SliderGrabActive]     = ImVec4(0.64f, 0.94f, 0.84f, 1.00f);

  // Buttons
  colors[ImGuiCol_Button]               = ImVec4(0.16f, 0.20f, 0.25f, 1.00f);
  colors[ImGuiCol_ButtonHovered]        = ImVec4(0.52f, 0.88f, 0.76f, 0.28f);
  colors[ImGuiCol_ButtonActive]         = ImVec4(0.52f, 0.88f, 0.76f, 0.42f);

  // Headers / Combos / Selectables
  colors[ImGuiCol_Header]               = ImVec4(0.52f, 0.88f, 0.76f, 0.22f);
  colors[ImGuiCol_HeaderHovered]        = ImVec4(0.52f, 0.88f, 0.76f, 0.34f);
  colors[ImGuiCol_HeaderActive]         = ImVec4(0.52f, 0.88f, 0.76f, 0.46f);

  // Separators & Scrollbars
  colors[ImGuiCol_Separator]            = ImVec4(0.22f, 0.27f, 0.33f, 0.85f);
  colors[ImGuiCol_SeparatorHovered]     = ImVec4(0.52f, 0.88f, 0.76f, 0.65f);
  colors[ImGuiCol_SeparatorActive]      = ImVec4(0.52f, 0.88f, 0.76f, 0.90f);
  colors[ImGuiCol_ScrollbarBg]          = ImVec4(0.08f, 0.10f, 0.12f, 0.60f);
  colors[ImGuiCol_ScrollbarGrab]        = ImVec4(0.24f, 0.29f, 0.36f, 1.00f);
  colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.32f, 0.39f, 0.47f, 1.00f);
  colors[ImGuiCol_ScrollbarGrabActive]  = ImVec4(0.52f, 0.88f, 0.76f, 0.85f);
}

inline void draw_kinematics_tab(void) {
  ImGui::Checkbox("Enable Vector Targeting Subsystem", &config.targeting.master);
  ImGui::Separator();
  ImGui::Spacing();

  float col_width = (ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x) * 0.5f;

  ImGui::BeginChild("##KinematicsTargetingCard", ImVec2(col_width, 0), ImGuiChildFlags_Borders);
  ImGui::SectionHeader("\xEF\x81\x9B  Vector Solver & Actuation");

  ImGui::Checkbox("Autonomous Fire Dispatch", &config.targeting.auto_shoot);
  ImGui::Checkbox("Silent Angular Projection", &config.targeting.silent);

  ImGui::Spacing();
  ImGui::TextUnformatted("Target Selection Heuristic");
  const char* target_items[] = { "Angular Delta (FOV)", "Euclidean Distance", "Minimum Vitality", "Maximum Vitality" };
  ImGui::SetNextItemWidth(-1);
  ImGui::Combo("##TargetType", (int*)&config.targeting.target_type, target_items, IM_ARRAYSIZE(target_items));

  ImGui::Spacing();
  ImGui::TextUnformatted("Angular Acceptance Cone (FOV)");
  ImGui::SetNextItemWidth(-1);
  ImGui::SliderFloat("##TargetingFOV", &config.targeting.fov, 0.1f, 180.0f, "%.0f\xC2\xB0");
  ImGui::Checkbox("Project Acceptance Cone Overlay", &config.targeting.draw_fov);

  ImGui::Spacing();
  ImGui::Separator();
  ImGui::Spacing();

  ImGui::Checkbox("Require Manual Trigger Gate", &config.targeting.use_key);
  ImGui::TextUnformatted("Trigger Gate Key:");
  ImGui::SameLine();
  ImGui::KeybindBox(&config.targeting.key.waiting, &config.targeting.key.button, "TargetingKey");
  ImGui::EndChild();

  ImGui::SameLine();

  ImGui::BeginChild("##KinematicsDeflectionAndFilterCard", ImVec2(0, 0), ImGuiChildFlags_Borders);
  ImGui::SectionHeader("\xEF\x81\xAD  Directional Active Deflection");
  ImGui::Checkbox("Enable Active Deflection (Pyro Only)", &config.deflection.master);
  ImGui::Checkbox("Require Manual Trigger Gate##Deflection", &config.deflection.use_key);
  ImGui::TextUnformatted("Trigger Gate Key:");
  ImGui::SameLine();
  ImGui::KeybindBox(&config.deflection.key.waiting, &config.deflection.key.button, "DeflectionKey");

  ImGui::Spacing();
  ImGui::TextUnformatted("Deflection Cone Width (FOV)");
  ImGui::SetNextItemWidth(-1);
  ImGui::SliderFloat("##DeflectionFOV", &config.deflection.fov, 10.0f, 180.0f, "%.0f\xC2\xB0");

  ImGui::TextUnformatted("Forward Reach (Hammer Units)");
  ImGui::SetNextItemWidth(-1);
  ImGui::SliderFloat("##DeflectionRange", &config.deflection.range, 50.0f, 512.0f, "%.0f HU");

  ImGui::Spacing();
  ImGui::TextUnformatted("Deflectable Projectile Classes");
  ImGui::Checkbox("Soldier / Sentry Missiles##Deflection", &config.deflection.projectiles.rockets);
  ImGui::Checkbox("Demoman Grenades##Deflection", &config.deflection.projectiles.grenades);
  ImGui::Checkbox("Demoman Sticky Bombs##Deflection", &config.deflection.projectiles.stickies);
  ImGui::Checkbox("Arrows & Bolts##Deflection", &config.deflection.projectiles.arrows);
  ImGui::Checkbox("Flares & Fireballs##Deflection", &config.deflection.projectiles.flares);
  ImGui::Checkbox("Throwables & Liquids##Deflection", &config.deflection.projectiles.throwables);

  ImGui::Spacing();
  ImGui::Separator();
  ImGui::Spacing();

  ImGui::SectionHeader("\xEF\x86\x92  Optics State Machine");
  ImGui::Checkbox("Auto-Transition Scope", &config.targeting.auto_scope);
  ImGui::Checkbox("Auto-Unscope Post-Discharge", &config.targeting.auto_unscope);
  ImGui::Checkbox("Require Scoped State", &config.targeting.scoped_only);

  ImGui::Spacing();
  ImGui::NewLine();
  ImGui::SectionHeader("\xEF\x83\x80  Actor Filtering");
  ImGui::Checkbox("Exclude Steam Friends", &config.targeting.ignore_friends);
  ImGui::Checkbox("Suppress on 1st-Person Observation", &config.targeting.ignore_spectated);
  ImGui::EndChild();
}

inline void draw_telemetry_tab(void) {
  ImGui::Checkbox("Enable Spatial Telemetry & HUD Projection", &config.telemetry.master);
  ImGui::SameLine();
  ImGui::Checkbox("Observer Telemetry Overlay", &config.telemetry.observer_overlay);
  ImGui::Separator();
  ImGui::Spacing();

  float avail_x = ImGui::GetContentRegionAvail().x;
  float spacing = ImGui::GetStyle().ItemSpacing.x;
  float col_width = (avail_x - spacing * 2.0f) / 3.0f;

  // Column 1: Actors
  ImGui::BeginChild("##TelemetryPlayerChild", ImVec2(col_width, 0), ImGuiChildFlags_Borders);
  ImGui::SectionHeader("\xEF\x80\x87  Actor Telemetry");

  ImGui::Checkbox("Opposing Team##TelemetryPlayer", &config.telemetry.player.enemy);
  ImGui::Checkbox("Allied Team##TelemetryPlayer", &config.telemetry.player.team);
  ImGui::Checkbox("Steam Friends##TelemetryPlayer", &config.telemetry.player.friends);

  ImGui::Spacing();
  ImGui::Separator();
  ImGui::Spacing();

  ImGui::Checkbox("2D Bounding Volume##TelemetryPlayer", &config.telemetry.player.box);
  ImGui::Checkbox("Vitality Bar##TelemetryPlayer", &config.telemetry.player.health_bar);
  ImGui::Checkbox("Actor Identifier##TelemetryPlayer", &config.telemetry.player.name);

  ImGui::Spacing();
  ImGui::TextUnformatted("State Annotations");
  ImGui::SetNextItemWidth(-1);
  if (ImGui::BeginCombo("##PlayerFlags", "Select Annotations...")) {
    ImGui::Checkbox("Locked Target##TelemetryPlayer", &config.telemetry.player.flags.target_indicator);
    ImGui::Checkbox("Friend Marker##TelemetryPlayer", &config.telemetry.player.flags.friend_indicator);
    ImGui::Checkbox("Scoped Condition##TelemetryPlayer", &config.telemetry.player.flags.scoped_indicator);
    ImGui::EndCombo();
  }

  ImGui::Spacing();
  ImGui::Separator();
  ImGui::Spacing();

  ImGui::ColorEdit4("Opposing Color##TelemetryPlayer", config.telemetry.player.enemy_color.to_arr(), ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoInputs);
  ImGui::ColorEdit4("Allied Color##TelemetryPlayer", config.telemetry.player.team_color.to_arr(), ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoInputs);
  ImGui::ColorEdit4("Friend Color##TelemetryPlayer", config.telemetry.player.friend_color.to_arr(), ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoInputs);
  ImGui::EndChild();

  ImGui::SameLine();

  // Column 2: World Resources & Objectives
  ImGui::BeginChild("##TelemetryPickupChild", ImVec2(col_width, 0), ImGuiChildFlags_Borders);
  ImGui::SectionHeader("\xEF\x83\xBA  Resource Nodes");
  ImGui::Checkbox("2D Bounding Volume##TelemetryPickup", &config.telemetry.pickup.box);
  ImGui::Checkbox("Resource Class Label##TelemetryPickup", &config.telemetry.pickup.name);

  ImGui::Spacing();
  ImGui::NewLine();
  ImGui::SectionHeader("\xEF\x80\xA4  Map Objectives");
  ImGui::Checkbox("Objective Bounding Volume##TelemetryIntelligence", &config.telemetry.intelligence.box);
  ImGui::Checkbox("Objective Identifier##TelemetryIntelligence", &config.telemetry.intelligence.name);
  ImGui::EndChild();

  ImGui::SameLine();

  // Column 3: Engineered Constructs
  ImGui::BeginChild("##TelemetryBuildingsChild", ImVec2(0, 0), ImGuiChildFlags_Borders);
  ImGui::SectionHeader("\xEF\x82\x85  Constructs");
  ImGui::Checkbox("Include Allied Constructs##TelemetryBuildings", &config.telemetry.buildings.team);

  ImGui::Spacing();
  ImGui::Separator();
  ImGui::Spacing();

  ImGui::Checkbox("2D Bounding Volume##TelemetryBuildings", &config.telemetry.buildings.box);
  ImGui::Checkbox("Structural Integrity Bar##TelemetryBuildings", &config.telemetry.buildings.health_bar);
  ImGui::Checkbox("Construct Class Label##TelemetryBuildings", &config.telemetry.buildings.name);
  ImGui::EndChild();
}

inline void draw_materials_tab(void) {
  ImGui::Checkbox("Enable Material Pipeline & Shader Override", &config.materials.master);
  ImGui::Separator();
  ImGui::Spacing();

  const char* target_material_types[] = { "Unmodified", "Flat Lit", "Gouraud Shaded", "Fresnel Rim" };
  float col_width = (ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x) * 0.5f;

  // Left Column: Opposing & Local Actors
  ImGui::BeginChild("##MaterialsLeftCol", ImVec2(col_width, 0), ImGuiChildFlags_Borders);
  ImGui::SectionHeader("\xEF\x86\xB2  Opposing Actors");
  ImGui::Checkbox("Override Opposing Materials##MaterialsEnemyPlayer", &config.materials.player.enemy);

  ImGui::ColorEdit4("Visible##MaterialsEnemyPlayer", config.materials.player.enemy_color.to_arr(), ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoInputs);
  ImGui::SameLine(130);
  ImGui::SetNextItemWidth(-1);
  ImGui::Combo("##MaterialsEnemyPlayerMaterial", (int*)&config.materials.player.enemy_material_type, target_material_types, IM_ARRAYSIZE(target_material_types));

  ImGui::ColorEdit4("Z-Occluded##MaterialsEnemyPlayer", config.materials.player.enemy_color_z.to_arr(), ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoInputs);
  ImGui::SameLine(130);
  ImGui::SetNextItemWidth(-1);
  ImGui::Combo("##MaterialsEnemyPlayerMaterialz", (int*)&config.materials.player.enemy_material_z_type, target_material_types, IM_ARRAYSIZE(target_material_types));

  ImGui::SetNextItemWidth(-1);
  if (ImGui::BeginCombo("##MaterialsEnemyPlayerFlags", "Primary Pass Flags...")) {
    ImGui::Checkbox("Bypass Depth Buffer (XQZ)##EnemyBase", &config.materials.player.enemy_flags.ignore_z);
    ImGui::Checkbox("Wireframe Rasterization##EnemyBase", &config.materials.player.enemy_flags.wireframe);
    ImGui::EndCombo();
  }

  ImGui::Spacing();
  ImGui::TextDisabled("Secondary Shader Pass");
  ImGui::ColorEdit4("Visible##OverlayMaterialsEnemyPlayer", config.materials.player.enemy_overlay_color.to_arr(), ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoInputs);
  ImGui::SameLine(130);
  ImGui::SetNextItemWidth(-1);
  ImGui::Combo("##OverlayMaterialsEnemyPlayerMaterial", (int*)&config.materials.player.enemy_overlay_material_type, target_material_types, IM_ARRAYSIZE(target_material_types));

  ImGui::ColorEdit4("Z-Occluded##OverlayMaterialsEnemyPlayer", config.materials.player.enemy_overlay_color_z.to_arr(), ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoInputs);
  ImGui::SameLine(130);
  ImGui::SetNextItemWidth(-1);
  ImGui::Combo("##OverlayMaterialsEnemyPlayerMaterialz", (int*)&config.materials.player.enemy_overlay_material_z_type, target_material_types, IM_ARRAYSIZE(target_material_types));

  ImGui::SetNextItemWidth(-1);
  if (ImGui::BeginCombo("##OverlayMaterialsEnemyPlayerFlags", "Secondary Pass Flags...")) {
    ImGui::Checkbox("Bypass Depth Buffer (XQZ)##EnemyOverlay", &config.materials.player.enemy_overlay_flags.ignore_z);
    ImGui::Checkbox("Wireframe Rasterization##EnemyOverlay", &config.materials.player.enemy_overlay_flags.wireframe);
    ImGui::EndCombo();
  }

  ImGui::Spacing();
  ImGui::Separator();
  ImGui::Spacing();

  ImGui::SectionHeader("\xEF\x80\x87  Local Actor");
  ImGui::Checkbox("Override Local Material##MaterialsLocalPlayer", &config.materials.player.local);
  ImGui::ColorEdit4("Visible##MaterialsLocalPlayer", config.materials.player.local_color.to_arr(), ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoInputs);
  ImGui::SameLine(130);
  ImGui::SetNextItemWidth(-1);
  ImGui::Combo("##MaterialsLocalPlayerMaterial", (int*)&config.materials.player.local_material_type, target_material_types, IM_ARRAYSIZE(target_material_types));
  ImGui::Checkbox("Wireframe Rasterization##MaterialsLocalWireframe", &config.materials.player.local_flags.wireframe);
  ImGui::EndChild();

  ImGui::SameLine();

  // Right Column: Allied & Friend Actors
  ImGui::BeginChild("##MaterialsRightCol", ImVec2(0, 0), ImGuiChildFlags_Borders);
  ImGui::SectionHeader("\xEF\x83\x80  Allied Actors");
  ImGui::Checkbox("Override Allied Materials##MaterialsTeamPlayer", &config.materials.player.team);

  ImGui::ColorEdit4("Visible##MaterialsTeamPlayer", config.materials.player.team_color.to_arr(), ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoInputs);
  ImGui::SameLine(130);
  ImGui::SetNextItemWidth(-1);
  ImGui::Combo("##MaterialsTeamPlayerMaterial", (int*)&config.materials.player.team_material_type, target_material_types, IM_ARRAYSIZE(target_material_types));

  ImGui::ColorEdit4("Z-Occluded##MaterialsTeamPlayer", config.materials.player.team_color_z.to_arr(), ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoInputs);
  ImGui::SameLine(130);
  ImGui::SetNextItemWidth(-1);
  ImGui::Combo("##MaterialsTeamPlayerMaterialz", (int*)&config.materials.player.team_material_z_type, target_material_types, IM_ARRAYSIZE(target_material_types));

  ImGui::SetNextItemWidth(-1);
  if (ImGui::BeginCombo("##MaterialsTeamPlayerFlags", "Allied Pass Flags...")) {
    ImGui::Checkbox("Bypass Depth Buffer (XQZ)##Team", &config.materials.player.team_flags.ignore_z);
    ImGui::Checkbox("Wireframe Rasterization##Team", &config.materials.player.team_flags.wireframe);
    ImGui::EndCombo();
  }

  ImGui::Spacing();
  ImGui::Separator();
  ImGui::Spacing();

  ImGui::SectionHeader("\xEF\x80\x84  Friend Actors");
  ImGui::Checkbox("Override Friend Materials##MaterialsFriendPlayer", &config.materials.player.friends);

  ImGui::ColorEdit4("Visible##MaterialsFriendPlayer", config.materials.player.friend_color.to_arr(), ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoInputs);
  ImGui::SameLine(130);
  ImGui::SetNextItemWidth(-1);
  ImGui::Combo("##MaterialsFriendPlayerMaterial", (int*)&config.materials.player.friend_material_type, target_material_types, IM_ARRAYSIZE(target_material_types));

  ImGui::ColorEdit4("Z-Occluded##MaterialsFriendPlayer", config.materials.player.friend_color_z.to_arr(), ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoInputs);
  ImGui::SameLine(130);
  ImGui::SetNextItemWidth(-1);
  ImGui::Combo("##MaterialsFriendPlayerMaterialz", (int*)&config.materials.player.friend_material_z_type, target_material_types, IM_ARRAYSIZE(target_material_types));

  ImGui::SetNextItemWidth(-1);
  if (ImGui::BeginCombo("##MaterialsFriendPlayerFlags", "Friend Pass Flags...")) {
    ImGui::Checkbox("Bypass Depth Buffer (XQZ)##Friend", &config.materials.player.friends_flags.ignore_z);
    ImGui::Checkbox("Wireframe Rasterization##Friend", &config.materials.player.friends_flags.wireframe);
    ImGui::EndCombo();
  }
  ImGui::EndChild();
}

inline void draw_viewport_tab(void) {
  float col_width = (ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x) * 0.5f;

  // Left Column: Camera Frustum
  ImGui::BeginChild("##ViewportCameraCard", ImVec2(col_width, 0), ImGuiChildFlags_Borders);
  ImGui::SectionHeader("\xEF\x80\xB0  Camera Frustum & External View");

  ImGui::Checkbox("Enable External Camera View", &config.viewport.camera_frustum.enabled);
  ImGui::TextUnformatted("Toggle Keybind:");
  ImGui::SameLine();
  ImGui::KeybindBox(&config.viewport.camera_frustum.key.waiting, &config.viewport.camera_frustum.key.button, "CameraFrustumKey");

  ImGui::Spacing();
  ImGui::Separator();
  ImGui::Spacing();

  ImGui::TextUnformatted("Longitudinal Offset (Z)");
  ImGui::SetNextItemWidth(-1);
  ImGui::SliderFloat("##CameraZ", &config.viewport.camera_frustum.z, 20.0f, 500.0f, "%.1f u");

  ImGui::TextUnformatted("Lateral Offset (Y)");
  ImGui::SetNextItemWidth(-1);
  ImGui::SliderFloat("##CameraY", &config.viewport.camera_frustum.y, -50.0f, 50.0f, "%.1f u");

  ImGui::TextUnformatted("Vertical Offset (X)");
  ImGui::SetNextItemWidth(-1);
  ImGui::SliderFloat("##CameraX", &config.viewport.camera_frustum.x, -50.0f, 50.0f, "%.1f u");
  ImGui::EndChild();

  ImGui::SameLine();

  // Right Column: Field of View & Post-Process Filters
  ImGui::BeginChild("##ViewportFovAndFiltersCard", ImVec2(0, 0), ImGuiChildFlags_Borders);
  ImGui::SectionHeader("\xEF\x81\xAE  Projection Matrix (FOV)");

  ImGui::Checkbox("Override World Projection FOV", &config.viewport.override_fov);
  ImGui::SetNextItemWidth(-1);
  ImGui::SliderFloat("##OverrideFOV", &config.viewport.custom_fov, 30.1f, 150.0f, "%.0f\xC2\xB0");

  ImGui::Spacing();
  ImGui::Checkbox("Override Viewmodel Projection FOV", &config.viewport.override_viewmodel_fov);
  ImGui::SetNextItemWidth(-1);
  ImGui::SliderFloat("##OverrideViewmodelFOV", &config.viewport.custom_viewmodel_fov, 30.1f, 150.0f, "%.0f\xC2\xB0");

  ImGui::Spacing();
  ImGui::NewLine();
  ImGui::SectionHeader("\xEF\x81\xB0  HUD Post-Process Filters");
  ImGui::Checkbox("Suppress Scope Reticle Overlay", &config.viewport.filters.scope);
  ImGui::Checkbox("Suppress Optical Zoom Magnification", &config.viewport.filters.zoom);
  ImGui::EndChild();
}

inline void draw_simulation_tab(void) {
  float col_width = (ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x) * 0.5f;

  ImGui::BeginChild("##SimulationLocomotionCard", ImVec2(col_width, 0), ImGuiChildFlags_Borders);
  ImGui::SectionHeader("\xEF\x80\x93  Locomotion Kinematics");
  ImGui::Checkbox("Continuous Ground-Impulse Actuation", &config.simulation.locomotion.ground_impulse);
  ImGui::Checkbox("Suppress Entity Collision Push", &config.simulation.locomotion.suppress_collision_push);

  ImGui::Spacing();
  ImGui::NewLine();
  ImGui::SectionHeader("\xEF\x83\xA7  Engine Runtime Directives");
  ImGui::Checkbox("Bypass sv_pure Whitelist Check", &config.simulation.directives.bypass_sv_pure);
  ImGui::Checkbox("Disable Frame Sleep Throttling", &config.simulation.directives.disable_frame_sleep);
  ImGui::EndChild();

  ImGui::SameLine();

  ImGui::BeginChild("##SimulationSchedulerCard", ImVec2(0, 0), ImGuiChildFlags_Borders);
  ImGui::SectionHeader("\xEF\x80\x97  Command Queue & Tick Scheduler");
  ImGui::Checkbox("Enable Command Batch Shift", &config.simulation.directives.tick_scheduler);
  ImGui::Checkbox("Render Queue Telemetry Widget", &config.simulation.directives.tick_telemetry_widget);

  ImGui::Spacing();
  ImGui::TextUnformatted("Packet Choke / Recharge Key:");
  ImGui::SameLine();
  ImGui::KeybindBox(&config.simulation.directives.recharge_button.waiting, &config.simulation.directives.recharge_button.button, "RechargeKey");
  ImGui::EndChild();
}

inline void draw_navigation_tab(void) {
  ImGui::Checkbox("Enable Autonomous A* Graph Navigation", &config.navigation.master);
  ImGui::Separator();
  ImGui::Spacing();

  ImGui::BeginChild("##NavigationCard", ImVec2(0, 0), ImGuiChildFlags_Borders);
  ImGui::SectionHeader("\xEF\x84\xA4  MicroPather Waypoint Solver");
  ImGui::Checkbox("Actuate Waypoint Locomotion", &config.navigation.walk);

  ImGui::Spacing();
  ImGui::Checkbox("Orient View Vector to Active Path", &config.navigation.look_at_path);
  ImGui::TextUnformatted("Angular Interpolation Smoothing");
  ImGui::SetNextItemWidth(280);
  ImGui::SliderFloat("##LookSmoothness", &config.navigation.look_smoothness, 0.0f, 100.0f, "%.1f");
  ImGui::EndChild();
}

inline void draw_diagnostics_tab(void) {
  ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.95f, 0.76f, 0.35f, 1.00f));
  ImGui::TextUnformatted("\xEF\x81\xB1  Runtime Introspection & Experimental Diagnostics");
  ImGui::PopStyleColor();
  ImGui::Separator();
  ImGui::Spacing();

  float col_width = (ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x) * 0.5f;

  // Left Column: Engine & Navmesh Introspection
  ImGui::BeginChild("##DiagnosticsLeftCol", ImVec2(col_width, 0), ImGuiChildFlags_Borders);
  ImGui::SectionHeader("\xEF\x86\x88  Surface & Navmesh Introspection");

  ImGui::TextUnformatted("VGUI Surface Font Metrics");
  ImGui::SetNextItemWidth(-1);
  ImGui::SliderInt("##FontHeight", &config.diagnostics.font_height, 6, 60, "Height: %d px");
  ImGui::SetNextItemWidth(-1);
  ImGui::SliderInt("##FontWeight", &config.diagnostics.font_weight, 50, 800, "Weight: %d");

  ImGui::Spacing();
  ImGui::Separator();
  ImGui::Spacing();

  ImGui::Checkbox("Enumerate All World Entities", &config.diagnostics.render_all_entities);
  ImGui::Checkbox("Render Actor Condition Flag IDs", &config.diagnostics.show_active_flag_ids_of_players);
  ImGui::Checkbox("Bypass Friend Heuristic Checks", &config.diagnostics.disable_friend_checks);

  ImGui::Spacing();
  ImGui::Separator();
  ImGui::Spacing();

  ImGui::Checkbox("Render Navmesh Polygons", &config.diagnostics.draw_navmesh);
  ImGui::Checkbox("Render Computed A* Path", &config.diagnostics.draw_navigation_path);
  ImGui::Checkbox("Highlight Current Nav Area", &config.diagnostics.draw_navigation_current_area);
  ImGui::Checkbox("Highlight Next Nav Area", &config.diagnostics.draw_navigation_next_area);
  ImGui::Checkbox("Highlight Terminal Goal Area", &config.diagnostics.draw_navigation_goal);

  ImGui::Spacing();
  if (ImGui::Button("Invalidate & Reparse .nav Mesh", ImVec2(-1, 0))) {
    mesh.map_name = "";
  }
  ImGui::EndChild();

  ImGui::SameLine();

  // Right Column: External Hardware Telemetry Bridge
  ImGui::BeginChild("##DiagnosticsRightCol", ImVec2(0, 0), ImGuiChildFlags_Borders);
  ImGui::SectionHeader("\xEF\x83\xA7  Hardware Telemetry Bridge");

  ImGui::Checkbox("Enable Webhook Dispatch", &config.diagnostics.bridge_master);
  ImGui::SameLine();
  ImGui::PushButtonRepeat(false);
  if (ImGui::Button("Dispatch Test", ImVec2(-1, 0))) {
    dispatch_hardware_telemetry();
  }
  ImGui::PopButtonRepeat();

  ImGui::Checkbox("Dispatch on Kill Event", &config.diagnostics.bridge_on_kill);
  ImGui::SameLine();
  ImGui::Checkbox("Dispatch on Death Event", &config.diagnostics.bridge_on_death);

  ImGui::Spacing();
  ImGui::Separator();
  ImGui::Spacing();

  const char* operation_types[] = { "Pulse", "Vibrate", "Acoustic Tone" };
  ImGui::TextUnformatted("Actuation Mode");
  ImGui::SetNextItemWidth(-1);
  ImGui::Combo("##BridgeOperation", (int*)&config.diagnostics.operation, operation_types, IM_ARRAYSIZE(operation_types));

  ImGui::SetNextItemWidth(-1);
  ImGui::SliderInt("##BridgeIntensity", &config.diagnostics.intensity, 1, 100, "Intensity: %d%%");
  ImGui::SetNextItemWidth(-1);
  ImGui::SliderInt("##BridgeDuration", &config.diagnostics.duration, 1, 15, "Duration: %d s");

  ImGui::Spacing();
  ImGui::TextUnformatted("Account Username");
  ImGui::SetNextItemWidth(-1);
  ImGui::InputText("##BridgeUser", &config.diagnostics.username);

  ImGui::TextUnformatted("Device Share Code");
  ImGui::SetNextItemWidth(-1);
  ImGui::InputText("##BridgeSharecode", &config.diagnostics.sharecode);

  ImGui::TextUnformatted("REST API Key");
  ImGui::SetNextItemWidth(-1);
  ImGui::InputText("##BridgeApiKey", &config.diagnostics.apikey, ImGuiInputTextFlags_Password);
  ImGui::EndChild();
}

inline void draw_tab(ImGuiStyle* style, const char* name, int* tab, int index) {
  bool selected = (*tab == index);
  if (selected) {
    ImGui::PushStyleColor(ImGuiCol_Button, style->Colors[ImGuiCol_ButtonActive]);
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.52f, 0.88f, 0.76f, 0.80f));
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.98f, 1.00f, 0.99f, 1.00f));
  } else {
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.13f, 0.16f, 0.20f, 0.75f));
  }

  ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.10f, 0.50f));
  if (ImGui::Button(name, ImVec2(-1, 34))) {
    *tab = index;
  }
  ImGui::PopStyleVar();

  if (selected) {
    ImGui::PopStyleColor(3);
  } else {
    ImGui::PopStyleColor(1);
  }
}

inline void draw_menu(void) {
  ImGuiStyle* style = &ImGui::GetStyle();
  style->WindowTitleAlign = ImVec2(0.02f, 0.5f);

  ImGui::SetNextWindowSize(ImVec2(800, 500), ImGuiCond_Always);
  if (ImGui::Begin("Flake Research & Development  \xC2\xB7  Source SDK 2013 Instrumentation", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse)) {
    static int tab = 0;

    // Left Navigation Sidebar
    ImGui::BeginChild("##NavigationSidebar", ImVec2(162, 0), ImGuiChildFlags_Borders);
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.52f, 0.88f, 0.76f, 1.00f));
    ImGui::TextCentered("SUBSYSTEMS");
    ImGui::PopStyleColor();
    ImGui::Separator();
    ImGui::Spacing();

    draw_tab(style, "\xEF\x81\x9B  Kinematics",  &tab, 0);
    draw_tab(style, "\xEF\x81\xAE  Telemetry",   &tab, 1);
    draw_tab(style, "\xEF\x86\xB2  Materials",   &tab, 2);
    draw_tab(style, "\xEF\x80\xB0  Viewport",    &tab, 3);
    draw_tab(style, "\xEF\x80\x93  Simulation",  &tab, 4);
    draw_tab(style, "\xEF\x84\xA4  Navigation",  &tab, 5);
    draw_tab(style, "\xEF\x86\x88  Diagnostics", &tab, 6);

    float footer_y = ImGui::GetWindowHeight() - 30.0f;
    if (ImGui::GetCursorPosY() < footer_y) {
      ImGui::SetCursorPosY(footer_y);
    }
    ImGui::Separator();
    ImGui::PushStyleColor(ImGuiCol_Text, style->Colors[ImGuiCol_TextDisabled]);
    ImGui::TextCentered("End / F11");
    ImGui::PopStyleColor();
    ImGui::EndChild();

    ImGui::SameLine();

    // Right Main Content Area
    ImGui::BeginChild("##MainTabContent", ImVec2(0, 0), ImGuiChildFlags_None);
    switch (tab) {
    case 0:
      draw_kinematics_tab();
      break;
    case 1:
      draw_telemetry_tab();
      break;
    case 2:
      draw_materials_tab();
      break;
    case 3:
      draw_viewport_tab();
      break;
    case 4:
      draw_simulation_tab();
      break;
    case 5:
      draw_navigation_tab();
      break;
    case 6:
      draw_diagnostics_tab();
      break;
    }
    ImGui::EndChild();
  }

  ImGui::End();
}


}
