#include "f1_io.h"

const S_IO_PORT *piopin;
u16 io_list_n=0; //IO列表长度 

void io_ini(const S_IO_PORT *pio,u16 n) //初始化IO,先都初始化成输入
{
	int i;
	piopin=pio;
	io_list_n=n;
	for(i = 0; i < io_list_n; i++)
	{
		io_ini_n(i,0);
	}
}
void io_ini_n(u8 n,u8 mod) //初始化IO,0输入1输出
{
	if(n< io_list_n)
	{
		u32 mode=mod==0?GPIO_IN_PP:GPIO_OUT_PP;
		switch(piopin[n].pin)
		{
		case 0: ((MGPIO_TypeDef*)piopin[n].port)->CT0=mode; break;
		case 1: ((MGPIO_TypeDef*)piopin[n].port)->CT1=mode; break;
		case 2: ((MGPIO_TypeDef*)piopin[n].port)->CT2=mode; break;
		case 3: ((MGPIO_TypeDef*)piopin[n].port)->CT3=mode; break;
		case 4: ((MGPIO_TypeDef*)piopin[n].port)->CT4=mode; break;
		case 5: ((MGPIO_TypeDef*)piopin[n].port)->CT5=mode; break;
		case 6: ((MGPIO_TypeDef*)piopin[n].port)->CT6=mode; break;
		case 7: ((MGPIO_TypeDef*)piopin[n].port)->CT7=mode; break;
		case 8: ((MGPIO_TypeDef*)piopin[n].port)->CT8=mode; break;
		case 9: ((MGPIO_TypeDef*)piopin[n].port)->CT9=mode; break;
		case 10: ((MGPIO_TypeDef*)piopin[n].port)->CT10=mode; break;
		case 11: ((MGPIO_TypeDef*)piopin[n].port)->CT11=mode; break;
		case 12: ((MGPIO_TypeDef*)piopin[n].port)->CT12=mode; break;
		case 13: ((MGPIO_TypeDef*)piopin[n].port)->CT13=mode; break;
		case 14: ((MGPIO_TypeDef*)piopin[n].port)->CT14=mode; break;
		case 15: ((MGPIO_TypeDef*)piopin[n].port)->CT15=mode; break;
		default: break;
		}
	}
}
u32 io_stat=0;
void io_get(void) //获得各引脚的状态
{
	int i;
	u32 r=0;
	for(i = 0; i < io_list_n; i++)
	{
		if(IO_IN(i))
		{
			r |= (1<<i);
		}
	}
	io_stat=r;
}

