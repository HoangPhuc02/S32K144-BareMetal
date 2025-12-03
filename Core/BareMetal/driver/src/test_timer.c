/* Copyright 2023 NXP */
/* License: BSD 3-clause
   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:
    1. Redistributions of source code must retain the above copyright
       notice, this list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright
       notice, this list of conditions and the following disclaimer in the
       documentation and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the
       names of its contributors may be used to endorse or promote products
       derived from this software without specific prior written permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
   LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
   CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
   ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
   POSSIBILITY OF SUCH DAMAGE.
*/

/*
 * main implementation: use this 'C' sample to create your own application
 *
 */
#include "reg.h"
#include "scg.h"
#include "pcc.h"
#include "lpit.h"
#define GPIOD_BLUE 0
#define GPIOD_GREEN 16
#define GPIOD_RED 15

void init_rgb_led(void) {
    /* 1. Turn on clock PORTD */
    pcc_config_t data;
    data.index = PCC_PORTD_INDEX;
    data.enable = true;
    data.source = PCC_NONE_PERIPHERAL_CLK_SOURCE;

    PCC_Config(&data);

    /* 2. Configuration MUX LED PIN to GPIO */
    IP_PORTD->PCR[GPIOD_RED]   = (IP_PORTD->PCR[GPIOD_RED]   & ~PORT_PCR_MUX_MASK) | PORT_PCR_MUX(1U);
    IP_PORTD->PCR[GPIOD_BLUE]  = (IP_PORTD->PCR[GPIOD_BLUE]  & ~PORT_PCR_MUX_MASK) | PORT_PCR_MUX(1U);
    IP_PORTD->PCR[GPIOD_GREEN] = (IP_PORTD->PCR[GPIOD_GREEN] & ~PORT_PCR_MUX_MASK) | PORT_PCR_MUX(1U);


    /* 3. Set direction: Output */
    IP_PTD->PDDR |= (1 << GPIOD_RED) | (1 << GPIOD_BLUE) | (1 << GPIOD_GREEN);

    /* 4. Turn off LED (set PDOR = 0) */
    IP_PTD->PSOR = (1 << GPIOD_RED) | (1 << GPIOD_BLUE) | (1 << GPIOD_GREEN);
}

p_lpit_callback_t MyTimerHandler(void)
{
    IP_PTD->PTOR = (1U << GPIOD_RED) | (1U << GPIOD_BLUE);  /* Toggle LED */
}

int main(void) {
	init_rgb_led();

	SCG_SIRCSetDivider(DISABLE, DIV_BY_1); //8MHz

	SCG_FIRCSetDivider(DISABLE, DIV_BY_4); //12MHz

	scg_sosc_config_t data_sosc;
	data_sosc.range = MEDIUM_FREQ;
	data_sosc.source = SCG_SOSC_SOURCE_INTERNAL;
	data_sosc.divider1 = DISABLE;
	data_sosc.divider2 = DIV_BY_2;
	SCG_SOSCInit(&data_sosc); //4Mhz

	scg_spll_config_t data_spll;
	data_spll.multi    = SCG_SPLL_MULT_16;
	data_spll.prediv   = SCG_SPLL_PREDIV_DIV4;
	data_spll.divider1 = DISABLE;
	data_spll.divider2 = DIV_BY_1;
	SCG_SPLLInit(&data_spll); // clock = (source * multi) / (prediv * 2) ; // 16 MHz

	lpit_config_value_t data_channel0;
	data_channel0.channel		= LPIT_CHANNEL3;
//	data_channel0.source		= LPIT_SOSCDIV2_CLK_SOURCE;
	data_channel0.source		= LPIT_SIRCDIV2_CLK_SOURCE;
//	data_channel0.source		= LPIT_FIRCDIV2_CLK_SOURCE;
//	data_channel0.source		= LPIT_SPLLDIV2_CLK_SOURCE;
	data_channel0.value			= 8000000U - 1U;
	data_channel0.func_callback = MyTimerHandler;
	LPIT_ConfigValue(&data_channel0);

    return 0;
}

