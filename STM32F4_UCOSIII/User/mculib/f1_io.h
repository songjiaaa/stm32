/*
io.h
设置IO
*/
#ifndef F1_IO_H
#define F1_IO_H

#include "stm32f1_sys.h"

typedef struct
{
	GPIO_TypeDef *port;
	u8 pin;
} S_IO_PORT;

#define IO_IN(x)	BIT_ADDR(((u32)(piopin[x].port)+8),piopin[x].pin)
#define IO_OUT(x)	BIT_ADDR(((u32)(piopin[x].port)+12),piopin[x].pin)

void io_ini(const S_IO_PORT *pio,u16 n); //初始化IO,先都初始化成输入
void io_ini_n(u8 n,u8 mod); //初始化IO,0输入1输出
void io_get(void);

extern const S_IO_PORT *piopin;
extern u32 io_stat;

#endif

