#ifndef __TRUE_RANDOM_H__
#define __TRUE_RANDOM_H__

// check ddl configuration
#if (DDL_TRNG_ENABLE != DDL_ON)
#error "TrueRandom library requires TRNG DDL to be enabled"
#endif

#if (DDL_PWC_ENABLE != DDL_ON)
#error "TrueRandom library requires PWC DDL to be enabled"
#endif

#include "Arduino.h"
#include <hc32_ddl.h>

#ifndef TRNG_TIMEOUT
#define TRNG_TIMEOUT 10
#endif

class TrueRandom
{
public:
    /**
     * @brief initialize true random number generator
     */
    void begin();

    /**
     * @brief deinitialize true random number generator
     */
    void end(void);

    /**
     * @brief get a 64-bit random number
     */
    uint64_t get();

    /**
     * @brief fill a buffer with random numbers
     * @param buffer pointer to buffer
     * @param size size of buffer
     */
    void fill(uint8_t *buffer, size_t size);
};

/**
 * @brief global TrueRandom instance
 */
extern TrueRandom RNG;
#endif
