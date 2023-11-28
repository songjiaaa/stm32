
#include "f1_led.h"

//S_LED_CTRL led_list[]=
//{//端口,引脚0~15,亮电平, 亮时间，周期，tick
	//GPIOB,13,1,  1,0,  0, //485
	//GPIOB,14,1,  1,0,  0, //can
//};
S_LED_CTRL *pled_list=0;
u8 led_len=0; //LED列表长度
void led_ini(S_LED_CTRL *pled,u8 n)
{
	int i;
	pled_list=pled;
	led_len=n;
	for(i = 0; i < led_len; i++)
	{
		if(pled_list[i].pin<8)
		{
			pled_list[i].port->CRL &= ~(0x0f<<pled_list[i].pin*4);
			pled_list[i].port->CRL |= (GPIO_OUT_PP<<pled_list[i].pin*4);
		}
		else if(pled_list[i].pin<16)
		{
			u8 pin=pled_list[i].pin-8;
			pin*=4;
			pled_list[i].port->CRH &= ~(0x0f<<pin);
			pled_list[i].port->CRH |= (GPIO_OUT_PP<<pin);
		}
		//初始化为不亮
		pled_list[i].port->BSRR=(1<<pled_list[i].pin)<<(16*pled_list[i].light_lev);
	}
}
void led_toggle(int offset) //对于非周期的，触发led
{
	if(offset<led_len)
	{
		pled_list[offset].tick=0;
	}
}
void led_period(int offset,int t) //设置led周期，单位tick，若为0则为关闭
{
	if(offset<led_len)
	{
		pled_list[offset].T=t;
		if(t==0)
		{
			pled_list[offset].tick=pled_list[offset].duty;
		}
	}
}
void led_tick(void)
{
	int i;
	for(i = 0; i < led_len; i++)
	{
		if(pled_list[i].tick<pled_list[i].duty)//若需要亮
		{
			pled_list[i].port->BSRR=(1<<pled_list[i].pin)<<(16*(1-pled_list[i].light_lev));
			pled_list[i].tick++;
		}
		else //若需要灭
		{
			//是否归零？
			if(pled_list[i].T>=pled_list[i].duty) //若不是触发型
			{
				if(pled_list[i].tick<pled_list[i].T)
				{
					pled_list[i].tick++;
				}
				else
				{
					pled_list[i].tick=0;
				}
			}
			pled_list[i].port->BSRR=(1<<pled_list[i].pin)<<(16*pled_list[i].light_lev);
		}
	}
}
