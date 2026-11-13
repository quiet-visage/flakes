#ifndef PRINT_HPP
#define PRINT_HPP

#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>
#include <time.h>

static FILE* log_file = NULL;
static void print(const char* fmt, ...) {

  if (!log_file) {
    log_file = fopen("/tmp/tf2.log", "w");
  }

  va_list args;

  va_start(args, fmt);

  vfprintf(log_file, fmt, args);

  fflush(log_file);

  va_end(args);
}

// TEMP DEBUG: returns true at most once every `seconds` per call site key.
static bool debug_every(int* last, int seconds = 3) {
  time_t now = time(nullptr);
  if (now - *last < seconds) return false;
  *last = (int)now;
  return true;
}

#endif
