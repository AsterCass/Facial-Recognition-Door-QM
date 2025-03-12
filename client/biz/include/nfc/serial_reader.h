#ifndef __NFC_READER_H__
#define __NFC_READER_H__

#ifdef __cplusplus
extern "C" {
#endif

// Configuration of the serial port,
// user can edit here to change the
// configuration of serial port selected.
#include "uart_port.h"
// Configuration comment end.
#define SERIAL_PORT_DEV_PATH "/dev/ttyS4"
#define SERIAL_PORT_BAUDRATE SERIAL_PORT_BAUDRATE_9600
#define SERIAL_PORT_NDATABITS SERIAL_PORT_NDATABITS_8
#define SERIAL_PORT_NSTOPBITS SERIAL_PORT_NSTOPBITS_1
#define SERIAL_PORT_PARITY SERIAL_PORT_PARITY_NONE
#define SERIAL_PORT_SEND_BUF_LEN 16
#define SERIAL_PORT_RECEIVE_BUF_LEN 512
#define SERIAL_PORT_POLL_TIMEOUT_MS 800
int serial_open(char* serial_name);
int serial_close(void);
int serial_write(unsigned char* sendbuf, unsigned int send_len_bytes);
int serial_read(unsigned char* result, unsigned int result_len_bytes, int poll_timeout_ms);

#ifdef __cplusplus
}
#endif

#endif
