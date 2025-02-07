#if REDIRECT_PRINTF_TO_DEBUGGER

#ifndef __GNUC__
  #error "only GCC is supported"
#endif

#include "semihosting.h"

/**
 * @brief implementation of _write that redirects everything to the debugger console via semihosting
 * @param file file descriptor. don't care
 * @param ptr pointer to the data to write
 * @param len length of the data to write
 * @return number of bytes written
 */
extern "C" int _write(int file, char *ptr, int len) {
  return sh_write(SH_STDOUT_FILENO, ptr, len);
}

/**
 * @brief implementation of _isatty that always returns 1
 * @param file file descriptor. don't care
 * @return everything is a tty. there are no files to be had
 */
extern "C" int _isatty(int file) {
  return 1;
}

#endif // REDIRECT_PRINTF_TO_DEBUGGER
