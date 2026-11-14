#ifndef INPUT_HPP
#define INPUT_HPP


#include "core/memory.hpp"
#include "client.hpp"

struct CameraThirdData_t {
  float pitch;
  float yaw;
  float dist;
  float lag;
  Vec3 hull_min;
  Vec3 hull_max;
};

static user_cmd* (*get_user_cmd_original)(void*, int sequence_number);

class Input {
public:

  user_cmd* get_user_cmd(int sequence_number) {
    user_cmd* usercmd = &this->commands[sequence_number % 90];

    return usercmd;
  }
  
  void to_thirdperson(void) {
    mem::call_vfunc<void()>(this, 32);
  }

  bool is_thirdperson(void) {
    return mem::call_vfunc<bool()>(this, 31);
  }  
  
  void to_firstperson(void) {
    mem::call_vfunc<void()>(this, 33);
  }

public:
  char pad0[0x100];
  
  user_cmd* commands;
};

inline static Input* input;

#endif
