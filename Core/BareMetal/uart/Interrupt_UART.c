/*
 * Interrupt_UART.c
 *
 *  Created on: Dec 2, 2025
 *      Author: TrinhNQ7
 */


#include "Interrupt_UART.h"

#include "stdint.h"
#include "string.h"
#include "stdio.h"

uint8_t buffer[30];
volatile uint8_t inx_buf = 0;

void LPUART_CommonHandler(LPUART_Type *LPUARTx)
{
		while(((LPUARTx->STAT >> LPUART_STAT_RDRF_SHIFT) & 1) != 1);
		buffer[inx_buf] = (uint16_t) (LPUARTx->DATA );
		inx_buf++;
}

void LPUART0_RxTx_IRQHandler(void)
{
    LPUART_CommonHandler(LPUART0);
}

void LPUART1_RxTx_IRQHandler(void)
{
    LPUART_CommonHandler(LPUART1);
}

void LPUART2_RxTx_IRQHandler(void)
{
    LPUART_CommonHandler(LPUART2);
}
