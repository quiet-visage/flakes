#ifndef MEM_HPP
#define MEM_HPP

// Safe(r) access to game memory.
//
// Every read/write of a game object field and every virtual call into the game
// goes through these helpers instead of hand-written pointer casts, so that:
//  - a null base pointer returns a default value instead of crashing the game
//    (the Makefile builds with -fno-delete-null-pointer-checks so the compiler
//    cannot optimise these checks away when the base is a `this` pointer);
//  - reads use memcpy, avoiding strict-aliasing / alignment undefined behaviour;
//  - virtual calls state their full signature once, in one place.

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <type_traits>

namespace mem {

template <typename T>
concept Trivial = std::is_trivially_copyable_v<T> && std::is_default_constructible_v<T>;

// Read a T at `base + offset`. Returns T{} if base is null.
template <Trivial T>
[[nodiscard]] inline T read(const void* base, std::ptrdiff_t offset) {
  if (base == nullptr) return T{};
  T value;
  std::memcpy(&value, static_cast<const std::byte*>(base) + offset, sizeof(T));
  return value;
}

// Write a T at `base + offset`. Does nothing if base is null.
template <Trivial T>
inline void write(void* base, std::ptrdiff_t offset, const T& value) {
  if (base == nullptr) return;
  std::memcpy(static_cast<std::byte*>(base) + offset, &value, sizeof(T));
}

// Address of the field at `base + offset`, or nullptr if base is null.
[[nodiscard]] inline void* field_address(void* base, std::ptrdiff_t offset) {
  if (base == nullptr) return nullptr;
  return static_cast<std::byte*>(base) + offset;
}

// Call virtual function number `index` on `object`, with the signature given
// as a function type (without the implicit `this`), e.g.
//   mem::call_vfunc<bool(int, float)>(object, 12, a, b);
// Arguments are converted to the declared parameter types, so an int literal
// passed to a float parameter still goes into the right register.
// Returns Ret{} (or nothing) if object is null.
template <typename Signature> struct VFunc;

template <typename Ret, typename... Args>
struct VFunc<Ret(Args...)> {
  static Ret call(void* object, std::size_t index, Args... args) {
    if (object == nullptr) {
      if constexpr (std::is_void_v<Ret>) return;
      else return Ret{};
    }
    using Fn = Ret (*)(void*, Args...);
    void** vtable = *static_cast<void***>(object);
    return reinterpret_cast<Fn>(vtable[index])(object, args...);
  }
};

template <typename Signature, typename... CallArgs>
inline decltype(auto) call_vfunc(const void* object, std::size_t index, CallArgs&&... args) {
  return VFunc<Signature>::call(const_cast<void*>(object), index, static_cast<CallArgs&&>(args)...);
}

// Resolve an x86-64 RIP-relative operand: `instruction` points at an
// instruction whose 32-bit signed displacement starts `disp_offset` bytes in
// and which is `length` bytes long. Returns the absolute target address.
[[nodiscard]] inline std::uintptr_t resolve_rip_relative(std::uintptr_t instruction, std::size_t disp_offset, std::size_t length) {
  std::int32_t displacement;
  std::memcpy(&displacement, reinterpret_cast<const void*>(instruction + disp_offset), sizeof(displacement));
  return instruction + length + displacement;
}

} // namespace mem

#endif
