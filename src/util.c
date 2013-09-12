#include "util.h"

int _is_number(const char c) {
  return c >= '0' && c <= '9';
}

int _is_add_or_sub_flag(const char c) {
  return c == '-' || c == '+';
}

int _is_space_or_newline(const char c) {
  if (c == ' ' || c == '\t' ||
      c == '\r' || c == '\n') {
    return 1;
  }
  return 0;
}

