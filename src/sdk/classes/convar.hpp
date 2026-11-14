export module sdk.classes.convar;
export import std.compat;
export import core.sys;
export import core.memory;
export import core.offsets;

export extern "C++" {


class Convar {
public:
  void set_int(int value) {
    mem::write<int>(this, offsets::convar::int_value, value);
  }

  int get_int() {
    return mem::read<int>(this, offsets::convar::int_value);
  }

  void set_float(float value) {
    mem::write<float>(this, offsets::convar::float_value, value);
  }

  float get_float(float value) {
    return mem::read<float>(this, offsets::convar::float_value);
  }

};


}
