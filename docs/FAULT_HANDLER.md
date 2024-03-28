# Arduino Core Fault Handler

the arduino core includes a fault handler to handle recoverable errors and cpu faults.

to learn how to debug a fault, see the [debugging a fault](./DEBUGGING_A_FAULT.md) documentation.


## Custom Fault Handler

to use a custom fault handler, first define the `CORE_DISABLE_FAULT_HANDLER` option in the `platformio.ini` file.
then, implement the interrupt handlers for the faults you want to handle.

available interrupt handlers are:
- `HardFault_Handler(void)`
- `BusFault_Handler(void)`
- `UsageFault_Handler(void)`
- `MemManage_Handler(void)`
- `NMI_Handler(void)`
