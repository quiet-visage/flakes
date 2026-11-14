#ifndef BUILDING_HPP
#define BUILDING_HPP


#include "core/memory.hpp"
#include "entity.hpp"

// Maybe make Building a base class for the different buildings
class Building : public Entity {
public:
  int get_health(void) {
    return mem::read<int>(this, NETVAR_OFFSET("DT_BaseObject", "m_iHealth", 0x1388));
  }

  int get_max_health(void) {
    return mem::read<int>(this, NETVAR_OFFSET("DT_BaseObject", "m_iMaxHealth", 0x138C));
  }

  bool is_sapped(void) {
    return mem::read<bool>(this, NETVAR_OFFSET("DT_BaseObject", "m_bHasSapper", 0x1380));
  }
  
  bool is_carried(void) {
    return mem::read<bool>(this, NETVAR_OFFSET("DT_BaseObject", "m_bCarried", 0x1396)) || this->is_carried_deploy();
  }

  bool is_carried_deploy(void) {
    return mem::read<bool>(this, NETVAR_OFFSET("DT_BaseObject", "m_bCarryDeploy", 0x1397));
  }
  
  // Sentry
  bool is_mini_sentry(void) {
    return mem::read<bool>(this, NETVAR_OFFSET("DT_BaseObject", "m_bMiniBuilding", 0x1399));
  }
  
  int get_building_level(void) {
    return mem::read<int>(this, NETVAR_OFFSET("DT_BaseObject", "m_iUpgradeLevel", 0x1334));
  }
};

#endif
