#ifndef SNIPER_DOT_HPP
#define SNIPER_DOT_HPP


#include "../mem.hpp"
#include "../interfaces/netvars.hpp"

class SniperDot {  
public:
  float get_charge_start_time(void) {
    return mem::read<float>(this, NETVAR_OFFSET("DT_SniperDot", "m_flChargeStartTime", 0x7E0));
  }
};

#endif

