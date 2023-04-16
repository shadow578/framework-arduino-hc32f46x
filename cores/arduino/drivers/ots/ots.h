#pragma once
#include "ddl_config.h"
#if (DDL_OTS_ENABLE == DDL_ON)

#ifndef OTS_USE_XTAL
#define OTS_USE_XTAL 1
#endif

#ifndef OTS_READ_TIMEOUT
#define OTS_READ_TIMEOUT 10
#endif

//
// OTS calibration values
// values here are taken from the documentation, and
// should (more or less) be good enough
//

// slope factor K
#ifndef OTS_K
#if OTS_USE_XTAL
#define OTS_K 737272.73f
#else
#define OTS_K 3002.59f
#endif
#endif

// offset M
#ifndef OTS_M
#if OTS_USE_XTAL
#define OTS_M 27.55f
#else
#define OTS_M 27.92f
#endif
#endif

class H32OTS
{
public:
    static void init();
    static bool read(float &temperature);
};
#endif
