#ifndef ENGINE_HPP
#define ENGINE_HPP


#include "core/memory.hpp"
class Player;
struct player_info;

#include "core/vec.hpp"

class Engine {
public:
  int get_localplayer_index(void) {
    return mem::call_vfunc<int()>(this, 12);
  }
  
  Vec2 get_screen_size(void) {
    int width = 0;
    int height = 0;

    mem::call_vfunc<void(int*, int*)>(this, 5, &width, &height);

    return Vec2{width, height};
  }

  bool is_in_game(void) {
    return mem::call_vfunc<bool()>(this, 26);
  }

  const char* get_level_name(void) {
    return mem::call_vfunc<const char*()>(this, 51);
  }

  bool get_player_info(int entity_index, player_info* pinfo) {
    return mem::call_vfunc<bool(int, player_info*)>(this, 8, entity_index, pinfo);
  }

  int get_player_index_from_id(int user_id) {
    return mem::call_vfunc<int(int)>(this, 9, user_id);
  }

  void client_cmd_unrestricted(const char* cmd) {
    mem::call_vfunc<void(const char*)>(this, 106, cmd);
  }
};

static inline Engine* engine;


#endif
