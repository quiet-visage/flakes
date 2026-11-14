#ifndef DEBUG_OVERLAY_HPP
#define DEBUG_OVERLAY_HPP

#include "core/vec.hpp"
#include "core/memory.hpp"

class DebugOverlay {
public:
  bool world_to_screen(Vec3* point, Vec3* screen) {
    // ScreenPosition() returns 0 when the point is on screen. Guard null here
    // explicitly: call_vfunc's default return (0) would otherwise mean "visible".
    if (this == nullptr) return false;
    return mem::call_vfunc<int(Vec3*, Vec3*)>(this, 9, point, screen) == 0;
  }

};

static inline DebugOverlay* overlay;

#endif
