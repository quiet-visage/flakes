export module sdk.classes.material_var;
export import std.compat;
export import core.sys;
export import core.memory;
export import core.vec;

export extern "C++" {



class MaterialVar {
public:
  //11
  void set_vec_value(RGBA_float color) {
    mem::call_vfunc<void(float, float, float)>(this, 11, color.r, color.g, color.b);
  }
};


}
