#ifndef PLAYER_LIST_HPP
#define PLAYER_LIST_HPP


#include "core/memory.hpp"
#include "sdk/interfaces/engine.hpp"

class Entity;
class Player;

class EntityList {
public:
  Entity* entity_from_index(unsigned int index) {
    return mem::call_vfunc<Entity*(unsigned int)>(this, 3, index);
  }

  Player* player_from_index(unsigned int index) {
    return (Player*)this->entity_from_index(index);
  }

  Entity* entity_from_handle(int handle) {
    if (handle == 0 || handle == -1) {
      return nullptr;
    }
    unsigned int index = (unsigned int)handle & 0xFFF; // ENT_ENTRY_MASK = (1 << 12) - 1
    if (index == 0xFFF) {
      return nullptr;
    }
    return this->entity_from_index(index);
  }
  
  Player* get_localplayer(void) {
    return this->player_from_index(engine->get_localplayer_index());
  }
  
  Entity* get_entity_from_id(int user_id) {
    return this->entity_from_index(engine->get_player_index_from_id(user_id));
  }

  Player* get_player_from_id(int user_id) {
    return this->player_from_index(engine->get_player_index_from_id(user_id));
  }

  
  int get_max_entities(void) {
    return mem::call_vfunc<int()>(this, 8);
  }
};

static inline EntityList* entity_list;

#endif
