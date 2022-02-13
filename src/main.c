// 头文件
#include <STC89xx.h>
#include <mcs51/lint.h>

// 定义P1 变量值为0x90，实际就是寄存器的 P1 HEAD位置
#define _P1 0x90
// 单片机晶振频率 单位(hz) 11059200 Hz
// 11059200Hz*10^-3  = 11059.2 GHz
// 11059.2GHz*10^3 = 11.0592 MHz
#define FOSC 11059200
// 设定时器每1毫秒溢出一次
#define T_NUMBER_1MS (65536 - FOSC / 12 / 1000)
// 串口通信设置波特率为9600时，定时器T1每发送一帧定时器初始计数值
#define T_NUMBER_SMS 0xFD//(256 - FOSC / 32 / 12 / 9600)

/**
 * led灯亮灭控制
 * P1寄存器地址为0x90
 * P10-P17控制8个led晶体管亮灭
 */

// 声明寄存器位置
SFR(P1, 0x90);
// 类似 sbit led0 = P1^0
// 通过寄存器位置获取 P1-0 针脚的地址，将地址赋予LED1
SBIT(LED0, _P1, 0);
// 通过寄存器位置获取 P1-1 针脚的地址，将地址赋予LED2
SBIT(LED1, _P1, 1);
// 通过寄存器位置获取 P1-2 针脚的地址，将地址赋予LED3
SBIT(LED2, _P1, 2);
SBIT(LED3, _P1, 3);
SBIT(LED4, _P1, 4);
SBIT(LED5, _P1, 5);
SBIT(LED6, _P1, 6);
SBIT(LED7, _P1, 7);

/**
 * 蜂鸣器控制
 * P00针脚0：响，1：不响
 */

/**
 * 定时器控制(内置2个定时器)，定时器使用16bit来定义模式与数值，分为TCON寄存器(8bit)与TMOD寄存器(8bit)，计数块TH1, TL1, TH0, TL0组成完整定时器
 * 任一定时器包含：TCON, TMOD, TH(0~1), TL(0~1)
 * TCON: 定时器控制寄存器，共8位，可位寻址：TF1, TR1, TF0, TR0分别表示T1, T0定时器标置位
 * TF标置位：当定时器数值溢出时，由硬件将该位置1
 * TR标置位：当该位置1时定时器开始计时，由软件控制
 * TMOD: 定时器模式寄存器，共8位，高4位控制T1定时器，低4位控制T0定时器，主要依赖：M1, M0(T0为第1位，第0位，T1为第5位，第4位)来选择定时器模式。可控制计数块THx与TLx不同用法
 * 分为00/01/10/11四种不同法，常用01模式，THx与TLx组成16位定时器
 */

// 寄存器TMOD设置定时器模式，不可位寻址
SFR(T_MOD, 0x89);
// 寄存器TCON选择使用哪个定时器，可位寻址TF，第5位为T0定时器控制位TF0，第4位为T0控制位TR0
SBIT(T0_TF, 0x88, 5);
SBIT(T0_TR, 0x88, 4);
// T0计数块TH0, TL0
SFR(T0_TL, 0x8A);
SFR(T0_TH, 0x8C);

/**
 * 中断系统
 * 单片机8个中断源，4个外部中断，3个定时器中断，1个串口中断
 * 外部中断：INT0, INT1, INT2, INT3
 * 定时器中断：Timer0, Timer1, Timer2
 * 串口中断：UART
 * 1. 开启允许控制中断（操作IE寄存器）
 * 2. 设置中断优先级，中断方式
 * 3. 实现中断函数，中断过程需要实现的内容
 */

// 允许中断开启标置位
SBIT(INT_EA, 0xA8, 7);// IE寄存器最高位置1，开启允许中断控制
// Timer0中断控制
SBIT(T0_ET, 0xA8, 1);// IE寄存器第1位置1， 开启允许中断控制
// Timer1中断控制
SBIT(T1_ET, 0xA8, 3);
// 允许串口中断
SBIT(S_ES, 0xA8, 4);
// 开启外部中断0控制位
SBIT(INT0_EX, 0xA8, 0);
// 设置外部中断0中断方式
SBIT(INT_IT, 0x88, 0);

/**
 * 串口通信控制
 * TCON寄存器开启定时器T1作为串口通信发生器
 * SCON寄存器IE/ES开启串口中断(RART)TI为发送串口信息，RI为读取串口信息
 * PCON寄存器SMOD设置串口通信模式
 */

// T1定时器
SBIT(T1_TF, 0x88, 7);
SBIT(T1_TR, 0x88, 6);
// T1计数块TH1, TL1
SFR(T1_TL, 0x8B);
SFR(T1_TH, 0x8D);

// SCON设置
// 允许/禁止串口接收信息位REN
SBIT(S_REN, _SCON, 4);
// 串口通信模式位SM0, SM1
SBIT(S_M0, _SCON, 7);
SBIT(S_M1, _SCON, 6);
// 中断标置位TI,RI
SBIT(S_TI, _SCON, 1);
SBIT(S_RI, _SCON, 0);
// PCON设置，TODO PCON不能位寻址？
SBIT(S_MOD, _PCON, 7);

/**
 * 数码管控制
 * 数码管根据电流导通方向分为共阳极，共阴极数码管，以共阴极为例
 * 阴极端为二极管电流流出方向，由锁存器U1控制WE1(Q1), WE2(Q2), WE3(Q3), WE4(Q4)
 * 阳极商为二极管电流流入方向，由锁存器U2控制，单片机P2IO口控制a(Q1), b(Q2), c(Q3), d(Q4), e(Q5), f(Q6), g(Q7), dp(Q8)
 * 锁存器OE低电平有效，LE(U2(P06),U1(P07))为高电平时，D->Q为输入状态，LE为低电平时，D端输入不影响Q端输出，此时为锁存状态
 */
// 电机开启前，将U2锁存器控制的数码管关闭
SBIT(U2_LE, _P0, 6);
// U1的Q5-Q8为电机输出控制位
SBIT(U1_LE, _P0, 7);

/**
 * 液晶显示控制
 * 1602显示屏分16引脚，RS(P36)数据/命令控制位，RW(P35)读/写控制位，E(P34)使能控制位，拉高电平时执行操作，拉低电平时完成操作，D0-D7(P2)I/O引脚
 * 读写时序图
 * 当使能信叫为高电平时为非空闲状态，低电平时为空闲状态
 * 写指令 38H:设置 16x2 显示，5x7 点阵，8 位数据接口;
 * 写指令 0CH:开显示，关闭光标;
 * 写指令 06H:读或写完一个字符后，地址指针、光标均加 1;
 * 写指令 01H:数据指针清零、所示显示清零
 * 显示操作
 */

SBIT(RS_1602, _P3, 6);
SBIT(RW_1602, _P3, 5);
SBIT(E_1602, _P3, 4);

/**
 * 10	2		16
 * 0	0000	0x0
 * 1	0001	0x1
 * 2	0010	0x2
 * 3	0011	0x3
 * 4	0100	0x4
 * 5	0101	0x5
 * 6	0110	0x6
 * 7	0111	0x7
 * 8	1000	0x8
 * 9	1001	0x9
 * 10	1010	0xA
 * 11	1011	0xB
 * 12	1100	0xC
 * 13	1101	0xD
 * 14	1110	0xE
 * 15	1111	0xF
 */
/**
 * char				1Byte		8bit		-128~127|0~255
 * signed char		1Byte		8bit		-128~127
 * unsigned char	1Byte		8bit		0~255
 * int				2~4Byte		16~32bit
 * unsigned int		2~4Byte		16~32bit
 * short			2Byte		16bit
 * unsigned short	2Byte		16bit
 * long				4Byte		32bit
 * unsigned long	4Byte		32bit
 * float			4Byte		32bit
 * double			8Byte		64bit
 * long double		16Byte		128bit
 */

// 数码管编码表
// 1-4数码管位置
unsigned char NUM_POS[] = {0xFE, 0xFD, 0xFB, 0xF7};
// 1-9数字编码
unsigned char NUM_DIG[] = {0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F};

// 电机节拍编码
unsigned char MOTOR_ANG[] = {0xDF, 0xCF, 0xEF, 0x5F, 0x7F, 0x3F, 0xBF, 0x9F};

unsigned char ledH = 0xFF;
unsigned char ledL = 0x00;
unsigned char index = 0x00;
unsigned long count = 0;

void delay(unsigned short dt) {
	while (dt > 0)
	{
		dt--;
	}
}
// T0计时器
void startTick0() {
	// 定时器T0初始化
	// 16位定时器，选用模式01
	T_MOD = 0b00000001;
	// 初始计数值，低8位，高8位初始化
	T0_TL = T_NUMBER_1MS;
	T0_TH = T_NUMBER_1MS >> 8;
	// 开启T0定时，T0的TR位置1，这里应该是已经默认TF为0
	// T0_TF = 0x00;
	T0_TR = 0x01;
}
bool isFullTick0() {
	return T0_TF == 0x01;
}
void resetTick0() {
	// 重新调整初始化状态值
	T0_TL = T_NUMBER_1MS;
	T0_TH = T_NUMBER_1MS >> 8;
	T0_TF = 0x00;
}
// T1计时器，使用中断方式1，TL溢出后循环读取TH值作为初始值
// 串口通信中，TL,TH初始值同波特率计算公式获得
// 串口通信方式1：
// 波特率=2SMOD/32*T1溢出率
// T1溢出率=FOSC/(12*(256-x))
void startTick1() {
	T_MOD = 0x20;
	T1_TL = T_NUMBER_SMS;
	T1_TH = T_NUMBER_SMS;
	T1_TR = 0x01;
}
bool isFullTick1() {
	return T1_TF == 0x01;
}
void resetTick1() {
	T1_TL = T_NUMBER_SMS;
	T1_TH = T_NUMBER_SMS;
	T1_TF = 0x00;
}

// 开启串口通信
void startSerial() {
	S_REN = 0x01;
	S_M0 = 0x00;
	S_M1 = 0x01;
	// TODO 这个地方注释后就可以正常接收到数据？，不知道什么原因
	// SMOD为0时，波特率不加倍，为1时波特率加倍
	// 测试结果：如果SMOD设置为1，调试工具频率加倍不能正常工作，频率一致可以正常工作，已经跟作者反映了问题
	S_MOD = 0x01;
}
// 发送串口数据
void sendBuf(unsigned char buf) {
	SBUF = buf;
}
// 接收串口数据
void reciveBuf() {
	P1 = SBUF;
}

// 开启T0中断
void startTimer0Inter() {
	INT_EA = 0x01;
	T0_ET = 0x01;
}
// 开启T1中断
void startTimer1Inter() {
	INT_EA = 0x01;
	T1_ET = 0x01;
}
// 开启外部中断0
void startInter0Inter() {
	INT_EA = 0x01;
	INT0_EX = 0x01;
	// IT位设置INT0中断触发方式
	INT_IT = 0x01;// 下降沿触发
}
// 开启串口通信中断
void startInter4Inter() {
	INT_EA = 0x01;
	S_ES = 0x01;
}

// 控制led灯闪烁
void ledLight(unsigned char h, unsigned char l) {
	LED0 = l;
	LED1 = l;
	LED2 = l;
	LED3 = l;
	LED4 = h;
	LED5 = h;
	LED6 = h;
	LED7 = h;
}
// 控制蜂鸣器声音
void beenMusic() {
	P00 = ((P00 == 0x01) ? 0x00 : 0x01);
}

// 数码管显示
void chooseView(unsigned char index) {
	P2 = NUM_POS[index];// 选择控制哪个数码管
	U1_LE = 0x01;// 开启U2锁存器输入模式
	U1_LE = 0x00;// 锁存U2输入
}
void writeNum(unsigned char num) {
	P2 = NUM_DIG[num];// 控制数码管显示内容
	U2_LE = 0x01;// 开启U1锁存器输入模式
	U2_LE = 0x00;// 锁存U1输入
}

// 液晶显示器
unsigned char readData() {
	unsigned char status;
	RS_1602 = 0;
	RW_1602 = 1; //进入读 1602 状态模式
	E_1602 = 1; //拉高使能信号
	status = P2;    //将 1602 状态数据读取
	E_1602 = 0; //拉低使能，完成读操作
	return status;  //将状态值返
}
void isReady() {
	P2 = 0xFF;
    while (readData() & 0x80); //bit7 等于 1 表示忙，一直检测到 0 为止
}
void writeData(unsigned char data) {
	isReady(); //检测 1602 是否处于空闲状态
    RS_1602 = 1;
    RW_1602 = 0; //进入写数据模式
    P2 = data; //将数据输出
    E_1602 = 1; //拉高使能信号
    E_1602 = 0; //拉低使能，完成写操作
}
void writeCmd(unsigned char cmd) {
	isReady(); //检测 1602 是否处于空闲状态
    RS_1602 = 0;
    RW_1602 = 0; //进入写指令模式
    P2 = cmd;    //将指令数据输出
    E_1602 = 1; //拉高使能信号
    E_1602 = 0; //拉低使能，完成写操作
}
void viewData(unsigned char row, unsigned char col, char *data) {
    unsigned char addr = (row - 1) * 0x40 + (col - 1); //组合成地址
    writeCmd(0x80 + addr);                   //写地址命令
    while (*data) //判断data字符串是否已结束
    {
        writeData(*data++); //将data字符串数据依次写入
    }
}

/**
 * 步进电机控制
 * 电机接U1锁存器：控制信号接P07，P07接上拉电阻，默认为高电平
 * 电机启动频率：转动机电每秒最少需要节拍数，假设电机启动频率为550，则每个节拍持续时间要大于1/550，让定时器T1每2ms中断一次
 */

void startMotor() {
	// 将数码管所有位置0，关闭数码管
	P2 = 0x00;
	U2_LE = 1;
	U2_LE = 0;
	// 打开U1锁存器控制步进电机，并保持打开状态
	U1_LE = 1;
	// 计算节拍
    // count = (180 * 4096) / 360;
}
void rotateMotor() {
	// 根据步进电机类型，每2ms输入一次节拍
	P2 = MOTOR_ANG[index];
	// 循环电机8个节拍
	index++;
	if (index > 8)
	{
		index = 0;
	}
}

void main()
{
	// 测试定时器0
	// startTick0();
	// 测试T0中断
	// startTimer0Inter();

	// 测试定时器1
	startTick1();
	// 测试T1中断，串口通信时关闭此中断，用外部中断4
	// startTimer1Inter();
	
	// 测试外部中断0
	// startInter0Inter();

	// 测试串口通信
	startSerial();
	// 测试串口中断
	startInter4Inter();

	// 测试液晶显示
	// writeCmd(0x38); //设置 16x2 显示，5x7 点阵，8 位数据接口
	// writeCmd(0x0C); //开显示，关闭光标
	// writeCmd(0x06); //读或写完一个字符后，地址指针、光标均加 1
	// writeCmd(0x01); //数据指针清零、所示显示清零
	// viewData(1, 3, "HELLO WORLD");
	// viewData(2, 3, "Hello World");

	// 测试电机转动
	// startMotor();

	// 主循环
    while (1)
    {
		// delay延时，可用定时器替代
		// delay(50000);

		// 测试数码管
		// chooseView(index);
		// writeNum(count);
		// count++;
		// if (count > 8)
		// {
		// 	count = 0;
		// 	index++;
		// 	if (index > 3)
		// 	{
		// 		index = 0;
		// 	}
		// }

		// 检测定时器T0中TF的状态值，如果每置1一次则表示当前累加值溢出一次
		// if (isFullTick0())
		// {
		// 	resetTick0();
		// 	// 每重置一次做一次标识
		// 	count++;
		// 	if (count >= 10)
		// 	{
		// 		// count循环累计
		// 		count = 0;
		// 		ledH = ((ledH == 0xFF) ? 0x00 : 0xFF);
		// 		ledL = ((ledL == 0x00) ? 0xFF : 0x00);
		// 		ledLight(ledH, ledL);
		// 		beenMusic();
		// 	}
		// }

		// 检测串口信息
		// if (S_RI == 0x01)
		// {
		// 	S_RI = 0x00;
		// 	P1 = SBUF;
		// }
    }
}

// 实现外部中断0中断函数
void inter0Inter() __interrupt(0) {
	ledH = ((ledH == 0xFF) ? 0x00 : 0xFF);
	ledL = ((ledL == 0x00) ? 0xFF : 0x00);
	ledLight(ledH, ledL);
	// beenMusic();
}

// 实现串口中断函数，从实验来看，中断函数像是不停在轮询
void inter4Inter() __interrupt(4) {
	// 退出中断
	if (S_RI == 0x01)
	{
		S_RI = 0x00;
		reciveBuf();
		// 发送数据
		sendBuf(SBUF);
	}
	else if (S_TI == 0x01)
	{
		S_TI = 0x00;
	}
}

// 实现T0中断函数
void timer0Inter() __interrupt(1) {
	resetTick0();
	count++;
	if (count > 2)
	{
		count = 0;
		// rotateMotor();
	}
}
// 实现T1中断函数
void timer1Inter() __interrupt(3) {
	resetTick1();
	count++;
	if (count > 5000)
	{
		count = 0;
		ledH = ((ledH == 0xFF) ? 0x00 : 0xFF);
		ledL = ((ledL == 0x00) ? 0xFF : 0x00);
		ledLight(ledH, ledL);
		beenMusic();
	}
}