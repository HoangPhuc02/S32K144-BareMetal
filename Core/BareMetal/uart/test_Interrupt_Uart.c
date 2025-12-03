/*
 * test_Interrupt_Uart.c
 *
 *  Created on: Dec 1, 2025
 *      Author: quitrinh
 */
#ifndef MAIN_DEFINED
#define MAIN_DEFINED
//#ifdef TEST3_MAIN
#include "Gpio.h"
#include "Port.h"
#include "UART.h"
#include "Clock.h"
#include "Nvic.h"
#include "Pcc_Registers.h"
#include "Interrupt_UART.h"
#include <string.h>
#define LED_PORT	PORTD
#define LED_BLUE 	0
#define LED_RED 	15
#define LED_GREEN 	16
#define LED_GPIO	GPIOD

void delay_ms(uint32_t ms){
	for (volatile uint32_t i = 0; i < ms * 4000; i++);
}

int main(void) {
	SOSC_init_8MHz();
	SPLL_init_160MHz();
	NormalRUNmode_80MHz();
	Port_ConfigType portLedbCfg = {
			.portBase			= LED_PORT,
			.pinNumber			= LED_BLUE,
			.muxValue 			= 1,
			.pullEnable			= 0,
			.pullSelect			= 0,
			.interruptConfig	= 0
	};

	PORT_Init(&portLedbCfg);

	Gpio_ConfigType gpioLedbCfg = {
			.pGpiox				= LED_GPIO,
			.pinNumber			= LED_BLUE,
			.Direction			= 1,
			.Value				= 0
	};
	GPIO_Init(&gpioLedbCfg);

	Port_ConfigType portLedrCfg = {
			.portBase			= LED_PORT,
			.pinNumber			= LED_RED,
			.muxValue 			= 1,
			.pullEnable			= 0,
			.pullSelect			= 0,
			.interruptConfig	= 0
	};

	PORT_Init(&portLedrCfg);

	Gpio_ConfigType gpioLedrCfg = {
			.pGpiox				= LED_GPIO,
			.pinNumber			= LED_RED,
			.Direction			= 1,
			.Value				= 0
	};
	GPIO_Init(&gpioLedrCfg);

	Port_ConfigType portLedxCfg = {
			.portBase			= LED_PORT,
			.pinNumber			= LED_GREEN,
			.muxValue 			= 1,
			.pullEnable			= 0,
			.pullSelect			= 0,
			.interruptConfig	= 0
	};

	PORT_Init(&portLedxCfg);

	Gpio_ConfigType gpioLedxCfg = {
			.pGpiox				= LED_GPIO,
			.pinNumber			= LED_GREEN,
			.Direction			= 1,
			.Value				= 0
	};
	GPIO_Init(&gpioLedxCfg);

    /* Configure system clocks */
    SOSC_init_8MHz();
    SPLL_init_160MHz();
    NormalRUNmode_80MHz();

    /* Initialize UART1 on PTC6 (TX), PTC7 (RX) */
    LPUART_init(
        LPUART1,            /* UART instance */
        PORTC,              /* PORTC base	*/
        PCC_PORTC_INDEX,    /* PCC index for PORTC */
        6, 7,               /* TX pin = 6, RX pin = 7 */
        2,                  /* mux ALT2	*/
        2,                  /* PCS = SIRCDIV2_CLK */
        15,                 /* OSR */
        52,                 /* SBR = 52 for 9600 baud at 8MHz */
        UART_PARITY_EVEN    /* Parity */
    );
    NVIC_SetPriority(LPUART1_RxTx_IRQn, 2);
    NVIC_EnableInterrupt(LPUART1_RxTx_IRQn);
    GPIO_WriteToOutputPin(LED_GPIO, LED_RED, 1);
    GPIO_WriteToOutputPin(LED_GPIO, LED_GREEN, 1);

    while (1)
    {
		if(strcmp(buffer, "AB") == 0) {
			GPIO_WriteToOutputPin(LED_GPIO, LED_GREEN, 0);
			GPIO_WriteToOutputPin(LED_GPIO, LED_BLUE, 1);
			delay_ms(1000);
			GPIO_WriteToOutputPin(LED_GPIO, LED_GREEN, 1);
			inx_buf = 0;
			memset(buffer, 0, strlen(buffer));
		}
		else if (strcmp(buffer, "CD") == 0) {
			GPIO_WriteToOutputPin(LED_GPIO, LED_RED, 0);
			GPIO_WriteToOutputPin(LED_GPIO, LED_BLUE, 1);
			delay_ms(1000);
			GPIO_WriteToOutputPin(LED_GPIO, LED_RED, 1);
			inx_buf = 0;
			memset(buffer, 0, strlen(buffer));
		}

        GPIO_ToggleOutputPin(LED_GPIO, LED_BLUE);
        delay_ms(500);
    }
}
#endif
