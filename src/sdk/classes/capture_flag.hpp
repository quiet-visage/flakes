export module sdk.classes.capture_flag;
export import std.compat;
export import core.sys;
export import core.memory;
export import sdk.classes.entity;

export extern "C++" {



enum flag_status {
  HOME = 0,
  STOLEN,
  DROPPED
};

class CaptureFlag : public Entity {
public:
  enum flag_status get_status(void) {
    return (flag_status)mem::read<int>(this, netvar_offset<"DT_CaptureFlag", "m_nFlagStatus", 0xC48>());
  }
  
};


}
