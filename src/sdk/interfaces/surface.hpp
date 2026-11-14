#ifndef SURFACE_HPP
#define SURFACE_HPP


#include "core/memory.hpp"
#include <wchar.h>

#include "core/vec.hpp"

#include "gui/config.hpp"

class Surface {
public:
  void set_rgba(int r, int g, int b, int a) {
    mem::call_vfunc<void(int, int, int, int)>(this, 10, r, g, b, a);
  }

  void set_rgba(RGBA color) {
    mem::call_vfunc<void(int, int, int, int)>(this, 10, color.r, color.g, color.b, color.a);
  }

  
  void draw_line(int x1, int y1, int x2, int y2) {
    mem::call_vfunc<void(int, int, int, int)>(this, 15, x1, y1, x2, y2);
  }

  
  void draw_filled_rect(int x1, int y1, int x2, int y2) {
    mem::call_vfunc<void(int, int, int, int)>(this, 12, x1, y1, x2, y2);
  }

  void draw_outlined_rect(int x1, int y1, int x2, int y2) {
    mem::call_vfunc<void(int, int, int, int)>(this, 14, x1, y1, x2, y2);
  }
  
  unsigned long text_create_font(void) {
    return mem::call_vfunc<unsigned long()>(this, 66);
  }

  char text_set_font_glyph_set(unsigned long font, const char *name, int tall, int weight, int blur, int scanlines, int flags) {
    return mem::call_vfunc<char(unsigned long, const char*, int, int, int, int, int, int, int)>(this, 67, font, name, tall, weight, blur, scanlines, flags, 0, 0);
  }

  int get_font_height(unsigned long font) {
    return config.diagnostics.font_height; //CHANGE ME: use virtual function GetFontTall instead
  }
  
  int get_character_width(unsigned long font, int character) {
    return mem::call_vfunc<int(unsigned long, int)>(this, 74, font, character);
  }

  unsigned int get_string_width(unsigned long font, const wchar_t* string) {
    unsigned int string_width = 0;
    for (unsigned int i = 0; i < wcslen(string); ++i) {
      string_width += get_character_width(font, string[i]);
    }

    return string_width;
  }
  
  void draw_set_text_font(unsigned long font) {
    mem::call_vfunc<void(unsigned long)>(this, 17, font);
  }

  void draw_set_text_color(int r, int g, int b, int a) {
    mem::call_vfunc<void(int, int, int, int)>(this, 18, r, g, b, a);
  }

  void draw_set_text_color(RGBA color) {
    mem::call_vfunc<void(int, int, int, int)>(this, 18, color.r, color.g, color.b, color.a);
  }
  
  void draw_set_text_pos(unsigned int x, unsigned int y) {
    mem::call_vfunc<void(unsigned int, unsigned int)>(this, 20, x, y);
  }

  void draw_print_text(const wchar_t *text, int text_len) {
    mem::call_vfunc<void(const wchar_t*, int, int)>(this, 22, text, text_len, 0);
  }

  void set_cursor_visible(bool visible) {
    mem::call_vfunc<void(bool)>(this, 52, visible);
  }

  void draw_circle(int x, int y, int radius, int segments) {
    mem::call_vfunc<void(int, int, int, int)>(this, 99, x, y, radius, segments);
  }

};

static inline Surface* surface;

#endif
