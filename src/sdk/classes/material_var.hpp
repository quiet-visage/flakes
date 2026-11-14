#ifndef MATERIAL_VAR_HPP
#define MATERIAL_VAR_HPP


#include "core/memory.hpp"
#include "core/vec.hpp"

class MaterialVar {
public:
  //11
  void set_vec_value(RGBA_float color) {
    mem::call_vfunc<void(float, float, float)>(this, 11, color.r, color.g, color.b);
  }
};

#endif
