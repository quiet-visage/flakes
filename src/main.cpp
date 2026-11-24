export module main_mod;
export import std.compat;
export import core.sys;
export import core.print;
export import core.assert;
export import core.vec;
export import core.memory;
export import gui.config_io;
export import sdk.classes.keyvalues;
export import sdk.interfaces.engine;
export import sdk.interfaces.client;
export import sdk.interfaces.netvars;
export import sdk.interfaces.entity_list;
export import sdk.interfaces.debug_overlay;
export import sdk.interfaces.surface;
export import sdk.interfaces.engine_trace;
export import sdk.interfaces.render_view;
export import sdk.interfaces.material_system;
export import sdk.interfaces.model_render;
export import sdk.interfaces.convar_system;
export import sdk.interfaces.prediction;
export import sdk.interfaces.steam_client;
export import sdk.interfaces.steam_friends;
export import sdk.interfaces.input;
export import sdk.interfaces.attribute_manager;
export import sdk.interfaces.global_vars;
export import sdk.interfaces.move_helper;
export import sdk.interfaces.game_movement;
export import sdk.interfaces.client_state;
export import sdk.interfaces.game_event_manager;
export import hooks.sdl;
export import hooks.vulkan;
export import hooks.hooks;
export import hooks.client_mode_create_move;
export import hooks.client_create_move;
export import hooks.cl_move;
export import hooks.paint_traverse;
export import hooks.override_view;
export import hooks.draw_view_model;
export import hooks.in_cond;
export import hooks.draw_model_execute;
export import hooks.load_white_list;
export import hooks.should_draw_local_player;
export import hooks.should_draw_this_player;
export import hooks.draw_view_models;
export import hooks.fire_event_client_side;
export import hooks.frame_stage_notify;
export import hooks.intro_menu_on_tick;
export import hooks.class_menu_show_panel;
export import hooks.team_menu_show_panel;
export import hooks.map_info_menu_show_panel;
export import hooks.text_window_show_panel;
export import hooks.scene_end;
export import core.random_seed;

export extern "C++" {
struct ModelRenderInfo;









inline void** client_mode_vtable;
inline void** vgui_vtable;
inline void** client_vtable;
inline void** model_render_vtable;
inline void** game_event_manager_vtable;
inline void** render_view_vtable;

funchook_t* funchook;

/*
void signal_handler(int signal) {
  error_box((
	     "Signal recieved: " + std::to_string(signal)
	     + "\n" + std::format("{:p}", __builtin_return_address(0))
	     + "\n" + std::format("{:p}", __builtin_return_address(1))
	     + "\n" + std::format("{:p}", __builtin_return_address(2))
	     + "\n" + std::format("{:p}", __builtin_return_address(3))
	     + "\n" + std::format("{:p}", __builtin_return_address(4))
	     ).c_str());
}
*/

__attribute__((constructor))
void entry() {  

  //std::signal(SIGKILL, signal_handler);
  
  // Interfaces
  client = (Client*)get_interface("./tf/bin/linux64/client.so", "VClient017");
  error_assert(client == nullptr, "VClient017 is missing");
  get_netvars().init(client);
  
  engine = (Engine*)get_interface("./bin/linux64/engine.so", "VEngineClient014");
  error_assert(engine == nullptr, "VEngineClient014 is missing");
  
  vgui = get_interface("./bin/linux64/vgui2.so", "VGUI_Panel009");
  error_assert(vgui == nullptr, "VGUI_Panel009 is missing");
  
  surface = (Surface*)get_interface("./bin/linux64/vguimatsurface.so", "VGUI_Surface030");
  error_assert(surface == nullptr, "VGUI_Surface030 is missing");
  
  unsigned long func_address = (unsigned long)sigscan_module("client.so", "48 8D 05 ? ? ? ? 48 8B 38 48 8B 07 FF 90 ? ? ? ? 48 8D 15 ? ? ? ? 84 C0");
  error_assert(func_address == 0, "Failed to find CInput signature");
  int32_t input_eaddr = *(int32_t*)(func_address + 0x3);
  unsigned long next_instruction = (unsigned long)(func_address + 0x7);
  input = (Input*)(*(void**)(next_instruction + (intptr_t)input_eaddr));
  error_assert(input == nullptr, "CInput is missing");

  unsigned long check_stuck_address = (unsigned long)sigscan_module("client.so", "48 8D 05 ? ? ? ? 48 89 85 ? ? ? ? 74 ? 48 8B 38");
  error_assert(check_stuck_address == 0, "Failed to find CMoveHelper signature");
  int32_t move_helper_eaddr = *(int32_t*)(check_stuck_address + 0x3);
  unsigned long check_stuck_next_instruction = (unsigned long)(check_stuck_address + 0x7);
  move_helper = (MoveHelper*)(*(void**)(check_stuck_next_instruction + (intptr_t)move_helper_eaddr));
  error_assert(move_helper == nullptr, "CMoveHelper is missing");
  
  unsigned long rcon_addr_change_address = (unsigned long)sigscan_module("engine.so", "48 8D 05 ? ? ? ? 48 8B 40 20 48 85 C0");
  if (rcon_addr_change_address == 0) {
    rcon_addr_change_address = (unsigned long)sigscan_module("engine.so", "48 8D 05 ? ? ? ? 48 8B 80 F0 8E 00 00");
  }

  if (rcon_addr_change_address != 0) {
    int32_t client_state_eaddr = *(int32_t*)(rcon_addr_change_address + 0x3);
    unsigned long rcon_addr_change_next_instruction = (unsigned long)(rcon_addr_change_address + 0x7);
    client_state = (ClientState*)((void*)(rcon_addr_change_next_instruction + (intptr_t)client_state_eaddr));
  } else {
    void* engine_base = get_module_base_address("engine.so");
    if (engine_base != nullptr) {
      client_state = (ClientState*)((uintptr_t)engine_base + 0xa548a0);
    }
  }
  error_assert(client_state == nullptr, "CClientState is missing");
  
  prediction = (Prediction*)get_interface("./tf/bin/linux64/client.so", "VClientPrediction001");
  error_assert(prediction == nullptr, "VClientPrediction001 is missing");
 
  game_movement = (GameMovement*)get_interface("./tf/bin/linux64/client.so", "GameMovement001");
  error_assert(game_movement == nullptr, "GameMovement001 is missing");
 
  overlay = (DebugOverlay*)get_interface("./bin/linux64/engine.so", "VDebugOverlay003");
  error_assert(overlay == nullptr, "VDebugOverlay003 is missing");
 
  entity_list = (EntityList*)get_interface("./tf/bin/linux64/client.so", "VClientEntityList003");
  error_assert(entity_list == nullptr, "VClientEntityList003 is missing");
 
  render_view = (RenderView*)get_interface("./bin/linux64/engine.so", "VEngineRenderView014");
  error_assert(render_view == nullptr, "VEngineRenderView014 is missing");
 
  engine_trace = (EngineTrace*)get_interface("./bin/linux64/engine.so", "EngineTraceClient003");
  error_assert(engine_trace == nullptr, "EngineTraceClient003 is missing");
 
  model_render = (ModelRender*)get_interface("./bin/linux64/engine.so", "VEngineModel016");
  error_assert(model_render == nullptr, "VEngineModel016 is missing");

  material_system = (MaterialSystem*)get_interface("./bin/linux64/materialsystem.so", "VMaterialSystem082");
  error_assert(material_system == nullptr, "VMaterialSystem082 is missing");
 
  convar_system = (ConvarSystem*)get_interface("./bin/linux64/libvstdlib.so", "VEngineCvar004");
  error_assert(convar_system == nullptr, "VEngineCvar004 is missing");
 
  game_event_manager = (GameEventManager*)get_interface("./bin/linux64/engine.so", "GAMEEVENTSMANAGER002");
  error_assert(game_event_manager == nullptr, "GAMEEVENTSMANAGER002 is missing");
  {
    // https://developer.valvesoftware.com/wiki/Team_Fortress_2/Scripting/Game_Events
    std::vector<const char*> events = {	"client_beginconnect", "client_connected", "client_disconnect", "game_newmap",
					"teamplay_round_start", "scorestats_accumulated_update", "mvm_reset_stats",
					"player_connect_client", "player_spawn", "player_changeclass", "player_hurt",
					"vote_cast", "item_pickup", "revive_player_notify", "localplayer_respawn"};
    
    for (const char* event : events) {
      game_event_manager->add_listener((IGameEventListener*)game_event_manager, event, false);
      
      if (!game_event_manager->find_listener((IGameEventListener*)game_event_manager, event)) {
	print("Failed to add event listener: %s\n", event);
      }
    }
  }
  
  {
    steam_client = (SteamClient*)get_interface("../../../linux64/steamclient.so", "SteamClient020");
    error_assert(steam_client == nullptr, "SteamClient020 is missing");
    int steam_pipe = steam_client->create_steam_pipe();
    int steam_user = steam_client->connect_to_global_user(steam_pipe);
    steam_friends = (SteamFriends*)steam_client->get_steam_friends_interface(steam_user, steam_pipe, "SteamFriends017");
    error_assert(steam_friends == nullptr, "SteamFriends017 is missing");
  }

  client_vtable = *(void ***)client;
  error_assert(client_vtable == nullptr, "client_vtable is missing");
  void* hud_process_input_addr = client_vtable[10];
  error_assert(hud_process_input_addr == nullptr, "HudProcessInput is missing");
  int32_t client_mode_eaddr = *(int32_t *)((uintptr_t)(hud_process_input_addr) + 0x3);
  void* client_mode_next_instruction = (void *)((uintptr_t)(hud_process_input_addr) + 0x7);
  void* client_mode_interface = *(void **)((uintptr_t)(client_mode_next_instruction) + (intptr_t)client_mode_eaddr);
  error_assert(client_mode_interface == nullptr, "ClientModeShared is missing");
  
  unsigned long hud_update = (unsigned long)client_vtable[11];
  error_assert(hud_update == 0, "HudUpdate is missing");
  // In 64-bit TF2 client.so, HudUpdate accesses gpGlobals via `48 8B 05 [disp32]`
  // Scan for the instruction within the first 32 bytes of HudUpdate
  bool found_gp_globals = false;
  for (unsigned long i = 0; i < 32; ++i) {
    unsigned char* p = (unsigned char*)(hud_update + i);
    if (p[0] == 0x48 && p[1] == 0x8B && p[2] == 0x05) {
      int32_t disp = *(int32_t*)(p + 3);
      unsigned long next_ip = (unsigned long)(p + 7);
      global_vars = (GlobalVars*)(*(void**)(next_ip + (intptr_t)disp));
      found_gp_globals = true;
      break;
    }
  }
  if (!found_gp_globals || global_vars == nullptr) {
    void* client_base = get_module_base_address("client.so");
    if (client_base != nullptr) {
      global_vars = *(GlobalVars**)((uintptr_t)client_base + 0x2eab2e0);
    }
  }
  error_assert(global_vars == nullptr, "CGlobalVars is missing");
 
  // VMT Function Hooks
  client_mode_vtable = *(void***)client_mode_interface;  
  client_mode_create_move_original = (bool (*)(void*, float, user_cmd*))client_mode_vtable[22];
  if (!write_to_table(client_mode_vtable, 22, (void*)client_mode_create_move_hook)) {
    print("ClientModeShared::CreateMove hook failed\n");
  } else {
    print("ClientModeShared::CreateMove hooked\n");
  }
  
  client_create_move_original = (void (*)(void*, int, float, bool))client_vtable[21];
  if (!write_to_table(client_vtable, 21, (void*)client_create_move_hook)) {
    print("Client::CreateMove hook failed\n");
  } else {
    print("Client::CreateMove hooked\n");
  }
  
  override_view_original = (void (*)(void*, view_setup*))client_mode_vtable[17];  
  if (!write_to_table(client_mode_vtable, 17, (void*)override_view_hook)) {
    print("OverrideView hook failed\n");
  } else {
    print("OverrideView hooked\n");
  }

  draw_view_model_original = (bool (*)(void*))client_mode_vtable[25];  
  if (!write_to_table(client_mode_vtable, 25, (void*)draw_view_model_hook)) {
    print("ShouldDrawViewModel hook failed\n");
  } else {
    print("ShouldDrawViewModel hooked\n");
  }
  
  vgui_vtable = *(void ***)vgui;
  paint_traverse_original = (void (*)(void*, void*, bool, bool))vgui_vtable[42];  
  if (!write_to_table(vgui_vtable, 42, (void*)paint_traverse_hook)) {
    print("PaintTraverse hook failed\n");
  } else {
    print("PaintTraverse hooked\n");
  }

  model_render_vtable = *(void ***)model_render;    
  draw_model_execute_original = (void (*)(void*, void*, ModelRenderInfo*, VMatrix*))model_render_vtable[19];  
  if (!write_to_table(model_render_vtable, 19, (void*)draw_model_execute_hook)) {
    print("DrawModelExecute hook failed\n");
  } else {
    print("DrawModelExecute hooked\n");
  }
  
  game_event_manager_vtable = *(void***)game_event_manager;
  fire_event_client_side_original = (bool (*)(void*, GameEvent*))game_event_manager_vtable[9];
  if (!write_to_table(game_event_manager_vtable, 9, (void*)fire_event_client_side_hook)) {
    print("FireEventClientSide hook failed\n");
  } else {
    print("FireEventClientSide hooked\n");
  }  

  
  frame_stage_notify_original = (void (*)(void*, ClientFrameStage))client_vtable[35];
  if (!write_to_table(client_vtable, 35, (void*)frame_stage_notify_hook)) {
    print("FrameStageNotify hook failed\n");
  } else {
    print("FrameStageNotify hooked\n");
  }  

  render_view_vtable = *(void***)render_view;

  scene_end_original = (void (*)(void*, void*))render_view_vtable[9];
  if (!write_to_table(render_view_vtable, 9, (void*)scene_end_hook)) {
    print("SceneEnd hook failed\n");
  } else {
    print("SceneEnd hooked\n");
  }  
  
  
  // Non-VMT Function hooks
  funchook = funchook_create();
  
  in_cond_original = (bool (*)(void*, int))sigscan_module("client.so", "55 83 FE ? 48 89 E5 41 54 41 89 F4");
  
  load_white_list_original = (void* (*)(void*, const char*))sigscan_module("engine.so", "55 48 89 E5 41 55 41 54 49 89 F4 53 48 89 FB 48 83 EC ? 48 8B 07 FF 50 30");
  if (!load_white_list_original) {
    void* engine_base = get_module_base_address("engine.so");
    if (engine_base) load_white_list_original = (void* (*)(void*, const char*))((uintptr_t)engine_base + 0x599e50);
  }

  cl_move_original = (void (*)(float, bool))sigscan_module("engine.so", "55 48 89 E5 41 57 41 56 41 55 41 54 53 48 81 EC ? ? ? ? 83 3D ? ? ? ? ? F3 0F 11 85");
  if (!cl_move_original) {
    void* engine_base = get_module_base_address("engine.so");
    if (engine_base) cl_move_original = (void (*)(float, bool))((uintptr_t)engine_base + 0x3b94d0);
  }

  should_draw_local_player_original = (bool (*)(void*))sigscan_module("client.so", "55 48 89 E5 53 48 89 FB 48 83 EC 08 48 8D 05 ? ? ? ? 48 8B 38 48 8B 07 FF 90 D0 00 00 00 84 C0 74 ? 48 8B 03 48 89 DF 48 8B 5D F8 48 8B 80 50 0D 00 00");
  if (!should_draw_local_player_original) {
    void* client_base = get_module_base_address("client.so");
    if (client_base) should_draw_local_player_original = (bool (*)(void*))((uintptr_t)client_base + 0x15428e0);
  }

  should_draw_this_player_original = (bool (*)(void*))sigscan_module("client.so", "55 48 89 E5 41 54 53 E8 ? ? ? ? 84 C0 75");
  if (!should_draw_this_player_original) {
    void* client_base = get_module_base_address("client.so");
    if (client_base) should_draw_this_player_original = (bool (*)(void*))((uintptr_t)client_base + 0x1c5da70);
  }

  draw_view_models_original = (void (*)(void*, view_setup*, bool))sigscan_module("client.so", "55 48 89 E5 41 57 41 56 41 55 49 89 F5 41 54 49 89 FC 53 89 D3 48 81 EC A8 01 00 00");
  if (!draw_view_models_original) {
    void* client_base = get_module_base_address("client.so");
    if (client_base) draw_view_models_original = (void (*)(void*, view_setup*, bool))((uintptr_t)client_base + 0x17117e0);
  }

  attribute_hook_value_float_original = (float (*)(float, const char*, Entity*, void*, bool))sigscan_module("client.so", "55 48 89 E5 41 57 49 89 FF 41 56 41 55 41 54 53 48 89 F3 48 83 EC 58 4C 8B");
  if (!attribute_hook_value_float_original) {
    void* client_base = get_module_base_address("client.so");
    if (client_base) attribute_hook_value_float_original = (float (*)(float, const char*, Entity*, void*, bool))((uintptr_t)client_base + 0x150ba40);
  }
  error_assert(attribute_hook_value_float_original == nullptr, "Failed to find CAttributeManager::AttribHookValue<float>()");

  intro_menu_on_tick_original = (void (*)(void*))sigscan_module("client.so", "55 48 89 E5 41 54 53 48 89 FB 48 8B BF ? ? ? ? 48 8B 07 FF 90 ? ? ? ? 84 C0");
  if (!intro_menu_on_tick_original) {
    void* client_base = get_module_base_address("client.so");
    if (client_base) intro_menu_on_tick_original = (void (*)(void*))((uintptr_t)client_base + 0x1e43520);
  }
  
  class_menu_show_panel_original = (void (*)(void*, bool))sigscan_module("client.so", "55 48 89 E5 41 55 41 54 41 89 F4 40 0F B6 F6 53 48 89 FB 48 83 EC 48 E8 ? ? ? ? 4C 8D 2D");
  if (!class_menu_show_panel_original) {
    void* client_base = get_module_base_address("client.so");
    if (client_base) class_menu_show_panel_original = (void (*)(void*, bool))((uintptr_t)client_base + 0x1e13e30);
  }

  team_menu_show_panel_original = (void (*)(void*, bool))sigscan_module("client.so", "55 48 89 E5 41 56 41 55 41 54 53 48 89 FB 48 83 EC ? 40 84 F6 0F 85");
  if (!team_menu_show_panel_original) {
    void* client_base = get_module_base_address("client.so");
    if (client_base) team_menu_show_panel_original = (void (*)(void*, bool))((uintptr_t)client_base + 0x1e58450);
  }

  map_info_menu_show_panel_original = (void (*)(void*, bool))sigscan_module("client.so", "55 48 8D 15 ? ? ? ? 48 89 E5 41 54 41 89 F4 53 48 8B 07 48 89 FB 48 8B 80 18 01 00 00 48 39 D0 75 ? E8 ? ? ? ? 44 38 E0 74 ? 45 84 E4");
  if (!map_info_menu_show_panel_original) {
    void* client_base = get_module_base_address("client.so");
    if (client_base) map_info_menu_show_panel_original = (void (*)(void*, bool))((uintptr_t)client_base + 0x1e38e20);
  }

  text_window_show_panel_original = (void (*)(void*, bool))sigscan_module("client.so", "55 48 8D 15 ? ? ? ? 48 89 E5 41 54 41 89 F4 53 48 8B 07 48 89 FB 48 8B 80 18 01 00 00 48 39 D0 75 ? E8 ? ? ? ? 44 38 E0 74 ? 48 8B 03 BA 01 00 00 00 BE 01");
  if (!text_window_show_panel_original) {
    void* client_base = get_module_base_address("client.so");
    if (client_base) text_window_show_panel_original = (void (*)(void*, bool))((uintptr_t)client_base + 0x1e601c0);
  }
  
  int rv;
  
  auto safe_prepare = [](funchook_t* fh, void** target, void* hook, const char* name) {
    if (*target == nullptr) {
      print("WARNING: skipping hook for %s (null target)\n", name);
      return;
    }
    int rv = funchook_prepare(fh, target, hook);
    if (rv != 0) {
      print("WARNING: funchook_prepare failed for %s (code %d)\n", name, rv);
    } else {
      print("Hook prepared for %s\n", name);
    }
  };

  safe_prepare(funchook, (void**)&in_cond_original, (void*)in_cond_hook, "InCond");
  safe_prepare(funchook, (void**)&load_white_list_original, (void*)load_white_list_hook, "LoadWhiteList");
  safe_prepare(funchook, (void**)&cl_move_original, (void*)cl_move_hook, "CL_Move");
  safe_prepare(funchook, (void**)&should_draw_local_player_original, (void*)should_draw_local_player_hook, "ShouldDrawLocalPlayer");
  safe_prepare(funchook, (void**)&should_draw_this_player_original, (void*)should_draw_this_player_hook, "ShouldDrawThisPlayer");
  safe_prepare(funchook, (void**)&draw_view_models_original, (void*)draw_view_models_hook, "DrawViewModels");
  safe_prepare(funchook, (void**)&intro_menu_on_tick_original, (void*)intro_menu_on_tick_hook, "CTFIntroMenu::OnTick");
  safe_prepare(funchook, (void**)&class_menu_show_panel_original, (void*)class_menu_show_panel_hook, "CTFClassMenu::ShowPanel");
  safe_prepare(funchook, (void**)&team_menu_show_panel_original, (void*)team_menu_show_panel_hook, "CTFTeamMenu::ShowPanel");
  safe_prepare(funchook, (void**)&map_info_menu_show_panel_original, (void*)map_info_menu_show_panel_hook, "CTFMapInfoMenu::ShowPanel");
  safe_prepare(funchook, (void**)&text_window_show_panel_original, (void*)text_window_show_panel_hook, "CTFTextWindow::ShowPanel");

  key_values_constructor_original = (KeyValues* (*)(void*, const char*))sigscan_module("client.so", "55 B8 FF FF FF FF 66 0F EF C0 48 89 E5 53 48 89 FB 48 89 F7 48 83 EC 08 BE 01 00 00 00 48 89 03 0F 11 43 08 48 C7 43 18 00 00 00 00 48 C7 43 20 00 00 01 00");
  if (key_values_constructor_original == nullptr) {
    key_values_constructor_original = (KeyValues* (*)(void*, const char*))sigscan_module("client.so", "55 B8 FF FF FF FF 66 0F EF C0 48 89 E5 53");
  }
  if (key_values_constructor_original == nullptr) {
    void* client_base = get_module_base_address("client.so");
    if (client_base) key_values_constructor_original = (KeyValues* (*)(void*, const char*))((uintptr_t)client_base + 0x1f032a0);
  }
  error_assert(key_values_constructor_original == nullptr, "Failed to find KeyValues() constructor");

  key_values_set_int_original = (void (*)(void*, const char*, int))sigscan_module("client.so", "55 48 89 E5 53 89 D3 48 83 EC 08 48 85 F6 74 ? 80 3E 00 74 ? BA 01 00 00 00 E8 ? ? ? ? 48 89 C7 48 85 FF 74 ? 89 5F 18 C6 47 20 02");
  if (key_values_set_int_original == nullptr) {
    void* client_base = get_module_base_address("client.so");
    if (client_base) key_values_set_int_original = (void (*)(void*, const char*, int))((uintptr_t)client_base + 0x1f040b0);
  }
  error_assert(key_values_set_int_original == nullptr, "Failed to find KeyValues::SetInt()");
  // Do NOT hook KeyValues::SetInt() via funchook - the hook only forwards to original,
  // and detouring SetInt causes race conditions across background threads during startup.
  
  key_values_load_from_buffer_original = (bool (*)(void*, const char*, const char*, void*, const char*))sigscan_module("client.so", "55 48 89 E5 41 57 41 56 41 55 41 54 53 48 81 EC 88 00 00 00 48 85 D2 48 89 BD 70 FF FF FF");
  if (key_values_load_from_buffer_original == nullptr) {
    key_values_load_from_buffer_original = (bool (*)(void*, const char*, const char*, void*, const char*))sigscan_module("client.so", "55 48 89 E5 41 57 41 56 41 55 41 54 53 48 81 EC ? ? ? ? 48 85 D2 48 89 BD");
  }
  if (key_values_load_from_buffer_original == nullptr) {
    void* client_base = get_module_base_address("client.so");
    if (client_base) key_values_load_from_buffer_original = (bool (*)(void*, const char*, const char*, void*, const char*))((uintptr_t)client_base + 0x1f06cd0);
  }
  error_assert(key_values_load_from_buffer_original == nullptr, "Failed to find KeyValues::LoadFromBuffer()");  
  
  // Hook Vulkan if present
  // Determine if we're in Vulkan mode
  void* lib_dxvk_base_address = get_module_base_address("libdxvk_d3d9.so");
  if (lib_dxvk_base_address != nullptr) {  
    print("DXVK detected at %p\n", lib_dxvk_base_address);
    void* lib_vulkan_handle = dlopen("libvulkan.so.1", RTLD_LAZY | RTLD_NOLOAD);
    if (lib_vulkan_handle == nullptr)
      lib_vulkan_handle = dlopen("/usr/lib/pressure-vessel/overrides/lib/x86_64-linux-gnu/libvulkan.so.1", RTLD_LAZY | RTLD_NOLOAD);
    if (lib_vulkan_handle == nullptr)
      lib_vulkan_handle = dlopen("/run/host/usr/lib/libvulkan.so.1", RTLD_LAZY | RTLD_NOLOAD);
    if (lib_vulkan_handle == nullptr)
      lib_vulkan_handle = dlopen("/run/host/usr/lib64/libvulkan.so.1", RTLD_LAZY | RTLD_NOLOAD); // Some distributions have a lib64 directory instead
    
    if (lib_vulkan_handle == nullptr) {
      print("Failed to dlopen libvulkan.so.1: %s\n", dlerror());
    } else {
      print("Vulkan loaded at %p\n", lib_vulkan_handle);

      // https://github.com/bruhmoment21/UniversalHookX/blob/main/UniversalHookX/src/hooks/backend/vulkan/hook_vulkan.cpp#L47
      VkInstanceCreateInfo create_info = {};
      constexpr const char* instance_extension = "VK_KHR_surface";
      
      create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
      create_info.enabledExtensionCount = 1;
      create_info.ppEnabledExtensionNames = &instance_extension;
  
      // Create Vulkan Instance without any debug feature
      VkResult inst_res = vkCreateInstance(&create_info, vk_allocator, &vk_instance);
      print("vkCreateInstance result: %d (instance: %p)\n", (int)inst_res, (void*)vk_instance);
  
      uint32_t gpu_count = 0;
      vkEnumeratePhysicalDevices(vk_instance, &gpu_count, nullptr);
      print("Vulkan physical devices found: %u\n", gpu_count);
      IM_ASSERT(gpu_count > 0);

      VkPhysicalDevice* gpus = new VkPhysicalDevice[sizeof(VkPhysicalDevice) * gpu_count];
      vkEnumeratePhysicalDevices(vk_instance, &gpu_count, gpus);

      // If a number >1 of GPUs got reported, find discrete GPU if present, or use first one available. This covers
      // most common cases (multi-gpu/integrated+dedicated graphics). Handling more complicated setups (multiple
      // dedicated GPUs) is out of scope of this sample.
      int use_gpu = 0;
      for (int i = 0; i < (int)gpu_count; ++i) {
	VkPhysicalDeviceProperties properties;
	vkGetPhysicalDeviceProperties(gpus[i], &properties);
	if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
	  use_gpu = i;
	  break;
	}
      }
	
      vk_physical_device = gpus[use_gpu];

      delete[] gpus;

      vkGetPhysicalDeviceQueueFamilyProperties(vk_physical_device, &count, nullptr);

      queue_families = (VkQueueFamilyProperties*)malloc(count*sizeof(VkQueueFamilyProperties));

      vkGetPhysicalDeviceQueueFamilyProperties(vk_physical_device, &count, queue_families);

      for (uint32_t i = 0; i < count; ++i) {
	if (queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
	  queue_family = i;
	  break;
	}
      }
  
      error_assert(queue_family == (uint32_t)-1, "queue_family fail\n");
  
      constexpr const char* device_extension = "VK_KHR_swapchain";
      constexpr const float queue_priority = 1.0f;

      VkDeviceQueueCreateInfo queue_info = { };
      queue_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
      queue_info.queueFamilyIndex = queue_family;
      queue_info.queueCount = 1;
      queue_info.pQueuePriorities = &queue_priority;

      VkDeviceCreateInfo create_info2 = { };
      create_info2.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
      create_info2.queueCreateInfoCount = 1;
      create_info2.pQueueCreateInfos = &queue_info;
      create_info2.enabledExtensionCount = 1;
      create_info2.ppEnabledExtensionNames = &device_extension;

      VkDevice vk_fake_device = VK_NULL_HANDLE;

      VkResult dev_res = vkCreateDevice(vk_physical_device, &create_info2, vk_allocator, &vk_fake_device);
      print("vkCreateDevice result: %d (fake_device: %p)\n", (int)dev_res, (void*)vk_fake_device);
      error_assert(vk_fake_device == nullptr, "Failed to create Vulkan dummy device\n");
      
      queue_present_original = (VkResult (*)(VkQueue, const VkPresentInfoKHR*))vkGetDeviceProcAddr(vk_fake_device, "vkQueuePresentKHR");
      acquire_next_image_original = (VkResult (*)(VkDevice, VkSwapchainKHR, uint64_t, VkSemaphore, VkFence, uint32_t*))vkGetDeviceProcAddr(vk_fake_device, "vkAcquireNextImageKHR");
      acquire_next_image2_original = (VkResult (*)(VkDevice, const VkAcquireNextImageInfoKHR*,  uint32_t*))vkGetDeviceProcAddr(vk_fake_device, "vkAcquireNextImage2KHR");
      create_swapchain_original = (VkResult (*)(VkDevice, const VkSwapchainCreateInfoKHR*, const VkAllocationCallbacks*, VkSwapchainKHR*))vkGetDeviceProcAddr(vk_fake_device, "vkCreateSwapchainKHR");

      print("Vulkan device procs: vkQueuePresentKHR=%p, vkAcquireNextImageKHR=%p, vkAcquireNextImage2KHR=%p, vkCreateSwapchainKHR=%p\n",
            (void*)queue_present_original, (void*)acquire_next_image_original, (void*)acquire_next_image2_original, (void*)create_swapchain_original);

      vkDestroyDevice(vk_fake_device, vk_allocator);

      // Hook the functions safely
      safe_prepare(funchook, (void**)&queue_present_original, (void*)queue_present_hook, "vkQueuePresentKHR");
      safe_prepare(funchook, (void**)&acquire_next_image_original, (void*)acquire_next_image_hook, "vkAcquireNextImageKHR");
      safe_prepare(funchook, (void**)&acquire_next_image2_original, (void*)acquire_next_image2_hook, "vkAcquireNextImage2KHR");
      safe_prepare(funchook, (void**)&create_swapchain_original, (void*)create_swapchain_hook, "vkCreateSwapchainKHR");

      print("Vulkan hooks prepared successfully\n");
      // Keep lib_vulkan_handle open to prevent unmapping hook targets before funchook_install
    }
  } else {
    print("DXVK not detected (running in OpenGL mode)\n");
  }

  /* SDL Hooking */
  SDL_SetEventFilter(event_filter, nullptr);
  //SDL_AddEventWatch(event_watch, nullptr);
    
  rv = funchook_install(funchook, 0);
  error_assert(rv != 0, "Non-VMT related hooks failed\n");

  void* lib_sdl_handle = dlopen("/usr/lib/x86_64-linux-gnu/libSDL2-2.0.so.0", RTLD_LAZY | RTLD_NOLOAD);

  error_assert(lib_sdl_handle == nullptr, "Failed to load SDL2\n");
 
  print("SDL2 loaded at %p\n", lib_sdl_handle);

  if (!sdl_hook(lib_sdl_handle, "SDL_PollEvent", (void*)poll_event_hook, (void **)&poll_event_original)) {
    print("Failed to hook SDL_PollEvent\n");
    return;
  }  
  
  if (!sdl_hook(lib_sdl_handle, "SDL_GL_SwapWindow", (void*)swap_window_hook, (void **)&swap_window_original)) {
    print("Failed to hook SDL_GL_SwapWindow\n");
    return;
  }

  
  if (!sdl_hook(lib_sdl_handle, "SDL_GetWindowFlags", (void*)get_window_flags_hook, (void **)&get_window_flags_original)) {
    print("Failed to hook SDL_GetWindowFlags\n");
    return;
  }

  if (!sdl_hook(lib_sdl_handle, "SDL_GetWindowWMInfo", (void*)get_window_WM_info_hook, (void **)&get_window_WM_info_original)) {
    print("Failed to hook SDL_GetWindowWMInfo\n");
    return;
  }
  
  if (!sdl_hook(lib_sdl_handle, "SDL_GetWindowSize", (void*)get_window_size_hook, (void **)&get_window_size_original)) {
    print("Failed to hook SDL_GetWindowSize\n");
    return;
  }


  dlclose(lib_sdl_handle);
  
  // Misc static variables and hookable things
  unsigned long func_address_2 = (unsigned long)sigscan_module("client.so", "48 8D 05 ? ? ? ? BA ? ? ? ? 89 10"); // credz: vannie / @clsendmove on github
  error_assert(func_address_2 == 0, "Failed to find random_seed signature");
  int32_t random_seed_eaddr = *(int32_t*)(func_address_2 + 0x3);
  unsigned long func_address_2_next_instruction = (unsigned long)(func_address_2 + 0x7);
  random_seed = (uint32_t*)((void*)(func_address_2_next_instruction + (intptr_t)random_seed_eaddr));

  // Auto-load default configuration profile if available
  try {
    load_config_file("default");
  } catch (...) {
    print("[Config] Exception during auto-load\n");
  }

  return;
}

__attribute__((destructor))
void __exit() {

  print("Uninjecting...\n");

  print("Unhooking VMT functions\n");
  // Unhook VMT Functions
  if (!write_to_table(client_mode_vtable, 22, (void*)client_mode_create_move_original)) {
    print("ClientMode::CreateMove failed to restore hook\n"); 
  }

  if (!write_to_table(client_vtable, 21, (void*)client_create_move_original)) {
    print("Client::CreateMove failed to restore hook\n");
  }

  if (!write_to_table(client_mode_vtable, 17, (void*)override_view_original)) {
    print("OverrideView failed to restore hook\n");
  }

  if (!write_to_table(client_mode_vtable, 25, (void*)draw_view_model_original)) {
    print("ShouldDrawViewModel failed to restore hook\n");
  }
  
  if (!write_to_table(vgui_vtable, 42, (void*)paint_traverse_original)) {
    print("PaintTraverse failed to restore hook\n");
  }   

  if (!write_to_table(model_render_vtable, 19, (void*)draw_model_execute_original)) {
    print("DrawModelExecute failed to restore hook\n");
  }
  
  if (!write_to_table(game_event_manager_vtable, 9, (void*)fire_event_client_side_original)) {
    print("FireEventClientSide failed to restore hook\n");
  }

  if (!write_to_table(client_vtable, 35, (void*)frame_stage_notify_original)) {
    print("FrameStageNotify failed to restore hook\n");
  }

  if (!write_to_table(render_view_vtable, 9, (void*)scene_end_original)) {
    print("SceneEnd failed to restore hook\n");
  }


  print("Unhooking Non-VMT functions\n");
  // Unhook Non-VMT Functions
  funchook_uninstall(funchook, 0);

  print("Unhooking SDL functions\n");
  SDL_SetEventFilter(nullptr, nullptr);
  
  // Unhook SDL
  void* lib_sdl_handle = dlopen("/usr/lib/x86_64-linux-gnu/libSDL2-2.0.so.0", RTLD_LAZY | RTLD_NOLOAD);

  if (!restore_sdl_hook(lib_sdl_handle, "SDL_GL_SwapWindow", (void*)swap_window_original)) {
    print("Failed to restore SDL_GL_SwapWindow\n");
  }
    
  if (!restore_sdl_hook(lib_sdl_handle, "SDL_PollEvent", (void*)poll_event_original)) {
    print("Failed to restore SDL_PollEvent\n");
  }

  if (!restore_sdl_hook(lib_sdl_handle, "SDL_GetWindowFlags", (void*)get_window_flags_original)) {
    print("Failed to restore SDL_GetWindowFlags\n");
  }

  if (!restore_sdl_hook(lib_sdl_handle, "SDL_GetWindowWMInfo", (void*)get_window_WM_info_original)) {
    print("Failed to restore SDL_GetWindowWMInfo\n");
  }

  if (!restore_sdl_hook(lib_sdl_handle, "SDL_GetWindowSize", (void*)get_window_size_original)) {
    print("Failed to restore SDL_GetWindowSize\n");
  }

  dlclose(lib_sdl_handle);

  print("Restoring camera frustum\n");
  // Restore camera frustum state on module unload
  {
    Player* localplayer = entity_list->get_localplayer();
    if (localplayer != nullptr)
      localplayer->set_thirdperson(false);
  }

  print("Fixing cursor\n");
  // Fix cursor visibility when we've removed/unhooked the menu
  if (menu_focused == true)
    surface->set_cursor_visible(!menu_focused);
}


}
