#ifndef LOOKUP_HPP
#define LOOKUP_HPP

// Finding things in the game: interfaces, signatures and RIP-relative globals.
//
// Every lookup returns std::expected instead of a raw pointer, so a failure
// carries a message and the caller has to decide what to do with it (abort
// startup cleanly, or skip one feature) instead of crashing on a null pointer.

#include <cstdint>
#include <expected>
#include <format>
#include <string>

#include <dlfcn.h>

#include "libsigscan/libsigscan.h"
#include "core/memory.hpp"
#include "core/print.hpp"

namespace lookup {

using Error = std::string;

template <typename T>
using Result = std::expected<T, Error>;

// CreateInterface(version) from an already-loaded game library.
template <typename T = void>
Result<T*> interface(const char* lib_path, const char* version) {
  void* handle = dlopen(lib_path, RTLD_NOLOAD | RTLD_NOW);
  if (handle == nullptr) {
    return std::unexpected(std::format("{} is not loaded", lib_path));
  }

  using CreateInterfaceFn = void* (*)(const char*, int*);
  auto create_interface = reinterpret_cast<CreateInterfaceFn>(dlsym(handle, "CreateInterface"));
  dlclose(handle);

  if (create_interface == nullptr) {
    return std::unexpected(std::format("{} does not export CreateInterface", lib_path));
  }

  void* found = create_interface(version, nullptr);
  if (found == nullptr) {
    return std::unexpected(std::format("interface {} not found in {}", version, lib_path));
  }

  print("%s interface found at %p\n", version, found);
  return static_cast<T*>(found);
}

// Address of the first match of an IDA-style pattern in modules matching `module`.
inline Result<std::uintptr_t> pattern(const char* module, const char* ida_pattern, const char* what) {
  void* address = sigscan_module(module, ida_pattern);
  if (address == nullptr) {
    return std::unexpected(std::format("signature for {} not found in {}", what, module));
  }
  return reinterpret_cast<std::uintptr_t>(address);
}

// A function located by signature, cast to its function pointer type.
template <typename Fn>
Result<Fn> function(const char* module, const char* ida_pattern, const char* what) {
  return pattern(module, ida_pattern, what).transform([](std::uintptr_t address) {
    return reinterpret_cast<Fn>(address);
  });
}

// The target of a RIP-relative instruction located by signature, e.g.
// `lea rax, [rip + disp32]` (displacement at +3, instruction length 7).
inline Result<std::uintptr_t> rip_relative(const char* module, const char* ida_pattern, const char* what,
                                           std::size_t disp_offset = 3, std::size_t length = 7) {
  return pattern(module, ida_pattern, what).transform([=](std::uintptr_t address) {
    return mem::resolve_rip_relative(address, disp_offset, length);
  });
}

// Read a non-null pointer stored at `address` (e.g. a global `CInput* g_pInput`).
template <typename T = void>
Result<T*> deref(std::uintptr_t address, const char* what) {
  T* value = mem::read<T*>(reinterpret_cast<const void*>(address), 0);
  if (value == nullptr) {
    return std::unexpected(std::format("{} is null", what));
  }
  return value;
}

} // namespace lookup

#endif
