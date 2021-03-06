/*
 * Uart setup helpers for Allwinner SoCs
 *
 * Copyright (C) 2013  Alexandru Gagniuc <mr.nuke.me@gmail.com>
 * Subject to the GNU GPL v2, or (at your option) any later version.
 */

#include "uart.h"
#include <arch/io.h>
#include <console/uart.h>
#include <drivers/uart/uart8250reg.h>

/**
 * \brief Configure line control settings for UART
 */
static void a20_uart_configure(struct a20_uart *uart, u32 baud_rate, u8 data_bits,
			enum uart_parity parity, u8 stop_bits)
{
	u32 reg32;
	u16 div;

	div = (u16) uart_baudrate_divisor(baud_rate,
		uart_platform_refclk(), 16);
	/* Enable access to Divisor Latch register */
	write32(&uart->lcr, UART8250_LCR_DLAB);
	/* Set baudrate */
	write32(&uart->dlh, (div >> 8) & 0xff);
	write32(&uart->dll, div & 0xff);
	/* Set line control */
	reg32 = (data_bits - 5) & UART8250_LCR_WLS_MSK;
	switch (parity) {
	case UART_PARITY_ODD:
		reg32 |= UART8250_LCR_PEN;
		break;
	case UART_PARITY_EVEN:
		reg32 |= UART8250_LCR_PEN;
		reg32 |= UART8250_LCR_EPS;
		break;
	case UART_PARITY_NONE:	/* Fall through */
	default:
		break;
	}
	write32(&uart->lcr, reg32);
}

static void a20_uart_enable_fifos(struct a20_uart *uart)
{
	write32(&uart->fcr, UART8250_FCR_FIFO_EN);
}

static int tx_fifo_full(struct a20_uart *uart)
{
	/* This may be a misnomer, or a typo in the datasheet. THRE indicates
	 * that the TX register is empty, not that the FIFO is not full, but
	 * this may be due to a datasheet typo. Keep the current name to signal
	 * intent. */
	return !(read32(&uart->lsr) & UART8250_LSR_THRE);
}

static int rx_fifo_empty(struct a20_uart *uart)
{
	return !(read32(&uart->lsr) & UART8250_LSR_DR);
}

/**
 * \brief Read a single byte from the UART.
 *
 * Blocks until at least a byte is available.
 */
static u8 a20_uart_rx_blocking(struct a20_uart *uart)
{
	while (rx_fifo_empty(uart)) ;

	return read32(&uart->rbr);
}

/**
 * \brief Write a single byte to the UART.
 *
 * Blocks until there is space in the FIFO.
 */
static void a20_uart_tx_blocking(struct a20_uart *uart, u8 data)
{
	while (tx_fifo_full(uart)) ;

	return write32(&uart->thr, data);
}


void uart_init(int idx)
{
	struct a20_uart *uart_base = uart_platform_baseptr(idx);

	/* Use default 8N1 encoding */
	a20_uart_configure(uart_base, default_baudrate(),
		8, UART_PARITY_NONE, 1);
	a20_uart_enable_fifos(uart_base);
}

unsigned char uart_rx_byte(int idx)
{
	return a20_uart_rx_blocking(uart_platform_baseptr(idx));
}

void uart_tx_byte(int idx, unsigned char data)
{
	a20_uart_tx_blocking(uart_platform_baseptr(idx), data);
}

void uart_tx_flush(int idx)
{
}
