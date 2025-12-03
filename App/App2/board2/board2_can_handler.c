/**
 * @file board2_can_handler.c
 * @brief Board 2 CAN Communication Handler (Implementation)
 * @details CAN command/data processing for Board 2
 * 
 * @date December 3, 2025
 * @version 1.0
 */

#include "board2_can_handler.h"
#include "board2_uart_handler.h"
#include "app_config.h"
#include "protocol.h"

/* Driver API includes */
// #include "can.h"
// #include "pcc.h"
// #include "port.h"

/*******************************************************************************
 * PRIVATE VARIABLES
 ******************************************************************************/

/** CAN handler state */
static struct {
    bool initialized;                       /**< Initialization flag */
    uint16_t last_adc_value;                /**< Last received ADC value */
    bool data_valid;                        /**< Data valid flag */
    uint32_t last_rx_timestamp;             /**< Last RX timestamp (ms) */
    can_message_t rx_queue[CAN_RX_QUEUE_SIZE]; /**< RX message queue */
    uint8_t rx_queue_head;                  /**< Queue head index */
    uint8_t rx_queue_tail;                  /**< Queue tail index */
    uint8_t rx_queue_count;                 /**< Queue count */
} s_canHandler = {
    .initialized = false,
    .last_adc_value = 0,
    .data_valid = false,
    .last_rx_timestamp = 0,
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
static app_status_t Board2_CanHandler_InitPeripheral(void);

/**
 * @brief Process ADC data message
 * @param[in] frame Received CAN frame
 */
static void Board2_CanHandler_ProcessAdcData(const can_message_t *frame);

/**
 * @brief Process acknowledge message
 * @param[in] frame Received CAN frame
 */
static void Board2_CanHandler_ProcessAcknowledge(const can_message_t *frame);

/**
 * @brief Enqueue received message
 * @param[in] frame CAN frame to enqueue
 * @return true if enqueued, false if queue full
 */
static bool Board2_CanHandler_EnqueueMessage(const can_message_t *frame);

/**
 * @brief Dequeue received message
 * @param[out] frame Output CAN frame
 * @return true if dequeued, false if queue empty
 */
static bool Board2_CanHandler_DequeueMessage(can_message_t *frame);

/*******************************************************************************
 * PUBLIC FUNCTIONS
 ******************************************************************************/

/**
 * @brief Initialize CAN handler
 */
app_status_t Board2_CanHandler_Init(void)
{
    app_status_t status;
    
    /* Check if already initialized */
    if (s_canHandler.initialized) {
        return APP_STATUS_ALREADY_INITIALIZED;
    }
    
    /* Initialize CAN peripheral */
    status = Board2_CanHandler_InitPeripheral();
    if (status != APP_STATUS_SUCCESS) {
        return status;
    }
    
    /* Initialize state */
    s_canHandler.last_adc_value = 0;
    s_canHandler.data_valid = false;
    s_canHandler.last_rx_timestamp = 0;
    s_canHandler.rx_queue_head = 0;
    s_canHandler.rx_queue_tail = 0;
    s_canHandler.rx_queue_count = 0;
    
    /* Mark as initialized */
    s_canHandler.initialized = true;
    
    return APP_STATUS_SUCCESS;
}

/**
 * @brief Send CAN command to Board 1
 */
app_status_t Board2_CanHandler_SendCommand(command_type_t command)
{
    can_message_t frame;
    
    /* Check initialization */
    if (!s_canHandler.initialized) {
        return APP_STATUS_NOT_INITIALIZED;
    }
    
    /* Pack command */
    if (!Protocol_PackCommand(command, &frame)) {
        return APP_STATUS_ERROR;
    }
    
    /* Send via CAN driver */
    // if (CAN_Send(CAN_INSTANCE, CAN_MB_TX_CMD, frame.id, frame.data, frame.length) != CAN_STATUS_SUCCESS) {
    //     return APP_STATUS_ERROR;
    // }
    
    /* Wait for transmission complete (with timeout) */
    // uint32_t timeout = CAN_TIMEOUT_MS;
    // while (CAN_IsMbBusy(CAN_INSTANCE, CAN_MB_TX_CMD) && timeout > 0) {
    //     timeout--;
    //     /* Delay 1ms */
    // }
    
    // if (timeout == 0) {
    //     return APP_STATUS_TIMEOUT;
    // }
    
    return APP_STATUS_SUCCESS;
}

/**
 * @brief Process received CAN messages
 */
void Board2_CanHandler_ProcessMessages(void)
{
    can_message_t frame;
    
    /* Process all queued messages */
    while (Board2_CanHandler_DequeueMessage(&frame)) {
        /* Determine message type by ID */
        if (frame.id == CAN_ID_DATA_ADC_VALUE) {
            Board2_CanHandler_ProcessAdcData(&frame);
        } else if (frame.id == CAN_ID_ACK_START || frame.id == CAN_ID_ACK_STOP) {
            Board2_CanHandler_ProcessAcknowledge(&frame);
        }
    }
}

/**
 * @brief Check if Board 1 is actively sending data
 */
bool Board2_CanHandler_IsReceivingData(void)
{
    /* Check if data received within last 2 seconds */
    // uint32_t current_time = SystemTick_GetMilliseconds();
    // if ((current_time - s_canHandler.last_rx_timestamp) < 2000) {
    //     return s_canHandler.data_valid;
    // }
    
    return false;
}

/**
 * @brief Get last received ADC value
 */
app_status_t Board2_CanHandler_GetLastAdcValue(uint16_t *adc_value)
{
    /* Validate input */
    if (adc_value == NULL) {
        return APP_STATUS_INVALID_PARAM;
    }
    
    /* Check if data valid */
    if (!s_canHandler.data_valid) {
        return APP_STATUS_ERROR;
    }
    
    /* Return value */
    *adc_value = s_canHandler.last_adc_value;
    
    return APP_STATUS_SUCCESS;
}

/**
 * @brief CAN receive callback
 */
void Board2_CanHandler_RxCallback(const can_message_t *frame)
{
    /* Enqueue received message for processing in main loop */
    Board2_CanHandler_EnqueueMessage(frame);
}

/*******************************************************************************
 * PRIVATE FUNCTIONS
 ******************************************************************************/

/**
 * @brief Initialize CAN peripheral
 */
static app_status_t Board2_CanHandler_InitPeripheral(void)
{
    /* Enable PCC clock for CAN */
    // PCC_EnableClock(PCC_FlexCAN0_INDEX);
    
    /* Configure CAN pins */
    // PCC_EnableClock(PCC_PORTE_INDEX);
    // PORT_SetPinMux(CAN_TX_PORT_B2, CAN_TX_PIN_B2, PORT_MUX_ALT5);  /* CAN0_TX */
    // PORT_SetPinMux(CAN_RX_PORT_B2, CAN_RX_PIN_B2, PORT_MUX_ALT5);  /* CAN0_RX */
    
    /* Initialize CAN peripheral */
    // can_config_t config;
    // config.baudrate = CAN_BAUDRATE_KBPS * 1000;  /* Convert to bps */
    // config.clock_source = CAN_CLK_SOSCDIV2;      /* 8MHz / 2 = 4MHz */
    // CAN_Init(CAN_INSTANCE, &config);
    
    /* Configure RX mailbox for ADC data */
    // can_filter_t filter_data;
    // filter_data.id = CAN_ID_DATA_ADC_VALUE;
    // filter_data.mask = 0x7FF;  /* Exact match */
    // CAN_ConfigRxFilter(CAN_INSTANCE, CAN_MB_RX_DATA, &filter_data);
    
    /* Configure RX mailbox for acknowledge */
    // can_filter_t filter_ack;
    // filter_ack.id = CAN_ID_ACK_START;
    // filter_ack.mask = 0x7FE;  /* Match 0x300-0x301 */
    // CAN_ConfigRxFilter(CAN_INSTANCE, CAN_MB_RX_ACK, &filter_ack);
    
    /* Enable CAN interrupts */
    // CAN_EnableInterrupt(CAN_INSTANCE, CAN_MB_RX_DATA);
    // CAN_EnableInterrupt(CAN_INSTANCE, CAN_MB_RX_ACK);
    // NVIC_EnableIRQ(CAN0_ORed_0_15_MB_IRQn);
    
    return APP_STATUS_SUCCESS;
}

/**
 * @brief Process ADC data message
 */
static void Board2_CanHandler_ProcessAdcData(const can_message_t *frame)
{
    adc_data_message_t adc_msg;
    
    /* Unpack ADC data */
    if (!Protocol_UnpackAdcValue(frame, &adc_msg)) {
        return;
    }
    
    /* Store value */
    s_canHandler.last_adc_value = adc_msg.adc_value;
    s_canHandler.data_valid = true;
    // s_canHandler.last_rx_timestamp = SystemTick_GetMilliseconds();
    
    /* Forward to UART handler */
    Board2_UartHandler_SendAdcValue(adc_msg.adc_value);
}

/**
 * @brief Process acknowledge message
 */
static void Board2_CanHandler_ProcessAcknowledge(const can_message_t *frame)
{
    ack_message_t ack_msg;
    
    /* Unpack acknowledge */
    if (!Protocol_UnpackAcknowledge(frame, &ack_msg)) {
        return;
    }
    
    /* Handle acknowledge */
    switch (ack_msg.ack) {
        case ACK_START_SUCCESS:
            /* Board 1 started ADC sampling */
            // Send notification via UART
            Board2_UartHandler_SendString("ADC sampling started\r\n");
            break;
            
        case ACK_STOP_SUCCESS:
            /* Board 1 stopped ADC sampling */
            // Send notification via UART
            Board2_UartHandler_SendString("ADC sampling stopped\r\n");
            /* Clear data valid flag */
            s_canHandler.data_valid = false;
            break;
            
        case ACK_ERROR:
            /* Error occurred */
            Board2_UartHandler_SendString("Error: Command failed\r\n");
            break;
            
        default:
            break;
    }
}

/**
 * @brief Enqueue received message
 */
static bool Board2_CanHandler_EnqueueMessage(const can_message_t *frame)
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
static bool Board2_CanHandler_DequeueMessage(can_message_t *frame)
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
    
    /* Check if data mailbox has data */
    // if (CAN_IsMailboxReady(CAN_INSTANCE, CAN_MB_RX_DATA)) {
    //     /* Read message */
    //     CAN_Receive(CAN_INSTANCE, CAN_MB_RX_DATA, &frame.id, frame.data, &frame.length);
    //     
    //     /* Process in callback */
    //     Board2_CanHandler_RxCallback(&frame);
    // }
    
    /* Check if ack mailbox has data */
    // if (CAN_IsMailboxReady(CAN_INSTANCE, CAN_MB_RX_ACK)) {
    //     /* Read message */
    //     CAN_Receive(CAN_INSTANCE, CAN_MB_RX_ACK, &frame.id, frame.data, &frame.length);
    //     
    //     /* Process in callback */
    //     Board2_CanHandler_RxCallback(&frame);
    // }
}
