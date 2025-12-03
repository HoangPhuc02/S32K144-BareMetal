#include "adc.h"
#include "gpio.h"
#include "port.h"

#define RED_LED   15U /* RED LED */
#define GREEN_LED 16U /* GREEN LED */
#define BLUE_LED  0U  /* BLUE LED */
#define SWITCH_2  12U /* SW2 */

#define DEFAULT_LIBRARY

volatile uint32_t adcResultInMv_pot = 0;

void WDOG_disable(void);
void delay(void);
void PORT_init(void);
void ADC_clock_init(void);
void ADC_init(void);
uint8_t adc_complete(void);

void PORTC_IRQHandler(void) {
    if (PORT_InterruptCheck(PORT_C, 12) == PORT_STATUS_TRIGGER_TRUE) {
        ADC_StartConversion(ADC_0, 'A', 12);
        PORT_InterruptClear(PORT_C, 12);
    }
}
void ADC0_IRQHandler(void) {
    if (ADC_InterruptCheck(ADC_0, 'A') == ADC_STATUS_CONVERSION_COMPLETED) {
        adcResultInMv_pot = ADC_ReadValue(ADC_0, 'A');
    }
}

int main(void) {
    WDOG_disable();   /* Disable watchdog */
    ADC_clock_init(); /* Init ADC clock */
    PORT_init();      /* Init port clocks and gpio outputs */
    ADC_init();       /* Init ADC resolution 12 bit*/

    S32_NVIC->ISER[PORTC_IRQn / 32] = (1 << (PORTC_IRQn % 32)); /* Enable NVIC for PORT C */
    S32_NVIC->ISER[ADC0_IRQn / 32] = (1 << (ADC0_IRQn % 32));   /* Enable NVIC for ADC 0 */

    for (;;) {
        if (adcResultInMv_pot > 3750) { /* If result > 3.75V : RED */
            GPIO_SetPin(GPIO_D, GREEN_LED);
            GPIO_SetPin(GPIO_D, BLUE_LED);
            GPIO_ClearPin(GPIO_D, RED_LED);
        } else if (adcResultInMv_pot > 2500) { /* If result > 2.5V : YELLOW */
            GPIO_SetPin(GPIO_D, BLUE_LED);
            GPIO_ClearPin(GPIO_D, RED_LED);
            GPIO_ClearPin(GPIO_D, GREEN_LED);
        } else if (adcResultInMv_pot > 1250) { /* If result > 1.25V : GREEN */
            GPIO_SetPin(GPIO_D, RED_LED);
            GPIO_SetPin(GPIO_D, BLUE_LED);
            GPIO_ClearPin(GPIO_D, GREEN_LED);
        } else { /* OFF */
            GPIO_SetPin(GPIO_D, RED_LED);
            GPIO_SetPin(GPIO_D, GREEN_LED);
            GPIO_SetPin(GPIO_D, BLUE_LED);
        }
    }
}

void WDOG_disable(void) {
    WDOG->CNT = 0xD928C520;   /* Unlock watchdog */
    WDOG->TOVAL = 0x0000FFFF; /* Maximum timeout value */
    WDOG->CS = 0x00002100;    /* Disable watchdog */
}
void delay(void) {
    for (volatile int i = 0; i < 2000000; i++)
        ;
}
void PORT_init(void) {
    PCC->PCCn[PCC_PORTD_INDEX] |= PCC_PCCn_CGC_MASK; /* Enable clock for PORT D */
    PCC->PCCn[PCC_PORTC_INDEX] |= PCC_PCCn_CGC_MASK; /* Enable clock for PORT C */

    port_pin_config_t ledConfigs;                        /* Config options for LED */
    ledConfigs.field.MUX = PORT_MUX_GPIO;                /* Assign pin as GPIO */
    ledConfigs.field.IRQC = PORT_INTERRUPT_DISABLE;      /* Disable interrupt */
    ledConfigs.field.PE = PORT_RESISTOR_DISABLE;         /* No pull up/down resistor */
    port_pin_config_t buttonConfigs;                     /* Config options for push button */
    buttonConfigs.field.MUX = PORT_MUX_GPIO;             /* Assign pin as GPIO */
    buttonConfigs.field.IRQC = PORT_INTERRUPT_FALL_EDGE; /* Enable interrupt on falling edge */
    buttonConfigs.field.PE = PORT_RESISTOR_ENABLE;       /* Enable internal pull resistor */
    buttonConfigs.field.PS = PORT_RESISTOR_PULLUP;       /* Select internal pull up resistor */

    PORT_Init(PORT_D, BLUE_LED, &ledConfigs);
    PORT_Init(PORT_D, RED_LED, &ledConfigs);
    PORT_Init(PORT_D, GREEN_LED, &ledConfigs);
    PORT_Init(PORT_C, SWITCH_2, &buttonConfigs);

    GPIO_Init(GPIO_D, BLUE_LED, GPIO_OUTPUT);  /* Assign the blue LED pin as output */
    GPIO_Init(GPIO_D, RED_LED, GPIO_OUTPUT);   /* Assign the red LED pin as output */
    GPIO_Init(GPIO_D, GREEN_LED, GPIO_OUTPUT); /* Assign the green LED pin as output */
    GPIO_Init(GPIO_C, SWITCH_2, GPIO_INPUT);   /* Assign the push button pin as input */
}
void ADC_clock_init(void) {
    SCG->FIRCCSR &= ~SCG_FIRCCSR_FIRCEN_MASK; /* Disable FIRC */
    SCG->FIRCDIV |= SCG_FIRCDIV_FIRCDIV2(1);  /* FIRCDIV2 = 1 */
    SCG->FIRCCSR |= SCG_FIRCCSR_FIRCEN_MASK;  /* Enable FIRC */
}
void ADC_init(void) {
    PCC->PCCn[PCC_ADC0_INDEX] &= ~PCC_PCCn_CGC_MASK; /* Disable clock to change PCS */
    PCC->PCCn[PCC_ADC0_INDEX] |= PCC_PCCn_PCS(3);    /* PCS=1: Select FIRCDIV2 */
    PCC->PCCn[PCC_ADC0_INDEX] |= PCC_PCCn_CGC_MASK;  /* Enable bus clock in ADC */

    ADC_StartConversion(ADC_0, 'A', ADC_MODULE_DISABLE);              /* Disable slot A */
    ADC_InterruptConfig(ADC_0, 'A', ADC_CONVERSION_INTERRUPT_ENABLE); /* Enable interrupt on slot A */

    adc_module_config_1_t adcConfigs;        /* Config options for ADC */
    adcConfigs.field.MODE = ADC_MODE_12_BIT; /* Select ADC resolution */
    adcConfigs.field.ADICLK = ADC_ALTCLK1;   /* Select input clock for ADC */
    adcConfigs.field.ADIV = ADC_CLK_DIV_1;   /* Select divide ratio for the clock used by ADC */
    ADC_Init(ADC_0, &adcConfigs, 5000U);
}
uint8_t adc_complete(void) {
    return ((ADC0->SC1[0] & ADC_SC1_COCO_MASK) >> ADC_SC1_COCO_SHIFT); /* Wait for completion */
}