#include "com_fun.h"

u8 check_sum(u8 *p,int n)
{
	u8 acc=0;
	int i;
	for(i = 0; i < n; i++)
	{
		acc+=p[i];
	}
	return acc;
}

//CRC(16位) = X16+X15+X2+1 　　CRC(CCITT) = X16+X12+X5+1
//计算CRC16，0x11021
static const u16 crc_ta[16]=	// CRC 余式表
{
	0x0000,0x1021,0x2042,0x3063,0x4084,0x50a5,0x60c6,0x70e7,
	0x8108,0x9129,0xa14a,0xb16b,0xc18c,0xd1ad,0xe1ce,0xf1ef,
};
u16 cal_crc(u8 *ptr,u32 len)
{
	u16 crc;
	u8 da;
	crc=0;
	while(len--!=0)
	{
		da=((u8)(crc/256))/16;      //暂存 CRC 的高四位
		crc<<=4;                   // CRC 右移 4 位，相当于取 CRC 的低 12 位）
		crc^=crc_ta[da^(*ptr/16)]; // CRC 的高 4 位和本字节的前半字节相加后查表计算 CRC，
		//然后加上上一次 CRC 的余数
		da=((u8)(crc/256))/16;  // 暂存 CRC 的高 4 位
		crc<<=4;                  //CRC 右移 4 位， 相当于 CRC 的低 12 位）
		crc^=crc_ta[da^(*ptr&0x0f)];// CRC 的高 4 位和本字节的后半字节相加后查表计算
		//CRC，然后再加上上一次 CRC 的余数
		ptr++;
	}
	return(crc);
}

