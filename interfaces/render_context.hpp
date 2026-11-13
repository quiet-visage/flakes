#ifndef RENDER_CONTEXT_HPP
#define RENDER_CONTEXT_HPP


#include "../mem.hpp"
enum MaterialCullMode {
  MATERIAL_CULLMODE_CCW,
  MATERIAL_CULLMODE_CW
};

enum StencilComparisonMode {
  STENCILCOMPARISONFUNCTION_NEVER = 1,
  STENCILCOMPARISONFUNCTION_LESS = 2,
  STENCILCOMPARISONFUNCTION_EQUAL = 3,
  STENCILCOMPARISONFUNCTION_LESSEQUAL = 4,
  STENCILCOMPARISONFUNCTION_GREATER = 5,
  STENCILCOMPARISONFUNCTION_NOTEQUAL = 6,
  STENCILCOMPARISONFUNCTION_GREATEREQUAL = 7,
  STENCILCOMPARISONFUNCTION_ALWAYS = 8,
  STENCILCOMPARISONFUNCTION_FORCE_DWORD = 0x7fffffff
};

enum StencilOperation {
  STENCILOPERATION_KEEP = 1,
  STENCILOPERATION_ZERO = 2,
  STENCILOPERATION_REPLACE = 3,
  STENCILOPERATION_INCRSAT = 4,
  STENCILOPERATION_DECRSAT = 5,
  STENCILOPERATION_INVERT = 6,
  STENCILOPERATION_INCR = 7,
  STENCILOPERATION_DECR = 8,
  STENCILOPERATION_FORCE_DWORD = 0x7fffffff
};

class RenderContext {
public:

  //11
  void set_depth_range(float near, float far) {
    mem::call_vfunc<void(float, float)>(this, 11, near, far);
  }

  
  void set_stencil_test_mask(int mask) {
    mem::call_vfunc<void(int)>(this, 123, mask);
  }

  void set_stencil_write_mask(int mask) {
    mem::call_vfunc<void(int)>(this, 124, mask);
  }
  
  void set_stencil_reference_count(int reference_count) {
    mem::call_vfunc<void(int)>(this, 122, reference_count);
  }

  void set_stencil_fail_mode(StencilOperation stencil_operation) {
    mem::call_vfunc<void(StencilOperation)>(this, 118, stencil_operation);
  }
  
  void set_stencil_zfail_mode(StencilOperation stencil_operation) {
    mem::call_vfunc<void(StencilOperation)>(this, 119, stencil_operation);
  }
  
  void set_stencil_pass_mode(StencilOperation stencil_operation) {
    mem::call_vfunc<void(StencilOperation)>(this, 120, stencil_operation);
  }

  
  void set_stencil_compare_mode(StencilComparisonMode stencil_compare_mode) {
    mem::call_vfunc<void(StencilComparisonMode)>(this, 121, stencil_compare_mode);
  }
  
  void clear_buffers(bool clear_color, bool clear_depth, bool clear_stencil) {
    mem::call_vfunc<void(bool, bool, bool)>(this, 12, clear_color, clear_depth, clear_stencil);
  }
  
  
  void set_stencil_enable(bool value) {
    mem::call_vfunc<void(bool)>(this, 117, value);
  }
  
  void set_cull_mode(MaterialCullMode cull_mode) {
    mem::call_vfunc<void(MaterialCullMode)>(this, 40, cull_mode);
  }
  
};

#endif
