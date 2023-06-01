# HardFault Handler

the arduino core includes a `HardFault` handler that prints information about the fault using the panic mechanism.
using the handler, information about the fault and what caused it can be extracted from the mcu.

## Determining the Fault Cause

the fault cause can be determined by analysing the `FSR / FAR` section of the output.
it contains the state of the fault status registers.
See the matching Section in the "System control block" chapter of the [ARM Cortex M4 Generic User Guide](https://developer.arm.com/documentation/dui0553/b/) for more information.
Alternatively, one may reference the same section in the [ARM Cortex M3 Generic User Guide](https://developer.arm.com/documentation/dui0552/a/cortex-m3-peripherals/system-control-block), as it is available as a web page and is virtually identical.

if the fault was caused by a `UsageFault`, `BusFault` or `MemoryManagementFault`, a section listing all set fault flags will be printed.

## Example

this section will show an example of how the output of the `HardFault` handler can be used to determine the cause of a fault.

example fault output:

```
*** HARDFAULT ***
- FSR / FAR:
SCB->HFSR = 0x40000000
SCB->CFSR = 0x02000000
SCB->DFSR = 0x00000000
SCB->AFSR = 0x00000000
- Usage fault:
 * DIVBYZERO
- Stack frame:
R0 = 0x0000000a
R1 = 0x00000000
R2 = 0x0000000a
R3 = 0x00000000
R12 = 0x1fff8b4a
LR = 0x0001a32f
PC = 0x0001a2fe
PSR = 0x01000000
- Misc:
LR = 0xfffffff9
***
```

### Fault Cause

as indicated by the set `DIVBYZERO` flag, the fault was caused by a division by zero somewhere in the code.

### Code Location

to find the instruction that caused the fault, the `PC` register value in the `Stack frame` block can be used.
In the example, the `PC` register value is `0x0001a2fe`.

Additionally, the address of the calling function is also printed in the `Stack frame` block, in the `LR` register.
In the example, the `LR` register value is `0x0001a32f`.

#### Finding the Instruction

to find the corresponding instruction, use the following command:

```
arm-none-eabi-objdump -S <firmware.elf> | grep -A 1 -B 1 1a2fe
```

> Note: replace `<firmware.elf>` with the path to the firmware elf file.

> Note: remove prefixing zeros from the `PC` register value in the command.

> Note: A and B options determine the number of lines before and after the matching line to output.

which yields the following output:

```
$ arm-none-eabi-objdump -S <firmware.elf> | grep -A 1 -B 1 1a2fe
   1a2fc:       683b            ldr     r3, [r7, #0]
   1a2fe:       fb92 f3f3       sdiv    r3, r2, r3
   1a302:       4618            mov     r0, r3
```

from this output, we can see that the fault was caused by the `sdiv` instruction at address `0x1a2fe`, which attemted to divide the value of register `r2` by the value of register `r3`.

From the `Stack frame` block, we can see that the value of register `r2` was `0x0000000a` and the value of register `r3` was `0x00000000`.
Thus, the instruction attempted to divide `0x0000000a` by `0x00000000`, which caused the Divide By Zero fault.

#### Finding the Source Code Line

the value of the `PC` register can also be used to find the corresponding source code line.
to do so, use the following command:

```
arm-none-eabi-addr2line -a 1a2fe -e <firmware.elf>
```

> Note: replace `<firmware.elf>` with the path to the firmware elf file.

> Note: remove prefixing zeros from the `PC` register value in the command.

which yields the following output:

```
$ arm-none-eabi-addr2line -a 1a2fe -e <firmware.elf>
<path/main.cpp:34
```

from this output, we can see that the offending code is located at (or around) line 34 in the file `main.cpp`.

Additionally, the same process can be repeated with the value of the `LR` register, to find the source code line of the calling function.
In this example, the address `0x0001a32f` corresponds to line 50 in the file `main.cpp`.

taking a look at the source code, we find the following code, which is indeed a division with a zero divisor:

```cpp
//...

int divide(int a, int b)
{
  return a / b; // line 34
}

void main()
{
  // ...
  int a = 10;
  int b = 0;
  int c = divide(a, b); // line 50
  printf("c = %d\n", c);
}
```

# Reference

for more information, see the following links:

- [https://www.keil.com/appnotes/files/apnt209.pdf](https://www.keil.com/appnotes/files/apnt209.pdf)
- [https://blog.feabhas.com/2018/09/updated-developing-a-generic-hard-fault-handler-for-arm-cortex-m3-cortex-m4-using-gcc/](https://blog.feabhas.com/2018/09/updated-developing-a-generic-hard-fault-handler-for-arm-cortex-m3-cortex-m4-using-gcc/)
- [https://developer.arm.com/documentation/dui0552/a/cortex-m3-peripherals/system-control-block](https://developer.arm.com/documentation/dui0552/a/cortex-m3-peripherals/system-control-block) and following sections
- [https://developer.arm.com/documentation/dui0553/b/](https://developer.arm.com/documentation/dui0553/b/), "System control block" chapter
