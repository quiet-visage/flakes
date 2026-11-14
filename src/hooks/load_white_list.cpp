#include "gui/config.hpp"

void* (*load_white_list_original)(void*, const char*);

void* load_white_list_hook(void* me, const char* str) {
  if (config.simulation.directives.bypass_sv_pure == true) {
    return nullptr;
  }

  return load_white_list_original(me, str);
}
