#include "nfc/nfc_tool.h"

#ifndef WIN32
#include "nfc/serial_reader.h"
#include <algorithm>
#include <cstdio>
#include <cstring>

#include "enums/general_enums.h"

bool isFirst = true;

// 字节校验
unsigned char xor_checksum(unsigned char *buf_l, unsigned int len) {
    unsigned char index, checknum = 0;

    for (index = 0; index < len; index++) {
        checknum = checknum + buf_l[index];
    }

    return checknum;
}

// 显示16进制字节数据
void print_hex(unsigned char *buffer, unsigned int len) {
    unsigned int i;

    for (i = 0; i < len; i++) {
        printf("%02X, ", buffer[i]);
    }
    printf("\n");
}

// 发送数据给模块进行交流
int send_command_debug(unsigned char cmd1, unsigned char cmd2, unsigned char cmd3,
                       unsigned char *data, unsigned char len) {
    int ret;
    static unsigned char send_buf[50] = {0};

    // for devices with a single report
    // header
    send_buf[0] = 0xEE;
    send_buf[1] = 0xEE;
    send_buf[2] = 0xEE;
    send_buf[3] = 0x88;
    // len   (cmd + para + data + checksum)
    send_buf[4] = len + 3;
    send_buf[5] = 0x00;
    // cmd1
    send_buf[6] = cmd1;
    // cmd2
    send_buf[7] = cmd2;
    // cmd3
    send_buf[8] = cmd3;
    // data
    int i = 9;
    for (; i < len + 9; i++) {
        send_buf[i] = data[i - 9];
    }
    send_buf[i] = xor_checksum(send_buf + 4, i - 4);
    //    printf("send_command:");
    //    print_hex(send_buf, i + 1);
    ret = serial_write(send_buf, i + 1);
    if (ret < 0) {
        printf("serial_write failed\n");
        return -1;
    }

    return 0;
}

// 寻卡返回
int rev_command_debug(unsigned char *zbuf, int ms, int *len) {
    //    unsigned char receive_buf[64];
    memset(zbuf, 0x00, 64);
    // print_hex(receive_buf, 32);
    *len = serial_read(zbuf, 64, ms);
    if (*len < 0) {
        printf("rev nfc info failed\n");
        return -1;
    }
    //    printf("rev_command:");
    //    print_hex(zbuf, *len);
    return 0;
}

// 寻卡返回
int check_head(unsigned char *buf, int num) {
    int j = 0;
    int i = 0;
    unsigned char check_buf[4] = {0xEE, 0xEE, 0xEE, 0x88};
    for (int i = 0; i < 4; i++) {
        if (buf[i + j] != check_buf[i]) {
            if (buf[i + j] == 0x00) {
                i--;
                j++;
            } else
                return 0;
        }
    }
    return i + j;
}

// 寻卡返回
// M0	    0x01
// TYPE_A	0x02
// 15693	    0x03
// FELICA	0x04
// ID_CARD	0x05
// M1	    0x06
// TYPE_B	0x07
int rev_command_debug_find(unsigned char *zbuf, int seq) {
    unsigned char buf_check[10] = {0xEE, 0xEE, 0xEE, 0x88, 0x00, 0x00, 0x12, 0xFF, 0x05};
    int ret;
    unsigned char receive_buf[32];
    memset(receive_buf, 0x00, sizeof(receive_buf));
    ret = serial_read(receive_buf, 32, 1000);
    if (ret < 0) {
        printf("chose card failed,%d\n", ret);
        // print_hex(receive_buf, 32);
        return 0;
    }
    //    print_hex(receive_buf, ret);
    for (int i = 0; i < 4; i++) {
        if (receive_buf[i] != buf_check[i])
            return 0;
    }
    for (int i = 6; i < 2; i++) {
        if (receive_buf[i] != buf_check[i])
            return 0;
    }
    if (receive_buf[8] == 0x05) // 卡类型
        return 5;
    return 0;
}

NfcCardData getCardData() {
    NfcCardData data = {};
    // QTime time;
    // time.start();

    unsigned char rev_buf[64] = {0};
    unsigned char send_buf[64] = {0};
    int ret = 0;

    //    if (isFirst)
    //    {
    ret = serial_open("/dev/ttyS4");
    if (ret) {
        printf("nfc read open fail ret:%d\n", ret);
        return data;
    }
    //        isFirst = false;
    //    }

    ret = send_command_debug(0x12, 0xFF, 0x00, send_buf, 0x00);
    if (ret) {
        printf("send_command_debug fail ret:%d\n", ret);
    }

    int len = 0;
    ret = rev_command_debug(rev_buf, 100, &len);

    // convert
    std::string cardNo;
    for (int i = 0; i < len; i++) {
        char temp[3];
        sprintf(temp, "%02x", rev_buf[i]);
        cardNo += temp;
    }
    if (cardNo.size() < 22) {
        return data;
    }
    std::string type = cardNo.substr(16, 2);
    if ("05" == type) {
        data.isExist = true;
        data.cardType = IdCard;
        data.cardNo = cardNo.substr(18, 16);
        std::transform(data.cardNo.begin(), data.cardNo.end(), data.cardNo.begin(), ::toupper);
    } else if ("02" == type) {
        data.isExist = true;
        data.cardType = ICCard;
        data.cardNo = cardNo.substr(18, 8);
        std::transform(data.cardNo.begin(), data.cardNo.end(), data.cardNo.begin(), ::toupper);
    }

    serial_close();

    // qDebug() << "[op:getCardData] Time spend " << time.elapsed() / 1000.0 << "s";

    return data;
}

#else

NfcCardData getCardData() {
    return {};
}

#endif
