module;

#include <unistd.h>
#include <dlfcn.h>
#include <sys/mman.h>
#include <csignal>
#include <climits>
#include <cfloat>
#include <cmath>
#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#if __has_include(<GL/glew.h>)
#include <GL/glew.h>
#endif
#include <GL/gl.h>
#include <vulkan/vulkan.h>
#include <curl/curl.h>

#define IMGUI_DISABLE_OBSOLETE_FUNCTIONS
#include "funchook.h"
#include "micropather/micropather.h"
#include "md5/md5.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_stdlib.h"
#include "imgui/imgui_impl_sdl2.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_impl_vulkan.h"

#undef RTLD_LAZY
#undef RTLD_NOW
#undef RTLD_NOLOAD
#undef RTLD_DEFAULT
#undef PROT_READ
#undef PROT_WRITE
#undef PROT_EXEC
#undef _SC_PAGESIZE
#undef SIGFPE
#undef SIGKILL
#undef INT_MAX
#undef INT_MIN
#undef FLT_MAX
#undef M_PI
#undef SDL_BUTTON
#undef SDL_BUTTON_LEFT
#undef SDL_BUTTON_MIDDLE
#undef SDL_BUTTON_RIGHT
#undef SDL_BUTTON_X1
#undef SDL_BUTTON_X2
#undef GLEW_OK
#undef VK_NULL_HANDLE
#undef VK_TRUE
#undef VK_API_VERSION_1_4
#undef CURL_HTTP_VERSION_1_1
#undef IM_COL32
#undef IM_ARRAYSIZE
#undef IM_ASSERT
#undef MP_VECTOR

export module core.sys;
export import std.compat;

#if !__has_include(<GL/glew.h>)
extern "C" {
  unsigned int glewInit(void);
  const unsigned char* glewGetErrorString(unsigned int error);
}
#endif

extern "C" void* sigscan_pid_module(int pid, const char* regex, const char* ida_pattern);

export extern "C++" {
  // POSIX / C constants & functions
  inline constexpr int RTLD_LAZY = 0x00001;
  inline constexpr int RTLD_NOW = 0x00002;
  inline constexpr int RTLD_NOLOAD = 0x00004;
  inline void* const RTLD_DEFAULT = nullptr;

  inline constexpr int PROT_READ = 0x1;
  inline constexpr int PROT_WRITE = 0x2;
  inline constexpr int PROT_EXEC = 0x4;

  inline constexpr int SIGFPE = 8;
  inline constexpr int SIGKILL = 9;

  inline constexpr int INT_MAX = __INT_MAX__;
  inline constexpr int INT_MIN = (-__INT_MAX__ - 1);
  inline constexpr float FLT_MAX = __FLT_MAX__;
  inline constexpr double M_PI = 3.14159265358979323846;

  using ::dlopen;
  using ::dlsym;
  using ::dlclose;
  using ::dlerror;
  using ::mprotect;
  using ::usleep;
  using ::sleep;
  using ::getpid;
  using ::kill;
  using ::access;
  using ::sysconf;
  using ::_SC_PAGESIZE;

  // SDL2 types, enums, constants, and functions
  using ::Uint8;
  using ::Uint16;
  using ::Uint32;
  using ::Uint64;
  using ::SDL_bool;
  using ::SDL_Window;
  using ::SDL_Event;
  using ::SDL_KeyboardEvent;
  using ::SDL_Keycode;
  using ::SDL_Scancode;
  using ::SDL_GLContext;
  using ::SDL_SysWMinfo;
  using ::SDL_eventaction;

  using ::SDL_KEYDOWN;
  using ::SDL_KEYUP;
  using ::SDL_MOUSEBUTTONDOWN;
  using ::SDL_MESSAGEBOX_ERROR;

  using ::SDLK_UNKNOWN;
  using ::SDLK_ESCAPE;
  using ::SDLK_SPACE;
  using ::SDLK_END;
  using ::SDLK_LCTRL;
  using ::SDLK_w;
  using ::SDLK_a;
  using ::SDLK_s;
  using ::SDLK_d;

  using ::SDL_SCANCODE_LSHIFT;
  using ::SDL_SCANCODE_LALT;

  inline constexpr int SDL_BUTTON_LEFT = 1;
  inline constexpr int SDL_BUTTON_MIDDLE = 2;
  inline constexpr int SDL_BUTTON_RIGHT = 3;
  inline constexpr int SDL_BUTTON_X1 = 4;
  inline constexpr int SDL_BUTTON_X2 = 5;
  inline constexpr Uint32 SDL_BUTTON(Uint32 x) { return 1u << (x - 1); }

  using ::SDL_ShowSimpleMessageBox;
  using ::SDL_GetKeyboardState;
  using ::SDL_GetMouseState;
  using ::SDL_GetKeyFromScancode;
  using ::SDL_GetKeyName;
  using ::SDL_PollEvent;
  using ::SDL_AddEventWatch;
  using ::SDL_SetEventFilter;
  using ::SDL_GetWindowFlags;
  using ::SDL_GetWindowSize;
  using ::SDL_GetWindowWMInfo;
  using ::SDL_GL_GetCurrentContext;
  using ::SDL_GL_CreateContext;
  using ::SDL_GL_MakeCurrent;
  using ::SDL_GL_SwapWindow;

  // OpenGL / GLEW
  using ::GLenum;
  using ::glewInit;
  using ::glewGetErrorString;
  inline constexpr unsigned int GLEW_OK = 0;

  // Vulkan
  inline constexpr std::nullptr_t VK_NULL_HANDLE = nullptr;
  inline constexpr uint32_t VK_TRUE = 1U;
  inline constexpr uint32_t VK_API_VERSION_1_4 = (1U << 22) | (4U << 12);

  using ::VkInstance;
  using ::VkInstanceCreateInfo;
  using ::VkPhysicalDevice;
  using ::VkPhysicalDeviceProperties;
  using ::VkDevice;
  using ::VkDeviceCreateInfo;
  using ::VkDeviceQueueCreateInfo;
  using ::VkQueue;
  using ::VkQueueFamilyProperties;
  using ::VkCommandPoolCreateInfo;
  using ::VkCommandBufferAllocateInfo;
  using ::VkCommandBufferBeginInfo;
  using ::VkDescriptorPool;
  using ::VkDescriptorPoolSize;
  using ::VkDescriptorPoolCreateInfo;
  using ::VkRenderPass;
  using ::VkRenderPassCreateInfo;
  using ::VkRenderPassBeginInfo;
  using ::VkPipelineCache;
  using ::VkPipelineStageFlags;
  using ::VkAttachmentDescription;
  using ::VkAttachmentReference;
  using ::VkSubpassDescription;
  using ::VkImage;
  using ::VkImageView;
  using ::VkImageViewCreateInfo;
  using ::VkFramebufferCreateInfo;
  using ::VkFence;
  using ::VkFenceCreateInfo;
  using ::VkSemaphore;
  using ::VkSemaphoreCreateInfo;
  using ::VkSubmitInfo;
  using ::VkPresentInfoKHR;
  using ::VkSwapchainKHR;
  using ::VkSwapchainCreateInfoKHR;
  using ::VkAcquireNextImageInfoKHR;
  using ::VkAllocationCallbacks;
  using ::VkExtent2D;
  using ::VkResult;

  using ::VK_SUCCESS;
  using ::VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  using ::VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
  using ::VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  using ::VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  using ::VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  using ::VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  using ::VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  using ::VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  using ::VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  using ::VK_STRUCTURE_TYPE_SUBMIT_INFO;
  using ::VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  using ::VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
  using ::VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  using ::VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
  using ::VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;
  using ::VK_QUEUE_GRAPHICS_BIT;
  using ::VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
  using ::VK_DESCRIPTOR_TYPE_SAMPLER;
  using ::VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  using ::VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
  using ::VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
  using ::VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER;
  using ::VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER;
  using ::VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  using ::VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
  using ::VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
  using ::VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
  using ::VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
  using ::VK_FORMAT_B8G8R8A8_UNORM;
  using ::VK_SAMPLE_COUNT_1_BIT;
  using ::VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  using ::VK_ATTACHMENT_LOAD_OP_LOAD;
  using ::VK_ATTACHMENT_STORE_OP_STORE;
  using ::VK_ATTACHMENT_STORE_OP_DONT_CARE;
  using ::VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
  using ::VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
  using ::VK_PIPELINE_BIND_POINT_GRAPHICS;
  using ::VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
  using ::VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  using ::VK_SUBPASS_CONTENTS_INLINE;
  using ::VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
  using ::VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  using ::VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
  using ::VK_IMAGE_VIEW_TYPE_2D;
  using ::VK_IMAGE_ASPECT_COLOR_BIT;
  using ::VK_FENCE_CREATE_SIGNALED_BIT;

  using ::vkCreateInstance;
  using ::vkEnumeratePhysicalDevices;
  using ::vkGetPhysicalDeviceProperties;
  using ::vkGetPhysicalDeviceQueueFamilyProperties;
  using ::vkCreateDevice;
  using ::vkDestroyDevice;
  using ::vkGetDeviceProcAddr;
  using ::vkGetDeviceQueue;
  using ::vkCreateDescriptorPool;
  using ::vkCreateRenderPass;
  using ::vkCreateCommandPool;
  using ::vkDestroyCommandPool;
  using ::vkAllocateCommandBuffers;
  using ::vkFreeCommandBuffers;
  using ::vkResetCommandBuffer;
  using ::vkBeginCommandBuffer;
  using ::vkEndCommandBuffer;
  using ::vkCmdBeginRenderPass;
  using ::vkCmdEndRenderPass;
  using ::vkCreateImageView;
  using ::vkDestroyImageView;
  using ::vkCreateFramebuffer;
  using ::vkDestroyFramebuffer;
  using ::vkCreateFence;
  using ::vkDestroyFence;
  using ::vkWaitForFences;
  using ::vkResetFences;
  using ::vkCreateSemaphore;
  using ::vkDestroySemaphore;
  using ::vkQueueSubmit;
  using ::vkQueuePresentKHR;
  using ::vkCreateSwapchainKHR;
  using ::vkGetSwapchainImagesKHR;
  using ::vkAcquireNextImageKHR;
  using ::vkAcquireNextImage2KHR;

  // libcurl
  using ::CURL;
  using ::CURLcode;
  using ::curl_slist;
  using ::curl_easy_init;
  using ::curl_easy_setopt;
  using ::curl_easy_perform;
  using ::curl_slist_append;
  using ::CURLOPT_URL;
  using ::CURLOPT_NOPROGRESS;
  using ::CURLOPT_POST;
  using ::CURLOPT_COPYPOSTFIELDS;
  using ::CURLOPT_VERBOSE;
  using ::CURLOPT_HEADER;
  using ::CURLOPT_HTTPHEADER;
  using ::CURLOPT_HTTP_VERSION;
  using ::CURLOPT_TCP_KEEPALIVE;
  inline constexpr long CURL_HTTP_VERSION_1_1 = 2L;

  // funchook & libsigscan & md5 & micropather
  using ::funchook_t;
  using ::funchook_create;
  using ::funchook_prepare;
  using ::funchook_install;
  using ::funchook_uninstall;

  using ::sigscan_pid_module;
  inline void* sigscan_module(const char* regex, const char* ida_pattern) {
    return sigscan_pid_module(-1, regex, ida_pattern);
  }

  using ::MD5_PseudoRandom;

  namespace micropather {
    using ::micropather::Graph;
    using ::micropather::MicroPather;
    using ::micropather::StateCost;
  }
  template <typename T>
  using MP_VECTOR = std::vector<T>;

  // ImGui types, enums, constants, and functions
  using ::ImVec2;
  using ::ImVec4;
  using ::ImFont;
  using ::ImFontConfig;
  using ::ImDrawList;
  using ::ImGuiIO;
  using ::ImGuiStyle;
  using ::ImGuiSliderFlags;
  using ::ImU32;
  using ::ImWchar;

  using ::ImGuiChildFlags_Borders;
  using ::ImGuiChildFlags_None;
  using ::ImGuiCol_Border;
  using ::ImGuiCol_BorderShadow;
  using ::ImGuiCol_Button;
  using ::ImGuiCol_ButtonActive;
  using ::ImGuiCol_ButtonHovered;
  using ::ImGuiCol_CheckMark;
  using ::ImGuiCol_ChildBg;
  using ::ImGuiCol_FrameBg;
  using ::ImGuiCol_FrameBgActive;
  using ::ImGuiCol_FrameBgHovered;
  using ::ImGuiCol_Header;
  using ::ImGuiCol_HeaderActive;
  using ::ImGuiCol_HeaderHovered;
  using ::ImGuiCol_PlotHistogram;
  using ::ImGuiCol_PopupBg;
  using ::ImGuiCol_ScrollbarBg;
  using ::ImGuiCol_ScrollbarGrab;
  using ::ImGuiCol_ScrollbarGrabActive;
  using ::ImGuiCol_ScrollbarGrabHovered;
  using ::ImGuiCol_Separator;
  using ::ImGuiCol_SeparatorActive;
  using ::ImGuiCol_SeparatorHovered;
  using ::ImGuiCol_SliderGrab;
  using ::ImGuiCol_SliderGrabActive;
  using ::ImGuiCol_Text;
  using ::ImGuiCol_TextDisabled;
  using ::ImGuiCol_TitleBg;
  using ::ImGuiCol_TitleBgActive;
  using ::ImGuiCol_TitleBgCollapsed;
  using ::ImGuiCol_WindowBg;
  using ::ImGuiColorEditFlags_NoInputs;
  using ::ImGuiColorEditFlags_NoTooltip;
  using ::ImGuiCond_Always;
  using ::ImGuiCond_FirstUseEver;
  using ::ImGuiConfigFlags_NoMouseCursorChange;
  using ::ImGuiInputTextFlags_Password;
  using ::ImGuiKey_End;
  using ::ImGuiKey_F11;
  using ::ImGuiMouseButton_Left;
  using ::ImGuiStyleVar_ButtonTextAlign;
  using ::ImGuiStyleVar_FramePadding;
  using ::ImGuiWindowFlags_NoCollapse;
  using ::ImGuiWindowFlags_NoResize;
  using ::ImGuiWindowFlags_NoScrollWithMouse;
  using ::ImGuiWindowFlags_NoScrollbar;
  using ::ImGuiItemFlags_ButtonRepeat;

  using ::ImGui_ImplOpenGL3_Init;
  using ::ImGui_ImplOpenGL3_NewFrame;
  using ::ImGui_ImplOpenGL3_RenderDrawData;
  using ::ImGui_ImplSDL2_InitForOpenGL;
  using ::ImGui_ImplSDL2_InitForVulkan;
  using ::ImGui_ImplSDL2_NewFrame;
  using ::ImGui_ImplSDL2_ProcessEvent;
  using ::ImGui_ImplVulkanH_Frame;
  using ::ImGui_ImplVulkanH_FrameSemaphores;
  using ::ImGui_ImplVulkan_Init;
  using ::ImGui_ImplVulkan_InitInfo;
  using ::ImGui_ImplVulkan_NewFrame;
  using ::ImGui_ImplVulkan_RenderDrawData;

  inline constexpr ImU32 IM_COL32(int r, int g, int b, int a) {
    return (((ImU32)(a) << 24) | ((ImU32)(b) << 16) | ((ImU32)(g) << 8) | ((ImU32)(r) << 0));
  }

  template <typename T, std::size_t N>
  inline constexpr int IM_ARRAYSIZE(const T (&)[N]) {
    return static_cast<int>(N);
  }

  inline void IM_ASSERT(bool expr) {
    if (!expr) __builtin_trap();
  }

  inline ImGuiStyle orig_style;

  namespace ImGui {
    using ::ImGui::Begin;
    using ::ImGui::BeginChild;
    using ::ImGui::BeginCombo;
    using ::ImGui::Button;
    using ::ImGui::CalcTextSize;
    using ::ImGui::Checkbox;
    using ::ImGui::ColorEdit4;
    using ::ImGui::Combo;
    using ::ImGui::CreateContext;
    using ::ImGui::End;
    using ::ImGui::EndChild;
    using ::ImGui::EndCombo;
    using ::ImGui::GetBackgroundDrawList;
    using ::ImGui::GetContentRegionAvail;
    using ::ImGui::GetCurrentContext;
    using ::ImGui::GetCursorPosY;
    using ::ImGui::GetDrawData;
    using ::ImGui::GetIO;
    using ::ImGui::GetStyle;
    using ::ImGui::GetTextLineHeightWithSpacing;
    using ::ImGui::GetWindowHeight;
    using ::ImGui::GetWindowSize;
    using ::ImGui::InputText;
    using ::ImGui::IsAnyItemActive;
    using ::ImGui::IsKeyPressed;
    using ::ImGui::IsMouseDown;
    using ::ImGui::NewFrame;
    using ::ImGui::NewLine;
    using ::ImGui::PopItemFlag;
    using ::ImGui::PopStyleColor;
    using ::ImGui::PopStyleVar;
    using ::ImGui::ProgressBar;
    using ::ImGui::PushItemFlag;
    using ::ImGui::PushStyleColor;
    using ::ImGui::PushStyleVar;
    using ::ImGui::Render;
    using ::ImGui::SameLine;
    using ::ImGui::Selectable;
    using ::ImGui::Separator;
    using ::ImGui::SetCursorPosX;
    using ::ImGui::SetCursorPosY;
    using ::ImGui::SetItemDefaultFocus;
    using ::ImGui::SetNextItemWidth;
    using ::ImGui::SetNextWindowSize;
    using ::ImGui::SliderFloat;
    using ::ImGui::SliderInt;
    using ::ImGui::Spacing;
    using ::ImGui::StyleColorsDark;
    using ::ImGui::TextDisabled;
    using ::ImGui::TextUnformatted;

    inline void PushButtonRepeat(bool repeat) {
      PushItemFlag(ImGuiItemFlags_ButtonRepeat, repeat);
    }

    inline void PopButtonRepeat() {
      PopItemFlag();
    }

    inline bool IsImGuiFullyInitialized(void) {
      return (ImGui::GetCurrentContext() != nullptr && ImGui::GetIO().BackendPlatformUserData != nullptr);
    }

    inline void TextCentered(std::string text) {
      auto windowWidth = ImGui::GetWindowSize().x;
      auto textWidth   = ImGui::CalcTextSize(text.c_str()).x;

      ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
      ImGui::TextUnformatted(text.c_str());
    }

    inline void SectionHeader(const char* label) {
      ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.52f, 0.88f, 0.76f, 1.00f));
      ImGui::TextUnformatted(label);
      ImGui::PopStyleColor();
      ImGui::Separator();
      ImGui::Spacing();
    }

    inline void SliderFloatHeightPad(const char* label, float* v, float v_min, float v_max, float height, const char* format = "%.3f", ImGuiSliderFlags flags = 0) {
      float orig_x = ImGui::GetStyle().FramePadding.x;
      float pad_y = (height > 0.0f) ? height : ImGui::GetStyle().FramePadding.y;
      ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(orig_x, pad_y));
      ImGui::SliderFloat(label, v, v_min, v_max, format, flags);
      ImGui::PopStyleVar(1);
    }

    inline void SliderIntHeightAndLengthPad(const char* label, int* v, int v_min, int v_max, int width, int height, const char* format = "%d", ImGuiSliderFlags flags = 0) {
      ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(width, height));
      ImGui::SliderInt(label, v, v_min, v_max, format, flags);
      ImGui::PopStyleVar(1);
    }

    inline std::string GetKeyName(SDL_Scancode key) {
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

    inline void KeybindBox(bool* waitingFlag, int* keycode, const char* id_suffix = "") {
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

    inline void KeybindEvent(const SDL_Event* event, bool* waitingFlag, int* keycode) {
      if (!*waitingFlag)
        return;

      if (event->type == SDL_KEYDOWN && event->key.repeat == 0) {
        if (event->key.keysym.sym == SDLK_ESCAPE) {
          *waitingFlag = false;
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
}
