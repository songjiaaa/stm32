#include "step_pwm.h"
#include "stm32f4xx.h"
#include "stm32f4_sys.h" 

void step_io_init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOC|RCC_AHB1Periph_GPIOD, ENABLE);

	//IMD1-PCout(10) IMD2-PCout(11) IMD3-PCout(12)
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;    //��ͨ���ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;   //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;     //����
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	PCout(10)=0;PCout(11)=0;PCout(12)=0;             //����
	PCout(7)=0;                 //����
	//S_EN-PDout(2)                                 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;        //���ʹ������
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;     //����
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	PDout(2)=0;	
}

//	step_pwm(999,179);  		//�������pwm
//	TIM3_PWM_Init(999,179);     //�������pwm
void step_pwm(u16 arr,u16 psc)
{
//	����:Tout= ((arr+1)*(psc+1))/Tclk��Ƶ�ʣ�Tclk/((arr+1)*(psc+1))  Tclk = 84Mhz
//	RCC->AHB1ENR|=0x7ff;        	
	GPIO_InitTypeDef  GPIO_InitStructure;
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);	

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;       // pwm 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;     //����
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource8,GPIO_AF_TIM3);  //����PC8��TIM3����
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);   

	TIM3->CR1=0x0081;		//����Ƶ��ARR(auto reload register)����,���ϼ���,ʹ�ܼ����� tDTS = tCK_INT
	TIM3->CR2=0;
	TIM3->SMCR=0;
	TIM3->DIER=0;		    //DMA���жϲ�����
	TIM3->CCMR1=0;		
	TIM3->CCMR2=0x0068;     //PWMģʽ3������Ԥװ��
	TIM3->CCER=0x0100;		//3ͨ��������� 0x0100
	TIM3->PSC=psc;	 		//Ԥ��Ƶ  36MHz
	TIM3->ARR=arr;		    //�Զ�װ�ؼĴ���
	TIM3->CCR1=0;		    //�Ƚϵ�ֵ
	TIM3->CCR2=0;		    //�Ƚϵ�ֵ
	TIM3->CCR3=(arr+1)/2;		    //�Ƚϵ�ֵ  �������α�����
	TIM3->CCR4=0;		    //�Ƚϵ�ֵ
	TIM3->EGR=1;		    //UG��һ
}

//arr���Զ���װֵ
//psc��ʱ��Ԥ��Ƶ��
void TIM3_PWM_Init(u32 arr,u32 psc)
{		 					 
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);  	  
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;          
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;        
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;        
	GPIO_Init(GPIOC,&GPIO_InitStructure);              
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource8,GPIO_AF_TIM3);
	
	TIM_TimeBaseStructure.TIM_Period = arr;
	TIM_TimeBaseStructure.TIM_Prescaler = psc;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseStructure);//
	
	//��ʼ��PWMģʽ	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = (arr+1)/2;                 //ռ�ձ�
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	
	TIM_OC3Init(TIM3, &TIM_OCInitStructure);
	TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);
	TIM_ARRPreloadConfig(TIM3, ENABLE);

	TIM_Cmd(TIM3, ENABLE);  								  
}  

//���Ӷ�ʱ�� Tclk = 164Mhz
//��ʱ��8��ģʽ��ע���������1
void TIM8_config(u32 Cycle)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8,ENABLE); 
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;          
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;        
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;        
	GPIO_Init(GPIOC,&GPIO_InitStructure);              
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource8,GPIO_AF_TIM8); 


    TIM_TimeBaseStructure.TIM_Period = Cycle-1;                                                   
    TIM_TimeBaseStructure.TIM_Prescaler =168-1;                    //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ                                                     
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;     //����ʱ�ӷָTDTS= Tck_tim            
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //TIM���ϼ���ģʽ
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;            //�ظ�������һ��Ҫ=0
    TIM_TimeBaseInit(TIM8, &TIM_TimeBaseStructure);                                       

    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;          //ѡ��ʱ��ģʽ��TIM�����ȵ���ģʽ1       
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ��
    TIM_OCInitStructure.TIM_Pulse = Cycle/2;                    //���ô�װ�벶��Ĵ���������ֵ                                   
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;      //�������       

    TIM_OC3Init(TIM8, &TIM_OCInitStructure);                                                         

    TIM_SelectMasterSlaveMode(TIM8, TIM_MasterSlaveMode_Enable);
    TIM_SelectOutputTrigger(TIM8, TIM_TRGOSource_Update);
    

    TIM_OC3PreloadConfig(TIM8, TIM_OCPreload_Enable);                              
    TIM_ARRPreloadConfig(TIM8, ENABLE);
	TIM_CtrlPWMOutputs(TIM8, ENABLE);   //�߼���ʱ��һ��Ҫ���ϣ������ʹ��	
}

/***��ʱ��4��ģʽ������Ҫ����1***/
void TIM4_config(u32 PulseNum)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure; 
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

    TIM_TimeBaseStructure.TIM_Period = PulseNum-1;   
    TIM_TimeBaseStructure.TIM_Prescaler =0;    
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;     
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);  

    TIM_SelectInputTrigger(TIM4, TIM_TS_ITR0);
    //TIM_InternalClockConfig(TIM3);
    TIM4->SMCR|=0x07;                                  //���ô�ģʽ�Ĵ��� 
    TIM_ITRxExternalClockConfig(TIM4, TIM_TS_ITR3);

    //TIM_ARRPreloadConfig(TIM3, ENABLE);         
    TIM_ITConfig(TIM4,TIM_IT_Update,DISABLE);

    NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;        
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;     
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
    NVIC_Init(&NVIC_InitStructure);
}



void Pulse_output(u32 Cycle,u32 PulseNum)
{
	//arr = Tclk/psc/Cycle
	u32 temp_hz = 168000000/(Cycle*168);
	TIM4->ARR = PulseNum;
    TIM_Cmd(TIM4, ENABLE);
    TIM_ClearITPendingBit(TIM4,TIM_IT_Update);
    TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE);
//    TIM8->ARR = temp_hz;
	TIM8_config(temp_hz);
    TIM_Cmd(TIM8, ENABLE);
}


void TIM4_IRQHandler(void) 
{ 
    if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)     // TIM_IT_CC1
    { 
        TIM_ClearITPendingBit(TIM4, TIM_IT_Update); // ����жϱ�־λ 
//        TIM_CtrlPWMOutputs(TIM8, DISABLE);  //�����ʹ��
        TIM_Cmd(TIM8, DISABLE); 
        TIM_Cmd(TIM4, DISABLE); 
        TIM_ITConfig(TIM4, TIM_IT_Update, DISABLE); 
    } 
} 

