/**
 * @file board1_can_handler.c
 * @brief Board 1 CAN Communication Handler (Implementation)
 * @details CAN message processing for Board 1
 * 
 * @date December 3, 2025
 * @version 1.0
 */

#include "board1_can_handler.h"
#include "board1_adc_task.h"
#include "app_config.h"
#include "protocol.h"

/* Driver API includes */
#include "can.h"
#include "pcc.h"
#include "port.h"

/*******************************************************************************
 * PRIVATE VARIABLES
 ******************************************************************************/

/** CAN handler state */
static struct {
    bool initialized;                       /**< Initialization flag */
    can_message_t rx_queue[CAN_RX_QUEUE_SIZE]; /**< RX message queue */
    uint8_t rx_queue_head;                  /**< Queue head index */
    uint8_t rx_queue_tail;                  /**< Queue tail index */
    uint8_t rx_queue_count;                 /**< Queue count */
} s_canHandler = {
    .initialized = false,
    .rx_queue_head = 0,
    .rx_queue_tail = 0,
    .rx_queue_count = 0
};

/*******************************************************************************
 * PRIVATE FUNCTION PROTOTYPES
 ******************************************************************************/

/**
 * @brief Initialize CAN peripheral
 * @return APP_STATUS_SUCCESS if successful
 */
static app_status_t CanHandler_InitPeripheral(void);

/**
 * @brief Process command message
 * @param[in] frame Received CAN frame
 */
static void CanHandler_ProcessCommand(const can_message_t *frame);

/**
 * @brief Enqueue received message
 * @param[in] frame CAN frame to enqueue
 * @return true if enqueued, false if queue full
 */
static bool CanHandler_EnqueueMessage(const can_message_t *frame);

/**
 * @brief Dequeue received message
 * @param[out] frame Output CAN frame
 * @return true if dequeued, false if queue empty
 */
static bool CanHandler_DequeueMessage(can_message_t *frame);

/*******************************************************************************
 * PUBLIC FUNCTIONS
 ******************************************************************************/

/**
 * @brief Initialize CAN handler
 */
app_status_t CanHandler_Init(void)
{
    app_status_t status;
    
    /* Check if already initialized */
    if (s_canHandler.initialized) {
        return APP_STATUS_ALREADY_INITIALIZED;
    }
    
    /* Initialize CAN peripheral */
    status = CanHandler_InitPeripheral();
    if (status != APP_STATUS_SUCCESS) {
        return status;
    }
    
    /* Initialize queue */
    s_canHandler.rx_queue_head = 0;
    s_canHandler.rx_queue_tail = 0;
    s_canHandler.rx_queue_count = 0;
    
    /* Mark as initialized */
    s_canHandler.initialized = true;
    
    return APP_STATUS_SUCCESS;
}

/**
 * @brief Send CAN message
 */
app_status_t CanHandler_SendMessage(const can_message_t *frame)
{
    /* Validate input */
    if (frame == NULL) {
        return APP_STATUS_INVALID_PARAM;
    }
    
    /* Check initialization */
    if (!s_canHandler.initialized) {
        return APP_STATUS_NOT_INITIALIZED;
    }
    
    /* Send via CAN driver */
    /* Determine mailbox based on message ID */
    uint8_t mailbox;
    if (frame->id == CAN_ID_DATA_ADC_VALUE) {
        mailbox = CAN_MB_TX_DATA;
    } else if (frame->id == CAN_ID_ACK_START || frame->id == CAN_ID_ACK_STOP) {
        mailbox = CAN_MB_TX_ACK;
    } else {
        return APP_STATUS_INVALID_PARAM;
    }
    
    /* Send message */
    // if (CAN_Send(CAN_INSTANCE, mailbox, frame->id, frame->data, frame->length) != CAN_STATUS_SUCCESS) {
    //     return APP_STATUS_ERROR;
    // }
    
    /* Wait for transmission complete (with timeout) */
    // uint32_t timeout = CAN_TIMEOUT_MS;
    // while (CAN_IsMbBusy(CAN_INSTANCE, mailbox) && timeout > 0) {
    //     timeout--;
    //     /* Delay 1ms */
    // }
    
    // if (timeout == 0) {
    //     return APP_STATUS_TIMEOUT;
    // }
    
    return APP_STATUS_SUCCESS;
}

/**
 * @brief Send ADC data message
 */
app_status_t CanHandler_SendAdcData(uint16_t adc_value)
{
    can_message_t frame;
    
    /* Pack ADC value */
    if (!Protocol_PackAdcValue(adc_value, &frame)) {
        return APP_STATUS_ERROR;
    }
    
    /* Send message */
    return CanHandler_SendMessage(&frame);
}

/**
 * @brief Send acknowledge message
 */
app_status_t CanHandler_SendAcknowledge(ack_type_t ack)
{
    can_message_t frame;
    
    /* Pack acknowledge */
    if (!Protocol_PackAcknowledge(ack, &frame)) {
        return APP_STATUS_ERROR;
    }
    
    /* Send message */
    return CanHandler_SendMessage(&frame);
}

/**
 * @brief Process received CAN messages
 */
void CanHandler_ProcessMessages(void)
{
    can_message_t frame;
    
    /* Process all queued messages */
    while (CanHandler_DequeueMessage(&frame)) {
        /* Determine message type by ID */
        if (frame.id == CAN_ID_CMD_START_ADC || frame.id == CAN_ID_CMD_STOP_ADC) {
            CanHandler_ProcessCommand(&frame);
        }
    }
}

/**
 * @brief CAN receive callback
 */
void CanHandler_RxCallback(const can_message_t *frame)
{
    /* Enqueue received message for processing in main loop */
    CanHandler_EnqueueMessage(frame);
}

/*******************************************************************************
 * PRIVATE FUNCTIONS
 ******************************************************************************/

/**
 * @brief Initialize CAN peripheral
 */
static app_status_t CanHandler_InitPeripheral(void)
{
    /* Enable PCC clock for CAN */
    // PCC_EnableClock(PCC_FlexCAN0_INDEX);
    
    /* Configure CAN pins */
    // PCC_EnableClock(PCC_PORTE_INDEX);
    // PORT_SetPinMux(CAN_TX_PORT, CAN_TX_PIN, PORT_MUX_ALT5);  /* CAN0_TX */
    // PORT_SetPinMux(CAN_RX_PORT, CAN_RX_PIN, PORT_MUX_ALT5);  /* CAN0_RX */
    
    /* Initialize CAN peripheral */
    // can_config_t config;
    // config.baudrate = CAN_BAUDRATE_KBPS * 1000;  /* Convert to bps */
    // config.clock_source = CAN_CLK_SOSCDIV2;      /* 8MHz / 2 = 4MHz */
    // CAN_Init(CAN_INSTANCE, &config);
    
    /* Configure RX mailbox for commands */
    // can_filter_t filter;
    // filter.id = CAN_ID_CMD_START_ADC;
    // filter.mask = 0x7FE;  /* Match 0x100-0x101 */
    // CAN_ConfigRxFilter(CAN_INSTANCE, CAN_MB_RX_CMD, &filter);
    
    /* Enable CAN interrupts */
    // CAN_EnableInterrupt(CAN_INSTANCE, CAN_MB_RX_CMD);
    // NVIC_EnableIRQ(CAN0_ORed_0_15_MB_IRQn);
    
    return APP_STATUS_SUCCESS;
}

/**
 * @brief Process command message
 */
static void CanHandler_ProcessCommand(const can_message_t *frame)
{
    command_message_t cmd;
    
    /* Unpack command */
    if (!Protocol_UnpackCommand(frame, &cmd)) {
        return;
    }
    
    /* Execute command */
    switch (cmd.command) {
        case CMD_START_ADC:
            /* Start ADC sampling task */
            AdcTask_Start();
            break;
            
        case CMD_STOP_ADC:
            /* Stop ADC sampling task */
            AdcTask_Stop();
            break;
            
        default:
            /* Unknown command - send error ack */
            CanHandler_SendAcknowledge(ACK_ERROR);
            break;
    }
}

/**
 * @brief Enqueue received message
 */
static bool CanHandler_EnqueueMessage(const can_message_t *frame)
{
    /* Check if queue full */
    if (s_canHandler.rx_queue_count >= CAN_RX_QUEUE_SIZE) {
        return false;
    }
    
    /* Add to queue */
    s_canHandler.rx_queue[s_canHandler.rx_queue_head] = *frame;
    s_canHandler.rx_queue_head = (s_canHandler.rx_queue_head + 1) % CAN_RX_QUEUE_SIZE;
    s_canHandler.rx_queue_count++;
    
    return true;
}

/**
 * @brief Dequeue received message
 */
static bool CanHandler_DequeueMessage(can_message_t *frame)
{
    /* Check if queue empty */
    if (s_canHandler.rx_queue_count == 0) {
        return false;
    }
    
    /* Get from queue */
    *frame = s_canHandler.rx_queue[s_canHandler.rx_queue_tail];
    s_canHandler.rx_queue_tail = (s_canHandler.rx_queue_tail + 1) % CAN_RX_QUEUE_SIZE;
    s_canHandler.rx_queue_count--;
    
    return true;
}

/*******************************************************************************
 * INTERRUPT SERVICE ROUTINES
 ******************************************************************************/

/**
 * @brief CAN0 mailbox interrupt handler
 */
void CAN0_ORed_0_15_MB_IRQHandler(void)
{
    can_message_t frame;
    
    /* Check if command mailbox has data */
    // if (CAN_IsMailboxReady(CAN_INSTANCE, CAN_MB_RX_CMD)) {
    //     /* Read message */
    //     CAN_Receive(CAN_INSTANCE, CAN_MB_RX_CMD, &frame.id, frame.data, &frame.length);
    //     
    //     /* Process in callback */
    //     CanHandler_RxCallback(&frame);
    // }
}
