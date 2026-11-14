export module sdk.interfaces.convar_system;
export import std.compat;
export import core.sys;
export import core.memory;
export import sdk.classes.convar;

export extern "C++" {



class ConvarSystem {
public:
  Convar* find_var(const char* var_name) {
    return mem::call_vfunc<Convar*(const char*)>(this, 12, var_name);
  }

};

inline ConvarSystem* convar_system;


}
