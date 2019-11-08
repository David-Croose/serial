#ifndef _SERIAL_H_
#define _SERIAL_H_

#include <stdint.h>

typedef struct {
	uint32_t tick;
	uint32_t tick_max;

	void (*init)(void);

	void (*start_timer)(void);
	void (*stop_timer)(void);

	void (*enable_uart_rx)(void);
	void (*disable_uart_rx)(void);
    void (*send_uart)(const uint8_t *p, uint32_t len);

	void (*send_frame)(const uint8_t *p, uint32_t len);
    /**
     * receive a frame from queque
     * @param p: the memory to store the received frame
     * @param plen: the length(in bytes) of @p
     * @param reallen: the real length(in bytes) of received frame
     * @param timeout: the max time of waiting for frame
     * @return: 1 --- got a frame
     *          0 --- timeout to get a frame
     */
	uint8_t (*recv_frame)(uint8_t *p, uint32_t plen, uint32_t *reallen, uint32_t timeout);
	void (*clear_frame)(void);

	uint8_t data[100];
	uint32_t data_cnt;
} serial_handle_t;

void serial_init(serial_handle_t *h);
void serial_get_byte(serial_handle_t *h, uint8_t byte);
void serial_get_frame(serial_handle_t *h);
void serial_clear_frame(serial_handle_t *h);
int32_t serial_recv_frame(serial_handle_t *h, uint8_t *p, uint32_t plen, uint32_t *reallen, uint32_t timeout);
void serial_send_frame(serial_handle_t *h, const uint8_t *p, uint32_t len);

#endif
