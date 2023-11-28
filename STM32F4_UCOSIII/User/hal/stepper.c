#include "stepper.h"
 
TIM_TypeDef *timer_array[]={TIM1,TIM3};
u32 *pin_front_dir=0;
u32 *pin_back_dir=0;
u16 *ccr_front=0; 
u16 *ccr_back=0;

void stepper_ini(TIM_TypeDef *pt1,u32 *pin1,u16 *ccr1)
{
	timer_array[0]=pt1;
	pin_front_dir=pin1;
	ccr_front=ccr1;
}

void stepper_setv(TIM_TypeDef *pt1,float speed) //输入电机号，和速度，单位度每秒
{
	float out=speed;
	u16 *pccr=ccr_front;

	out/=2;
	if(out<0)
	{
		*pin_front_dir=1;
		out=-out;
	}
	else
	{
		*pin_front_dir=0;
	}
	
	if(out<1)
	{
		*pccr=0;
	}
	else
	{
		*pccr=pt1->ARR/2;
//		*pccr=out;
	}
}

