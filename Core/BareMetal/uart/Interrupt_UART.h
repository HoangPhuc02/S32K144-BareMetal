/*
 * Interrupt_UART.h
 *
 *  Created on: Dec 2, 2025
 *      Author: TrinhNQ7
 */

#ifndef INTERRUPT_UART_H_
#define INTERRUPT_UART_H_

#include "UART_Registers.h"
#include "Port_Registers.h"
#include "stdint.h"

extern uint8_t buffer[30];
extern volatile uint8_t inx_buf;

void LPUART_CommonHandler				(LPUART_Type *uart);

#endif /* INTERRUPT_UART_H_ */
