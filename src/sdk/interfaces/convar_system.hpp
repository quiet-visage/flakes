#ifndef CONVAR_SYSTEM_HPP
#define CONVAR_SYSTEM_HPP


#include "core/memory.hpp"
#include "sdk/classes/convar.hpp"

class ConvarSystem {
public:
  Convar* find_var(const char* var_name) {
    return mem::call_vfunc<Convar*(const char*)>(this, 12, var_name);
  }

};

inline static ConvarSystem* convar_system;

#endif
