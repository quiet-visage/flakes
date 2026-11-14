export module sdk.interfaces.move_helper;
export import std.compat;
export import core.sys;
export import core.memory;

export extern "C++" {


class Player;

class MoveHelper {
public:
  void set_host(Player* player) {
    mem::call_vfunc<void(Player*)>(this, 12, player);
  }
};

inline MoveHelper* move_helper;


}
