#ifndef MENU_HPP
#define MENU_HPP

#include <string>
#include "config.hpp"

#include "../imgui/dearimgui.hpp"

#include "../imgui/imgui.h"
#include "../imgui/imgui_internal.h"
#include "../imgui/imgui_stdlib.h"

#include <SDL2/SDL_mouse.h>

#include "../hacks/navbot/navmesh.hpp"
#include "../hacks/pishock/pishock.hpp"
#include "../print.hpp"

inline static SDL_Window* sdl_window = NULL;
inline static bool menu_focused = false;
inline static ImFont* ui_font = nullptr;

alignas(8) static constexpr unsigned char google_sans_code_nerd_font_ttf[] = {
#embed "GoogleSansCodeNerdFont-Medium.ttf"
};

static void get_input(SDL_Event* event) {
  ImGui::KeybindEvent(event, &config.aimbot.key.waiting, &config.aimbot.key.button);
  ImGui::KeybindEvent(event, &config.airblast.key.waiting, &config.airblast.key.button);
  ImGui::KeybindEvent(event, &config.visuals.thirdperson.key.waiting, &config.visuals.thirdperson.key.button);
  ImGui::KeybindEvent(event, &config.misc.exploits.recharge_button.waiting, &config.misc.exploits.recharge_button.button);
}

static void init_imgui_fonts(void) {
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

static void set_imgui_theme(void) {
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

static void draw_aim_tab(void) {
  ImGui::Checkbox("Enable Vector Targeting Subsystem", &config.aimbot.master);
  ImGui::Separator();
  ImGui::Spacing();

  float col_width = (ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x) * 0.5f;

  ImGui::BeginChild("##AimGeneralCard", ImVec2(col_width, 0), ImGuiChildFlags_Borders);
  ImGui::SectionHeader("\xEF\x81\x9B  Vector Solver & Actuation");

  ImGui::Checkbox("Autonomous Fire Dispatch", &config.aimbot.auto_shoot);
  ImGui::Checkbox("Silent Angular Projection", &config.aimbot.silent);

  ImGui::Spacing();
  ImGui::TextUnformatted("Target Selection Heuristic");
  const char* target_items[] = { "Angular Delta (FOV)", "Euclidean Distance", "Minimum Vitality", "Maximum Vitality" };
  ImGui::SetNextItemWidth(-1);
  ImGui::Combo("##TargetType", (int*)&config.aimbot.target_type, target_items, IM_ARRAYSIZE(target_items));

  ImGui::Spacing();
  ImGui::TextUnformatted("Angular Acceptance Cone (FOV)");
  ImGui::SetNextItemWidth(-1);
  ImGui::SliderFloat("##AimFOV", &config.aimbot.fov, 0.1f, 180.0f, "%.0f\xC2\xB0");
  ImGui::Checkbox("Project Acceptance Cone Overlay", &config.aimbot.draw_fov);

  ImGui::Spacing();
  ImGui::Separator();
  ImGui::Spacing();

  ImGui::Checkbox("Require Manual Trigger Gate", &config.aimbot.use_key);
  ImGui::TextUnformatted("Trigger Gate Key:");
  ImGui::SameLine();
  ImGui::KeybindBox(&config.aimbot.key.waiting, &config.aimbot.key.button, "AimKey");
  ImGui::EndChild();

  ImGui::SameLine();

  ImGui::BeginChild("##AimClassAndFilterCard", ImVec2(0, 0), ImGuiChildFlags_Borders);
  ImGui::SectionHeader("\xEF\x81\xAD  Pyro Auto Airblast");
  ImGui::Checkbox("Enable Auto Airblast (Pyro Only)", &config.airblast.master);
  ImGui::Checkbox("Require Manual Trigger Gate##Airblast", &config.airblast.use_key);
  ImGui::TextUnformatted("Trigger Gate Key:");
  ImGui::SameLine();
  ImGui::KeybindBox(&config.airblast.key.waiting, &config.airblast.key.button, "AirblastKey");

  ImGui::Spacing();
  ImGui::TextUnformatted("Deflection Cone Width (FOV)");
  ImGui::SetNextItemWidth(-1);
  ImGui::SliderFloat("##AirblastFOV", &config.airblast.fov, 10.0f, 180.0f, "%.0f\xC2\xB0");

  ImGui::TextUnformatted("Forward Reach (Hammer Units)");
  ImGui::SetNextItemWidth(-1);
  ImGui::SliderFloat("##AirblastRange", &config.airblast.range, 50.0f, 512.0f, "%.0f HU");

  ImGui::Spacing();
  ImGui::TextUnformatted("Deflectable Projectile Classes");
  ImGui::Checkbox("Soldier / Sentry Missiles##Airblast", &config.airblast.projectiles.rockets);
  ImGui::Checkbox("Demoman Grenades##Airblast", &config.airblast.projectiles.grenades);
  ImGui::Checkbox("Demoman Sticky Bombs##Airblast", &config.airblast.projectiles.stickies);
  ImGui::Checkbox("Arrows & Bolts##Airblast", &config.airblast.projectiles.arrows);
  ImGui::Checkbox("Flares & Fireballs##Airblast", &config.airblast.projectiles.flares);
  ImGui::Checkbox("Throwables & Liquids##Airblast", &config.airblast.projectiles.throwables);

  ImGui::Spacing();
  ImGui::Separator();
  ImGui::Spacing();

  ImGui::SectionHeader("\xEF\x86\x92  Optics State Machine");
  ImGui::Checkbox("Auto-Transition Scope", &config.aimbot.auto_scope);
  ImGui::Checkbox("Auto-Unscope Post-Discharge", &config.aimbot.auto_unscope);
  ImGui::Checkbox("Require Scoped State", &config.aimbot.scoped_only);

  ImGui::Spacing();
  ImGui::NewLine();
  ImGui::SectionHeader("\xEF\x83\x80  Actor Filtering");
  ImGui::Checkbox("Exclude Steam Friends", &config.aimbot.ignore_friends);
  ImGui::EndChild();
}

static void draw_esp_tab(void) {
  ImGui::Checkbox("Enable Spatial Telemetry & HUD Projection", &config.esp.master);
  ImGui::Separator();
  ImGui::Spacing();

  float avail_x = ImGui::GetContentRegionAvail().x;
  float spacing = ImGui::GetStyle().ItemSpacing.x;
  float col_width = (avail_x - spacing * 2.0f) / 3.0f;

  // Column 1: Actors
  ImGui::BeginChild("##EspPlayerChild", ImVec2(col_width, 0), ImGuiChildFlags_Borders);
  ImGui::SectionHeader("\xEF\x80\x87  Actor Telemetry");

  ImGui::Checkbox("Opposing Team##EspPlayer", &config.esp.player.enemy);
  ImGui::Checkbox("Allied Team##EspPlayer", &config.esp.player.team);
  ImGui::Checkbox("Steam Friends##EspPlayer", &config.esp.player.friends);

  ImGui::Spacing();
  ImGui::Separator();
  ImGui::Spacing();

  ImGui::Checkbox("2D Bounding Volume##EspPlayer", &config.esp.player.box);
  ImGui::Checkbox("Vitality Bar##EspPlayer", &config.esp.player.health_bar);
  ImGui::Checkbox("Actor Identifier##EspPlayer", &config.esp.player.name);

  ImGui::Spacing();
  ImGui::TextUnformatted("State Annotations");
  ImGui::SetNextItemWidth(-1);
  if (ImGui::BeginCombo("##PlayerFlags", "Select Annotations...")) {
    ImGui::Checkbox("Locked Target##EspPlayer", &config.esp.player.flags.target_indicator);
    ImGui::Checkbox("Friend Marker##EspPlayer", &config.esp.player.flags.friend_indicator);
    ImGui::Checkbox("Scoped Condition##EspPlayer", &config.esp.player.flags.scoped_indicator);
    ImGui::EndCombo();
  }

  ImGui::Spacing();
  ImGui::Separator();
  ImGui::Spacing();

  ImGui::ColorEdit4("Opposing Color##EspPlayer", config.esp.player.enemy_color.to_arr(), ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoInputs);
  ImGui::ColorEdit4("Allied Color##EspPlayer", config.esp.player.team_color.to_arr(), ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoInputs);
  ImGui::ColorEdit4("Friend Color##EspPlayer", config.esp.player.friend_color.to_arr(), ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoInputs);
  ImGui::EndChild();

  ImGui::SameLine();

  // Column 2: World Resources & Objectives
  ImGui::BeginChild("##EspPickupChild", ImVec2(col_width, 0), ImGuiChildFlags_Borders);
  ImGui::SectionHeader("\xEF\x83\xBA  Resource Nodes");
  ImGui::Checkbox("2D Bounding Volume##EspPickup", &config.esp.pickup.box);
  ImGui::Checkbox("Resource Class Label##EspPickup", &config.esp.pickup.name);

  ImGui::Spacing();
  ImGui::NewLine();
  ImGui::SectionHeader("\xEF\x80\xA4  Map Objectives");
  ImGui::Checkbox("Objective Bounding Volume##EspIntelligence", &config.esp.intelligence.box);
  ImGui::Checkbox("Objective Identifier##EspIntelligence", &config.esp.intelligence.name);
  ImGui::EndChild();

  ImGui::SameLine();

  // Column 3: Engineered Constructs
  ImGui::BeginChild("##EspBuildingsChild", ImVec2(0, 0), ImGuiChildFlags_Borders);
  ImGui::SectionHeader("\xEF\x82\x85  Constructs");
  ImGui::Checkbox("Include Allied Constructs##EspBuildings", &config.esp.buildings.team);

  ImGui::Spacing();
  ImGui::Separator();
  ImGui::Spacing();

  ImGui::Checkbox("2D Bounding Volume##EspBuildings", &config.esp.buildings.box);
  ImGui::Checkbox("Structural Integrity Bar##EspBuildings", &config.esp.buildings.health_bar);
  ImGui::Checkbox("Construct Class Label##EspBuildings", &config.esp.buildings.name);
  ImGui::EndChild();
}

static void draw_chams_tab(void) {
  ImGui::Checkbox("Enable Material Pipeline & Shader Override", &config.chams.master);
  ImGui::Separator();
  ImGui::Spacing();

  const char* target_material_types[] = { "Unmodified", "Flat Lit", "Gouraud Shaded", "Fresnel Rim" };
  float col_width = (ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x) * 0.5f;

  // Left Column: Opposing & Local Actors
  ImGui::BeginChild("##ChamsLeftCol", ImVec2(col_width, 0), ImGuiChildFlags_Borders);
  ImGui::SectionHeader("\xEF\x86\xB2  Opposing Actors");
  ImGui::Checkbox("Override Opposing Materials##EnemyChamsEnemyPlayer", &config.chams.player.enemy);

  ImGui::ColorEdit4("Visible##ChamsEnemyPlayer", config.chams.player.enemy_color.to_arr(), ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoInputs);
  ImGui::SameLine(130);
  ImGui::SetNextItemWidth(-1);
  ImGui::Combo("##ChamsEnemyPlayerMaterial", (int*)&config.chams.player.enemy_material_type, target_material_types, IM_ARRAYSIZE(target_material_types));

  ImGui::ColorEdit4("Z-Occluded##ChamsEnemyPlayer", config.chams.player.enemy_color_z.to_arr(), ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoInputs);
  ImGui::SameLine(130);
  ImGui::SetNextItemWidth(-1);
  ImGui::Combo("##ChamsEnemyPlayerMaterialz", (int*)&config.chams.player.enemy_material_z_type, target_material_types, IM_ARRAYSIZE(target_material_types));

  ImGui::SetNextItemWidth(-1);
  if (ImGui::BeginCombo("##ChamsEnemyPlayerFlags", "Primary Pass Flags...")) {
    ImGui::Checkbox("Bypass Depth Buffer (XQZ)##EnemyBase", &config.chams.player.enemy_flags.ignore_z);
    ImGui::Checkbox("Wireframe Rasterization##EnemyBase", &config.chams.player.enemy_flags.wireframe);
    ImGui::EndCombo();
  }

  ImGui::Spacing();
  ImGui::TextDisabled("Secondary Shader Pass");
  ImGui::ColorEdit4("Visible##OverlayChamsEnemyPlayer", config.chams.player.enemy_overlay_color.to_arr(), ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoInputs);
  ImGui::SameLine(130);
  ImGui::SetNextItemWidth(-1);
  ImGui::Combo("##OverlayChamsEnemyPlayerMaterial", (int*)&config.chams.player.enemy_overlay_material_type, target_material_types, IM_ARRAYSIZE(target_material_types));

  ImGui::ColorEdit4("Z-Occluded##OverlayChamsEnemyPlayer", config.chams.player.enemy_overlay_color_z.to_arr(), ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoInputs);
  ImGui::SameLine(130);
  ImGui::SetNextItemWidth(-1);
  ImGui::Combo("##OverlayChamsEnemyPlayerMaterialz", (int*)&config.chams.player.enemy_overlay_material_z_type, target_material_types, IM_ARRAYSIZE(target_material_types));

  ImGui::SetNextItemWidth(-1);
  if (ImGui::BeginCombo("##OverlayChamsEnemyPlayerFlags", "Secondary Pass Flags...")) {
    ImGui::Checkbox("Bypass Depth Buffer (XQZ)##EnemyOverlay", &config.chams.player.enemy_overlay_flags.ignore_z);
    ImGui::Checkbox("Wireframe Rasterization##EnemyOverlay", &config.chams.player.enemy_overlay_flags.wireframe);
    ImGui::EndCombo();
  }

  ImGui::Spacing();
  ImGui::Separator();
  ImGui::Spacing();

  ImGui::SectionHeader("\xEF\x80\x87  Local Actor");
  ImGui::Checkbox("Override Local Material##ChamsLocalPlayer", &config.chams.player.local);
  ImGui::ColorEdit4("Visible##ChamsLocalPlayer", config.chams.player.local_color.to_arr(), ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoInputs);
  ImGui::SameLine(130);
  ImGui::SetNextItemWidth(-1);
  ImGui::Combo("##ChamsLocalPlayerMaterial", (int*)&config.chams.player.local_material_type, target_material_types, IM_ARRAYSIZE(target_material_types));
  ImGui::Checkbox("Wireframe Rasterization##ChamsLocalWireframe", &config.chams.player.local_flags.wireframe);
  ImGui::EndChild();

  ImGui::SameLine();

  // Right Column: Allied & Friend Actors
  ImGui::BeginChild("##ChamsRightCol", ImVec2(0, 0), ImGuiChildFlags_Borders);
  ImGui::SectionHeader("\xEF\x83\x80  Allied Actors");
  ImGui::Checkbox("Override Allied Materials##ChamsTeamPlayer", &config.chams.player.team);

  ImGui::ColorEdit4("Visible##ChamsTeamPlayer", config.chams.player.team_color.to_arr(), ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoInputs);
  ImGui::SameLine(130);
  ImGui::SetNextItemWidth(-1);
  ImGui::Combo("##ChamsTeamPlayerMaterial", (int*)&config.chams.player.team_material_type, target_material_types, IM_ARRAYSIZE(target_material_types));

  ImGui::ColorEdit4("Z-Occluded##ChamsTeamPlayer", config.chams.player.team_color_z.to_arr(), ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoInputs);
  ImGui::SameLine(130);
  ImGui::SetNextItemWidth(-1);
  ImGui::Combo("##ChamsTeamPlayerMaterialz", (int*)&config.chams.player.team_material_z_type, target_material_types, IM_ARRAYSIZE(target_material_types));

  ImGui::SetNextItemWidth(-1);
  if (ImGui::BeginCombo("##ChamsTeamPlayerFlags", "Allied Pass Flags...")) {
    ImGui::Checkbox("Bypass Depth Buffer (XQZ)##Team", &config.chams.player.team_flags.ignore_z);
    ImGui::Checkbox("Wireframe Rasterization##Team", &config.chams.player.team_flags.wireframe);
    ImGui::EndCombo();
  }

  ImGui::Spacing();
  ImGui::Separator();
  ImGui::Spacing();

  ImGui::SectionHeader("\xEF\x80\x84  Friend Actors");
  ImGui::Checkbox("Override Friend Materials##ChamsFriendPlayer", &config.chams.player.friends);

  ImGui::ColorEdit4("Visible##ChamsFriendPlayer", config.chams.player.friend_color.to_arr(), ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoInputs);
  ImGui::SameLine(130);
  ImGui::SetNextItemWidth(-1);
  ImGui::Combo("##ChamsFriendPlayerMaterial", (int*)&config.chams.player.friend_material_type, target_material_types, IM_ARRAYSIZE(target_material_types));

  ImGui::ColorEdit4("Z-Occluded##ChamsFriendPlayer", config.chams.player.friend_color_z.to_arr(), ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoInputs);
  ImGui::SameLine(130);
  ImGui::SetNextItemWidth(-1);
  ImGui::Combo("##ChamsFriendPlayerMaterialz", (int*)&config.chams.player.friend_material_z_type, target_material_types, IM_ARRAYSIZE(target_material_types));

  ImGui::SetNextItemWidth(-1);
  if (ImGui::BeginCombo("##ChamsFriendPlayerFlags", "Friend Pass Flags...")) {
    ImGui::Checkbox("Bypass Depth Buffer (XQZ)##Friend", &config.chams.player.friends_flags.ignore_z);
    ImGui::Checkbox("Wireframe Rasterization##Friend", &config.chams.player.friends_flags.wireframe);
    ImGui::EndCombo();
  }
  ImGui::EndChild();
}

static void draw_visuals_tab(void) {
  float col_width = (ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x) * 0.5f;

  // Left Column: Camera Frustum
  ImGui::BeginChild("##VisualsCameraCard", ImVec2(col_width, 0), ImGuiChildFlags_Borders);
  ImGui::SectionHeader("\xEF\x80\xB0  Camera Frustum & Third-Person");

  ImGui::Checkbox("Enable External Camera View", &config.visuals.thirdperson.enabled);
  ImGui::TextUnformatted("Toggle Keybind:");
  ImGui::SameLine();
  ImGui::KeybindBox(&config.visuals.thirdperson.key.waiting, &config.visuals.thirdperson.key.button, "ThirdpersonKey");

  ImGui::Spacing();
  ImGui::Separator();
  ImGui::Spacing();

  ImGui::TextUnformatted("Longitudinal Offset (Z)");
  ImGui::SetNextItemWidth(-1);
  ImGui::SliderFloat("##CameraZ", &config.visuals.thirdperson.z, 20.0f, 500.0f, "%.1f u");

  ImGui::TextUnformatted("Lateral Offset (Y)");
  ImGui::SetNextItemWidth(-1);
  ImGui::SliderFloat("##CameraY", &config.visuals.thirdperson.y, -50.0f, 50.0f, "%.1f u");

  ImGui::TextUnformatted("Vertical Offset (X)");
  ImGui::SetNextItemWidth(-1);
  ImGui::SliderFloat("##CameraX", &config.visuals.thirdperson.x, -50.0f, 50.0f, "%.1f u");
  ImGui::EndChild();

  ImGui::SameLine();

  // Right Column: Field of View & Post-Process Filters
  ImGui::BeginChild("##VisualsFovAndRemovalsCard", ImVec2(0, 0), ImGuiChildFlags_Borders);
  ImGui::SectionHeader("\xEF\x81\xAE  Projection Matrix (FOV)");

  ImGui::Checkbox("Override World Projection FOV", &config.visuals.override_fov);
  ImGui::SetNextItemWidth(-1);
  ImGui::SliderFloat("##OverrideFOV", &config.visuals.custom_fov, 30.1f, 150.0f, "%.0f\xC2\xB0");

  ImGui::Spacing();
  ImGui::Checkbox("Override Viewmodel Projection FOV", &config.visuals.override_viewmodel_fov);
  ImGui::SetNextItemWidth(-1);
  ImGui::SliderFloat("##OverrideViewmodelFOV", &config.visuals.custom_viewmodel_fov, 30.1f, 150.0f, "%.0f\xC2\xB0");

  ImGui::Spacing();
  ImGui::NewLine();
  ImGui::SectionHeader("\xEF\x81\xB0  HUD Post-Process Filters");
  ImGui::Checkbox("Suppress Scope Reticle Overlay", &config.visuals.removals.scope);
  ImGui::Checkbox("Suppress Optical Zoom Magnification", &config.visuals.removals.zoom);
  ImGui::EndChild();
}

static void draw_misc_tab(void) {
  float col_width = (ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x) * 0.5f;

  ImGui::BeginChild("##MiscMovementCard", ImVec2(col_width, 0), ImGuiChildFlags_Borders);
  ImGui::SectionHeader("\xEF\x80\x93  Locomotion Kinematics");
  ImGui::Checkbox("Continuous Ground-Impulse (Bhop)", &config.misc.movement.bhop);
  ImGui::Checkbox("Suppress Entity Collision Push", &config.misc.movement.no_push);

  ImGui::Spacing();
  ImGui::NewLine();
  ImGui::SectionHeader("\xEF\x83\xA7  Engine Runtime");
  ImGui::Checkbox("Bypass sv_pure Whitelist Check", &config.misc.exploits.bypasspure);
  ImGui::Checkbox("Disable Frame Sleep Throttling", &config.misc.exploits.no_engine_sleep);
  ImGui::EndChild();

  ImGui::SameLine();

  ImGui::BeginChild("##MiscExploitsCard", ImVec2(0, 0), ImGuiChildFlags_Borders);
  ImGui::SectionHeader("\xEF\x80\x97  Command Queue & Tick-Rate");
  ImGui::Checkbox("Enable Command Batch Shift", &config.misc.exploits.tickbase);
  ImGui::Checkbox("Render Queue Telemetry Widget", &config.misc.exploits.tickbase_indicator);

  ImGui::Spacing();
  ImGui::TextUnformatted("Packet Choke / Recharge Key:");
  ImGui::SameLine();
  ImGui::KeybindBox(&config.misc.exploits.recharge_button.waiting, &config.misc.exploits.recharge_button.button, "RechargeKey");
  ImGui::EndChild();
}

static void draw_navbot_tab(void) {
  ImGui::Checkbox("Enable Autonomous A* Graph Navigation", &config.navbot.master);
  ImGui::Separator();
  ImGui::Spacing();

  ImGui::BeginChild("##NavbotCard", ImVec2(0, 0), ImGuiChildFlags_Borders);
  ImGui::SectionHeader("\xEF\x84\xA4  MicroPather Waypoint Solver");
  ImGui::Checkbox("Actuate Waypoint Locomotion", &config.navbot.walk);

  ImGui::Spacing();
  ImGui::Checkbox("Orient View Vector to Active Path", &config.navbot.look_at_path);
  ImGui::TextUnformatted("Angular Interpolation Smoothing");
  ImGui::SetNextItemWidth(280);
  ImGui::SliderFloat("##LookSmoothness", &config.navbot.look_smoothness, 0.0f, 100.0f, "%.1f");
  ImGui::EndChild();
}

static void draw_debug_tab(void) {
  ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.95f, 0.76f, 0.35f, 1.00f));
  ImGui::TextUnformatted("\xEF\x81\xB1  Runtime Introspection & Experimental Diagnostics");
  ImGui::PopStyleColor();
  ImGui::Separator();
  ImGui::Spacing();

  float col_width = (ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x) * 0.5f;

  // Left Column: Engine & Navmesh Introspection
  ImGui::BeginChild("##DebugLeftCol", ImVec2(col_width, 0), ImGuiChildFlags_Borders);
  ImGui::SectionHeader("\xEF\x86\x88  Surface & Navmesh Introspection");

  ImGui::TextUnformatted("VGUI Surface Font Metrics");
  ImGui::SetNextItemWidth(-1);
  ImGui::SliderInt("##FontHeight", &config.debug.font_height, 6, 60, "Height: %d px");
  ImGui::SetNextItemWidth(-1);
  ImGui::SliderInt("##FontWeight", &config.debug.font_weight, 50, 800, "Weight: %d");

  ImGui::Spacing();
  ImGui::Separator();
  ImGui::Spacing();

  ImGui::Checkbox("Enumerate All World Entities", &config.debug.debug_render_all_entities);
  ImGui::Checkbox("Render Actor Condition Flag IDs", &config.debug.show_active_flag_ids_of_players);
  ImGui::Checkbox("Bypass Friend Heuristic Checks", &config.debug.disable_friend_checks);

  ImGui::Spacing();
  ImGui::Separator();
  ImGui::Spacing();

  ImGui::Checkbox("Render Navmesh Polygons", &config.debug.debug_draw_navmesh);
  ImGui::Checkbox("Render Computed A* Path", &config.debug.debug_draw_navbot_path);
  ImGui::Checkbox("Highlight Current Nav Area", &config.debug.debug_draw_navbot_current_area);
  ImGui::Checkbox("Highlight Next Nav Area", &config.debug.debug_draw_navbot_next_area);
  ImGui::Checkbox("Highlight Terminal Goal Area", &config.debug.debug_draw_navbot_goal);

  ImGui::Spacing();
  if (ImGui::Button("Invalidate & Reparse .nav Mesh", ImVec2(-1, 0))) {
    mesh.map_name = "";
  }
  ImGui::EndChild();

  ImGui::SameLine();

  // Right Column: External Hardware Telemetry Bridge
  ImGui::BeginChild("##DebugRightCol", ImVec2(0, 0), ImGuiChildFlags_Borders);
  ImGui::SectionHeader("\xEF\x83\xA7  Hardware Telemetry Bridge (PiShock)");

  ImGui::Checkbox("Enable Webhook Dispatch", &config.debug.pishock_master);
  ImGui::SameLine();
  ImGui::PushButtonRepeat(false);
  if (ImGui::Button("Dispatch Test", ImVec2(-1, 0))) {
    pishock();
  }
  ImGui::PopButtonRepeat();

  ImGui::Checkbox("Dispatch on Kill Event", &config.debug.pishock_on_kill);
  ImGui::SameLine();
  ImGui::Checkbox("Dispatch on Death Event", &config.debug.pishock_on_death);

  ImGui::Spacing();
  ImGui::Separator();
  ImGui::Spacing();

  const char* operation_types[] = { "Shock", "Vibrate", "Beep" };
  ImGui::TextUnformatted("Actuation Mode");
  ImGui::SetNextItemWidth(-1);
  ImGui::Combo("##PiShockOperation", (int*)&config.debug.operation, operation_types, IM_ARRAYSIZE(operation_types));

  ImGui::SetNextItemWidth(-1);
  ImGui::SliderInt("##PiShockIntensity", &config.debug.intensity, 1, 100, "Intensity: %d%%");
  ImGui::SetNextItemWidth(-1);
  ImGui::SliderInt("##PiShockDuration", &config.debug.duration, 1, 15, "Duration: %d s");

  ImGui::Spacing();
  ImGui::TextUnformatted("Account Username");
  ImGui::SetNextItemWidth(-1);
  ImGui::InputText("##PiShockUser", &config.debug.username);

  ImGui::TextUnformatted("Device Share Code");
  ImGui::SetNextItemWidth(-1);
  ImGui::InputText("##PiShockSharecode", &config.debug.sharecode);

  ImGui::TextUnformatted("REST API Key");
  ImGui::SetNextItemWidth(-1);
  ImGui::InputText("##PiShockApiKey", &config.debug.apikey, ImGuiInputTextFlags_Password);
  ImGui::EndChild();
}

static void draw_tab(ImGuiStyle* style, const char* name, int* tab, int index) {
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

static void draw_menu(void) {
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
      draw_aim_tab();
      break;
    case 1:
      draw_esp_tab();
      break;
    case 2:
      draw_chams_tab();
      break;
    case 3:
      draw_visuals_tab();
      break;
    case 4:
      draw_misc_tab();
      break;
    case 5:
      draw_navbot_tab();
      break;
    case 6:
      draw_debug_tab();
      break;
    }
    ImGui::EndChild();
  }

  ImGui::End();
}

#endif
