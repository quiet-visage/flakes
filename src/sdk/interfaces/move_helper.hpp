#ifndef MOVE_HELPER_HPP
#define MOVE_HELPER_HPP


#include "core/memory.hpp"
class Player;

class MoveHelper {
public:
  void set_host(Player* player) {
    mem::call_vfunc<void(Player*)>(this, 12, player);
  }
};

inline static MoveHelper* move_helper;

#endif
