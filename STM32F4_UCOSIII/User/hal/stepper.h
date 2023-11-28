#ifndef STEPPER_H
#define STEPPER_H

#include "stm32f4xx.h"

void stepper_ini(TIM_TypeDef *pt1,u32 *pin1,u16 *ccr1);
void stepper_setv(TIM_TypeDef *pt1,float speed); //输入电机号，和速度，单位度每秒

#endif

