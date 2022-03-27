/**
 * i2c通信驱动实现
 */

#include <STC89xx.h>
#include "I2C.h"

// 通信线路管脚
SBIT(I2C_SCL, _P2, 1);
SBIT(I2C_SDA, _P3, 6);

void I2CDelay() {
    unsigned char n = 0x04;
    while (n > 0)
    {
        n--;
    }
}

void I2CStart() {
    // 初始化准备针脚信号
    I2C_SCL = 0x00;
    I2C_SDA = 0x01;
    // 延时一下准备发起开始信号
    I2CDelay();
    // SCL高电平时SDA下降沿
    I2C_SCL = 0x01;
    I2CDelay();
    I2C_SDA = 0x00;
    I2CDelay();
    // 将SCL拉低
    I2C_SCL = 0x00;
    I2CDelay();
}

void I2CStop() {
    // 初始化准备针脚信号
    I2C_SCL = 0x00;
    I2C_SDA = 0x00;
    // 延时一下准备发起结束信号
    I2CDelay();
    // 拉高SCL
    I2C_SCL = 0x01;
    I2CDelay();
    // 拉高SDA
    I2C_SDA = 0x01;
    I2CDelay();
    I2C_SCL = 0x00;
}

/**
 * 写入的字节数据data与0x80右移的每一位取掩码，目的是将写入字节的每一位取出来
 * 将反馈信号ack返回作为标识码：0为写入成功，1为写入失败
 */
unsigned char I2CWrite(unsigned char data) {
    unsigned char ack;
    unsigned char mask;
    // 对比data每一位是否为0，为0说明发送低电平，非0发送高电平
    for (mask = 0x80; mask > 0; mask >>= 1)
    {
        if ((mask & data) == 0)
        {
            I2C_SDA = 0x00;
        }
        else
        {
            I2C_SDA = 0x01;
        }
        // SDA保存数据后，将SCL拉高，把当前位写入进去
        I2CDelay();
        I2C_SCL = 0x01;
        I2CDelay();
        // 数据写入完成后，将SCL拉低，再次进入循环写下一位
        I2C_SCL = 0x00;
        I2CDelay();
    }
    // (反馈信号)
    // 写完一个字节数据后，释放总线，将主机SDA拉高，目的是为了更好分辩从机发过来的应答信号
    I2C_SDA = 0x01;
    I2CDelay();
    // SCL拉高，读取当前已经拉高的SDA值作为ack信号
    I2C_SCL = 0x01;
    I2CDelay();
    ack = I2C_SDA;// 这里读取的应该信号应该是从机发过来的，1代表没写入成功，0代码表写入成功了，这就解释了为什么释放总线时要先把SDA拉高
    I2CDelay();
    // 最后把SCL拉低
    I2C_SCL = 0x00;
    // 写入一个字节数据结束
    return ack;
}

/**
 * 读取字节时，ack信号：0表示SDA拉低继续读，1表示SDA拉高不继续读
 */
unsigned char I2CAckRead(unsigned char ack) {
    unsigned char data = 0x00;
    unsigned char mask;
    // TODO 拉高SDA确保主机被释放？
    I2C_SDA = 0x01;
    // 将读到的每一位组装成一个字节
    for (mask = 0x80; mask > 0; mask >>= 1)
    {
        I2CDelay();
        // 将SCL拉高，准备读取当前位
        I2C_SCL = 0x01;
        if (I2C_SDA == 0x00)
        {
            // SDA为0：将当前位在data相应位置0
            // mask 0x1000 0000, ~mask 0x0111 1111
            // &运算可以将data中~mask为1的位保持不变，为0的位(当前循环中的位)强制为0
            data &= ~mask;
        }
        else
        {
            // SDA为1：将当前位在data相应位置1
            // mask 0x1000 0000 | 0x0000 0000 = 0x1000 0000
            // |运算可以将data中mask为1的位（当前循环中的位）强制为1
            data |= mask;
        }
        I2CDelay();
        // 将SCL拉低，准备读取下一位(为SDA读取下一位数据做准备)
        I2C_SCL = 0x00;
    }
    // (反馈信号)
    // 完成读取一个字节数据后，ack信号给SDA，stop前一个所馈信号给1，表示不再继续读，这个是主机主动发给从机的信号
    I2C_SDA = ack;
    I2CDelay();
    // 拉高SCL，读取ack信号来判断是否继续读取数据
    I2C_SCL = 0x01;
    I2CDelay();
    // 拉低SCL
    I2C_SCL = 0x00;
    return data;
}