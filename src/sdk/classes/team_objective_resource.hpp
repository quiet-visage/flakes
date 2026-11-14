#ifndef TEAM_OBJECTIVE_RESOURCE_HPP
#define TEAM_OBJECTIVE_RESOURCE_HPP


#include "core/memory.hpp"
#include "entity.hpp"

#include "core/print.hpp"

#define MAX_CONTROL_POINTS 8

class TeamObjectiveResource {
public:

  // Control Point methods
  int get_num_control_points(void) {
    return mem::read<int>(this, NETVAR_OFFSET("DT_BaseTeamObjectiveResource", "m_iNumControlPoints", 0x7B8));
  }

  int get_owning_team(int index) {
    return mem::read<int>(this, NETVAR_OFFSET("DT_BaseTeamObjectiveResource", "m_iOwner", 0x1AE4) + index * sizeof(int));
  }

  bool is_locked(int index) {
    return mem::read<bool>(this, NETVAR_OFFSET("DT_BaseTeamObjectiveResource", "m_bCPLocked", 0x1914) + index * sizeof(bool));
  }

  bool can_team_capture(int index, enum tf_team team) {
    int array_index = index + ((int)(team) * MAX_CONTROL_POINTS);
    return mem::read<bool>(this, NETVAR_OFFSET("DT_BaseTeamObjectiveResource", "m_bTeamCanCap", 0xF74) + array_index * sizeof(bool));
  }

  Vec3 get_origin(int index) {
    return mem::read<Vec3>(this, NETVAR_OFFSET("DT_BaseTeamObjectiveResource", "m_vCPPositions[0]", 0x7CC) + index * sizeof(Vec3));
  }
};

#endif
