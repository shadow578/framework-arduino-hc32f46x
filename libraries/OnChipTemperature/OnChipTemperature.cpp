#include "OnChipTemperature.h"

#include <core_debug.h>

#define OTS_READ_TIMEOUT_MS 25 // 25 ms

// guessed 25 clock cycles per read
#define OTS_READ_TIMEOUT (OTS_READ_TIMEOUT_MS * (SYSTEM_CLOCK_FREQUENCIES.hclk / 25 / 1000))

//
// global OTS object
//
OnChipTemperature ChipTemperature;

//
// OnChipTemperature class implementation
//
void OnChipTemperature::begin()
{
    // create default OTS config:
    // XTAL clock, no auto-off, K and M taken from examples
    stc_ots_init_t config = {
        .enAutoOff = OtsAutoOff_Disable,
        .enClkSel = OtsClkSel_Xtal,
        .f32SlopeK = 737272.73f,
        .f32OffsetM = 27.55f,
    };
    begin(&config);
}

void OnChipTemperature::begin(const stc_ots_init_t *config)
{
    CORE_ASSERT(config != NULL, "OnChipTemperature::begin: config is NULL")

    // enable OTS clock
    PWC_Fcg3PeriphClockCmd(PWC_FCG3_PERIPH_OTS, Enable);

    // initialize OTS peripheral
    OTS_Init(config);

    // setup OTS clock:
    switch (config->enClkSel)
    {
    case OtsClkSel_Xtal:
        // enable XTAL
        CLK_XtalCmd(Enable);
        break;
    case OtsClkSel_Hrc:
        // enable HRC
        CLK_HrcCmd(Enable);
        CLK_Xtal32Cmd(Enable);
        break;
    default:
        CORE_ASSERT_FAIL("invalid clock source passed to OnChipTemperature::begin()");
        return;
    }

    // LRC is always needed
    CLK_LrcCmd(Enable);
    CORE_DEBUG_PRINTF("OTS initialized\n");
}

bool OnChipTemperature::read(float &temperature)
{
    // should we read?
    uint32_t now;
    if (!shouldRead(now))
    {
        return false;
    }

    // yes, read temperature and update last read value
    if (!read_internal(temperature))
    {
        // read failed
        return false;
    }

    // read ok, set last temperature and time
    this->lastReadMillis = now;
    return true;
}

//
// private:
//
bool OnChipTemperature::read_internal(float &temperature)
{
    en_result_t err = OTS_Polling(&temperature, OTS_READ_TIMEOUT);
    if (err != Ok)
    {
        CORE_DEBUG_PRINTF("OnChipTemperature::read failed! err=%d\n", int(err));
        return false;
    }

    return true;
}

bool OnChipTemperature::shouldRead(uint32_t &now)
{
    // skip checks if minimum time is zero
    if (this->minimumReadDeltaMillis == 0)
    {
        return true;
    }

    // check minimum time has passed
    now = millis();
    if (now > (this->lastReadMillis + this->minimumReadDeltaMillis))
    {
        return true;
    }

    return false;
}
