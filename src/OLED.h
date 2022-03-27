/**
 * OLED显示屏
 */

#ifndef OLED_H
#define OLED_H

#define WIDTH 128
#define HIGHT 64

extern void OLEDInit();
extern void OLEDClear();
extern void OLEDPrint6X8String(unsigned char x, unsigned char y, unsigned char data[]);
extern void OLEDPrint8X16String(unsigned char x, unsigned char y, unsigned char data[]);

#endif
