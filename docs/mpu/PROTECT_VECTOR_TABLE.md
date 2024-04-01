# `PROTECT_VECTOR_TABLE` Option

when setting the `PROTECT_VECTOR_TABLE` option to `1`, the ARM core's MPU will be configured to prevent write access to the vector table. this can be useful to prevent accidental writes to the vector table, which could cause the system to behave unpredictably or crash.

if an attempt is made to write to the vector table, the MPU will trigger a fault, which will be handled by the core's fault handler.
the fault handler will print the location of the attempted write, making it easier to identify the cause of the fault.

> [!NOTE]
> enabling this option increases the flash usage by about 320 bytes (1056 bytes with `CORE_DEBUG`) and the RAM usage by about 384 bytes.

> [!CAUTION]
> this option is **enabled** by default.
> consider **not** disabling this option unless you're really short on memory and are absolutely sure that it's ok to disable this protection.
