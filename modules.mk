# Auto-generated C++23 modules dependency makefile
STD_CC ?= $(shell find /usr/include/c++ -name "std.cc" 2>/dev/null | head -n1)
STD_COMPAT_CC ?= $(shell find /usr/include/c++ -name "std.compat.cc" 2>/dev/null | head -n1)

STD_OBJS = obj/std.o obj/std.compat.o

obj/std.o: $(STD_CC)
	@mkdir -p obj gcm.cache
	$(CC) $(CFLAGS) -c $< -o $@

obj/std.compat.o: $(STD_COMPAT_CC) obj/std.o
	@mkdir -p obj gcm.cache
	$(CC) $(CFLAGS) -c $< -o $@

obj/src_core_assert.hpp.o: src/core/assert.hpp obj/src_core_sys.hpp.o obj/std.compat.o
	@mkdir -p obj gcm.cache
	$(CC) $(CFLAGS) -x c++ -c $< -o $@

obj/src_core_entity_cache.hpp.o: src/core/entity_cache.hpp obj/src_core_sys.hpp.o obj/src_core_vec.hpp.o obj/src_sdk_classes_entity.hpp.o obj/std.compat.o
	@mkdir -p obj gcm.cache
	$(CC) $(CFLAGS) -x c++ -c $< -o $@

obj/src_core_lookup.hpp.o: src/core/lookup.hpp obj/src_core_memory.hpp.o obj/src_core_print.hpp.o obj/src_core_sys.hpp.o obj/std.compat.o
	@mkdir -p obj gcm.cache
	$(CC) $(CFLAGS) -x c++ -c $< -o $@

obj/src_core_math.hpp.o: src/core/math.hpp obj/src_core_vec.hpp.o obj/std.compat.o
	@mkdir -p obj gcm.cache
	$(CC) $(CFLAGS) -x c++ -c $< -o $@

obj/src_core_memory.hpp.o: src/core/memory.hpp obj/src_core_print.hpp.o obj/std.compat.o
	@mkdir -p obj gcm.cache
	$(CC) $(CFLAGS) -x c++ -c $< -o $@

obj/src_core_offsets.hpp.o: src/core/offsets.hpp obj/std.compat.o
	@mkdir -p obj gcm.cache
	$(CC) $(CFLAGS) -x c++ -c $< -o $@

obj/src_core_print.hpp.o: src/core/print.hpp obj/src_core_sys.hpp.o obj/std.compat.o
	@mkdir -p obj gcm.cache
	$(CC) $(CFLAGS) -x c++ -c $< -o $@

obj/src_core_random_seed.hpp.o: src/core/random_seed.hpp obj/std.compat.o
	@mkdir -p obj gcm.cache
	$(CC) $(CFLAGS) -x c++ -c $< -o $@

obj/src_core_sys.hpp.o: src/core/sys.hpp obj/std.compat.o
	@mkdir -p obj gcm.cache
	$(CC) $(CFLAGS) -x c++ -c $< -o $@

obj/src_core_vec.hpp.o: src/core/vec.hpp obj/std.compat.o
	@mkdir -p obj gcm.cache
	$(CC) $(CFLAGS) -x c++ -c $< -o $@

obj/src_gui_config.hpp.o: src/gui/config.hpp obj/src_core_sys.hpp.o obj/src_core_vec.hpp.o obj/src_sdk_classes_player.hpp.o obj/std.compat.o
	@mkdir -p obj gcm.cache
	$(CC) $(CFLAGS) -x c++ -c $< -o $@

obj/src_gui_indicators.hpp.o: src/gui/indicators.hpp obj/src_core_sys.hpp.o obj/src_gui_config.hpp.o obj/std.compat.o obj/src_subsystems_observer_telemetry_observer_telemetry.hpp.o obj/src_subsystems_tick_scheduler_tick_scheduler.hpp.o
	@mkdir -p obj gcm.cache
	$(CC) $(CFLAGS) -x c++ -c $< -o $@

obj/src_gui_menu.hpp.o: src/gui/menu.hpp obj/src_core_print.hpp.o obj/src_core_sys.hpp.o obj/src_gui_config.hpp.o obj/std.compat.o obj/src_subsystems_graph_navigation_navmesh.hpp.o obj/src_subsystems_hardware_bridge_hardware_bridge.hpp.o
	@mkdir -p obj gcm.cache
	$(CC) $(CFLAGS) -x c++ -c $< -o $@

obj/src_hooks_cl_move.cpp.o: src/hooks/cl_move.cpp obj/src_core_print.hpp.o obj/src_core_sys.hpp.o obj/src_gui_config.hpp.o obj/src_hooks_cl_move.hpp.o obj/vendor_imgui_dearimgui.hpp.o obj/std.compat.o obj/src_subsystems_tick_scheduler_tick_scheduler.hpp.o
	@mkdir -p obj gcm.cache
	$(CC) $(CFLAGS)  -c $< -o $@

obj/src_hooks_cl_move.hpp.o: src/hooks/cl_move.hpp obj/src_core_sys.hpp.o obj/std.compat.o
	@mkdir -p obj gcm.cache
	$(CC) $(CFLAGS) -x c++ -c $< -o $@

obj/src_hooks_class_menu_show_panel.cpp.o: src/hooks/class_menu_show_panel.cpp obj/src_core_sys.hpp.o obj/src_gui_config.hpp.o obj/src_sdk_interfaces_entity_list.hpp.o obj/std.compat.o
	@mkdir -p obj gcm.cache
	$(CC) $(CFLAGS)  -c $< -o $@

obj/src_hooks_client_create_move.cpp.o: src/hooks/client_create_move.cpp obj/src_core_entity_cache.hpp.o obj/src_core_sys.hpp.o obj/src_sdk_classes_player.hpp.o obj/src_sdk_interfaces_global_vars.hpp.o obj/src_sdk_interfaces_steam_friends.hpp.o obj/std.compat.o
	@mkdir -p obj gcm.cache
	$(CC) $(CFLAGS)  -c $< -o $@

obj/src_hooks_client_mode_create_move.cpp.o: src/hooks/client_mode_create_move.cpp obj/src_core_print.hpp.o obj/src_core_sys.hpp.o obj/src_gui_config.hpp.o obj/vendor_imgui_dearimgui.hpp.o obj/src_sdk_classes_player.hpp.o obj/src_sdk_interfaces_client.hpp.o obj/src_sdk_interfaces_convar_system.hpp.o obj/src_sdk_interfaces_engine.hpp.o obj/src_sdk_interfaces_entity_list.hpp.o obj/std.compat.o obj/src_subsystems_active_deflection_active_deflection.cpp.o obj/src_subsystems_proximity_fuze_proximity_fuze.cpp.o obj/src_subsystems_graph_navigation_graph_navigation.cpp.o obj/src_subsystems_graph_navigation_parse_navmesh.cpp.o obj/src_subsystems_locomotion_locomotion.cpp.o obj/src_subsystems_state_prediction_state_prediction.cpp.o obj/src_subsystems_tick_scheduler_tick_scheduler.cpp.o obj/src_subsystems_tick_scheduler_tick_scheduler.hpp.o obj/src_subsystems_vector_targeting_vector_targeting.cpp.o
	@mkdir -p obj gcm.cache
	$(CC) $(CFLAGS)  -c $< -o $@

obj/src_hooks_draw_model_execute.cpp.o: src/hooks/draw_model_execute.cpp obj/src_core_assert.hpp.o obj/src_core_print.hpp.o obj/src_core_sys.hpp.o obj/src_core_vec.hpp.o obj/src_gui_config.hpp.o obj/src_sdk_classes_material.hpp.o obj/src_sdk_classes_player.hpp.o obj/src_sdk_interfaces_entity_list.hpp.o obj/src_sdk_interfaces_material_system.hpp.o obj/src_sdk_interfaces_model_render.hpp.o obj/src_sdk_interfaces_render_view.hpp.o obj/std.compat.o obj/src_subsystems_material_override_material_override.cpp.o obj/src_subsystems_material_override_material_override.hpp.o
	@mkdir -p obj gcm.cache
	$(CC) $(CFLAGS)  -c $< -o $@

obj/src_hooks_draw_view_model.cpp.o: src/hooks/draw_view_model.cpp obj/src_core_sys.hpp.o obj/src_gui_config.hpp.o obj/std.compat.o
	@mkdir -p obj gcm.cache
	$(CC) $(CFLAGS)  -c $< -o $@

obj/src_hooks_draw_view_models.cpp.o: src/hooks/draw_view_models.cpp obj/src_core_print.hpp.o obj/src_core_sys.hpp.o obj/src_core_vec.hpp.o obj/std.compat.o
	@mkdir -p obj gcm.cache
	$(CC) $(CFLAGS)  -c $< -o $@

obj/src_hooks_fire_event_client_side.cpp.o: src/hooks/fire_event_client_side.cpp obj/src_core_print.hpp.o obj/src_core_sys.hpp.o obj/src_sdk_classes_player.hpp.o obj/src_sdk_interfaces_game_event_manager.hpp.o obj/std.compat.o obj/src_subsystems_graph_navigation_navmesh.hpp.o obj/src_subsystems_hardware_bridge_hardware_bridge.hpp.o
	@mkdir -p obj gcm.cache
	$(CC) $(CFLAGS)  -c $< -o $@

obj/src_hooks_frame_stage_notify.cpp.o: src/hooks/frame_stage_notify.cpp obj/src_core_entity_cache.hpp.o obj/src_core_print.hpp.o obj/src_core_sys.hpp.o obj/src_gui_config.hpp.o obj/src_sdk_classes_player.hpp.o obj/src_sdk_interfaces_global_vars.hpp.o obj/src_sdk_interfaces_steam_friends.hpp.o obj/std.compat.o
	@mkdir -p obj gcm.cache
	$(CC) $(CFLAGS)  -c $< -o $@

obj/src_hooks_hooks.cpp.o: src/hooks/hooks.cpp obj/src_core_print.hpp.o obj/src_core_sys.hpp.o obj/std.compat.o
	@mkdir -p obj gcm.cache
	$(CC) $(CFLAGS)  -c $< -o $@

obj/src_hooks_in_cond.cpp.o: src/hooks/in_cond.cpp obj/src_core_sys.hpp.o obj/src_gui_config.hpp.o obj/src_sdk_classes_player.hpp.o obj/std.compat.o
	@mkdir -p obj gcm.cache
	$(CC) $(CFLAGS)  -c $< -o $@

obj/src_hooks_intro_menu_on_tick.cpp.o: src/hooks/intro_menu_on_tick.cpp obj/src_core_print.hpp.o obj/src_core_sys.hpp.o obj/src_sdk_classes_player.hpp.o obj/src_sdk_interfaces_engine.hpp.o obj/src_sdk_interfaces_entity_list.hpp.o obj/src_sdk_interfaces_global_vars.hpp.o obj/std.compat.o
	@mkdir -p obj gcm.cache
	$(CC) $(CFLAGS)  -c $< -o $@

obj/src_hooks_load_white_list.cpp.o: src/hooks/load_white_list.cpp obj/src_core_sys.hpp.o obj/src_gui_config.hpp.o obj/std.compat.o
	@mkdir -p obj gcm.cache
	$(CC) $(CFLAGS)  -c $< -o $@

obj/src_hooks_map_info_menu_show_panel.cpp.o: src/hooks/map_info_menu_show_panel.cpp obj/src_core_sys.hpp.o obj/src_gui_config.hpp.o obj/std.compat.o
	@mkdir -p obj gcm.cache
	$(CC) $(CFLAGS)  -c $< -o $@

obj/src_hooks_override_view.cpp.o: src/hooks/override_view.cpp obj/src_core_sys.hpp.o obj/src_core_vec.hpp.o obj/src_gui_config.hpp.o obj/src_sdk_classes_player.hpp.o obj/src_sdk_interfaces_convar_system.hpp.o obj/src_sdk_interfaces_entity_list.hpp.o obj/std.compat.o obj/src_subsystems_camera_frustum_camera_frustum.cpp.o
	@mkdir -p obj gcm.cache
	$(CC) $(CFLAGS)  -c $< -o $@

obj/src_hooks_paint_traverse.cpp.o: src/hooks/paint_traverse.cpp obj/src_core_memory.hpp.o obj/src_core_sys.hpp.o obj/src_gui_config.hpp.o obj/src_sdk_classes_player.hpp.o obj/src_sdk_interfaces_engine.hpp.o obj/src_sdk_interfaces_entity_list.hpp.o obj/src_sdk_interfaces_surface.hpp.o obj/std.compat.o obj/src_subsystems_graph_navigation_draw_navmesh.cpp.o obj/src_subsystems_spatial_telemetry_spatial_telemetry_entity.cpp.o obj/src_subsystems_spatial_telemetry_spatial_telemetry.hpp.o obj/src_subsystems_spatial_telemetry_spatial_telemetry_player.cpp.o
	@mkdir -p obj gcm.cache
	$(CC) $(CFLAGS)  -c $< -o $@

obj/src_hooks_scene_end.cpp.o: src/hooks/scene_end.cpp obj/src_core_sys.hpp.o obj/std.compat.o
	@mkdir -p obj gcm.cache
	$(CC) $(CFLAGS)  -c $< -o $@

obj/src_hooks_sdl.cpp.o: src/hooks/sdl.cpp obj/src_core_print.hpp.o obj/src_core_sys.hpp.o obj/src_gui_indicators.hpp.o obj/src_gui_menu.hpp.o obj/vendor_imgui_dearimgui.hpp.o obj/src_sdk_interfaces_surface.hpp.o obj/std.compat.o
	@mkdir -p obj gcm.cache
	$(CC) $(CFLAGS)  -c $< -o $@

obj/src_hooks_should_draw_local_player.cpp.o: src/hooks/should_draw_local_player.cpp obj/src_core_sys.hpp.o obj/src_sdk_classes_player.hpp.o obj/src_sdk_interfaces_entity_list.hpp.o obj/src_sdk_interfaces_input.hpp.o obj/std.compat.o
	@mkdir -p obj gcm.cache
	$(CC) $(CFLAGS)  -c $< -o $@

obj/src_hooks_should_draw_this_player.cpp.o: src/hooks/should_draw_this_player.cpp obj/src_core_print.hpp.o obj/src_core_sys.hpp.o obj/src_sdk_classes_player.hpp.o obj/src_sdk_interfaces_entity_list.hpp.o obj/src_sdk_interfaces_input.hpp.o obj/std.compat.o
	@mkdir -p obj gcm.cache
	$(CC) $(CFLAGS)  -c $< -o $@

obj/src_hooks_team_menu_show_panel.cpp.o: src/hooks/team_menu_show_panel.cpp obj/src_core_print.hpp.o obj/src_core_sys.hpp.o obj/src_gui_config.hpp.o obj/src_sdk_interfaces_engine.hpp.o obj/std.compat.o
	@mkdir -p obj gcm.cache
	$(CC) $(CFLAGS)  -c $< -o $@

obj/src_hooks_text_window_show_panel.cpp.o: src/hooks/text_window_show_panel.cpp obj/src_core_sys.hpp.o obj/src_gui_config.hpp.o obj/std.compat.o
	@mkdir -p obj gcm.cache
	$(CC) $(CFLAGS)  -c $< -o $@

obj/src_hooks_vulkan.cpp.o: src/hooks/vulkan.cpp obj/src_core_print.hpp.o obj/src_core_sys.hpp.o obj/src_gui_indicators.hpp.o obj/src_gui_menu.hpp.o obj/src_sdk_interfaces_engine.hpp.o obj/src_sdk_interfaces_surface.hpp.o obj/std.compat.o obj/src_subsystems_spatial_telemetry_spatial_telemetry.cpp.o
	@mkdir -p obj gcm.cache
	$(CC) $(CFLAGS)  -c $< -o $@

obj/src_main.cpp.o: src/main.cpp obj/src_core_assert.hpp.o obj/src_core_memory.hpp.o obj/src_core_print.hpp.o obj/src_core_random_seed.hpp.o obj/src_core_sys.hpp.o obj/src_core_vec.hpp.o obj/src_hooks_cl_move.cpp.o obj/src_hooks_class_menu_show_panel.cpp.o obj/src_hooks_client_create_move.cpp.o obj/src_hooks_client_mode_create_move.cpp.o obj/src_hooks_draw_model_execute.cpp.o obj/src_hooks_draw_view_model.cpp.o obj/src_hooks_draw_view_models.cpp.o obj/src_hooks_fire_event_client_side.cpp.o obj/src_hooks_frame_stage_notify.cpp.o obj/src_hooks_hooks.cpp.o obj/src_hooks_in_cond.cpp.o obj/src_hooks_intro_menu_on_tick.cpp.o obj/src_hooks_load_white_list.cpp.o obj/src_hooks_map_info_menu_show_panel.cpp.o obj/src_hooks_override_view.cpp.o obj/src_hooks_paint_traverse.cpp.o obj/src_hooks_scene_end.cpp.o obj/src_hooks_sdl.cpp.o obj/src_hooks_should_draw_local_player.cpp.o obj/src_hooks_should_draw_this_player.cpp.o obj/src_hooks_team_menu_show_panel.cpp.o obj/src_hooks_text_window_show_panel.cpp.o obj/src_hooks_vulkan.cpp.o obj/src_sdk_classes_keyvalues.hpp.o obj/src_sdk_interfaces_attribute_manager.hpp.o obj/src_sdk_interfaces_client.hpp.o obj/src_sdk_interfaces_client_state.hpp.o obj/src_sdk_interfaces_convar_system.hpp.o obj/src_sdk_interfaces_debug_overlay.hpp.o obj/src_sdk_interfaces_engine.hpp.o obj/src_sdk_interfaces_engine_trace.hpp.o obj/src_sdk_interfaces_entity_list.hpp.o obj/src_sdk_interfaces_game_event_manager.hpp.o obj/src_sdk_interfaces_game_movement.hpp.o obj/src_sdk_interfaces_global_vars.hpp.o obj/src_sdk_interfaces_input.hpp.o obj/src_sdk_interfaces_material_system.hpp.o obj/src_sdk_interfaces_model_render.hpp.o obj/src_sdk_interfaces_move_helper.hpp.o obj/src_sdk_interfaces_netvars.hpp.o obj/src_sdk_interfaces_prediction.hpp.o obj/src_sdk_interfaces_render_view.hpp.o obj/src_sdk_interfaces_steam_client.hpp.o obj/src_sdk_interfaces_steam_friends.hpp.o obj/src_sdk_interfaces_surface.hpp.o obj/std.compat.o
	@mkdir -p obj gcm.cache
	$(CC) $(CFLAGS)  -c $< -o $@

obj/src_sdk_classes_building.hpp.o: src/sdk/classes/building.hpp obj/src_core_memory.hpp.o obj/src_core_sys.hpp.o obj/src_sdk_classes_entity.hpp.o obj/std.compat.o
	@mkdir -p obj gcm.cache
	$(CC) $(CFLAGS) -x c++ -c $< -o $@

obj/src_sdk_classes_capture_flag.hpp.o: src/sdk/classes/capture_flag.hpp obj/src_core_memory.hpp.o obj/src_core_sys.hpp.o obj/src_sdk_classes_entity.hpp.o obj/std.compat.o
	@mkdir -p obj gcm.cache
	$(CC) $(CFLAGS) -x c++ -c $< -o $@

obj/src_sdk_classes_convar.hpp.o: src/sdk/classes/convar.hpp obj/src_core_memory.hpp.o obj/src_core_offsets.hpp.o obj/src_core_sys.hpp.o obj/std.compat.o
	@mkdir -p obj gcm.cache
	$(CC) $(CFLAGS) -x c++ -c $< -o $@

obj/src_sdk_classes_entity.hpp.o: src/sdk/classes/entity.hpp obj/src_core_memory.hpp.o obj/src_core_offsets.hpp.o obj/src_core_sys.hpp.o obj/src_core_vec.hpp.o obj/src_sdk_interfaces_entity_list.hpp.o obj/src_sdk_interfaces_netvars.hpp.o obj/std.compat.o
	@mkdir -p obj gcm.cache
	$(CC) $(CFLAGS) -x c++ -c $< -o $@

obj/src_sdk_classes_keyvalues.hpp.o: src/sdk/classes/keyvalues.hpp obj/src_core_memory.hpp.o obj/src_core_sys.hpp.o obj/std.compat.o
	@mkdir -p obj gcm.cache
	$(CC) $(CFLAGS) -x c++ -c $< -o $@

obj/src_sdk_classes_material.hpp.o: src/sdk/classes/material.hpp obj/src_core_memory.hpp.o obj/src_core_sys.hpp.o obj/src_core_vec.hpp.o obj/src_sdk_classes_material_var.hpp.o obj/std.compat.o
	@mkdir -p obj gcm.cache
	$(CC) $(CFLAGS) -x c++ -c $< -o $@

obj/src_sdk_classes_material_var.hpp.o: src/sdk/classes/material_var.hpp obj/src_core_memory.hpp.o obj/src_core_sys.hpp.o obj/src_core_vec.hpp.o obj/std.compat.o
	@mkdir -p obj gcm.cache
	$(CC) $(CFLAGS) -x c++ -c $< -o $@

obj/src_sdk_classes_player.hpp.o: src/sdk/classes/player.hpp obj/src_core_entity_cache.hpp.o obj/src_core_memory.hpp.o obj/src_core_offsets.hpp.o obj/src_core_print.hpp.o obj/src_core_sys.hpp.o obj/src_core_vec.hpp.o obj/src_sdk_classes_entity.hpp.o obj/src_sdk_classes_weapon.hpp.o obj/src_sdk_interfaces_attribute_manager.hpp.o obj/src_sdk_interfaces_entity_list.hpp.o obj/src_sdk_interfaces_steam_friends.hpp.o obj/std.compat.o
	@mkdir -p obj gcm.cache
	$(CC) $(CFLAGS) -x c++ -c $< -o $@

obj/src_sdk_classes_sniper_dot.hpp.o: src/sdk/classes/sniper_dot.hpp obj/src_core_memory.hpp.o obj/src_core_sys.hpp.o obj/src_sdk_interfaces_netvars.hpp.o obj/std.compat.o
	@mkdir -p obj gcm.cache
	$(CC) $(CFLAGS) -x c++ -c $< -o $@

obj/src_sdk_classes_team_objective_resource.hpp.o: src/sdk/classes/team_objective_resource.hpp obj/src_core_memory.hpp.o obj/src_core_print.hpp.o obj/src_core_sys.hpp.o obj/src_sdk_classes_entity.hpp.o obj/std.compat.o
	@mkdir -p obj gcm.cache
	$(CC) $(CFLAGS) -x c++ -c $< -o $@

obj/src_sdk_classes_weapon.hpp.o: src/sdk/classes/weapon.hpp obj/src_core_memory.hpp.o obj/src_core_print.hpp.o obj/src_core_sys.hpp.o obj/src_sdk_classes_entity.hpp.o obj/src_sdk_interfaces_global_vars.hpp.o obj/std.compat.o
	@mkdir -p obj gcm.cache
	$(CC) $(CFLAGS) -x c++ -c $< -o $@

obj/src_sdk_interfaces_attribute_manager.hpp.o: src/sdk/interfaces/attribute_manager.hpp obj/src_core_sys.hpp.o obj/std.compat.o
	@mkdir -p obj gcm.cache
	$(CC) $(CFLAGS) -x c++ -c $< -o $@

obj/src_sdk_interfaces_client.hpp.o: src/sdk/interfaces/client.hpp obj/src_core_memory.hpp.o obj/src_core_sys.hpp.o obj/src_core_vec.hpp.o obj/std.compat.o
	@mkdir -p obj gcm.cache
	$(CC) $(CFLAGS) -x c++ -c $< -o $@

obj/src_sdk_interfaces_client_state.hpp.o: src/sdk/interfaces/client_state.hpp obj/src_core_sys.hpp.o obj/std.compat.o
	@mkdir -p obj gcm.cache
	$(CC) $(CFLAGS) -x c++ -c $< -o $@

obj/src_sdk_interfaces_convar_system.hpp.o: src/sdk/interfaces/convar_system.hpp obj/src_core_memory.hpp.o obj/src_core_sys.hpp.o obj/src_sdk_classes_convar.hpp.o obj/std.compat.o
	@mkdir -p obj gcm.cache
	$(CC) $(CFLAGS) -x c++ -c $< -o $@

obj/src_sdk_interfaces_debug_overlay.hpp.o: src/sdk/interfaces/debug_overlay.hpp obj/src_core_memory.hpp.o obj/src_core_sys.hpp.o obj/src_core_vec.hpp.o obj/std.compat.o
	@mkdir -p obj gcm.cache
	$(CC) $(CFLAGS) -x c++ -c $< -o $@

obj/src_sdk_interfaces_engine.hpp.o: src/sdk/interfaces/engine.hpp obj/src_core_memory.hpp.o obj/src_core_sys.hpp.o obj/src_core_vec.hpp.o obj/std.compat.o
	@mkdir -p obj gcm.cache
	$(CC) $(CFLAGS) -x c++ -c $< -o $@

obj/src_sdk_interfaces_engine_trace.hpp.o: src/sdk/interfaces/engine_trace.hpp obj/src_core_memory.hpp.o obj/src_core_sys.hpp.o obj/src_core_vec.hpp.o obj/src_sdk_classes_entity.hpp.o obj/std.compat.o
	@mkdir -p obj gcm.cache
	$(CC) $(CFLAGS) -x c++ -c $< -o $@

obj/src_sdk_interfaces_entity_list.hpp.o: src/sdk/interfaces/entity_list.hpp obj/src_core_memory.hpp.o obj/src_core_sys.hpp.o obj/src_sdk_interfaces_engine.hpp.o obj/std.compat.o
	@mkdir -p obj gcm.cache
	$(CC) $(CFLAGS) -x c++ -c $< -o $@

obj/src_sdk_interfaces_game_event_manager.hpp.o: src/sdk/interfaces/game_event_manager.hpp obj/src_core_sys.hpp.o obj/std.compat.o
	@mkdir -p obj gcm.cache
	$(CC) $(CFLAGS) -x c++ -c $< -o $@

obj/src_sdk_interfaces_game_movement.hpp.o: src/sdk/interfaces/game_movement.hpp obj/src_core_memory.hpp.o obj/src_core_sys.hpp.o obj/src_core_vec.hpp.o obj/std.compat.o
	@mkdir -p obj gcm.cache
	$(CC) $(CFLAGS) -x c++ -c $< -o $@

obj/src_sdk_interfaces_global_vars.hpp.o: src/sdk/interfaces/global_vars.hpp obj/src_core_sys.hpp.o obj/std.compat.o
	@mkdir -p obj gcm.cache
	$(CC) $(CFLAGS) -x c++ -c $< -o $@

obj/src_sdk_interfaces_input.hpp.o: src/sdk/interfaces/input.hpp obj/src_core_memory.hpp.o obj/src_core_sys.hpp.o obj/src_sdk_interfaces_client.hpp.o obj/std.compat.o
	@mkdir -p obj gcm.cache
	$(CC) $(CFLAGS) -x c++ -c $< -o $@

obj/src_sdk_interfaces_material_system.hpp.o: src/sdk/interfaces/material_system.hpp obj/src_core_memory.hpp.o obj/src_core_sys.hpp.o obj/src_sdk_classes_keyvalues.hpp.o obj/src_sdk_classes_material.hpp.o obj/src_sdk_interfaces_render_context.hpp.o obj/std.compat.o
	@mkdir -p obj gcm.cache
	$(CC) $(CFLAGS) -x c++ -c $< -o $@

obj/src_sdk_interfaces_model_render.hpp.o: src/sdk/interfaces/model_render.hpp obj/src_core_memory.hpp.o obj/src_core_sys.hpp.o obj/src_sdk_classes_material.hpp.o obj/std.compat.o
	@mkdir -p obj gcm.cache
	$(CC) $(CFLAGS) -x c++ -c $< -o $@

obj/src_sdk_interfaces_move_helper.hpp.o: src/sdk/interfaces/move_helper.hpp obj/src_core_memory.hpp.o obj/src_core_sys.hpp.o obj/std.compat.o
	@mkdir -p obj gcm.cache
	$(CC) $(CFLAGS) -x c++ -c $< -o $@

obj/src_sdk_interfaces_netvars.hpp.o: src/sdk/interfaces/netvars.hpp obj/src_core_print.hpp.o obj/src_core_sys.hpp.o obj/src_sdk_interfaces_client.hpp.o obj/std.compat.o
	@mkdir -p obj gcm.cache
	$(CC) $(CFLAGS) -x c++ -c $< -o $@

obj/src_sdk_interfaces_prediction.hpp.o: src/sdk/interfaces/prediction.hpp obj/src_core_sys.hpp.o obj/src_core_vec.hpp.o obj/std.compat.o
	@mkdir -p obj gcm.cache
	$(CC) $(CFLAGS) -x c++ -c $< -o $@

obj/src_sdk_interfaces_render_context.hpp.o: src/sdk/interfaces/render_context.hpp obj/src_core_memory.hpp.o obj/src_core_sys.hpp.o obj/std.compat.o
	@mkdir -p obj gcm.cache
	$(CC) $(CFLAGS) -x c++ -c $< -o $@

obj/src_sdk_interfaces_render_view.hpp.o: src/sdk/interfaces/render_view.hpp obj/src_core_memory.hpp.o obj/src_core_sys.hpp.o obj/src_core_vec.hpp.o obj/src_sdk_interfaces_client.hpp.o obj/src_sdk_interfaces_engine.hpp.o obj/std.compat.o
	@mkdir -p obj gcm.cache
	$(CC) $(CFLAGS) -x c++ -c $< -o $@

obj/src_sdk_interfaces_steam_client.hpp.o: src/sdk/interfaces/steam_client.hpp obj/src_core_memory.hpp.o obj/src_core_sys.hpp.o obj/std.compat.o
	@mkdir -p obj gcm.cache
	$(CC) $(CFLAGS) -x c++ -c $< -o $@

obj/src_sdk_interfaces_steam_friends.hpp.o: src/sdk/interfaces/steam_friends.hpp obj/src_core_memory.hpp.o obj/src_core_sys.hpp.o obj/std.compat.o
	@mkdir -p obj gcm.cache
	$(CC) $(CFLAGS) -x c++ -c $< -o $@

obj/src_sdk_interfaces_surface.hpp.o: src/sdk/interfaces/surface.hpp obj/src_core_memory.hpp.o obj/src_core_sys.hpp.o obj/src_core_vec.hpp.o obj/src_gui_config.hpp.o obj/std.compat.o
	@mkdir -p obj gcm.cache
	$(CC) $(CFLAGS) -x c++ -c $< -o $@

obj/src_subsystems_active_deflection_active_deflection.cpp.o: src/subsystems/active_deflection/active_deflection.cpp obj/src_core_entity_cache.hpp.o obj/src_core_math.hpp.o obj/src_core_sys.hpp.o obj/src_gui_config.hpp.o obj/src_sdk_classes_player.hpp.o obj/src_sdk_classes_weapon.hpp.o obj/src_sdk_interfaces_client.hpp.o obj/src_sdk_interfaces_convar_system.hpp.o obj/src_sdk_interfaces_engine_trace.hpp.o obj/src_sdk_interfaces_entity_list.hpp.o obj/std.compat.o obj/src_subsystems_active_deflection_active_deflection.hpp.o
	@mkdir -p obj gcm.cache
	$(CC) $(CFLAGS)  -c $< -o $@

obj/src_subsystems_active_deflection_active_deflection.hpp.o: src/subsystems/active_deflection/active_deflection.hpp obj/src_core_sys.hpp.o obj/src_core_vec.hpp.o obj/src_sdk_interfaces_client.hpp.o obj/std.compat.o
	@mkdir -p obj gcm.cache
	$(CC) $(CFLAGS) -x c++ -c $< -o $@

obj/src_subsystems_proximity_fuze_proximity_fuze.cpp.o: src/subsystems/proximity_fuze/proximity_fuze.cpp obj/src_core_entity_cache.hpp.o obj/src_core_math.hpp.o obj/src_core_sys.hpp.o obj/src_gui_config.hpp.o obj/src_sdk_classes_building.hpp.o obj/src_sdk_classes_player.hpp.o obj/src_sdk_classes_weapon.hpp.o obj/src_sdk_interfaces_client.hpp.o obj/src_sdk_interfaces_convar_system.hpp.o obj/src_sdk_interfaces_engine_trace.hpp.o obj/src_sdk_interfaces_entity_list.hpp.o obj/src_sdk_interfaces_global_vars.hpp.o obj/std.compat.o obj/src_subsystems_proximity_fuze_proximity_fuze.hpp.o
	@mkdir -p obj gcm.cache
	$(CC) $(CFLAGS)  -c $< -o $@

obj/src_subsystems_proximity_fuze_proximity_fuze.hpp.o: src/subsystems/proximity_fuze/proximity_fuze.hpp obj/src_core_sys.hpp.o obj/src_core_vec.hpp.o obj/src_sdk_interfaces_client.hpp.o obj/std.compat.o
	@mkdir -p obj gcm.cache
	$(CC) $(CFLAGS) -x c++ -c $< -o $@

obj/src_subsystems_camera_frustum_camera_frustum.cpp.o: src/subsystems/camera_frustum/camera_frustum.cpp obj/src_core_math.hpp.o obj/src_core_sys.hpp.o obj/src_gui_config.hpp.o obj/src_sdk_classes_player.hpp.o obj/src_sdk_interfaces_engine_trace.hpp.o obj/src_sdk_interfaces_entity_list.hpp.o obj/src_sdk_interfaces_input.hpp.o obj/std.compat.o
	@mkdir -p obj gcm.cache
	$(CC) $(CFLAGS)  -c $< -o $@

obj/src_subsystems_graph_navigation_draw_navmesh.cpp.o: src/subsystems/graph_navigation/draw_navmesh.cpp obj/src_core_print.hpp.o obj/src_core_sys.hpp.o obj/src_core_vec.hpp.o obj/src_gui_config.hpp.o obj/src_sdk_classes_player.hpp.o obj/src_sdk_interfaces_entity_list.hpp.o obj/src_sdk_interfaces_render_view.hpp.o obj/src_sdk_interfaces_surface.hpp.o obj/std.compat.o obj/src_subsystems_graph_navigation_navmesh.hpp.o obj/src_subsystems_spatial_telemetry_spatial_telemetry.hpp.o
	@mkdir -p obj gcm.cache
	$(CC) $(CFLAGS)  -c $< -o $@

obj/src_subsystems_graph_navigation_graph_navigation.cpp.o: src/subsystems/graph_navigation/graph_navigation.cpp obj/src_core_entity_cache.hpp.o obj/src_core_math.hpp.o obj/src_core_sys.hpp.o obj/src_gui_config.hpp.o obj/src_sdk_classes_capture_flag.hpp.o obj/src_sdk_classes_player.hpp.o obj/src_sdk_classes_team_objective_resource.hpp.o obj/src_sdk_interfaces_client.hpp.o obj/src_sdk_interfaces_entity_list.hpp.o obj/std.compat.o obj/src_subsystems_graph_navigation_nav_graph.hpp.o obj/src_subsystems_graph_navigation_navmesh.hpp.o
	@mkdir -p obj gcm.cache
	$(CC) $(CFLAGS)  -c $< -o $@

obj/src_subsystems_graph_navigation_nav_graph.hpp.o: src/subsystems/graph_navigation/nav_graph.hpp obj/src_core_sys.hpp.o obj/std.compat.o obj/src_subsystems_graph_navigation_navmesh.hpp.o
	@mkdir -p obj gcm.cache
	$(CC) $(CFLAGS) -x c++ -c $< -o $@

obj/src_subsystems_graph_navigation_navmesh.hpp.o: src/subsystems/graph_navigation/navmesh.hpp obj/src_core_math.hpp.o obj/src_core_sys.hpp.o obj/src_core_vec.hpp.o obj/std.compat.o
	@mkdir -p obj gcm.cache
	$(CC) $(CFLAGS) -x c++ -c $< -o $@

obj/src_subsystems_graph_navigation_parse_navmesh.cpp.o: src/subsystems/graph_navigation/parse_navmesh.cpp obj/src_core_print.hpp.o obj/src_core_sys.hpp.o obj/src_gui_config.hpp.o obj/std.compat.o obj/src_subsystems_graph_navigation_navmesh.hpp.o
	@mkdir -p obj gcm.cache
	$(CC) $(CFLAGS)  -c $< -o $@

obj/src_subsystems_hardware_bridge_hardware_bridge.cpp.o: src/subsystems/hardware_bridge/hardware_bridge.cpp obj/src_core_sys.hpp.o obj/std.compat.o obj/src_subsystems_hardware_bridge_hardware_bridge.hpp.o
	@mkdir -p obj gcm.cache
	$(CC) $(CFLAGS)  -c $< -o $@

obj/src_subsystems_hardware_bridge_hardware_bridge.hpp.o: src/subsystems/hardware_bridge/hardware_bridge.hpp obj/src_core_print.hpp.o obj/src_core_sys.hpp.o obj/src_gui_config.hpp.o obj/std.compat.o
	@mkdir -p obj gcm.cache
	$(CC) $(CFLAGS) -x c++ -c $< -o $@

obj/src_subsystems_locomotion_locomotion.cpp.o: src/subsystems/locomotion/locomotion.cpp obj/src_core_sys.hpp.o obj/src_gui_config.hpp.o obj/src_sdk_classes_player.hpp.o obj/src_sdk_interfaces_client.hpp.o obj/src_sdk_interfaces_entity_list.hpp.o obj/std.compat.o
	@mkdir -p obj gcm.cache
	$(CC) $(CFLAGS)  -c $< -o $@

obj/src_subsystems_material_override_material_override.cpp.o: src/subsystems/material_override/material_override.cpp obj/src_core_assert.hpp.o obj/src_core_sys.hpp.o obj/src_gui_config.hpp.o obj/src_sdk_classes_keyvalues.hpp.o obj/src_sdk_classes_player.hpp.o obj/src_sdk_interfaces_entity_list.hpp.o obj/src_sdk_interfaces_material_system.hpp.o obj/std.compat.o obj/src_subsystems_material_override_material_override.hpp.o obj/src_subsystems_material_override_material_override_player.cpp.o obj/src_subsystems_material_override_material_override_wearable.cpp.o
	@mkdir -p obj gcm.cache
	$(CC) $(CFLAGS)  -c $< -o $@

obj/src_subsystems_material_override_material_override.hpp.o: src/subsystems/material_override/material_override.hpp obj/src_core_assert.hpp.o obj/src_core_sys.hpp.o obj/src_core_vec.hpp.o obj/src_sdk_classes_keyvalues.hpp.o obj/src_sdk_classes_material.hpp.o obj/src_sdk_classes_material_var.hpp.o obj/src_sdk_interfaces_material_system.hpp.o obj/src_sdk_interfaces_model_render.hpp.o obj/src_sdk_interfaces_render_view.hpp.o obj/std.compat.o
	@mkdir -p obj gcm.cache
	$(CC) $(CFLAGS) -x c++ -c $< -o $@

obj/src_subsystems_material_override_material_override_player.cpp.o: src/subsystems/material_override/material_override_player.cpp obj/src_core_sys.hpp.o obj/src_gui_config.hpp.o obj/src_sdk_classes_player.hpp.o obj/src_sdk_interfaces_entity_list.hpp.o obj/src_sdk_interfaces_material_system.hpp.o obj/src_sdk_interfaces_model_render.hpp.o obj/src_sdk_interfaces_render_view.hpp.o obj/std.compat.o obj/src_subsystems_material_override_material_override.hpp.o
	@mkdir -p obj gcm.cache
	$(CC) $(CFLAGS)  -c $< -o $@

obj/src_subsystems_material_override_material_override_wearable.cpp.o: src/subsystems/material_override/material_override_wearable.cpp obj/src_core_sys.hpp.o obj/src_gui_config.hpp.o obj/src_sdk_classes_entity.hpp.o obj/src_sdk_classes_player.hpp.o obj/src_sdk_interfaces_material_system.hpp.o obj/src_sdk_interfaces_model_render.hpp.o obj/src_sdk_interfaces_render_view.hpp.o obj/std.compat.o obj/src_subsystems_material_override_material_override.hpp.o
	@mkdir -p obj gcm.cache
	$(CC) $(CFLAGS)  -c $< -o $@

obj/src_subsystems_observer_telemetry_observer_telemetry.hpp.o: src/subsystems/observer_telemetry/observer_telemetry.hpp obj/src_core_sys.hpp.o obj/src_gui_config.hpp.o obj/src_sdk_classes_player.hpp.o obj/src_sdk_interfaces_engine.hpp.o obj/src_sdk_interfaces_entity_list.hpp.o obj/src_sdk_interfaces_global_vars.hpp.o obj/std.compat.o
	@mkdir -p obj gcm.cache
	$(CC) $(CFLAGS) -x c++ -c $< -o $@

obj/src_subsystems_spatial_telemetry_spatial_telemetry.cpp.o: src/subsystems/spatial_telemetry/spatial_telemetry.cpp obj/src_core_entity_cache.hpp.o obj/src_core_sys.hpp.o obj/src_gui_config.hpp.o obj/src_sdk_classes_player.hpp.o obj/src_sdk_interfaces_debug_overlay.hpp.o obj/std.compat.o
	@mkdir -p obj gcm.cache
	$(CC) $(CFLAGS)  -c $< -o $@

obj/src_subsystems_spatial_telemetry_spatial_telemetry.hpp.o: src/subsystems/spatial_telemetry/spatial_telemetry.hpp obj/src_core_sys.hpp.o obj/src_sdk_interfaces_surface.hpp.o obj/std.compat.o
	@mkdir -p obj gcm.cache
	$(CC) $(CFLAGS) -x c++ -c $< -o $@

obj/src_subsystems_spatial_telemetry_spatial_telemetry_entity.cpp.o: src/subsystems/spatial_telemetry/spatial_telemetry_entity.cpp obj/src_core_print.hpp.o obj/src_core_sys.hpp.o obj/src_core_vec.hpp.o obj/src_gui_config.hpp.o obj/src_sdk_classes_building.hpp.o obj/src_sdk_classes_player.hpp.o obj/src_sdk_interfaces_render_view.hpp.o obj/src_sdk_interfaces_surface.hpp.o obj/std.compat.o obj/src_subsystems_spatial_telemetry_spatial_telemetry.hpp.o
	@mkdir -p obj gcm.cache
	$(CC) $(CFLAGS)  -c $< -o $@

obj/src_subsystems_spatial_telemetry_spatial_telemetry_player.cpp.o: src/subsystems/spatial_telemetry/spatial_telemetry_player.cpp obj/src_core_math.hpp.o obj/src_core_print.hpp.o obj/src_core_sys.hpp.o obj/src_core_vec.hpp.o obj/src_gui_config.hpp.o obj/src_sdk_classes_player.hpp.o obj/src_sdk_interfaces_convar_system.hpp.o obj/src_sdk_interfaces_engine.hpp.o obj/src_sdk_interfaces_entity_list.hpp.o obj/src_sdk_interfaces_render_view.hpp.o obj/src_sdk_interfaces_surface.hpp.o obj/std.compat.o obj/src_subsystems_spatial_telemetry_spatial_telemetry.hpp.o obj/src_subsystems_vector_targeting_vector_targeting.hpp.o
	@mkdir -p obj gcm.cache
	$(CC) $(CFLAGS)  -c $< -o $@

obj/src_subsystems_state_prediction_state_prediction.cpp.o: src/subsystems/state_prediction/state_prediction.cpp obj/src_core_random_seed.hpp.o obj/src_core_sys.hpp.o obj/src_sdk_classes_player.hpp.o obj/src_sdk_interfaces_client.hpp.o obj/src_sdk_interfaces_entity_list.hpp.o obj/src_sdk_interfaces_game_movement.hpp.o obj/src_sdk_interfaces_global_vars.hpp.o obj/src_sdk_interfaces_move_helper.hpp.o obj/src_sdk_interfaces_prediction.hpp.o obj/std.compat.o
	@mkdir -p obj gcm.cache
	$(CC) $(CFLAGS)  -c $< -o $@

obj/src_subsystems_tick_scheduler_tick_scheduler.cpp.o: src/subsystems/tick_scheduler/tick_scheduler.cpp obj/src_core_sys.hpp.o obj/std.compat.o obj/src_subsystems_tick_scheduler_tick_scheduler.hpp.o
	@mkdir -p obj gcm.cache
	$(CC) $(CFLAGS)  -c $< -o $@

obj/src_subsystems_tick_scheduler_tick_scheduler.hpp.o: src/subsystems/tick_scheduler/tick_scheduler.hpp obj/src_core_sys.hpp.o obj/src_gui_config.hpp.o obj/src_gui_menu.hpp.o obj/src_hooks_cl_move.hpp.o obj/std.compat.o
	@mkdir -p obj gcm.cache
	$(CC) $(CFLAGS) -x c++ -c $< -o $@

obj/src_subsystems_vector_targeting_vector_targeting.cpp.o: src/subsystems/vector_targeting/vector_targeting.cpp obj/src_core_entity_cache.hpp.o obj/src_core_math.hpp.o obj/src_core_print.hpp.o obj/src_core_sys.hpp.o obj/src_gui_config.hpp.o obj/src_sdk_classes_player.hpp.o obj/src_sdk_interfaces_client.hpp.o obj/src_sdk_interfaces_convar_system.hpp.o obj/src_sdk_interfaces_engine.hpp.o obj/src_sdk_interfaces_engine_trace.hpp.o obj/src_sdk_interfaces_entity_list.hpp.o obj/src_sdk_interfaces_global_vars.hpp.o obj/std.compat.o obj/src_subsystems_observer_telemetry_observer_telemetry.hpp.o obj/src_subsystems_vector_targeting_vector_targeting.hpp.o
	@mkdir -p obj gcm.cache
	$(CC) $(CFLAGS)  -c $< -o $@

obj/src_subsystems_vector_targeting_vector_targeting.hpp.o: src/subsystems/vector_targeting/vector_targeting.hpp obj/src_core_sys.hpp.o obj/src_sdk_classes_player.hpp.o obj/std.compat.o
	@mkdir -p obj gcm.cache
	$(CC) $(CFLAGS) -x c++ -c $< -o $@

obj/vendor_imgui_dearimgui.hpp.o: vendor/imgui/dearimgui.hpp obj/src_core_sys.hpp.o obj/std.compat.o
	@mkdir -p obj gcm.cache
	$(CC) $(CFLAGS) -x c++ -c $< -o $@

MODULE_OBJS = $(STD_OBJS) \
  obj/src_core_assert.hpp.o \
  obj/src_core_entity_cache.hpp.o \
  obj/src_core_lookup.hpp.o \
  obj/src_core_math.hpp.o \
  obj/src_core_memory.hpp.o \
  obj/src_core_offsets.hpp.o \
  obj/src_core_print.hpp.o \
  obj/src_core_random_seed.hpp.o \
  obj/src_core_sys.hpp.o \
  obj/src_core_vec.hpp.o \
  obj/src_gui_config.hpp.o \
  obj/src_gui_indicators.hpp.o \
  obj/src_gui_menu.hpp.o \
  obj/src_hooks_cl_move.cpp.o \
  obj/src_hooks_cl_move.hpp.o \
  obj/src_hooks_class_menu_show_panel.cpp.o \
  obj/src_hooks_client_create_move.cpp.o \
  obj/src_hooks_client_mode_create_move.cpp.o \
  obj/src_hooks_draw_model_execute.cpp.o \
  obj/src_hooks_draw_view_model.cpp.o \
  obj/src_hooks_draw_view_models.cpp.o \
  obj/src_hooks_fire_event_client_side.cpp.o \
  obj/src_hooks_frame_stage_notify.cpp.o \
  obj/src_hooks_hooks.cpp.o \
  obj/src_hooks_in_cond.cpp.o \
  obj/src_hooks_intro_menu_on_tick.cpp.o \
  obj/src_hooks_load_white_list.cpp.o \
  obj/src_hooks_map_info_menu_show_panel.cpp.o \
  obj/src_hooks_override_view.cpp.o \
  obj/src_hooks_paint_traverse.cpp.o \
  obj/src_hooks_scene_end.cpp.o \
  obj/src_hooks_sdl.cpp.o \
  obj/src_hooks_should_draw_local_player.cpp.o \
  obj/src_hooks_should_draw_this_player.cpp.o \
  obj/src_hooks_team_menu_show_panel.cpp.o \
  obj/src_hooks_text_window_show_panel.cpp.o \
  obj/src_hooks_vulkan.cpp.o \
  obj/src_main.cpp.o \
  obj/src_sdk_classes_building.hpp.o \
  obj/src_sdk_classes_capture_flag.hpp.o \
  obj/src_sdk_classes_convar.hpp.o \
  obj/src_sdk_classes_entity.hpp.o \
  obj/src_sdk_classes_keyvalues.hpp.o \
  obj/src_sdk_classes_material.hpp.o \
  obj/src_sdk_classes_material_var.hpp.o \
  obj/src_sdk_classes_player.hpp.o \
  obj/src_sdk_classes_sniper_dot.hpp.o \
  obj/src_sdk_classes_team_objective_resource.hpp.o \
  obj/src_sdk_classes_weapon.hpp.o \
  obj/src_sdk_interfaces_attribute_manager.hpp.o \
  obj/src_sdk_interfaces_client.hpp.o \
  obj/src_sdk_interfaces_client_state.hpp.o \
  obj/src_sdk_interfaces_convar_system.hpp.o \
  obj/src_sdk_interfaces_debug_overlay.hpp.o \
  obj/src_sdk_interfaces_engine.hpp.o \
  obj/src_sdk_interfaces_engine_trace.hpp.o \
  obj/src_sdk_interfaces_entity_list.hpp.o \
  obj/src_sdk_interfaces_game_event_manager.hpp.o \
  obj/src_sdk_interfaces_game_movement.hpp.o \
  obj/src_sdk_interfaces_global_vars.hpp.o \
  obj/src_sdk_interfaces_input.hpp.o \
  obj/src_sdk_interfaces_material_system.hpp.o \
  obj/src_sdk_interfaces_model_render.hpp.o \
  obj/src_sdk_interfaces_move_helper.hpp.o \
  obj/src_sdk_interfaces_netvars.hpp.o \
  obj/src_sdk_interfaces_prediction.hpp.o \
  obj/src_sdk_interfaces_render_context.hpp.o \
  obj/src_sdk_interfaces_render_view.hpp.o \
  obj/src_sdk_interfaces_steam_client.hpp.o \
  obj/src_sdk_interfaces_steam_friends.hpp.o \
  obj/src_sdk_interfaces_surface.hpp.o \
  obj/src_subsystems_active_deflection_active_deflection.cpp.o \
  obj/src_subsystems_active_deflection_active_deflection.hpp.o \
  obj/src_subsystems_proximity_fuze_proximity_fuze.cpp.o \
  obj/src_subsystems_proximity_fuze_proximity_fuze.hpp.o \
  obj/src_subsystems_camera_frustum_camera_frustum.cpp.o \
  obj/src_subsystems_graph_navigation_draw_navmesh.cpp.o \
  obj/src_subsystems_graph_navigation_graph_navigation.cpp.o \
  obj/src_subsystems_graph_navigation_nav_graph.hpp.o \
  obj/src_subsystems_graph_navigation_navmesh.hpp.o \
  obj/src_subsystems_graph_navigation_parse_navmesh.cpp.o \
  obj/src_subsystems_hardware_bridge_hardware_bridge.cpp.o \
  obj/src_subsystems_hardware_bridge_hardware_bridge.hpp.o \
  obj/src_subsystems_locomotion_locomotion.cpp.o \
  obj/src_subsystems_material_override_material_override.cpp.o \
  obj/src_subsystems_material_override_material_override.hpp.o \
  obj/src_subsystems_material_override_material_override_player.cpp.o \
  obj/src_subsystems_material_override_material_override_wearable.cpp.o \
  obj/src_subsystems_observer_telemetry_observer_telemetry.hpp.o \
  obj/src_subsystems_spatial_telemetry_spatial_telemetry.cpp.o \
  obj/src_subsystems_spatial_telemetry_spatial_telemetry.hpp.o \
  obj/src_subsystems_spatial_telemetry_spatial_telemetry_entity.cpp.o \
  obj/src_subsystems_spatial_telemetry_spatial_telemetry_player.cpp.o \
  obj/src_subsystems_state_prediction_state_prediction.cpp.o \
  obj/src_subsystems_tick_scheduler_tick_scheduler.cpp.o \
  obj/src_subsystems_tick_scheduler_tick_scheduler.hpp.o \
  obj/src_subsystems_vector_targeting_vector_targeting.cpp.o \
  obj/src_subsystems_vector_targeting_vector_targeting.hpp.o \
  obj/vendor_imgui_dearimgui.hpp.o
