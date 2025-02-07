/**
 * Semihosting driver for HC32F460. 
 * Implements a subset of the ARM semihosting calls.
 * based on https://electronics.stackexchange.com/a/149403
 */

#ifndef SEMIHOSTING_H
#define SEMIHOSTING_H

#include <stdint.h>
#include <cstring>


#define SH_SYS_OPEN 0x01
#define SH_SYS_CLOSE 0x02
#define SH_SYS_WRITEC 0x03
#define SH_SYS_WRITE0 0x04
#define SH_SYS_WRITE 0x05

#define SH_STDOUT_FILENO 1
#define SH_STDERR_FILENO 2

#define SH_OPEN_R 0    // "r"
#define SH_OPEN_RB 1   // "rb"
#define SH_OPEN_RP 2   // "r+"
#define SH_OPEN_RPB 3  // "r+b"
#define SH_OPEN_W 4    // "w"
#define SH_OPEN_WB 5   // "wb"
#define SH_OPEN_WP 6   // "w+"
#define SH_OPEN_WPB 7  // "w+b"
#define SH_OPEN_A 8    // "a"
#define SH_OPEN_AB 9   // "ab"
#define SH_OPEN_AP 10  // "a+"
#define SH_OPEN_APB 11 // "a+b"


/**
 * @brief Send a semihosting command to the debugger.
 * @param command The command to send. One of SH_SYS_*.
 * @param param The parameter to pass to the command. depends on the command.
 */
inline int sh_syscall(const int command, const void *param)
{
    int result;
    asm volatile(
        "mov r0, %[cmd];"
        "mov r1, %[param];"
        "bkpt #0xAB;"
        "mov %[result], r0;"
            : [result] "=r" (result)
            : [cmd] "r" (command), 
              [param] "r" (param)
            : "r0", "r1", "memory"
    );
    return result;
}

/**
 * @brief open a file on the host
 * @param filename The name of the file to open.
 * @param mode The mode the file is opened in. corosponds to ISO C fopen modes.
 * @returns a file descriptor on success, or -1 on failure.
 * 
 * @note see https://developer.arm.com/documentation/dui0471/i/semihosting/sys-open--0x01-
 */
inline int sh_open(const char *filename, const int mode)
{
    const uint32_t filename_ptr = reinterpret_cast<uint32_t>(filename);
    const size_t filename_len = strlen(filename);
    const uint32_t param[3] = { filename_ptr, static_cast<uint32_t>(mode), filename_len };

    return sh_syscall(SH_SYS_OPEN, param);
}

/**
 * @brief close a file descriptor on the host
 * @param fd The file descriptor to close.
 * @returns 0 on success, or -1 on failure.
 * 
 * @note see https://developer.arm.com/documentation/dui0471/i/semihosting/sys-close--0x02-
 */
inline int sh_close(const int fd)
{
    const uint32_t param[1] = { static_cast<uint32_t>(fd) };

    return sh_syscall(SH_SYS_CLOSE, param);
}

/**
 * @brief write to a file descriptor on the host
 * @param fd The file descriptor to write to. may be SH_STDOUT_FILENO or SH_STDERR_FILENO.
 * @param buf The buffer to write from.
 * @param len The length of the buffer.
 * @returns 0 on success, or the number of bytes that were not written.
 * 
 * @note see https://developer.arm.com/documentation/dui0471/i/semihosting/sys-write--0x05-
 */
inline size_t sh_write(const int fd, const void *buf, const size_t len)
{
    const uint32_t buf_ptr = reinterpret_cast<uint32_t>(buf);
    const uint32_t param[3] = { static_cast<uint32_t>(fd), buf_ptr, len };

    return sh_syscall(SH_SYS_WRITE, param);
}

/**
 * @brief write a character to the host
 * @param c The character to write.
 * 
 * @note see https://developer.arm.com/documentation/dui0471/i/semihosting/sys-writec--0x03-
 */
inline void sh_writec(char c)
{
    sh_syscall(SH_SYS_WRITEC, &c);
}

/**
 * @brief write a null-terminated string to the host
 * @param str The string to write.
 * 
 * @note see https://developer.arm.com/documentation/dui0471/i/semihosting/sys-write0--0x04-
 */
inline void sh_write0(const char *str)
{
    sh_syscall(SH_SYS_WRITE0, str);
}

#endif // SEMIHOSTING_H
