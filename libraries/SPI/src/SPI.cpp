#include "SPI.h"
#include <core_debug.h>
#include <drivers/gpio/gpio.h>
#include <drivers/sysclock/sysclock.h>

#warning "SPI on the HC32F460 has not been tested yet. See https://github.com/shadow578/framework-arduino-hc32f46x/pull/29" 

/**
 * @brief given a integer v, round up to the next power of two
 * @note based on https://stackoverflow.com/a/466242
 */
inline uint32_t round_next_power_of_two(uint32_t v)
{
    v--;
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    v++;

    return v;
}

SPIClass SPI1(&SPI1_config);

void SPIClass::begin()
{
    // setup pins
    GPIO_SetFunc(this->mosi_pin, this->config->mosi_func, Disable);
    GPIO_SetFunc(this->miso_pin, this->config->miso_func, Disable);
    GPIO_SetFunc(this->clock_pin, this->config->sck_func, Disable);

    // enable peripheral clock
    PWC_Fcg1PeriphClockCmd(this->config->clock_id, Enable);

    // prepare SPI peripheral configuration
    stc_spi_init_t spiConfig = {
        .stcDelayConfig = {
            .enSsSetupDelayOption = SpiSsSetupDelayCustomValue,
            .enSsSetupDelayTime = SpiSsSetupDelaySck1,
            .enSsHoldDelayOption = SpiSsHoldDelayCustomValue,
            .enSsHoldDelayTime = SpiSsHoldDelaySck1,
            .enSsIntervalTimeOption = SpiSsIntervalCustomValue,
            .enSsIntervalTime = SpiSsIntervalSck6PlusPck2,
        },
        .enReadBufferObject = SpiReadReceiverBuffer, // reading DR reads the receive register
        //.enSckPolarity = SpiSckIdleLevelLow,
        //.enSckPhase = SpiSckOddSampleEvenChange,
        .enClkDiv = SpiClkDiv256,
        .enDataLength = SpiDataLengthBit8,
        .enFirstBitPosition = SpiFirstBitPositionMSB,
        //.enFrameNumber = SpiFrameNumber1,
        .enWorkMode = SpiWorkMode3Line,
        .enTransMode = SpiTransFullDuplex,
        .enMasterSlaveMode = SpiModeMaster,
        //.enCommAutoSuspendEn = Disable,
        //.enModeFaultErrorDetectEn = Disable,
        //.enParitySelfDetectEn = Disable,  
        //.enParityEn = Disable,
        //.enParity = SpiParityEven,
    };

    SPI_Init(this->config->register_base, &spiConfig);
    SPI_Cmd(this->config->register_base, Enable);

    // beginTransaction acts as a wrapper for setClockFrequency and setBitOrder
    // so calling it here just acts as setting the default config
    this->beginTransaction(SPISettings());
}

void SPIClass::end()
{
    SPI_DeInit(this->config->register_base);
}

void SPIClass::setClockFrequency(const uint32_t frequency)
{
    // get PCLK1 clock, which is the base clock of the SPI peripherals
    update_system_clock_frequencies();
    const uint32_t pclk1 = SYSTEM_CLOCK_FREQUENCIES.pclk1;

    // calculate nearest divider to match requested clock frequency
    // SPICLK = PCLK1 / DIV
    // => DIV = PCLK1 / SPICLK
    // (DIV must be in [2, 4, 8, 16, 32, 64, 128, 256])
    uint32_t div = pclk1 / frequency;
    div = round_next_power_of_two(div);

    // ensure bounds
    if (div < 2) 
    {
        CORE_DEBUG_PRINTF("SPI div constrained to 2\n");
        div = 2;
    }

    if (div > 256)
    {
        CORE_DEBUG_PRINTF("SPI div constrained to 256\n");
        div = 256;
    }

    CORE_DEBUG_PRINTF("setting spi div to %d (f_req=%d, f_eff=%d)\n", div, frequency, (pclk1 / div));
    this->setClockDivider(div);
}

void SPIClass::setClockDivider(const uint16_t divider)
{
    en_spi_clk_div_t ddl_divider;
    switch(divider)
    {
        case 2:
            ddl_divider = SpiClkDiv2;
            break;
        case 4:
            ddl_divider = SpiClkDiv4;
            break;
        case 8:
            ddl_divider = SpiClkDiv8;
            break;
        case 16:
            ddl_divider = SpiClkDiv16;
            break;
        case 32:
            ddl_divider = SpiClkDiv32;
            break;
        case 64:
            ddl_divider = SpiClkDiv64;
            break;
        case 128:
            ddl_divider = SpiClkDiv128;
            break;  
        case 256:
            ddl_divider = SpiClkDiv256;
            break;
        default:
            CORE_ASSERT_FAIL("Invalid SPI clock divider");
            return;
    }

    SPI_SetClockDiv(this->config->register_base, ddl_divider);
}

void SPIClass::setBitOrder(const BitOrder order)
{
    SPI_SetFirstBitPosition(this->config->register_base, 
                            order == MSBFIRST ? SpiFirstBitPositionMSB : SpiFirstBitPositionLSB);
}

void SPIClass::send(const en_spi_data_length_t data_len, const uint32_t data)
{
    // wait until the send buffer is empty before doing anything
    while (SPI_GetFlag(this->config->register_base, SpiFlagSendBufferEmpty) != Set);

    // set SPI data width
    SPI_SetDataLength(this->config->register_base, data_len);

    // we're fine using SPI_SendData32 here even when data_len != 32 bit, since 
    // all SPI_SendData* functions just write the data to the transmit register.
    SPI_SendData32(this->config->register_base, data);
}

uint32_t SPIClass::receive()
{
    // wait until the receive buffer is full, then read
    while (SPI_GetFlag(this->config->register_base, SpiFlagReceiveBufferFull) != Set)
    {
        // wait for receive buffer full
    }
    
    // we're fine using SPI_ReceiveData32 even with a data_len != 32 bit, since
    // all SPI_ReceiveData* functions just directly read the data register, and
    // casting the value as needed. 
    // so no need to use the specific functions, we can do the casts ourselves.
    return SPI_ReceiveData32(this->config->register_base);
}


void SPIClass::beginTransaction(SPISettings settings)
{
    // TODO: handle dataMode
    this->setClockFrequency(settings.clockFreq);
    this->setBitOrder(settings.bitOrder);
}

void SPIClass::endTransaction(void)
{

}
