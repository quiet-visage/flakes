#ifndef CAPTURE_FLAG_HPP
#define CAPTURE_FLAG_HPP


#include "core/memory.hpp"
#include "entity.hpp"

enum flag_status {
  HOME = 0,
  STOLEN,
  DROPPED
};

class CaptureFlag : public Entity {
public:
  enum flag_status get_status(void) {
    return (flag_status)mem::read<int>(this, NETVAR_OFFSET("DT_CaptureFlag", "m_nFlagStatus", 0xC48));
  }
  
};

#endif 
