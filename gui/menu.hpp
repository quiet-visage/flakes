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
  ImGui::Checkbox("Enable Aimbot", &config.aimbot.master);
  ImGui::Separator();
  ImGui::Spacing();

  float col_width = (ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x) * 0.5f;

  ImGui::BeginChild("##AimGeneralCard", ImVec2(col_width, 0), ImGuiChildFlags_Borders);
  ImGui::SectionHeader("\xEF\x81\x9B  Targeting & Activation");

  ImGui::Checkbox("Automatic Fire", &config.aimbot.auto_shoot);
  ImGui::Checkbox("Silent Aim", &config.aimbot.silent);

  ImGui::Spacing();
  ImGui::TextUnformatted("Target Priority");
  const char* target_items[] = { "Field of View (FOV)", "Distance", "Lowest Health", "Highest Health" };
  ImGui::SetNextItemWidth(-1);
  ImGui::Combo("##TargetType", (int*)&config.aimbot.target_type, target_items, IM_ARRAYSIZE(target_items));

  ImGui::Spacing();
  ImGui::TextUnformatted("Aim Field of View");
  ImGui::SetNextItemWidth(-1);
  ImGui::SliderFloat("##AimFOV", &config.aimbot.fov, 0.1f, 180.0f, "%.0f\xC2\xB0");
  ImGui::Checkbox("Render FOV Circle", &config.aimbot.draw_fov);

  ImGui::Spacing();
  ImGui::Separator();
  ImGui::Spacing();

  ImGui::Checkbox("Require Activation Key", &config.aimbot.use_key);
  ImGui::TextUnformatted("Activation Key:");
  ImGui::SameLine();
  ImGui::KeybindBox(&config.aimbot.key.waiting, &config.aimbot.key.button, "AimKey");
  ImGui::EndChild();

  ImGui::SameLine();

  ImGui::BeginChild("##AimClassAndFilterCard", ImVec2(0, 0), ImGuiChildFlags_Borders);
  ImGui::SectionHeader("\xEF\x86\x92  Sniper Behavior");
  ImGui::Checkbox("Auto Scope", &config.aimbot.auto_scope);
  ImGui::Checkbox("Auto Unscope", &config.aimbot.auto_unscope);
  ImGui::Checkbox("Scoped Only", &config.aimbot.scoped_only);

  ImGui::Spacing();
  ImGui::NewLine();
  ImGui::SectionHeader("\xEF\x83\x80  Target Filters");
  ImGui::Checkbox("Ignore Steam Friends", &config.aimbot.ignore_friends);
  ImGui::EndChild();
}

static void draw_esp_tab(void) {
  ImGui::Checkbox("Enable ESP Overlay", &config.esp.master);
  ImGui::Separator();
  ImGui::Spacing();

  float avail_x = ImGui::GetContentRegionAvail().x;
  float spacing = ImGui::GetStyle().ItemSpacing.x;
  float col_width = (avail_x - spacing * 2.0f) / 3.0f;

  // Column 1: Players
  ImGui::BeginChild("##EspPlayerChild", ImVec2(col_width, 0), ImGuiChildFlags_Borders);
  ImGui::SectionHeader("\xEF\x80\x87  Players");

  ImGui::Checkbox("Show Enemies##EspPlayer", &config.esp.player.enemy);
  ImGui::Checkbox("Show Teammates##EspPlayer", &config.esp.player.team);
  ImGui::Checkbox("Show Steam Friends##EspPlayer", &config.esp.player.friends);

  ImGui::Spacing();
  ImGui::Separator();
  ImGui::Spacing();

  ImGui::Checkbox("Bounding Box##EspPlayer", &config.esp.player.box);
  ImGui::Checkbox("Health Bar##EspPlayer", &config.esp.player.health_bar);
  ImGui::Checkbox("Player Name##EspPlayer", &config.esp.player.name);

  ImGui::Spacing();
  ImGui::TextUnformatted("Status Indicators");
  ImGui::SetNextItemWidth(-1);
  if (ImGui::BeginCombo("##PlayerFlags", "Select Flags...")) {
    ImGui::Checkbox("Aimbot Target##EspPlayer", &config.esp.player.flags.target_indicator);
    ImGui::Checkbox("Friend Tag##EspPlayer", &config.esp.player.flags.friend_indicator);
    ImGui::Checkbox("Scoped Status##EspPlayer", &config.esp.player.flags.scoped_indicator);
    ImGui::EndCombo();
  }

  ImGui::Spacing();
  ImGui::Separator();
  ImGui::Spacing();

  ImGui::ColorEdit4("Enemy Color##EspPlayer", config.esp.player.enemy_color.to_arr(), ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoInputs);
  ImGui::ColorEdit4("Teammate Color##EspPlayer", config.esp.player.team_color.to_arr(), ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoInputs);
  ImGui::ColorEdit4("Friend Color##EspPlayer", config.esp.player.friend_color.to_arr(), ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoInputs);
  ImGui::EndChild();

  ImGui::SameLine();

  // Column 2: World & Objectives
  ImGui::BeginChild("##EspPickupChild", ImVec2(col_width, 0), ImGuiChildFlags_Borders);
  ImGui::SectionHeader("\xEF\x83\xBA  Pickups");
  ImGui::Checkbox("Bounding Box##EspPickup", &config.esp.pickup.box);
  ImGui::Checkbox("Item Name##EspPickup", &config.esp.pickup.name);

  ImGui::Spacing();
  ImGui::NewLine();
  ImGui::SectionHeader("\xEF\x80\xA4  Objectives");
  ImGui::Checkbox("Intelligence Box##EspIntelligence", &config.esp.intelligence.box);
  ImGui::Checkbox("Intelligence Name##EspIntelligence", &config.esp.intelligence.name);
  ImGui::EndChild();

  ImGui::SameLine();

  // Column 3: Buildings
  ImGui::BeginChild("##EspBuildingsChild", ImVec2(0, 0), ImGuiChildFlags_Borders);
  ImGui::SectionHeader("\xEF\x82\x85  Buildings");
  ImGui::Checkbox("Include Team Buildings##EspBuildings", &config.esp.buildings.team);

  ImGui::Spacing();
  ImGui::Separator();
  ImGui::Spacing();

  ImGui::Checkbox("Bounding Box##EspBuildings", &config.esp.buildings.box);
  ImGui::Checkbox("Health Bar##EspBuildings", &config.esp.buildings.health_bar);
  ImGui::Checkbox("Building Type Name##EspBuildings", &config.esp.buildings.name);
  ImGui::EndChild();
}

static void draw_chams_tab(void) {
  ImGui::Checkbox("Enable Model Chams", &config.chams.master);
  ImGui::Separator();
  ImGui::Spacing();

  const char* target_material_types[] = { "None", "Flat", "Shaded", "Fresnel" };
  float col_width = (ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x) * 0.5f;

  // Left Column: Enemy & Local Chams
  ImGui::BeginChild("##ChamsLeftCol", ImVec2(col_width, 0), ImGuiChildFlags_Borders);
  ImGui::SectionHeader("\xEF\x86\xB2  Enemy Players");
  ImGui::Checkbox("Enable Enemy Chams##EnemyChamsEnemyPlayer", &config.chams.player.enemy);

  ImGui::ColorEdit4("Visible##ChamsEnemyPlayer", config.chams.player.enemy_color.to_arr(), ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoInputs);
  ImGui::SameLine(125);
  ImGui::SetNextItemWidth(-1);
  ImGui::Combo("##ChamsEnemyPlayerMaterial", (int*)&config.chams.player.enemy_material_type, target_material_types, IM_ARRAYSIZE(target_material_types));

  ImGui::ColorEdit4("Occluded##ChamsEnemyPlayer", config.chams.player.enemy_color_z.to_arr(), ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoInputs);
  ImGui::SameLine(125);
  ImGui::SetNextItemWidth(-1);
  ImGui::Combo("##ChamsEnemyPlayerMaterialz", (int*)&config.chams.player.enemy_material_z_type, target_material_types, IM_ARRAYSIZE(target_material_types));

  ImGui::SetNextItemWidth(-1);
  if (ImGui::BeginCombo("##ChamsEnemyPlayerFlags", "Base Pass Flags...")) {
    ImGui::Checkbox("Ignore Walls (XQZ)##EnemyBase", &config.chams.player.enemy_flags.ignore_z);
    ImGui::Checkbox("Wireframe##EnemyBase", &config.chams.player.enemy_flags.wireframe);
    ImGui::EndCombo();
  }

  ImGui::Spacing();
  ImGui::TextDisabled("Enemy Overlay Pass");
  ImGui::ColorEdit4("Visible##OverlayChamsEnemyPlayer", config.chams.player.enemy_overlay_color.to_arr(), ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoInputs);
  ImGui::SameLine(125);
  ImGui::SetNextItemWidth(-1);
  ImGui::Combo("##OverlayChamsEnemyPlayerMaterial", (int*)&config.chams.player.enemy_overlay_material_type, target_material_types, IM_ARRAYSIZE(target_material_types));

  ImGui::ColorEdit4("Occluded##OverlayChamsEnemyPlayer", config.chams.player.enemy_overlay_color_z.to_arr(), ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoInputs);
  ImGui::SameLine(125);
  ImGui::SetNextItemWidth(-1);
  ImGui::Combo("##OverlayChamsEnemyPlayerMaterialz", (int*)&config.chams.player.enemy_overlay_material_z_type, target_material_types, IM_ARRAYSIZE(target_material_types));

  ImGui::SetNextItemWidth(-1);
  if (ImGui::BeginCombo("##OverlayChamsEnemyPlayerFlags", "Overlay Pass Flags...")) {
    ImGui::Checkbox("Ignore Walls (XQZ)##EnemyOverlay", &config.chams.player.enemy_overlay_flags.ignore_z);
    ImGui::Checkbox("Wireframe##EnemyOverlay", &config.chams.player.enemy_overlay_flags.wireframe);
    ImGui::EndCombo();
  }

  ImGui::Spacing();
  ImGui::Separator();
  ImGui::Spacing();

  ImGui::SectionHeader("\xEF\x80\x87  Local Player");
  ImGui::Checkbox("Enable Local Chams##ChamsLocalPlayer", &config.chams.player.local);
  ImGui::ColorEdit4("Visible##ChamsLocalPlayer", config.chams.player.local_color.to_arr(), ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoInputs);
  ImGui::SameLine(125);
  ImGui::SetNextItemWidth(-1);
  ImGui::Combo("##ChamsLocalPlayerMaterial", (int*)&config.chams.player.local_material_type, target_material_types, IM_ARRAYSIZE(target_material_types));
  ImGui::Checkbox("Wireframe Mode##ChamsLocalWireframe", &config.chams.player.local_flags.wireframe);
  ImGui::EndChild();

  ImGui::SameLine();

  // Right Column: Teammate & Friend Chams
  ImGui::BeginChild("##ChamsRightCol", ImVec2(0, 0), ImGuiChildFlags_Borders);
  ImGui::SectionHeader("\xEF\x83\x80  Teammates");
  ImGui::Checkbox("Enable Teammate Chams##ChamsTeamPlayer", &config.chams.player.team);

  ImGui::ColorEdit4("Visible##ChamsTeamPlayer", config.chams.player.team_color.to_arr(), ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoInputs);
  ImGui::SameLine(125);
  ImGui::SetNextItemWidth(-1);
  ImGui::Combo("##ChamsTeamPlayerMaterial", (int*)&config.chams.player.team_material_type, target_material_types, IM_ARRAYSIZE(target_material_types));

  ImGui::ColorEdit4("Occluded##ChamsTeamPlayer", config.chams.player.team_color_z.to_arr(), ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoInputs);
  ImGui::SameLine(125);
  ImGui::SetNextItemWidth(-1);
  ImGui::Combo("##ChamsTeamPlayerMaterialz", (int*)&config.chams.player.team_material_z_type, target_material_types, IM_ARRAYSIZE(target_material_types));

  ImGui::SetNextItemWidth(-1);
  if (ImGui::BeginCombo("##ChamsTeamPlayerFlags", "Teammate Flags...")) {
    ImGui::Checkbox("Ignore Walls (XQZ)##Team", &config.chams.player.team_flags.ignore_z);
    ImGui::Checkbox("Wireframe##Team", &config.chams.player.team_flags.wireframe);
    ImGui::EndCombo();
  }

  ImGui::Spacing();
  ImGui::Separator();
  ImGui::Spacing();

  ImGui::SectionHeader("\xEF\x80\x84  Steam Friends");
  ImGui::Checkbox("Enable Friend Chams##ChamsFriendPlayer", &config.chams.player.friends);

  ImGui::ColorEdit4("Visible##ChamsFriendPlayer", config.chams.player.friend_color.to_arr(), ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoInputs);
  ImGui::SameLine(125);
  ImGui::SetNextItemWidth(-1);
  ImGui::Combo("##ChamsFriendPlayerMaterial", (int*)&config.chams.player.friend_material_type, target_material_types, IM_ARRAYSIZE(target_material_types));

  ImGui::ColorEdit4("Occluded##ChamsFriendPlayer", config.chams.player.friend_color_z.to_arr(), ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoInputs);
  ImGui::SameLine(125);
  ImGui::SetNextItemWidth(-1);
  ImGui::Combo("##ChamsFriendPlayerMaterialz", (int*)&config.chams.player.friend_material_z_type, target_material_types, IM_ARRAYSIZE(target_material_types));

  ImGui::SetNextItemWidth(-1);
  if (ImGui::BeginCombo("##ChamsFriendPlayerFlags", "Friend Flags...")) {
    ImGui::Checkbox("Ignore Walls (XQZ)##Friend", &config.chams.player.friends_flags.ignore_z);
    ImGui::Checkbox("Wireframe##Friend", &config.chams.player.friends_flags.wireframe);
    ImGui::EndCombo();
  }
  ImGui::EndChild();
}

static void draw_visuals_tab(void) {
  float col_width = (ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x) * 0.5f;

  // Left Column: Camera & Thirdperson
  ImGui::BeginChild("##VisualsCameraCard", ImVec2(col_width, 0), ImGuiChildFlags_Borders);
  ImGui::SectionHeader("\xEF\x80\xB0  Thirdperson Camera");

  ImGui::Checkbox("Enable Thirdperson", &config.visuals.thirdperson.enabled);
  ImGui::TextUnformatted("Toggle Key:");
  ImGui::SameLine();
  ImGui::KeybindBox(&config.visuals.thirdperson.key.waiting, &config.visuals.thirdperson.key.button, "ThirdpersonKey");

  ImGui::Spacing();
  ImGui::Separator();
  ImGui::Spacing();

  ImGui::TextUnformatted("Camera Distance (Z)");
  ImGui::SetNextItemWidth(-1);
  ImGui::SliderFloat("##CameraZ", &config.visuals.thirdperson.z, 20.0f, 500.0f, "%.1f");

  ImGui::TextUnformatted("Horizontal Offset (Y)");
  ImGui::SetNextItemWidth(-1);
  ImGui::SliderFloat("##CameraY", &config.visuals.thirdperson.y, -50.0f, 50.0f, "%.1f");

  ImGui::TextUnformatted("Vertical Offset (X)");
  ImGui::SetNextItemWidth(-1);
  ImGui::SliderFloat("##CameraX", &config.visuals.thirdperson.x, -50.0f, 50.0f, "%.1f");
  ImGui::EndChild();

  ImGui::SameLine();

  // Right Column: Field of View & HUD Removals
  ImGui::BeginChild("##VisualsFovAndRemovalsCard", ImVec2(0, 0), ImGuiChildFlags_Borders);
  ImGui::SectionHeader("\xEF\x81\xAE  Field of View (FOV)");

  ImGui::Checkbox("Override World FOV", &config.visuals.override_fov);
  ImGui::SetNextItemWidth(-1);
  ImGui::SliderFloat("##OverrideFOV", &config.visuals.custom_fov, 30.1f, 150.0f, "%.0f\xC2\xB0");

  ImGui::Spacing();
  ImGui::Checkbox("Override Viewmodel FOV", &config.visuals.override_viewmodel_fov);
  ImGui::SetNextItemWidth(-1);
  ImGui::SliderFloat("##OverrideViewmodelFOV", &config.visuals.custom_viewmodel_fov, 30.1f, 150.0f, "%.0f\xC2\xB0");

  ImGui::Spacing();
  ImGui::NewLine();
  ImGui::SectionHeader("\xEF\x81\xB0  Visual Removals");
  ImGui::Checkbox("Remove Sniper Scope Overlay", &config.visuals.removals.scope);
  ImGui::Checkbox("Remove Sniper Scope Zoom", &config.visuals.removals.zoom);
  ImGui::EndChild();
}

static void draw_misc_tab(void) {
  float col_width = (ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x) * 0.5f;

  ImGui::BeginChild("##MiscMovementCard", ImVec2(col_width, 0), ImGuiChildFlags_Borders);
  ImGui::SectionHeader("\xEF\x80\x93  Movement");
  ImGui::Checkbox("Auto Bunnyhop (Bhop)", &config.misc.movement.bhop);
  ImGui::Checkbox("Disable Teammate Push (No Push)", &config.misc.movement.no_push);

  ImGui::Spacing();
  ImGui::NewLine();
  ImGui::SectionHeader("\xEF\x83\xA7  Engine & Network");
  ImGui::Checkbox("Bypass sv_pure Restrictions", &config.misc.exploits.bypasspure);
  ImGui::Checkbox("Disable Engine Sleep", &config.misc.exploits.no_engine_sleep);
  ImGui::EndChild();

  ImGui::SameLine();

  ImGui::BeginChild("##MiscExploitsCard", ImVec2(0, 0), ImGuiChildFlags_Borders);
  ImGui::SectionHeader("\xEF\x80\x97  Tickbase Manipulation");
  ImGui::Checkbox("Enable Tickbase Shift (Doubletap)", &config.misc.exploits.tickbase);
  ImGui::Checkbox("Show Charge HUD Indicator", &config.misc.exploits.tickbase_indicator);

  ImGui::Spacing();
  ImGui::TextUnformatted("Recharge / Choke Key:");
  ImGui::SameLine();
  ImGui::KeybindBox(&config.misc.exploits.recharge_button.waiting, &config.misc.exploits.recharge_button.button, "RechargeKey");
  ImGui::EndChild();
}

static void draw_navbot_tab(void) {
  ImGui::Checkbox("Enable Navigation Bot", &config.navbot.master);
  ImGui::Separator();
  ImGui::Spacing();

  ImGui::BeginChild("##NavbotCard", ImVec2(0, 0), ImGuiChildFlags_Borders);
  ImGui::SectionHeader("\xEF\x84\xA4  Pathfinding & View Control");
  ImGui::Checkbox("Follow Computed Path", &config.navbot.walk);

  ImGui::Spacing();
  ImGui::Checkbox("Look Toward Active Path", &config.navbot.look_at_path);
  ImGui::TextUnformatted("Camera Look Smoothness");
  ImGui::SetNextItemWidth(260);
  ImGui::SliderFloat("##LookSmoothness", &config.navbot.look_smoothness, 0.0f, 100.0f, "%.1f");
  ImGui::EndChild();
}

static void draw_debug_tab(void) {
  ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.95f, 0.76f, 0.35f, 1.00f));
  ImGui::TextUnformatted("\xEF\x81\xB1  Developer Diagnostics — Intended for debugging and testing.");
  ImGui::PopStyleColor();
  ImGui::Separator();
  ImGui::Spacing();

  float col_width = (ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x) * 0.5f;

  // Left Column: Engine & Navmesh Debug
  ImGui::BeginChild("##DebugLeftCol", ImVec2(col_width, 0), ImGuiChildFlags_Borders);
  ImGui::SectionHeader("\xEF\x86\x88  Overlay & Navmesh");

  ImGui::TextUnformatted("Surface Font Metrics");
  ImGui::SetNextItemWidth(-1);
  ImGui::SliderInt("##FontHeight", &config.debug.font_height, 6, 60, "Height: %d px");
  ImGui::SetNextItemWidth(-1);
  ImGui::SliderInt("##FontWeight", &config.debug.font_weight, 50, 800, "Weight: %d");

  ImGui::Spacing();
  ImGui::Separator();
  ImGui::Spacing();

  ImGui::Checkbox("Render All World Entities", &config.debug.debug_render_all_entities);
  ImGui::Checkbox("Display Active Player Flag IDs", &config.debug.show_active_flag_ids_of_players);
  ImGui::Checkbox("Disable Steam Friend Checks", &config.debug.disable_friend_checks);

  ImGui::Spacing();
  ImGui::Separator();
  ImGui::Spacing();

  ImGui::Checkbox("Draw Navmesh Areas", &config.debug.debug_draw_navmesh);
  ImGui::Checkbox("Draw Navbot Path", &config.debug.debug_draw_navbot_path);
  ImGui::Checkbox("Highlight Current Nav Area", &config.debug.debug_draw_navbot_current_area);
  ImGui::Checkbox("Highlight Next Nav Area", &config.debug.debug_draw_navbot_next_area);
  ImGui::Checkbox("Highlight Navbot Goal", &config.debug.debug_draw_navbot_goal);

  ImGui::Spacing();
  if (ImGui::Button("Reload / Reparse Navmesh", ImVec2(-1, 0))) {
    mesh.map_name = "";
  }
  ImGui::EndChild();

  ImGui::SameLine();

  // Right Column: PiShock Integration
  ImGui::BeginChild("##DebugRightCol", ImVec2(0, 0), ImGuiChildFlags_Borders);
  ImGui::SectionHeader("\xEF\x83\xA7  PiShock Telemetry");

  ImGui::Checkbox("Enable PiShock", &config.debug.pishock_master);
  ImGui::SameLine();
  ImGui::PushButtonRepeat(false);
  if (ImGui::Button("Test Trigger", ImVec2(-1, 0))) {
    pishock();
  }
  ImGui::PopButtonRepeat();

  ImGui::Checkbox("Trigger on Kill", &config.debug.pishock_on_kill);
  ImGui::SameLine();
  ImGui::Checkbox("Trigger on Death", &config.debug.pishock_on_death);

  ImGui::Spacing();
  ImGui::Separator();
  ImGui::Spacing();

  const char* operation_types[] = { "Shock", "Vibrate", "Beep" };
  ImGui::TextUnformatted("Operation Mode");
  ImGui::SetNextItemWidth(-1);
  ImGui::Combo("##PiShockOperation", (int*)&config.debug.operation, operation_types, IM_ARRAYSIZE(operation_types));

  ImGui::SetNextItemWidth(-1);
  ImGui::SliderInt("##PiShockIntensity", &config.debug.intensity, 1, 100, "Intensity: %d%%");
  ImGui::SetNextItemWidth(-1);
  ImGui::SliderInt("##PiShockDuration", &config.debug.duration, 1, 15, "Duration: %d s");

  ImGui::Spacing();
  ImGui::TextUnformatted("Username");
  ImGui::SetNextItemWidth(-1);
  ImGui::InputText("##PiShockUser", &config.debug.username);

  ImGui::TextUnformatted("Share Code");
  ImGui::SetNextItemWidth(-1);
  ImGui::InputText("##PiShockSharecode", &config.debug.sharecode);

  ImGui::TextUnformatted("API Key");
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

  ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.12f, 0.50f));
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

  ImGui::SetNextWindowSize(ImVec2(780, 490), ImGuiCond_Always);
  if (ImGui::Begin("Flakes  \xC2\xB7  Team Fortress 2 GNU/Linux", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse)) {
    static int tab = 0;

    // Left Navigation Sidebar
    ImGui::BeginChild("##NavigationSidebar", ImVec2(148, 0), ImGuiChildFlags_Borders);
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.52f, 0.88f, 0.76f, 1.00f));
    ImGui::TextCentered("NAVIGATION");
    ImGui::PopStyleColor();
    ImGui::Separator();
    ImGui::Spacing();

    draw_tab(style, "\xEF\x81\x9B  Aimbot",  &tab, 0);
    draw_tab(style, "\xEF\x81\xAE  ESP",     &tab, 1);
    draw_tab(style, "\xEF\x86\xB2  Chams",   &tab, 2);
    draw_tab(style, "\xEF\x80\xB0  Visuals", &tab, 3);
    draw_tab(style, "\xEF\x80\x93  Misc",    &tab, 4);
    draw_tab(style, "\xEF\x84\xA4  Navbot",  &tab, 5);
    draw_tab(style, "\xEF\x86\x88  Debug",   &tab, 6);

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
