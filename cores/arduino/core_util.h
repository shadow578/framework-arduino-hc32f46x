#pragma once

#ifndef STRINGIFY
#define STRINGIFY_DETAIL(x) #x
#define STRINGIFY(x) STRINGIFY_DETAIL(x)
#endif

#define GET_VERSION_INT(major, minor, patch) ((major * 100000) + (minor * 1000) + patch)
#define ARDUINO_CORE_VERSION_INT GET_VERSION_INT(ARDUINO_CORE_MAJOR, ARDUINO_CORE_MINOR, ARDUINO_CORE_PATCH)
