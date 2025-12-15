/**
 * @file    can_srv.c
 * @brief   CAN Service Layer Implementation
 * @details Implementation của CAN service layer, wrapper cho CAN driver
 * 
 * @author  PhucPH32
 * @date    05/12/2025
 * @version 1.0
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "../inc/can_srv.h"
#include "S32K144.h"
#include <string.h>

/*******************************************************************************
 * Private Definitions
 ******************************************************************************/
#define CAN_MAX_MB      (32U)    /* Maximum message buffers */
#define CAN_RX_MB       (4U)     /* RX mailbox number */
#define CAN_TX_MB       (8U)     /* TX mailbox number */

/*******************************************************************************
 * Private Variables
 ******************************************************************************/
static bool s_can_initialized = false;
static CAN_Type *s_can_instance = NULL;
static can_srv_rx_callback_t s_rx_callback = NULL;

/*******************************************************************************
 * Private Functions
 ******************************************************************************/

/**
 * @brief Calculate CAN bit timing for given baudrate
 * @param baudrate Target baudrate in bps
 * @param presdiv Pointer to store prescaler divider
 * @param propseg Pointer to store propagation segment
 * @param pseg1 Pointer to store phase segment 1
 * @param pseg2 Pointer to store phase segment 2
 */
static void CAN_CalculateTiming(uint32_t baudrate, uint8_t *presdiv, uint8_t *propseg, uint8_t *pseg1, uint8_t *pseg2)
{
    /* Assuming 8MHz CAN clock */
    /* Bit time = (PROPSEG + PSEG1 + PSEG2 + 4) * (PRESDIV + 1) / f_CAN */
    /* For simplicity, use fixed timing values */
    
    switch (baudrate) {
        case 125000:
            *presdiv = 7;   /* (7+1) = 8 */
            *propseg = 7;   /* 8 time quantas */
            *pseg1 = 3;     /* 4 time quantas */
            *pseg2 = 3;     /* 4 time quantas */
            break;
        case 250000:
            *presdiv = 3;
            *propseg = 7;
            *pseg1 = 3;
            *pseg2 = 3;
            break;
        case 500000:
            *presdiv = 1;
            *propseg = 7;
            *pseg1 = 3;
            *pseg2 = 3;
            break;
        case 1000000:
            *presdiv = 0;
            *propseg = 7;
            *pseg1 = 3;
            *pseg2 = 3;
            break;
        default:
            *presdiv = 7;
            *propseg = 7;
            *pseg1 = 3;
            *pseg2 = 3;
            break;
    }
}

/*******************************************************************************
 * Public Functions
 ******************************************************************************/

can_srv_status_t CAN_SRV_Init(const can_srv_config_t *config)
{
    if (config == NULL) {
        return CAN_SRV_ERROR;
    }
    
    /* Use CAN0 as default instance */
    s_can_instance = CAN0;
    
    /* Enable CAN0 clock via PCC */
    /* PCC->PCCn[PCC_FlexCAN0_INDEX] = PCC_PCCn_CGC_MASK; */
    
    /* Enter freeze mode for configuration */
    s_can_instance->MCR |= CAN_MCR_FRZ_MASK | CAN_MCR_HALT_MASK;
    while (!(s_can_instance->MCR & CAN_MCR_FRZACK_MASK));
    
    /* Disable self-reception */
    s_can_instance->MCR |= CAN_MCR_SRXDIS_MASK;
    
    /* Enable individual RX masking */
    s_can_instance->MCR |= CAN_MCR_IRMQ_MASK;
    
    /* Set maximum number of message buffers */
    s_can_instance->MCR = (s_can_instance->MCR & ~CAN_MCR_MAXMB_MASK) | CAN_MCR_MAXMB(0x0F);
    
    /* Calculate and set bit timing */
    uint8_t presdiv, propseg, pseg1, pseg2;
    CAN_CalculateTiming(config->baudrate, &presdiv, &propseg, &pseg1, &pseg2);
    
    s_can_instance->CTRL1 = CAN_CTRL1_PRESDIV(presdiv) |
                            CAN_CTRL1_PROPSEG(propseg) |
                            CAN_CTRL1_PSEG1(pseg1) |
                            CAN_CTRL1_PSEG2(pseg2) |
                            CAN_CTRL1_RJW(1);
    
    /* Configure RX message buffer with filter */
    /* MB4 as RX */
    s_can_instance->RAMn[CAN_RX_MB * 4] = 0x04000000; /* CODE = EMPTY, IDE = 0 */
    
    /* Set RX filter */
    if (config->filter_extended) {
        s_can_instance->RAMn[CAN_RX_MB * 4] |= 0x00200000; /* IDE = 1 */
        s_can_instance->RAMn[CAN_RX_MB * 4 + 1] = (config->filter_id << 1);
        s_can_instance->RXIMR[CAN_RX_MB] = (config->filter_mask << 1);
    } else {
        s_can_instance->RAMn[CAN_RX_MB * 4 + 1] = (config->filter_id << 18);
        s_can_instance->RXIMR[CAN_RX_MB] = (config->filter_mask << 18);
    }
    
    /* Configure TX message buffer */
    /* MB8 as TX inactive */
    s_can_instance->RAMn[CAN_TX_MB * 4] = 0x08000000; /* CODE = INACTIVE */
    
    /* Exit freeze mode */
    s_can_instance->MCR &= ~(CAN_MCR_FRZ_MASK | CAN_MCR_HALT_MASK);
    while (s_can_instance->MCR & CAN_MCR_FRZACK_MASK);
    
    /* Enable RX interrupt for MB4 */
    s_can_instance->IMASK1 |= (1U << CAN_RX_MB);
    
    s_can_initialized = true;
    
    return CAN_SRV_SUCCESS;
}

can_srv_status_t CAN_SRV_Send(const can_srv_message_t *msg)
{
    if (!s_can_initialized) {
        return CAN_SRV_NOT_INITIALIZED;
    }
    
    if (msg == NULL || msg->length > 8) {
        return CAN_SRV_ERROR;
    }
    
    /* Wait for TX MB to be available */
    uint32_t code = (s_can_instance->RAMn[CAN_TX_MB * 4] >> 24) & 0x0F;
    if (code != 0x08) { /* Not INACTIVE */
        return CAN_SRV_ERROR;
    }
    
    /* Configure message buffer */
    uint32_t cs = 0x0C000000 | (msg->length << 16); /* CODE = TX_DATA */
    
    if (msg->isExtended) {
        cs |= 0x00200000; /* IDE = 1 */
        s_can_instance->RAMn[CAN_TX_MB * 4 + 1] = (msg->id << 1) | 0x01; /* SRR = 1 */
    } else {
        s_can_instance->RAMn[CAN_TX_MB * 4 + 1] = (msg->id << 18);
    }
    
    /* Copy data */
    uint32_t *data_ptr = (uint32_t *)&s_can_instance->RAMn[CAN_TX_MB * 4 + 2];
    uint32_t data_word1 = 0, data_word2 = 0;
    
    for (uint8_t i = 0; i < msg->length; i++) {
        if (i < 4) {
            data_word1 |= ((uint32_t)msg->data[i] << (24 - i * 8));
        } else {
            data_word2 |= ((uint32_t)msg->data[i] << (56 - i * 8));
        }
    }
    
    data_ptr[0] = data_word1;
    data_ptr[1] = data_word2;
    
    /* Activate transmission */
    s_can_instance->RAMn[CAN_TX_MB * 4] = cs;
    
    return CAN_SRV_SUCCESS;
}

can_srv_status_t CAN_SRV_Receive(can_srv_message_t *msg)
{
    if (!s_can_initialized) {
        return CAN_SRV_NOT_INITIALIZED;
    }
    
    if (msg == NULL) {
        return CAN_SRV_ERROR;
    }
    
    /* Check if RX MB has data */
    uint32_t iflag = s_can_instance->IFLAG1;
    if (!(iflag & (1U << CAN_RX_MB))) {
        return CAN_SRV_ERROR; /* No message available */
    }
    
    /* Lock the mailbox by reading control/status word */
    uint32_t cs = s_can_instance->RAMn[CAN_RX_MB * 4];
    
    /* Read ID */
    uint32_t id_word = s_can_instance->RAMn[CAN_RX_MB * 4 + 1];
    
    if (cs & 0x00200000) { /* IDE = 1 */
        msg->isExtended = true;
        msg->id = (id_word >> 1) & 0x1FFFFFFF;
    } else {
        msg->isExtended = false;
        msg->id = (id_word >> 18) & 0x7FF;
    }
    
    /* Read length */
    msg->length = (cs >> 16) & 0x0F;
    if (msg->length > 8) {
        msg->length = 8;
    }
    
    /* Read data */
    uint32_t *data_ptr = (uint32_t *)&s_can_instance->RAMn[CAN_RX_MB * 4 + 2];
    uint32_t data_word1 = data_ptr[0];
    uint32_t data_word2 = data_ptr[1];
    
    for (uint8_t i = 0; i < msg->length; i++) {
        if (i < 4) {
            msg->data[i] = (uint8_t)(data_word1 >> (24 - i * 8));
        } else {
            msg->data[i] = (uint8_t)(data_word2 >> (56 - i * 8));
        }
    }
    
    /* Unlock mailbox by reading free running timer */
    (void)s_can_instance->TIMER;
    
    /* Clear interrupt flag */
    s_can_instance->IFLAG1 = (1U << CAN_RX_MB);
    
    /* Call user callback if registered */
    if (s_rx_callback != NULL) {
        s_rx_callback(msg);
    }
    
    return CAN_SRV_SUCCESS;
}

can_srv_status_t CAN_SRV_RegisterCallback(can_srv_rx_callback_t callback)
{
    if (!s_can_initialized) {
        return CAN_SRV_NOT_INITIALIZED;
    }
    
    s_rx_callback = callback;
    
    return CAN_SRV_SUCCESS;
}
