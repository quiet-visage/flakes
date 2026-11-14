export module core.math;
export import std.compat;
export import core.vec;

export extern "C++" {
//180/pi
inline constexpr float radpi = 57.295779513082f;

//pi/180
inline constexpr float pideg = 0.017453293f;

inline float distance_3d(Vec3 location_one, Vec3 location_two) {
  return sqrt(((location_one.y - location_two.y)*(location_one.y - location_two.y)) +
	      ((location_one.x - location_two.x)*(location_one.x - location_two.x)) +
	      ((location_one.z - location_two.z)*(location_one.z - location_two.z)));
}

inline float distance_squared_2d(Vec3 location_one, Vec3 location_two) {
  return (location_one.x - location_two.x)*(location_one.x - location_two.x) + (location_one.y - location_two.y)*(location_one.y - location_two.y);
}

inline float azimuth_to_signed(float yaw) {
  yaw = std::fmod(yaw, 360.0f);
  if (yaw > 180.0f) yaw -= 360.0f;
  if (yaw <= -180.0f) yaw += 360.0f;
  return yaw;
}

inline void angle_vectors(Vec3 angles, Vec3* forward, Vec3* right, Vec3* up) {
  double sp, sy, sr, cp, cy, cr;
  __builtin_sincos(angles.x * pideg, &sp, &cp);
  __builtin_sincos(angles.y * pideg, &sy, &cy);

  if (forward) {
    forward->x = cp * cy;
    forward->y = cp * sy;
    forward->z = -sp;
  }

  if (right || up) {
    __builtin_sincos(angles.z * pideg, &sr, &cr);

    if (right) {
      right->x = (-1 * sr * sp * cy + -1 * cr * -sy);
      right->y = (-1 * sr * sp * sy + -1 * cr * cy);
      right->z = -1 * sr * cp;
    }

    if (up) {
      up->x = (cr * sp * cy + -sr * -sy);
      up->y = (cr * sp * sy + -sr * cy);
      up->z = cr * cp;
    }
  }
}

inline float clampf(float v, float lo, float hi) { return v < lo ? lo : (v > hi ? hi : v); }
}
