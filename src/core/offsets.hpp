export module core.offsets;
export import std.compat;

export extern "C++" {
// Hardcoded offsets into game objects that are NOT resolved through netvars,
// either because they are not networked or because the networked field has
// different semantics from what the code needs (see the notes on each).
// Netvar-backed fields use netvar_offset<table, prop, fallback>() at the call site.

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

namespace client_state {
  inline constexpr std::ptrdiff_t cl = 0xa548a0;                  // Global CClientState instance in .bss
  inline constexpr std::ptrdiff_t constructor = 0x3f6930;         // CClientState::CClientState()
  inline constexpr std::ptrdiff_t destructor = 0x3f6b60;          // CClientState::~CClientState()
  inline constexpr std::ptrdiff_t vtable = 0x978eb0;              // Primary vtable
  inline constexpr std::ptrdiff_t rtti_typeinfo = 0x978e68;       // RTTI typeinfo

  namespace signatures {
    inline constexpr const char* cl_primary = "48 8D 05 ? ? ? ? 48 8B 40 20 48 85 C0";
    inline constexpr const char* cl_secondary = "48 8D 05 ? ? ? ? 48 8B 80 F0 8E 00 00";
  }
}

namespace global_vars {
  inline constexpr std::ptrdiff_t gp_globals = 0x2eab2e0;         // CGlobalVars* in client.so .bss
}

namespace engine_functions {
  inline constexpr std::ptrdiff_t cl_move = 0x3b94d0;             // CL_Move() in engine.so
  inline constexpr std::ptrdiff_t load_white_list = 0x599e50;     // LoadWhiteList() in engine.so
}

namespace client_functions {
  inline constexpr std::ptrdiff_t attrib_hook_value_float = 0x150ba40;  // CAttributeManager::AttribHookValue<float>() in client.so
  inline constexpr std::ptrdiff_t should_draw_local_player = 0x15428e0; // C_BasePlayer::ShouldDrawLocalPlayer() in client.so
  inline constexpr std::ptrdiff_t draw_view_models = 0x17117e0;         // DrawViewModels() in client.so
  inline constexpr std::ptrdiff_t intro_menu_on_tick = 0x1e43520;       // CTFIntroMenu::OnTick() in client.so
  inline constexpr std::ptrdiff_t class_menu_show_panel = 0x1e13e30;    // CTFClassMenu::ShowPanel() in client.so
  inline constexpr std::ptrdiff_t team_menu_show_panel = 0x1e58450;     // CTFTeamMenu::ShowPanel() in client.so
  inline constexpr std::ptrdiff_t map_info_menu_show_panel = 0x1e38e20; // CTFMapInfoMenu::ShowPanel() in client.so
  inline constexpr std::ptrdiff_t text_window_show_panel = 0x1e601c0;   // CTFTextWindow::ShowPanel() in client.so
}

namespace keyvalues {
  // Struct member offsets
  inline constexpr std::ptrdiff_t key_name = 0x00;              // key-name symbol
  inline constexpr std::ptrdiff_t string_value = 0x08;          // string value
  inline constexpr std::ptrdiff_t wide_string_value = 0x10;     // wide-string value
  inline constexpr std::ptrdiff_t value = 0x18;                 // value union
  inline constexpr std::ptrdiff_t data_type = 0x20;             // data type byte
  inline constexpr std::ptrdiff_t escape_sequences = 0x21;      // escape flags
  inline constexpr std::ptrdiff_t evaluate_conditionals = 0x22; // conditional flags
  inline constexpr std::ptrdiff_t flags = 0x23;                 // bit 0 = use pointer at +0x40 for name lookups
  inline constexpr std::ptrdiff_t peer = 0x28;                  // next sibling (peer)
  inline constexpr std::ptrdiff_t sub = 0x30;                   // first subkey
  inline constexpr std::ptrdiff_t chain = 0x38;                 // chain
  inline constexpr std::ptrdiff_t system = 0x40;                // KeyValues system pointer

  // Function offsets in client.so
  namespace functions {
    inline constexpr std::ptrdiff_t constructor = 0x01f032a0;                    // KeyValues(const char* setName)
    inline constexpr std::ptrdiff_t constructor_with_system = 0x01f03300;        // KeyValues(const char*, <system ptr>)
    inline constexpr std::ptrdiff_t constructor_int = 0x01f042f0;                // KeyValues(setName, key, int val)
    inline constexpr std::ptrdiff_t constructor_two_ints = 0x01f04920;           // KeyValues(setName, key1, int v1, key2, int v2)
    inline constexpr std::ptrdiff_t set_name = 0x01f03250;                       // SetName
    inline constexpr std::ptrdiff_t find_key = 0x01f03e80;                       // FindKey(const char*, bool create)
    inline constexpr std::ptrdiff_t get_data_type = 0x01f04070;                  // GetDataType
    inline constexpr std::ptrdiff_t set_int = 0x01f040b0;                        // SetInt
    inline constexpr std::ptrdiff_t set_float = 0x01f040f0;                      // SetFloat (type 3)
    inline constexpr std::ptrdiff_t set_ptr = 0x01f04150;                        // SetPtr (type 4)
    inline constexpr std::ptrdiff_t get_ptr = 0x01f04190;                        // GetPtr
    inline constexpr std::ptrdiff_t is_empty = 0x01f041d0;                       // IsEmpty
    inline constexpr std::ptrdiff_t set_color = 0x01f04240;                      // SetColor (type 6)
    inline constexpr std::ptrdiff_t set_uint64 = 0x01f04280;                     // SetUint64 (type 7)
    inline constexpr std::ptrdiff_t get_float = 0x01f04390;                      // GetFloat
    inline constexpr std::ptrdiff_t get_int = 0x01f04440;                        // GetInt
    inline constexpr std::ptrdiff_t get_bool = 0x01f044e0;                       // GetBool(name, def, bool* gotDefault)
    inline constexpr std::ptrdiff_t get_uint64 = 0x01f04550;                     // GetUint64
    inline constexpr std::ptrdiff_t set_wstring = 0x01f04610;                    // SetWString (type 5)
    inline constexpr std::ptrdiff_t set_string = 0x01f04750;                     // SetString (type 1)
    inline constexpr std::ptrdiff_t get_color = 0x01f049f0;                      // GetColor
    inline constexpr std::ptrdiff_t get_string = 0x01f04ae0;                     // GetString
    inline constexpr std::ptrdiff_t unpack_into_structure = 0x01f04c60;          // UnpackIntoStructure
    inline constexpr std::ptrdiff_t get_wstring = 0x01f04f40;                    // GetWString
  }

  // Signatures
  namespace signatures {
    inline constexpr const char* constructor = "55 B8 FF FF FF FF 66 0F EF C0 48 89 E5 53 48 89 FB 48 89 F7 48 83 EC 08 BE 01 00 00 00 48 89 03 0F 11 43 08 48 C7 43 18 00 00 00 00 48 C7 43 20 00 00 01 00";
    inline constexpr const char* constructor_short = "55 B8 FF FF FF FF 66 0F EF C0 48 89 E5 53";
    inline constexpr const char* set_int = "55 48 89 E5 53 89 D3 48 83 EC 08 48 85 F6 74 ? 80 3E 00 74 ? BA 01 00 00 00 E8 ? ? ? ? 48 89 C7 48 85 FF 74 ? 89 5F 18 C6 47 20 02";
  }
}

} // namespace offsets
}
