#ifndef STM32F3_SYS_H
#define STM32F3_SYS_H	  

#include "stm32f373xc.h"
#include "main.h"

#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2)) 
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr)) 
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum)) 
//STM32F3的GPIO没有在bit band中，无法实现此操作
//IO口地址映射
//#define GPIOA_ODR_Addr    (GPIOA_BASE+20) //0x40020014
//#define GPIOB_ODR_Addr    (GPIOB_BASE+20) //0x48000400 + 0x14 
//#define GPIOC_ODR_Addr    (GPIOC_BASE+20) //0x40020814 
//#define GPIOD_ODR_Addr    (GPIOD_BASE+20) //0x40020C14 
//#define GPIOE_ODR_Addr    (GPIOE_BASE+20) //0x40021014 
//#define GPIOF_ODR_Addr    (GPIOF_BASE+20) //0x40021414    
//#define GPIOG_ODR_Addr    (GPIOG_BASE+20) //0x40021814   
//#define GPIOH_ODR_Addr    (GPIOH_BASE+20) //0x40021C14    
//#define GPIOI_ODR_Addr    (GPIOI_BASE+20) //0x40022014     
//
//#define GPIOA_IDR_Addr    (GPIOA_BASE+16) //0x40020010 
//#define GPIOB_IDR_Addr    (GPIOB_BASE+16) //0x40020410 
//#define GPIOC_IDR_Addr    (GPIOC_BASE+16) //0x40020810 
//#define GPIOD_IDR_Addr    (GPIOD_BASE+16) //0x40020C10 
//#define GPIOE_IDR_Addr    (GPIOE_BASE+16) //0x40021010 
//#define GPIOF_IDR_Addr    (GPIOF_BASE+16) //0x40021410 
//#define GPIOG_IDR_Addr    (GPIOG_BASE+16) //0x40021810 
//#define GPIOH_IDR_Addr    (GPIOH_BASE+16) //0x40021C10 
//#define GPIOI_IDR_Addr    (GPIOI_BASE+16) //0x40022010 
 
//IO口操作,只对单一的IO口!
//确保n的值小于16!
//#define PAout(n)   BIT_ADDR(GPIOA_ODR_Addr,n)  //输出 
//#define PAin(n)    BIT_ADDR(GPIOA_IDR_Addr,n)  //输入 
//
//#define PBout(n)   BIT_ADDR(GPIOB_ODR_Addr,n)  //输出 
//#define PBin(n)    BIT_ADDR(GPIOB_IDR_Addr,n)  //输入 
//
//#define PCout(n)   BIT_ADDR(GPIOC_ODR_Addr,n)  //输出 
//#define PCin(n)    BIT_ADDR(GPIOC_IDR_Addr,n)  //输入 
//
//#define PDout(n)   BIT_ADDR(GPIOD_ODR_Addr,n)  //输出 
//#define PDin(n)    BIT_ADDR(GPIOD_IDR_Addr,n)  //输入 
//
//#define PEout(n)   BIT_ADDR(GPIOE_ODR_Addr,n)  //输出 
//#define PEin(n)    BIT_ADDR(GPIOE_IDR_Addr,n)  //输入
//
//#define PFout(n)   BIT_ADDR(GPIOF_ODR_Addr,n)  //输出 
//#define PFin(n)    BIT_ADDR(GPIOF_IDR_Addr,n)  //输入

/////////////////////////////////////////////////////////////////
//RCC
#define RCC_AHBPeriph_GPIOA               RCC_AHBENR_GPIOAEN
#define RCC_AHBPeriph_GPIOB               RCC_AHBENR_GPIOBEN
#define RCC_AHBPeriph_GPIOC               RCC_AHBENR_GPIOCEN
#define RCC_AHBPeriph_GPIOD               RCC_AHBENR_GPIODEN
#define RCC_AHBPeriph_GPIOE               RCC_AHBENR_GPIOEEN
#define RCC_AHBPeriph_GPIOF               RCC_AHBENR_GPIOFEN
#define RCC_AHBPeriph_TS                  RCC_AHBENR_TSEN
#define RCC_AHBPeriph_CRC                 RCC_AHBENR_CRCEN
#define RCC_AHBPeriph_FLITF               RCC_AHBENR_FLITFEN
#define RCC_AHBPeriph_SRAM                RCC_AHBENR_SRAMEN
#define RCC_AHBPeriph_DMA2                RCC_AHBENR_DMA2EN
#define RCC_AHBPeriph_DMA1                RCC_AHBENR_DMA1EN
 
#define RCC_APB2Periph_SYSCFG            RCC_APB2ENR_SYSCFGEN
#define RCC_APB2Periph_ADC1              RCC_APB2ENR_ADC1EN
#define RCC_APB2Periph_SPI1              RCC_APB2ENR_SPI1EN
#define RCC_APB2Periph_USART1            RCC_APB2ENR_USART1EN
#define RCC_APB2Periph_TIM15             RCC_APB2ENR_TIM15EN
#define RCC_APB2Periph_TIM16             RCC_APB2ENR_TIM16EN
#define RCC_APB2Periph_TIM17             RCC_APB2ENR_TIM17EN
#define RCC_APB2Periph_TIM19             RCC_APB2ENR_TIM19EN
#define RCC_APB2Periph_SDADC1            RCC_APB2ENR_SDADC1EN
#define RCC_APB2Periph_SDADC2            RCC_APB2ENR_SDADC2EN
#define RCC_APB2Periph_SDADC3            RCC_APB2ENR_SDADC3EN

#define RCC_APB1Periph_TIM2              RCC_APB1ENR_TIM2EN
#define RCC_APB1Periph_TIM3              RCC_APB1ENR_TIM3EN
#define RCC_APB1Periph_TIM4              RCC_APB1ENR_TIM4EN
#define RCC_APB1Periph_TIM5              RCC_APB1ENR_TIM5EN
#define RCC_APB1Periph_TIM6              RCC_APB1ENR_TIM6EN
#define RCC_APB1Periph_TIM7              RCC_APB1ENR_TIM7EN
#define RCC_APB1Periph_TIM12             RCC_APB1ENR_TIM12EN
#define RCC_APB1Periph_TIM13             RCC_APB1ENR_TIM13EN
#define RCC_APB1Periph_TIM14             RCC_APB1ENR_TIM14EN
#define RCC_APB1Periph_TIM18             RCC_APB1ENR_TIM18EN
#define RCC_APB1Periph_WWDG              RCC_APB1ENR_WWDGEN
#define RCC_APB1Periph_SPI2              RCC_APB1ENR_SPI2EN
#define RCC_APB1Periph_SPI3              RCC_APB1ENR_SPI3EN
#define RCC_APB1Periph_USART2            RCC_APB1ENR_USART2EN
#define RCC_APB1Periph_USART3            RCC_APB1ENR_USART3EN
#define RCC_APB1Periph_I2C1              RCC_APB1ENR_I2C1EN
#define RCC_APB1Periph_I2C2              RCC_APB1ENR_I2C2EN
#define RCC_APB1Periph_USB               RCC_APB1ENR_USBEN
#define RCC_APB1Periph_CAN              RCC_APB1ENR_CANEN
#define RCC_APB1Periph_DAC2              RCC_APB1ENR_DAC2EN
#define RCC_APB1Periph_PWR               RCC_APB1ENR_PWREN
#define RCC_APB1Periph_DAC1              RCC_APB1ENR_DAC1EN
#define RCC_APB1Periph_CEC               RCC_APB1ENR_CECEN

#define RCC_MCOSource_NoClock            ((uint8_t)0x00)
#define RCC_MCOSource_LSI                ((uint8_t)0x02)
#define RCC_MCOSource_LSE                ((uint8_t)0x03)
#define RCC_MCOSource_SYSCLK             ((uint8_t)0x04)
#define RCC_MCOSource_HSI                ((uint8_t)0x05)
#define RCC_MCOSource_HSE                ((uint8_t)0x06)
#define RCC_MCOSource_PLLCLK_Div2        ((uint8_t)0x07)

/////////////////////////////////////////////////////////////////
//Ex_NVIC_Config专用定义
#define GPIO_A 0
#define GPIO_B 1
#define GPIO_C 2
#define GPIO_D 3
#define GPIO_E 4
#define GPIO_F 5
#define GPIO_G 6 
#define GPIO_H 				7 
#define GPIO_I 				8 
#define FTIR   1  //下降沿触发
#define RTIR   2  //上升沿触发

//GPIO设置专用宏定义
#define GPIO_MODE_IN    	0		//普通输入模式
#define GPIO_MODE_OUT		1		//普通输出模式
#define GPIO_MODE_AF		2		//AF功能模式
#define GPIO_MODE_AIN		3		//模拟输入模式

#define GPIO_SPEED_2M		0		//GPIO速度2Mhz
#define GPIO_SPEED_25M		1		//GPIO速度25Mhz
#define GPIO_SPEED_50M		2		//GPIO速度50Mhz
#define GPIO_SPEED_100M		3		//GPIO速度100Mhz

#define GPIO_PUPD_NONE		0		//不带上下拉
#define GPIO_PUPD_PU		1		//上拉
#define GPIO_PUPD_PD		2		//下拉
#define GPIO_PUPD_RES		3		//保留 

#define GPIO_OTYPE_PP		0		//推挽输出
#define GPIO_OTYPE_OD		1		//开漏输出 

//GPIO引脚编号定义
#define PIN0				(1<<0)
#define PIN1				(1<<1)
#define PIN2				(1<<2)
#define PIN3				(1<<3)
#define PIN4				(1<<4)
#define PIN5				(1<<5)
#define PIN6				(1<<6)
#define PIN7				(1<<7)
#define PIN8				(1<<8)
#define PIN9				(1<<9)
#define PIN10				(1<<10)
#define PIN11				(1<<11)
#define PIN12				(1<<12)
#define PIN13				(1<<13)
#define PIN14				(1<<14)
#define PIN15				(1<<15)

//GPIO
#define PAi      GPIOA->IDR
#define PAo      GPIOA->ODR
#define PBi      GPIOB->IDR
#define PBo      GPIOB->ODR
#define PCi      GPIOC->IDR
#define PCo      GPIOC->ODR
#define PDi      GPIOD->IDR
#define PDo      GPIOD->ODR
#define PEi      GPIOE->IDR
#define PEo      GPIOE->ODR
#define PFi      GPIOF->IDR
#define PFo      GPIOF->ODR

#define GPIO_AF_0  ((u8)0x00) // RTC_REFIN,MCO,SWDIO-JTMS,SWCLK-JTCK,JTDI
#define GPIO_AF_1  ((u8)0x01) // TIM2, TIM16, TIM17
#define GPIO_AF_2  ((u8)0x02) // TIM3,  TIM5, TIM13,
#define GPIO_AF_3  ((u8)0x03) // Touch
#define GPIO_AF_4  ((u8)0x04) // I2C1, I2C2 
#define GPIO_AF_5  ((u8)0x05) // SPI1, SPI2, IR_OUT
#define GPIO_AF_6  ((u8)0x06) // SPI1, SPI3
#define GPIO_AF_7  ((u8)0x07) // USART1, USART2, USART3,CEC
#define GPIO_AF_8  ((u8)0x08) // COMP1_OUT, COMP2_OUT
#define GPIO_AF_9  ((u8)0x09) // CAN, TIM13, TIM14, TIM15
#define GPIO_AF_10  ((u8)0x0A) // TIM2, TIM4, TIM12
#define GPIO_AF_11  ((u8)0x0B) // TIM19
#define GPIO_AF_14  ((u8)0x0E) //
#define GPIO_AF_15  ((u8)0x0F) // OUT

/////////////////////////////////////////////////////////////////  
void Stm32_Clock_Init(u8 PLL);  //时钟初始化  
void MY_NVIC_SetVectorTable(u32 NVIC_VectTab, u32 Offset);//设置偏移地址
void MY_NVIC_PriorityGroupConfig(u8 NVIC_Group);//设置NVIC分组
void MY_NVIC_Init(u8 NVIC_PreemptionPriority,u8 NVIC_SubPriority,u8 NVIC_Channel,u8 NVIC_Group);//设置中断
void Ex_NVIC_Config(u8 GPIOx,u8 BITx,u8 TRIM);//外部中断配置函数(只对GPIOA~G)
void GPIO_AF_Set(GPIO_TypeDef* GPIOx,u8 BITx,u8 AFx);		//GPIO复用功能设置
void GPIO_Set(GPIO_TypeDef* GPIOx,u32 BITx,u32 MODE,u32 OTYPE,u32 OSPEED,u32 PUPD);//GPIO设置函数  
//////////////////////////////////////////////////////////////////////////////
//以下为汇编函数
void WFI_SET(void);		//执行WFI指令
#define OS_OPEN_INT		__ASM volatile("cpsie i")//开启所有中断
#define OS_CLOSE_INT	__ASM volatile("cpsid i")//关闭所有中断
void MSR_MSP(u32 addr);	//设置堆栈地址
//////////////////////////////////////////////////////////////////////////////

//系统变量
extern u32 SYS_task;
extern u16 SYS_idle;
extern u16 SYS_idle_dis;
extern u32 SYS_time;

#define START_TASK(x)       SYS_task |= x

#define TASK_POOL_10			1	//周期任务
#define TASK_POOL_100			2	//周期任务

//#define TASK_                 //必须是2的幂次
void wait(vu32 i);
int void_putchar(int c);
extern int (*IO_putchar)(int);	//初始化的函数

#endif

