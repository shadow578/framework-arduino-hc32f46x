/**
 * Semihosting driver example.
 * 
 * For any of this do do anything, you must have run this in a debugging session.
 * Once connected, the debugger should break automatically.
 * From there, open the "Debug Console" and run: "monitor arm semihosting enable" to enable semihosting support.
 */

#include <Arduino.h>
#include <drivers/semihosting/semihosting.h>

void setup()
{
  // write to the hosts console
  sh_writec('H'); // ~= putchar('H');
  sh_write0("ello, world!\n"); // ~= puts("ello, world!\n");

  sh_write(SH_STDOUT_FILENO, "Hello, stdout!\n", 15); // ~= write(STDOUT_FILENO, "Hello, stdout!\n", 15);
  sh_write(SH_STDERR_FILENO, "Hello, stderr!\n", 15); // ~= write(STDERR_FILENO, "Hello, stderr!\n", 15);

  // with REDIRECT_PRINTF_TO_DEBUGGER=1, printf also writes to the host
  printf("Hello, printf!\n");

  // open a file on the host and write to it
  const int fd = sh_open("test.txt", SH_OPEN_A); // ~= fopen("test.txt", "a");
  if (fd != -1)
  {
    sh_write(fd, "Hello, file!\n", 13); // ~= write(fd, "Hello, file!\n", 13);
    sh_close(fd); // ~= close(fd);
  }
}

void loop() {}
