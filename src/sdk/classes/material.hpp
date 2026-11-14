#ifndef MATERIAL_HPP
#define MATERIAL_HPP


#include "core/memory.hpp"
#include "core/vec.hpp"

#include "material_var.hpp"

class Material {
public:

  //11
  MaterialVar* find_var(const char* var_name, bool* found = nullptr, bool complain = false) {
    return mem::call_vfunc<MaterialVar*(const char*, bool*, bool)>(this, 11, var_name, found, complain);
  }
  

  void increment_reference_count(void) {
    mem::call_vfunc<void()>(this, 12);
  }

  void alpha_modulate(float alpha) {
    mem::call_vfunc<void(float)>(this, 27, alpha);
  }
  
  void color_modulate(RGBA_float color) {
    mem::call_vfunc<void(float, float, float)>(this, 28, color.r, color.g, color.b);
  }

  void set_material_flag(int flag, bool on) {
    mem::call_vfunc<void(int, bool)>(this, 29, flag, on);
  }

  bool get_material_flag(int flag) {
    return mem::call_vfunc<bool(int)>(this, 30, flag);
  }
};

#endif
