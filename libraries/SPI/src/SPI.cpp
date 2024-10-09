#include "SPI.h"
#include <drivers/gpio/gpio.h>

void SPIClass::begin(const gpio_pin_t mosi_pin, const gpio_pin_t miso_pin, const gpio_pin_t clock_pin)
{
    // setup pins
    GPIO_SetFunc(mosi_pin, this->config->mosi_func, Disable);
    GPIO_SetFunc(miso_pin, this->config->miso_func, Disable);
    GPIO_SetFunc(clock_pin, this->config->sck_func, Disable);

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
        //.enReadBufferObject = SpiReadReceiverBuffer,
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
}

void SPIClass::end()
{
    SPI_DeInit(this->config->register_base);
}

void SPIClass::setClockDivider(const uint8_t divider)
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

    // set SPI mode
    SPI_SetDataLength(this->config->register_base, data_len);

    // send data
    SPI_SendData(this->config->register_base, data);
}

uint32_t SPIClass::receive()
{
    // wait until the receive buffer is full, then read
    while (SPI_GetFlag(this->config->register_base, SpiFlagReceiveBufferFull) != Set)
    {
        // wait for receive buffer full
    }
    
    return SPI_ReceiveData(this->config->register_base, data_len);
}
