#ifndef OFFSETS_HPP
#define OFFSETS_HPP

// Hardcoded offsets into game objects that are NOT resolved through netvars,
// either because they are not networked or because the networked field has
// different semantics from what the code needs (see the notes on each).
// Netvar-backed fields use NETVAR_OFFSET(table, prop, fallback) at the call site.

#include <cstddef>

namespace offsets {

namespace entity {
  inline constexpr std::ptrdiff_t renderable = 0x8;   // IClientRenderable sub-object
  inline constexpr std::ptrdiff_t networkable = 0x10; // IClientNetworkable sub-object
  inline constexpr std::ptrdiff_t model = 0x88;       // const model_t*
  inline constexpr std::ptrdiff_t model_name = 0x8;   // model_t::name (char*)

  // Interpolated origin the game renders with. Do not resolve via netvars: the
  // DT_BaseEntity "m_vecOrigin" RecvProp writes m_vecNetworkOrigin (0x448), the
  // raw position from the last server update, which makes spatial telemetry lag
  // and vector targeting compute against stale positions.
  inline constexpr std::ptrdiff_t origin = 0x328;
}

namespace player {
  // Flag that reads 1 while the player is alive; every caller of
  // get_lifestate() treats == 1 as "alive". Do not resolve via netvars: the real
  // m_lifeState (0xD1) uses LIFE_ALIVE = 0 / LIFE_DYING = 1 / LIFE_DEAD = 2, so
  // swapping it in makes every living player look dead and silently disables
  // vector targeting, spatial telemetry, material overrides, graph navigation,
  // and state prediction.
  inline constexpr std::ptrdiff_t alive_flag = 0x746;

  // m_iFOVStart, which always holds the actual FOV value. Do not resolve via
  // netvars: the networked m_iFOV (0x15D8) is 0 whenever the player uses their
  // default FOV, and the targeting acceptance cone overlay needs the real number.
  inline constexpr std::ptrdiff_t fov_start = 0x15DC;

  inline constexpr std::ptrdiff_t max_health = 0x1DF8;
  inline constexpr std::ptrdiff_t current_command = 1628 - 8; // CUserCmd* m_pCurrentCommand

  // Thirdperson flag sits 8 bytes before m_nForceTauntCam.
  inline constexpr std::ptrdiff_t thirdperson_from_force_taunt_cam = -0x8;
}

namespace client_class {
  inline constexpr std::ptrdiff_t class_id = 0x28; // ClientClass::m_ClassID
}

namespace convar {
  inline constexpr std::ptrdiff_t float_value = 0x54;
  inline constexpr std::ptrdiff_t int_value = 0x58;
}

} // namespace offsets

#endif
