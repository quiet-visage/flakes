export module sdk.interfaces.attribute_manager;
export import std.compat;
export import core.sys;

export extern "C++" {

class Entity;

inline float (*attribute_hook_value_float_original)(float, const char*, Entity*, void*, bool);

class AttributeManager {
public:
  float attrib_hook_value(float value, const char* attrib_name, Entity* entity) {
    return attribute_hook_value_float_original(value, attrib_name, entity, nullptr, true);
  }  
};

inline AttributeManager* attribute_manager = new AttributeManager();


}
