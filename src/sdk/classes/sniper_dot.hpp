export module sdk.classes.sniper_dot;
export import std.compat;
export import core.sys;
export import core.memory;
export import sdk.interfaces.netvars;

export extern "C++" {



class SniperDot {  
public:
  float get_charge_start_time(void) {
    return mem::read<float>(this, netvar_offset<"DT_SniperDot", "m_flChargeStartTime", 0x7E0>());
  }
};



}
