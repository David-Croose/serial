This is a component that makes serial reception from byte stream into frame.
An uart and a timer is needed for it's normally work.

Here is an example usage of this component:

    // define a @serial_handle_t instance with your low level port inside
    serial_handle_t serial_stm32 = {
        xxx,
        ...,
        xxx,
    };

    // get bytes in uart rx irq
    void uart_irq_handler(void) {
        serial_get_byte(&serial_stm32, stm32_get_byte());
    }

    // collect all bytes into frame in timer tick irq handler, the frames will be pushed into
    // a queue which you can read in other routine
    void timer_irq_handler(void) {
        serial_get_frame(&serial_stm32);
    }

    int main(void) {
        const uint8_t cmd[] = {0x11, 0x22, 0x33};
        uint8_t buf[100];
        uint32_t buflen;

        serial_init(&serial_stm32);    // initialize the @serial component

        while(1) {
            serial_clear_frame(&serial_stm32);      // clear received framed before you send
            serial_send_frame(&serial_stm32, cmd, sizeof(cmd));
            if (serial_recv_frame(&serial_stm32, buf, &buflen, 100)) {
                // read the @buf
            }
        }
    }

