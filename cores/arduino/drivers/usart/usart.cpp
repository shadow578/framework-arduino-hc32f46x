#include "usart.h"

//
// USART1 callbacks
//
void Usart1RxIrqCallback(void)
{
	usart_rx_irq(USART1);
}

void Usart1ErrIrqCallback(void)
{
	if (USART_GetStatus(M4_USART1, UsartFrameErr) == Set)
	{
		USART_ClearStatus(M4_USART1, UsartFrameErr);
	}

	if (USART_GetStatus(M4_USART1, UsartParityErr) == Set)
	{
		USART_ClearStatus(M4_USART1, UsartParityErr);
	}

	if (USART_GetStatus(M4_USART1, UsartOverrunErr) == Set)
	{
		USART_ClearStatus(M4_USART1, UsartOverrunErr);
	}
}

void Usart1TxIrqCallback(void)
{
	usart_tx_irq(USART1);
}

void Usart1TxCmpltIrqCallback(void)
{
	USART_FuncCmd(M4_USART1, UsartTxCmpltInt, Disable);
	USART_FuncCmd(M4_USART1, UsartTx, Disable);
}

//
// USART2 callbacks
//
void Usart2RxIrqCallback(void)
{
	usart_rx_irq(USART2);
}

void Usart2ErrIrqCallback(void)
{
	if (USART_GetStatus(M4_USART2, UsartFrameErr) == Set)
	{
		USART_ClearStatus(M4_USART2, UsartFrameErr);
	}

	if (USART_GetStatus(M4_USART2, UsartParityErr) == Set)
	{
		USART_ClearStatus(M4_USART2, UsartParityErr);
	}

	if (USART_GetStatus(M4_USART2, UsartOverrunErr) == Set)
	{
		USART_ClearStatus(M4_USART2, UsartOverrunErr);
	}
}

void Usart2TxIrqCallback(void)
{
	usart_tx_irq(USART2);
}

void Usart2TxCmpltIrqCallback(void)
{
	USART_FuncCmd(M4_USART2, UsartTxCmpltInt, Disable);
	USART_FuncCmd(M4_USART2, UsartTx, Disable);
}

//
// USART3 callbacks
//
void Usart3RxIrqCallback(void)
{
	usart_rx_irq(USART3);
}

void Usart3ErrIrqCallback(void)
{
	if (USART_GetStatus(M4_USART3, UsartFrameErr) == Set)
	{
		USART_ClearStatus(M4_USART3, UsartFrameErr);
	}

	if (USART_GetStatus(M4_USART3, UsartParityErr) == Set)
	{
		USART_ClearStatus(M4_USART3, UsartParityErr);
	}

	if (USART_GetStatus(M4_USART3, UsartOverrunErr) == Set)
	{
		USART_ClearStatus(M4_USART3, UsartOverrunErr);
	}
}

void Usart3TxIrqCallback(void)
{
	usart_tx_irq(USART3);
}

void Usart3TxCmpltIrqCallback(void)
{
	USART_FuncCmd(M4_USART3, UsartTxCmpltInt, Disable);
	USART_FuncCmd(M4_USART3, UsartTx, Disable);
}

//
// public api
//
void usart_init(usart_dev *dev)
{
	// clear ring buffers
	dev->rb->clear();
	dev->wb->clear();

	// enable clock
	PWC_Fcg1PeriphClockCmd(dev->clk_id, Enable);

	// initialize uart
	USART_UART_Init(dev->regs, dev->pstcInitCfg);
}

void usart_set_baud_rate(usart_dev *dev, uint32_t baud)
{
	USART_SetBaudrate(dev->regs, baud);
}

void usart_set_stop_bits(usart_dev *dev, en_usart_stop_bit_t stop_bits)
{
	USART_SetStopBitsLength(dev->regs, stop_bits);
}

void usart_set_parity(usart_dev *dev, en_usart_parity_t parity)
{
	USART_SetParity(dev->regs, parity);
}

void usart_set_data_width(usart_dev *dev, en_usart_data_len_t data_width)
{
	USART_SetDataLength(dev->regs, data_width);
}

void usart_enable(usart_dev *dev)
{
	// set rx IRQ
	stc_irq_regi_conf_t irqConf;
	irqConf.enIRQn = dev->RX_IRQ;
	if (dev->regs == M4_USART1)
	{
		irqConf.enIntSrc = INT_USART1_RI;
		irqConf.pfnCallback = &Usart1RxIrqCallback;
	}
	else if (dev->regs == M4_USART2)
	{
		irqConf.enIntSrc = INT_USART2_RI;
		irqConf.pfnCallback = &Usart2RxIrqCallback;
	}
	else if (dev->regs == M4_USART3)
	{
		irqConf.enIntSrc = INT_USART3_RI;
		irqConf.pfnCallback = &Usart3RxIrqCallback;
	}

	enIrqRegistration(&irqConf);
	NVIC_SetPriority(irqConf.enIRQn, dev->IRQ_priority);
	NVIC_ClearPendingIRQ(irqConf.enIRQn);
	NVIC_EnableIRQ(irqConf.enIRQn);

	// set rx error IRQ
	irqConf.enIRQn = dev->RX_error_IRQ;
	if (dev->regs == M4_USART1)
	{
		irqConf.enIntSrc = INT_USART1_EI;
		irqConf.pfnCallback = &Usart1ErrIrqCallback;
	}
	else if (dev->regs == M4_USART2)
	{
		irqConf.enIntSrc = INT_USART2_EI;
		irqConf.pfnCallback = &Usart2ErrIrqCallback;
	}
	else if (dev->regs == M4_USART3)
	{
		irqConf.enIntSrc = INT_USART3_EI;
		irqConf.pfnCallback = &Usart3ErrIrqCallback;
	}
	enIrqRegistration(&irqConf);
	NVIC_SetPriority(irqConf.enIRQn, dev->IRQ_priority);
	NVIC_ClearPendingIRQ(irqConf.enIRQn);
	NVIC_EnableIRQ(irqConf.enIRQn);

	// set tx IRQ
	irqConf.enIRQn = dev->TX_IRQ;
	if (dev->regs == M4_USART1)
	{
		irqConf.enIntSrc = INT_USART1_TI;
		irqConf.pfnCallback = &Usart1TxIrqCallback;
	}
	else if (dev->regs == M4_USART2)
	{
		irqConf.enIntSrc = INT_USART2_TI;
		irqConf.pfnCallback = &Usart2TxIrqCallback;
	}
	else if (dev->regs == M4_USART3)
	{
		irqConf.enIntSrc = INT_USART3_TI;
		irqConf.pfnCallback = &Usart3TxIrqCallback;
	}
	enIrqRegistration(&irqConf);
	NVIC_SetPriority(irqConf.enIRQn, dev->IRQ_priority);
	NVIC_ClearPendingIRQ(irqConf.enIRQn);
	NVIC_EnableIRQ(irqConf.enIRQn);

	// set tx complete IRQ
	irqConf.enIRQn = dev->TX_complete_IRQ;
	if (dev->regs == M4_USART1)
	{
		irqConf.enIntSrc = INT_USART1_TCI;
		irqConf.pfnCallback = &Usart1TxCmpltIrqCallback;
	}
	else if (dev->regs == M4_USART2)
	{
		irqConf.enIntSrc = INT_USART2_TCI;
		irqConf.pfnCallback = &Usart2TxCmpltIrqCallback;
	}
	else if (dev->regs == M4_USART3)
	{
		irqConf.enIntSrc = INT_USART3_TCI;
		irqConf.pfnCallback = &Usart3TxCmpltIrqCallback;
	}
	enIrqRegistration(&irqConf);
	NVIC_SetPriority(irqConf.enIRQn, dev->IRQ_priority);
	NVIC_ClearPendingIRQ(irqConf.enIRQn);
	NVIC_EnableIRQ(irqConf.enIRQn);

	// enable IRQs
	USART_FuncCmd(dev->regs, UsartRx, Enable);
	USART_FuncCmd(dev->regs, UsartRxInt, Enable);
}

void usart_disable(usart_dev *dev)
{
	// wait until ty buffer is empty
	while (!dev->wb->isEmpty())
		;

	// deinitialize
	USART_DeInit(dev->regs);

	// clear buffers
	usart_reset_rx(dev);
	usart_reset_tx(dev);
}

uint32_t usart_tx(usart_dev *dev, const uint8_t *buf, uint32_t len)
{
	uint32_t sentBytes = 0;
	uint32_t errors = 0;
	while (!dev->wb->isEmpty())
	{
		if (++errors > 500)
		{
			errors = 0;
			return sentBytes;
		}
	}
	while (sentBytes < len)
	{
		if (++errors > 500)
		{
			errors = 0;
			break;
		}

		if (dev->wb->push(buf[sentBytes]))
		{
			sentBytes++;
		}
		else
		{
			break;
		}
	}

	if (!dev->wb->isEmpty())
	{
		USART_FuncCmd(dev->regs, UsartTxAndTxEmptyInt, Enable);
	}

	return sentBytes;
}

uint32_t usart_rx(usart_dev *dev, uint8_t *buf, uint32_t len)
{
	uint32_t receivedBytes = 0;
	uint32_t errors = 0;
	while (usart_data_available(dev) && receivedBytes < len)
	{
		*buf++ = usart_getc(dev);
		receivedBytes++;
		if (++errors > 500)
		{
			errors = 0;
			break;
		}
	}

	return receivedBytes;
}

void usart_putudec(usart_dev *dev, uint32_t val)
{
	char digits[12];
	int i = 0;

	do
	{
		digits[i++] = val % 10 + '0';
		val /= 10;
	} while (val > 0);

	while (--i >= 0)
	{
		usart_putc(dev, digits[i]);
	}
}
