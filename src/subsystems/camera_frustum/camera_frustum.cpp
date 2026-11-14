#include "gui/config.hpp"

#include "sdk/classes/player.hpp"
#include "core/math.hpp"

#include "sdk/interfaces/entity_list.hpp"
#include "sdk/interfaces/input.hpp"
#include "sdk/interfaces/engine_trace.hpp"

void trace_hull(Vec3 vecStart, Vec3 vecEnd, Vec3 vecHullMin, Vec3 vecHullMax, unsigned int mask) {
	ray_t ray = engine_trace->init_ray(&vecStart, &vecEnd, &vecHullMin, &vecHullMax);
	struct trace_filter filter;
	struct trace_t trace;

	engine_trace->trace_ray(&ray, mask, &filter, &trace);
}

void update_camera_frustum(view_setup* setup) {
  Player* localplayer = entity_list->get_localplayer();
  if (localplayer == nullptr) return;

  if (config.viewport.camera_frustum.enabled == false) {
    localplayer->set_thirdperson(false);
    return;
  }

  static bool was_pressed = false;
  static bool do_external_cam = false;
  if (config.viewport.camera_frustum.enabled == true) {
    if (!was_pressed && is_button_down(config.viewport.camera_frustum.key)) {
      do_external_cam = !do_external_cam;
      was_pressed = true;
    } else if (was_pressed && !is_button_down(config.viewport.camera_frustum.key)) {
      was_pressed = false;
    }
  }

  if (do_external_cam == true) localplayer->set_thirdperson(true);
  else localplayer->set_thirdperson(false);

  if (input->is_thirdperson()) {
    Vec3 forward, right, up; angle_vectors(setup->angles, &forward, &right, &up);

    Vec3 offset;
    offset += right * config.viewport.camera_frustum.x;
    offset += up * config.viewport.camera_frustum.y;
    offset -= forward * config.viewport.camera_frustum.z;

    Vec3 origin = localplayer->get_shoot_pos();
    Vec3 start = origin;
    Vec3 end = origin + offset;

    float hull = 9.f;
    Vec3 mins = { -hull, -hull, -hull }, maxs = { hull, hull, hull };

    struct trace_t trace;
    engine_trace->trace_hull(&start, &end, &mins, &maxs, MASK_SOLID, &trace);
    end = trace.endpos;
    
    setup->origin = end;
  }

}
