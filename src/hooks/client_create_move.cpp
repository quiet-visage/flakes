#include "sdk/interfaces/global_vars.hpp"
#include "sdk/interfaces/steam_friends.hpp"

#include "sdk/classes/player.hpp"

#include "core/entity_cache.hpp"

void (*client_create_move_original)(void*, int, float, bool);

void client_create_move_hook(void* me, int sequence_number, float input_sample_frametime, bool active) {
  client_create_move_original(me, sequence_number, input_sample_frametime, active);
}
