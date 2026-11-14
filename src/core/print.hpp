export module core.print;
export import std.compat;
export import core.sys;

export extern "C++" {
  inline FILE* log_file = nullptr;

  inline void print(const char* fmt, ...) {
    if (!log_file) {
      log_file = fopen("/tmp/tf2.log", "w");
    }

    va_list args;
    __builtin_va_start(args, fmt);
    vfprintf(log_file, fmt, args);
    fflush(log_file);
    __builtin_va_end(args);
  }

  // TEMP DEBUG: returns true at most once every `seconds` per call site key.
  inline bool debug_every(int* last, int seconds = 3) {
    time_t now = time(nullptr);
    if (now - *last < seconds) return false;
    *last = (int)now;
    return true;
  }
}
