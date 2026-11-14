export module sdk.interfaces.debug_overlay;
export import std.compat;
export import core.sys;
export import core.vec;
export import core.memory;

export extern "C++" {


class DebugOverlay {
public:
  bool world_to_screen(Vec3* point, Vec3* screen) {
    // ScreenPosition() returns 0 when the point is on screen. Guard null here
    // explicitly: call_vfunc's default return (0) would otherwise mean "visible".
    if (this == nullptr) return false;
    return mem::call_vfunc<int(Vec3*, Vec3*)>(this, 9, point, screen) == 0;
  }

};

inline DebugOverlay* overlay;


}
