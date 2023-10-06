#include "TrueRandom.h"
#include "core_debug.h"

//
// TrueRandom global instance
//
TrueRandom RNG;

//
// TrueRandom implementation
//
void TrueRandom::begin()
{
    // enable TRNG clock
    PWC_Fcg0PeriphClockCmd(PWC_FCG0_PERIPH_TRNG, Enable);

    // initialize TRNG
    stc_trng_init_t config = {
        .enLoadCtrl = TrngLoadNewInitValue_Enable,
        .enShiftCount = TrngShiftCount_64,
    };
    TRNG_Init(&config);
}

void TrueRandom::end()
{
    TRNG_DeInit();
}

uint64_t TrueRandom::get()
{
    // TRNG generates 2 32-bit random numbers at a time
    uint32_t randoms[2];

    // get both random numbers
    TRNG_Generate(randoms, 2, TRNG_TIMEOUT);

    // build 64-bit random number
    uint64_t random = ((uint64_t)randoms[0] << 32) | randoms[1];
    return random;
}

void TrueRandom::fill(uint8_t *buffer, size_t size)
{
    CORE_ASSERT(buffer != NULL, "TrueRandom::fill: buffer is NULL")

    // fill buffer with random numbers
    while (size > 0)
    {
        // get random number
        uint64_t random = get();

        // copy random number to buffer
        size_t count = min(size, sizeof(random));
        memcpy(buffer, &random, count);
        buffer += count;
        size -= count;
    }
}
