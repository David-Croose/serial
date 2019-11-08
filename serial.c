#include <string.h>
#include "serial.h"

/**
 * serial initialization
 * @param h: the serial instance
 */
void serial_init(serial_handle_t *h) {
    // you should make sure that every member of @h is initialized before
	h->init();
}

/**
 * get a byte from uart
 * @param h: the serial instance
 * @param byte: the received byte from hardware uart
 */
void serial_get_byte(serial_handle_t *h, uint8_t byte) {
	h->tick = 0;
	h->data[h->data_cnt] = byte;

	if (++h->data_cnt >= sizeof(h->data)) {
		h->data_cnt = 0;		// TODO  this is not good
	}

	if (h->data_cnt == 1) {
		h->start_timer();
	}
}

/**
 * collect bytes into frames and push them into a queue
 * @param h: the serial instance
 * @note: it is needed to put this function in a timer callback or irq function
 */
void serial_get_frame(serial_handle_t *h) {
	if (++h->tick > h->tick_max) {
		h->tick = 0;
		h->disable_uart_rx();
		h->stop_timer();
		h->send_frame(h->data, h->data_cnt);
		memset(h->data, 0, sizeof(h->data));
		h->data_cnt = 0;
		h->enable_uart_rx();
	}
}

/**
 * clear the frame queue
 * @param h: the serial instance
 */
void serial_clear_frame(serial_handle_t *h) {
	h->clear_frame();
}

/**
 * receive frame
 * @param h: the serial instance
 * @param p: the memory to store the received frame
 * @param plen: the length(in bytes) of @p
 * @param reallen: the real length(in bytes) of received frame
 * @param timeout: the max time of waiting for frame
 * @return: 1 --- got a frame
 *          0 --- timeout to get a frame
 * @note: if the @plen is not big enough, you may lost some part of frames
 */
int32_t serial_recv_frame(serial_handle_t *h, uint8_t *p, uint32_t plen, uint32_t *reallen, uint32_t timeout) {
	if (h->recv_frame(p, plen, reallen, timeout)) {	/* if got frame */
		return 1;
	}
	return 0;
}

/**
 * send frame
 * @param h: the serial instance
 * @param p: the memory to be sent
 * @param len: the length(in bytes) of @p
 */
void serial_send_frame(serial_handle_t *h, const uint8_t *p, uint32_t len) {
    h->send_uart(p, len);
}
