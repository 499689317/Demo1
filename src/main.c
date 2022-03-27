#include <STC89xx.h>
#include <mcs51/lint.h>
#include "LCD1602.h"
#include "Timer0.h"
#include "Infrared.h"
#include "OLED.h"

void main()
{
	OLEDInit();
	// LCD1602Init();
	// InfraredInit();
    while (1)
    {
		OLEDPrint6X8String(0, 0, "hello world");
		OLEDPrint8X16String(0, 1, "HELLO WORLD");
		OLEDPrint6X8String(0, 3, "three row");
		OLEDPrint8X16String(0, 4, "123456789");
		OLEDPrint6X8String(0, 6, "123456789");
		OLEDPrint6X8String(0, 7, "tel no. 18720089980");
		// if (InfraredReady())
        // {
        //     InfraredReset();
        //     unsigned char sign = InfraredData();
		// 	if (sign == 68)
		// 	{
		// 		LCD1602Left();
		// 	} else if (sign == 64)
		// 	{
		// 		LCD1602Right();
		// 	} else if (sign == 70)
		// 	{
		// 		LCD1602Reset();
		// 	}
		// 	LCD1602PrintUnsignedNum(1,1, sign);
		// 	LCD1602PrintUnsignedNum(1,7, sign);
        //     LCD1602PrintUnsignedNum(1,13, sign);
		// 	LCD1602PrintUnsignedNum(2,19, sign);
        // }
    }
}

// void InfraredInter() __interrupt(2) {
//     InfraredLoop();
// }

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