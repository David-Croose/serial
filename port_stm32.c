/**************************************************************************************

				This file implements a instance for the component: serial
					The usart2 and timer16 is dedicated to that instance

**************************************************************************************/

#include <string.h>
#include "serial.h"
#include "tim.h"
#include "usart.h"
#include "FreeRTOS.h"
#include "queue.h"

typedef struct {
	uint8_t data[sizeof(((serial_handle_t *)NULL)->data)];
	uint32_t len;
} msg_t;

static QueueHandle_t xQueue;

/**
 * it is recommended that the uart interrupt priority is higher than timer.
 * the timer is halted after initialization.
 * the uart rx irq is active after initialization.
 */
static void init(void) {
	MX_TIM13_Init();
	MX_USART1_UART_Init();
	xQueue = xQueueCreate(3, sizeof(msg_t));
}

static void start_timer(void) {
	HAL_TIM_Base_Start_IT(&htim13);
}

static void stop_timer(void) {
	HAL_TIM_Base_Stop_IT(&htim13);
}

static void enable_uart_rxirq(void) {
	__HAL_UART_ENABLE_IT(&huart1, UART_IT_RXNE);
}

static void disable_uart_rxirq(void) {
	__HAL_UART_DISABLE_IT(&huart1, UART_IT_RXNE);
}

static void uart_send(const uint8_t *p, uint32_t len) {
    HAL_UART_Transmit(&huart1, (uint8_t *)p, len, 500); 
}

// return: 0 --- IRQ
//         1 --- RTOS
static uint8_t is_in_rtos(void) {
    return __get_IPSR() == 0;
}

static void os_queue_send(const uint8_t *p, uint32_t len) {
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	msg_t msg;

	msg.len = len > sizeof(msg.data) ? sizeof(msg.data) : len;
	memcpy(msg.data, p, msg.len);

    if (is_in_rtos()) {
    	xQueueSend(xQueue, &msg, 100);
    } else {
        xQueueSendFromISR(xQueue, &msg, &xHigherPriorityTaskWoken);
        if (xHigherPriorityTaskWoken) {
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        }
    }
}

static uint8_t os_queue_recv(uint8_t *p, uint32_t plen, uint32_t *reallen, uint32_t timeout) {
	msg_t msg;

	if (!is_in_rtos()) {
		return 0;
	}

	if (xQueueReceive(xQueue, &msg, timeout)) {
		*reallen = msg.len > plen ? plen : msg.len;
		memcpy(p, msg.data, *reallen);
		return 1;
	}

	return 0;
}

static void os_queue_clear(void) {
	xQueueReset(xQueue);
}

serial_handle_t serial_stm32 = {
	0,
	20,
	init,
	start_timer,
	stop_timer,
	enable_uart_rxirq,
	disable_uart_rxirq,
    uart_send,
	os_queue_send,
	os_queue_recv,
	os_queue_clear,
	// keep blank for @data
	// keep blank for @data_cnt
};


