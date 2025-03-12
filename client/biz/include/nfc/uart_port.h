#ifndef __SERIAL_PORT_H__
#define __SERIAL_PORT_H__
#include <QDebug>
#ifndef Q_OS_WIN

#ifdef __cplusplus
extern "C" {
#endif

#include <termios.h>
#include <unistd.h>

enum serial_port_flag
{
    SERIAL_PORT_TIMEOUT_ERR_F = -7,
    SERIAL_PORT_PARA_ERR_F    = -6,
    SERIAL_PORT_OPEN_ERR_F    = -5,
    SERIAL_PORT_CLOSE_ERR_F   = -4,
    SERIAL_PORT_SET_ERR_F     = -3,
    SERIAL_PORT_WRITE_ERR_F   = -2,
    SERIAL_PORT_READ_ERR_F    = -1,

    SERIAL_PORT_OKAY_F = 0,
};

enum serial_port_baudrate
{
    SERIAL_PORT_BAUDRATE_0      = B0,  // 0 baud (terminate the connection)
    SERIAL_PORT_BAUDRATE_50     = B50,
    SERIAL_PORT_BAUDRATE_75     = B75,
    SERIAL_PORT_BAUDRATE_110    = B110,
    SERIAL_PORT_BAUDRATE_134    = B134,
    SERIAL_PORT_BAUDRATE_150    = B150,
    SERIAL_PORT_BAUDRATE_200    = B200,
    SERIAL_PORT_BAUDRATE_300    = B300,
    SERIAL_PORT_BAUDRATE_600    = B600,
    SERIAL_PORT_BAUDRATE_1200   = B1200,
    SERIAL_PORT_BAUDRATE_1800   = B1800,
    SERIAL_PORT_BAUDRATE_2400   = B2400,
    SERIAL_PORT_BAUDRATE_4800   = B4800,
    SERIAL_PORT_BAUDRATE_9600   = B9600,
    SERIAL_PORT_BAUDRATE_19200  = B19200,
    SERIAL_PORT_BAUDRATE_38400  = B38400,
    SERIAL_PORT_BAUDRATE_57600  = B57600,
    SERIAL_PORT_BAUDRATE_115200 = B115200,
    SERIAL_PORT_BAUDRATE_230400 = B230400,
    SERIAL_PORT_BAUDRATE_921600 = B921600,
};

enum serial_port_ndatabits
{
    SERIAL_PORT_NDATABITS_5 = CS5,
    SERIAL_PORT_NDATABITS_6 = CS6,
    SERIAL_PORT_NDATABITS_7 = CS7,
    SERIAL_PORT_NDATABITS_8 = CS8,
};

enum serial_port_nstopbits
{
    SERIAL_PORT_NSTOPBITS_1,
    SERIAL_PORT_NSTOPBITS_2,
};

enum serial_port_parity
{
    SERIAL_PORT_PARITY_NONE,
    SERIAL_PORT_PARITY_ODD,
    SERIAL_PORT_PARITY_EVEN,
};

struct serial_port_config
{
    char* dev_path;

    enum serial_port_baudrate  baudrate;
    enum serial_port_ndatabits ndatabits;
    enum serial_port_nstopbits nstopbits;
    enum serial_port_parity    parity;
};

struct serial_port_operation
{
    int (*open)(char* path);
    int (*close)(int fd);
    int (*init)(int fd, enum serial_port_baudrate baudrate, enum serial_port_ndatabits ndatabits, enum serial_port_nstopbits nstopbits, enum serial_port_parity parity);
    int (*write)(int fd, char* buf, unsigned int len);
    int (*read)(int fd, char* buf, unsigned int len, int poll_timeout_ms);
};

struct serial_port
{
    struct serial_port_config*    config;
    struct serial_port_operation* operation;

    int poll_timeout_ms;

    int   fd;
    char* send_buf;
    int   send_buf_len;
    char* receive_buf;
    int   receive_buf_len;
};

#ifdef __cplusplus
}
#endif

#endif

#endif
