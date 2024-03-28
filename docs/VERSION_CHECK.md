# Checking the Arduino Core Version

the current version of the arduino core can be accessed using the `ARDUINO_CORE_MAJOR`, `ARDUINO_CORE_MINOR`, and `ARDUINO_CORE_PATCH` macros.
Additionally, the helper macros `GET_VERSION_INT(major, minor, patch)` and `ARDUINO_CORE_VERSION_INT` are available in `core_util.h`.

example:

```cpp
#include <core_util.h>

#if ARDUINO_CORE_VERSION_INT < GET_VERSION_INT(1, 0, 0)
  #error "expected Arduino core >= 1.0.0"
#endif
```
