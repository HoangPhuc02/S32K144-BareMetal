#include "can_T.h"

#define NODE_A

#define MSG_BUFF_SIZE	 4
#define CAN_20_TIMEOUT 180u
#define CAN_FD_TIMEOUT 730u
#define MB_MAX				 128u

/* 
and configure Rx MB at MB4
bit_rate_conf = 0x00DB0006 
* bit rate = 500kbps
* sample point at: 87.5 %
*/
void CAN_init(CAN_Type *hcan, uint32_t bit_rate_conf)
{
	uint8_t iterator;
	/*Enable clock access*/
	if(hcan == CAN0)
	{
		PCC->PCCn[PCC_FlexCAN0_INDEX] |= PCC_PCCn_CGC_Msk;
	}
	else if(hcan == CAN1)
	{
		PCC->PCCn[PCC_FlexCAN1_INDEX] |= PCC_PCCn_CGC_Msk;
	}
	else if(hcan == CAN2)
	{
		PCC->PCCn[PCC_FlexCAN2_INDEX] |= PCC_PCCn_CGC_Msk;
	}
	/*Disable the FlexCAN0 module before selecting the 
	* clock source MCR[MDIS]*/
	hcan->MCR |= CAN_MCR_MDIS_Enable;
	/*-	Set the clock source for FlexCAN0 to the oscillator
	 *	CTRL1[CLKSRC] = 0*/
	hcan->CTRL1 &= CAN_CTRL1_CLKSRC_Oscillator;
	/*	Enable FlexCAN0 module MCR[MDIS] = 0*/
	hcan->MCR &= CAN_MCR_MDIS_Disable;
	/* -	Wait for FRZACK bit to be set 
	 *(Freeze Mode entry/exit) enter */
	while(!((hcan->MCR & CAN_MCR_FRZACK_Msk) >> CAN_MCR_FRZACK_Pos));
	/* Set the CTRL1 register with specific configuration 
	 * in CTRL1: bit timing, sample rate, ...*/
	hcan->CTRL1 = bit_rate_conf;
	/* Clear MB  */
	for (iterator = 0; iterator < MB_MAX; iterator++)
	{
		hcan->RAMn[iterator] = 0;
	}
	/* Set acceptance ID mask for each MB*/
	for(iterator = 0; iterator < 16; iterator++ ) 
	{ 
		hcan->RXIMR[iterator] = 0xFFFFFFFF; 
	}
	/* Set acceptance ID mask for all MB */
	hcan->RXMGMASK = 0x1FFFFFFF;
	/* Configure Rx MB at MB4 */
	hcan->RAMn[ 4*MSG_BUFF_SIZE + 0] = 0x04000000;
#ifdef NODE_A /* Node A receives msg with std ID 0x511 */
	CAN0->RAMn[ 4*MSG_BUFF_SIZE + 1] = 0x14440000;  
#else /* Node B to receive msg with std ID 0x555 */
CAN0->RAMn[ 4*MSG_BUF_SIZE + 1] = 0x15540000; 
#endif

/*	Negate FlexCAN 1 halt state for 32 MBs */
	hcan->MCR = 0x0000001F;

while ((hcan->MCR & CAN_MCR_FRZACK_Msk) >> CAN_MCR_FRZACK_Pos) {}
/* Good practice: wait for FRZACK to clear (not in freeze mode) */
while ((hcan->MCR & CAN_MCR_NOTRDY_Msk) >> CAN_MCR_NOTRDY_Pos) {}
/* Good practice: wait for NOTRDY to clear (module ready) */
}

void CAN_transmit_msg(CAN_Type *hcan)
{
	hcan->IFLAG1 = 0x00000001; /* Clear CAN 0 MB 0 */
	hcan->RAMn[ 0*MSG_BUFF_SIZE + 2] = 0xA5112233;
	hcan->RAMn[ 0*MSG_BUFF_SIZE + 3] = 0xB6778899; 
#ifdef NODE_A
CAN0->RAMn[ 0*MSG_BUFF_SIZE + 1] = 0x15540000; /* MB0 word 1: Tx msg with STD ID 0x555 */
#else
CAN0->RAMn[ 0*MSG_BUFF_SIZE + 1] = 0x14440000; /* MB0 word 1: Tx msg with STD ID 0x511 */
#endif
CAN0->RAMn[ 0*MSG_BUFF_SIZE + 0] = 0x0C400000 | 8 << CAN_WMBn_CS_DLC_Pos; 
}

void CAN_send_msg(CAN_Type *hcan, uint32_t msg[2], uint8_t DLC)
{
    if (DLC > 8) { // DLC should be between 0 and 8
        DLC = 8; // Limit DLC to 8 (maximum for standard CAN)
    }

    hcan->IFLAG1 = 0x00000001; /* Clear CAN 0 MB 0 */
    
    hcan->RAMn[0*MSG_BUFF_SIZE + 2] = msg[0];  // Assign msg[0] to word 2 of message buffer 0
    hcan->RAMn[0*MSG_BUFF_SIZE + 3] = msg[1];  // Assign msg[1] to word 3 of message buffer 0

    #ifdef NODE_A
    CAN0->RAMn[0*MSG_BUFF_SIZE + 1] = 0x15540000;  /* MB0 word 1: Tx msg with STD ID 0x555 */
    #else
    CAN0->RAMn[0*MSG_BUFF_SIZE + 1] = 0x14440000;  /* MB0 word 1: Tx msg with STD ID 0x511 */
    #endif

    // Set the message buffer code and DLC
    CAN0->RAMn[0*MSG_BUFF_SIZE + 0] = 0x0C400000 | (DLC << CAN_WMBn_CS_DLC_Pos);
}


void CAN_receive_msg(uint32_t Rx_data[2]) { /* Receive msg from ID 0x556 using msg buffer 4 */
    uint8_t j;
    
    // Read two words of data (8 bytes)
    for (j = 0; j < 2; j++) { 
        Rx_data[j] = CAN0->RAMn[4*MSG_BUFF_SIZE + 2 + j];
    }
    
    // Clear the flag for MB4 without clearing others
    CAN0->IFLAG1 = 0x00000010; 
}



#ifdef CAN_20
void CAN_Init(void)
{
    #define MSG_BUF_SIZE 4 /* Msg Buffer Size. (CAN 2.0AB: 2 hdr + 2 data= 4 words) --> 16 bytes*/
uint32_t i=0;
PCC->PCCn[PCC_FLEXCAN0_INDEX] |= PCC_PCCn_CGC_MASK; /* CGC=1: enable clock to FlexCAN0 */
	
CAN0->MCR |= CAN_MCR_MDIS_MASK; /* MDIS=1: Disable module before selecting clock */
	
CAN0->CTRL1 &= ~CAN_CTRL1_CLKSRC_MASK; /* CLKSRC=0: Clock Source = oscillator (8 MHz) */
	
CAN0->MCR &= ~CAN_MCR_MDIS_MASK; /* MDIS=0; Enable module config. (Sets FRZ, HALT)*/
	
while (!((CAN0->MCR & CAN_MCR_FRZACK_MASK) >> CAN_MCR_FRZACK_SHIFT)) {}
/* Good practice: wait for FRZACK=1 on freeze mode entry/exit */
	
CAN0->CTRL1 = 0x00DB0006; /* Configure for 500 KHz bit time */
/* Time quanta freq = 16 time quanta x 500 KHz bit time= 8MHz */
/* PRESDIV+1 = Fclksrc/Ftq = 8 MHz/8 MHz = 1 */
/* so PRESDIV = 0 */
/* PSEG2 = Phase_Seg2 - 1 = 4 - 1 = 3 */
/* PSEG1 = PSEG2 = 3 */
/* PROPSEG= Prop_Seg - 1 = 7 - 1 = 6 */
/* RJW: since Phase_Seg2 >=4, RJW+1=4 so RJW=3. */
/* SMP = 1: use 3 bits per CAN sample */
/* CLKSRC=0 (unchanged): Fcanclk= Fosc= 8 MHz */
for(i=0; i<128; i++ ) 
{ /* CAN0: clear 32 msg bufs x 4 words/msg buf = 128 words*/
CAN0->RAMn[i] = 0; /* Clear msg buf word */
}
for(i=0; i<16; i++ ) { /* In FRZ mode, init CAN0 16 msg buf filters */
CAN0->RXIMR[i] = 0xFFFFFFFF; /* Check all ID bits for incoming messages */
}
CAN0->RXMGMASK = 0x1FFFFFFF; /* Global acceptance mask: check all ID bits */
CAN0->RAMn[ 4*MSG_BUF_SIZE + 0] = 0x04000000; /* Msg Buf 4, word 0: Enable for reception */
/* EDL,BRS,ESI=0: CANFD not used */
/* CODE=4: MB set to RX inactive */
/* IDE=0: Standard ID */
/* SRR, RTR, TIME STAMP = 0: not applicable */
#ifdef NODE_A /* Node A receives msg with std ID 0x511 */
CAN0->RAMn[ 4*MSG_BUF_SIZE + 1] = 0x14440000; /* Msg Buf 4, word 1: Standard ID = 0x111 */
#else /* Node B to receive msg with std ID 0x555 */
CAN0->RAMn[ 4*MSG_BUF_SIZE + 1] = 0x15540000; /* Msg Buf 4, word 1: Standard ID = 0x555 */
#endif
/* PRIO = 0: CANFD not used */
CAN0->MCR = 0x0000001F; /* Negate FlexCAN 1 halt state for 32 MBs */
while ((CAN0->MCR && CAN_MCR_FRZACK_MASK) >> CAN_MCR_FRZACK_SHIFT) {}
/* Good practice: wait for FRZACK to clear (not in freeze mode) */
while ((CAN0->MCR && CAN_MCR_NOTRDY_MASK) >> CAN_MCR_NOTRDY_SHIFT) {}
/* Good practice: wait for NOTRDY to clear (module ready) */
}
}
#endif
