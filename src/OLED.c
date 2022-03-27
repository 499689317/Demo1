#include "CODE.h"
#include "I2C.h"
#include "OLED.h"

// OLED写入1字节数据
void OLEDWriteByte(unsigned char type, unsigned char data) {
	I2CStart();
	if (type == 0x00)
	{
		// OLED写入命令
		I2CWrite(0x78);
		I2CWrite(0x00);
	} else if (type == 0x01)
	{
		// OLED写入数据
		I2CWrite(0x78);
		I2CWrite(0x40);
	}
	I2CWrite(data);
	I2CStop();
}
void OLEDPos(unsigned char x, unsigned char y) {
	OLEDWriteByte(0x00, (y + 0xB0));
	OLEDWriteByte(0x00, (x & 0xF0) >> 4 | 0x10);
	OLEDWriteByte(0x00, (x & 0x0F) | 0x01);
}
void OLEDScreen(unsigned char data) {
	unsigned char x = 0, y = 0;
	for (y = 0; y < 8; y++)
	{
		OLEDWriteByte(0x00, (y + 0xB0));
		OLEDWriteByte(0x00, 0x01);
		OLEDWriteByte(0x00, 0x00);
		for (x = 0; x < WIDTH; x++)
		{
			OLEDWriteByte(0x01, data);
		}
	}
}
void OLEDClear() {
	unsigned char x = 0, y = 0;
	for(y = 0; y < 8; y++)
	{
		OLEDWriteByte(0x00, (y + 0xB0));
		OLEDWriteByte(0x00, 0x01);
		OLEDWriteByte(0x00, 0x10);		  
		for(x = 0; x < WIDTH; x++)
		{
			OLEDWriteByte(0x01, 0x00);
		}
	}
}
void OLEDInit() {
	I2CDelay();
	OLEDWriteByte(0x00, 0xAE);//--display off
	OLEDWriteByte(0x00, 0x00);//--set low column address
	OLEDWriteByte(0x00, 0x10);//--set high column address
	OLEDWriteByte(0x00, 0x40);//--set start line address
	OLEDWriteByte(0x00, 0x81);// contract control
	OLEDWriteByte(0x00, 0xCF);// contract control
	OLEDWriteByte(0x00, 0xA1);// set segment remap
	OLEDWriteByte(0x00, 0xC8);// Com scan direction
	OLEDWriteByte(0x00, 0xA6);//--normal / reverse
	OLEDWriteByte(0x00, 0xA8);//--set multiplex ratio(1 to 64)
	OLEDWriteByte(0x00, 0x3F);//--1/32 duty
	OLEDWriteByte(0x00, 0xD3);//-set display offset
	OLEDWriteByte(0x00, 0x00);
	OLEDWriteByte(0x00, 0xD5);//set osc division
	OLEDWriteByte(0x00, 0x80);
	OLEDWriteByte(0x00, 0xD9);//Set Pre-Charge Period
	OLEDWriteByte(0x00, 0xF1);
	OLEDWriteByte(0x00, 0xDA);//set com pin configuartion
	OLEDWriteByte(0x00, 0x12);
	OLEDWriteByte(0x00, 0xDB);//set Vcomh
	OLEDWriteByte(0x00, 0x40);
	OLEDWriteByte(0x00, 0x20);//set Vcomh
	OLEDWriteByte(0x00, 0x02);
	OLEDWriteByte(0x00, 0x8D);//set charge pump enable
	OLEDWriteByte(0x00, 0x14);
	OLEDWriteByte(0x00, 0xA4);
	OLEDWriteByte(0x00, 0xA6);
	OLEDWriteByte(0x00, 0xAF);//--turn on oled panel

	OLEDScreen(0x00);
	OLEDPos(0, 0);
	OLEDScreen(0xFF);
	I2CDelay();
	OLEDClear();
}
// 字体大小为6X8，6为占据128列中的6列，8为占据1页
void OLEDPrint6X8String(unsigned char x, unsigned char y, unsigned char data[])
{
	unsigned char i = 0, j = 0, n = 0;
	while (data[j] != '\0')
	{
		n = data[j] - 32;
		if(x > WIDTH)
		{
			x = 0;
			y++;
		}
		OLEDPos(x, y);
		for(i = 0; i < 6; i++)
		{
			OLEDWriteByte(0x01, ASC6X8[n][i]);
		}
		x += 6;
		j++;
	}
}
// 8X16字体，8占据128列中的8列，16为占据2页
void OLEDPrint8X16String(unsigned char x, unsigned char y, unsigned char data[])
{
	unsigned char i = 0, j = 0, n = 0;
	while (data[j] != '\0')
	{
		n = data[j] - 32;
		if(x > WIDTH)
		{
			x = 0;
			y++;
		}
		OLEDPos(x, y);
		for(i = 0; i < 8; i++)
		{
			OLEDWriteByte(0x01, ASC8X16[n * 16 + i]);
		}
		OLEDPos(x, y + 1);
		for(i = 0; i < 8; i++)
		{
			OLEDWriteByte(0x01, ASC8X16[n * 16 + i + 8]);
		}
		x += 8;
		j++;
	}
}
