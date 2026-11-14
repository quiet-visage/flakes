export module core.assert;
export import std.compat;
export import core.sys;

export extern "C++" {
  // Create an error window with a custom message
  inline void error_box(const char* message) {
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,
                             "Fatal Error",
                             message,
                             nullptr);
  }

  // Asserts if expression is true
  // Creates an error window with a custom message, and then exits the program.
  inline void error_assert(bool expression, const char* message) {
    if (expression) {
      error_box(message);
      exit(1);
    }
  }
}
