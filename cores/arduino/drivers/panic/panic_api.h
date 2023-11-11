/**
 * user-overridable panic handler api
 */
#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

  /**
   * @brief prepare use of panic_puts()
   */
  void panic_begin(void);

  /**
   * @brief print string to panic output
   * @param str string to print
   */
  void panic_puts(const char *str);

  /**
   * @brief finalize panic, halt or reset MCU
   * @note this function must not return
   */
  __attribute__((noreturn)) void panic_end(void);

#ifdef __cplusplus
}
#endif
