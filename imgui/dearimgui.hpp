#ifndef DEARIMGUI_HPP
#define DEARIMGUI_HPP

#include <SDL2/SDL.h>

#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl2.h"

#include <SDL2/SDL_scancode.h>
#include <string>

static ImGuiStyle orig_style; // Defined in /hooks/sdl.cpp swap_window_hook

namespace ImGui {
  static bool IsImGuiFullyInitialized(void) {
    return (ImGui::GetCurrentContext() != nullptr && ImGui::GetIO().BackendPlatformUserData != nullptr);
  }

  static void TextCentered(std::string text) {
    auto windowWidth = ImGui::GetWindowSize().x;
    auto textWidth   = ImGui::CalcTextSize(text.c_str()).x;

    ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
    ImGui::TextUnformatted(text.c_str());
  }

  static void SectionHeader(const char* label) {
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.52f, 0.88f, 0.76f, 1.00f));
    ImGui::TextUnformatted(label);
    ImGui::PopStyleColor();
    ImGui::Separator();
    ImGui::Spacing();
  }

  static void SliderFloatHeightPad(const char* label, float* v, float v_min, float v_max, float height, const char* format = "%.3f", ImGuiSliderFlags flags = 0) {
    float orig_x = ImGui::GetStyle().FramePadding.x;
    float pad_y = (height > 0.0f) ? height : ImGui::GetStyle().FramePadding.y;
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(orig_x, pad_y));
    ImGui::SliderFloat(label, v, v_min, v_max, format, flags);
    ImGui::PopStyleVar(1);
  }

  static void SliderIntHeightAndLengthPad(const char* label, int* v, int v_min, int v_max, int width, int height, const char* format = "%d", ImGuiSliderFlags flags = 0) {
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(width, height));
    ImGui::SliderInt(label, v, v_min, v_max, format, flags);
    ImGui::PopStyleVar(1);
  }

  static std::string GetKeyName(SDL_Scancode key) {
    if (key >= 0) {
      const char* name = SDL_GetKeyName(SDL_GetKeyFromScancode(key));
      return (name && name[0] != '\0') ? std::string(name) : "None";
    }

    switch (-key) {
    case SDL_BUTTON_LEFT:   return "Mouse 1";
    case SDL_BUTTON_RIGHT:  return "Mouse 2";
    case SDL_BUTTON_MIDDLE: return "Mouse 3";
    case SDL_BUTTON_X1:     return "Mouse 4";
    case SDL_BUTTON_X2:     return "Mouse 5";
    default:                return "Mouse " + std::to_string(-key);
    }
  }

  static void KeybindBox(bool* waitingFlag, int* keycode, const char* id_suffix = "") {
    std::string buttonLabel;
    if (*waitingFlag)
      buttonLabel = "[ Press Key... ]";
    else if (*keycode == SDLK_UNKNOWN)
      buttonLabel = "[ None ]";
    else
      buttonLabel = "[ " + GetKeyName((SDL_Scancode)*keycode) + " ]";

    buttonLabel += "##Keybind";
    buttonLabel += id_suffix;

    if (*waitingFlag) {
      ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.52f, 0.88f, 0.76f, 0.38f));
      ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.52f, 0.88f, 0.76f, 0.90f));
    }

    if (ImGui::Button(buttonLabel.c_str(), ImVec2(115, 0))) {
      *waitingFlag = true;
    }

    if (*waitingFlag) {
      ImGui::PopStyleColor(2);
    }
  }

  static void KeybindEvent(const SDL_Event* event, bool* waitingFlag, int* keycode) {
    if (!*waitingFlag)
      return;

    if (event->type == SDL_KEYDOWN && event->key.repeat == 0) {
      if (event->key.keysym.sym == SDLK_ESCAPE) {
	*waitingFlag = false; // cancel binding
      } else {
	*keycode = event->key.keysym.scancode;
	*waitingFlag = false;
      }
    } else if (event->type == SDL_MOUSEBUTTONDOWN) {
      *keycode = -event->button.button;
      *waitingFlag = false;
    }
  }

}


#endif
