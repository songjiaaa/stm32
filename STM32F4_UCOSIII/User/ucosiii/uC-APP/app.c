/*
*********************************************************************************************************
*                                              EXAMPLE CODE
*
*                             (c) Copyright 2013; Micrium, Inc.; Weston, FL
*
*                   All rights reserved.  Protected by international copyright laws.
*                   Knowledge of the source code may not be used to write a similar
*                   product.  This file may only be used in accordance with a license
*                   and should not be redistributed in any way.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                            EXAMPLE CODE
*
*                                       IAR Development Kits
*                                              on the
*
*                                    STM32F429II-SK KICKSTART KIT
*
* Filename      : app.c
* Version       : V1.00
* Programmer(s) : YS
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#include  <includes.h>


/*
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*/

/* ----------------- APPLICATION GLOBALS -------------- */
static  OS_TCB   AppTaskStartTCB;
static  CPU_STK  AppTaskStartStk[APP_CFG_TASK_START_STK_SIZE];


#define TEST1_TASK_START_PRIO                           1u       
#define TEST1_TASK_START_STK_SIZE                      128        
static  CPU_STK  Test1TaskStartStk[TEST1_TASK_START_STK_SIZE];
static  OS_TCB   Test1TaskStartTCB;


static void Test1_Task(void *p_arg)
{
	OS_ERR      err;
	while(1)
	{
		//printf("Test1\r\n");
		OSTimeDly(50,OS_OPT_TIME_DLY,&err);
		PAout(7) ^=  1;
	}
}


#define TEST2_TASK_START_PRIO                           2u        
#define TEST2_TASK_START_STK_SIZE                      128        
static  CPU_STK  Test2TaskStartStk[TEST2_TASK_START_STK_SIZE];
static  OS_TCB   Test2TaskStartTCB;

static void Test2_Task(void *p_arg)
{
	OS_ERR      err;
	while(1)
	{
//		printf("Test2----------\r\n");
		OSTimeDly(500,OS_OPT_TIME_DLY,&err);
		PAout(6) ^=  1;
	}
}
/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static  void  AppTaskStart (void     *p_arg);

/*
*********************************************************************************************************
*                                                main()
*
* Description : This is the standard entry point for C code.  It is assumed that your code will call
*               main() once you have performed all necessary initialization.
*
* Arguments   : none
*
* Returns     : none
*********************************************************************************************************
*/




int main(void)
{
    OS_ERR  err;

    OSInit(&err);                                               /* Init uC/OS-III.                                      */

    OSTaskCreate((OS_TCB       *)&AppTaskStartTCB,              /* Create the start task                                */
                 (CPU_CHAR     *)"AppTaskStart",
                 (OS_TASK_PTR   )AppTaskStart,
                 (void         *)0u,
                 (OS_PRIO       )APP_CFG_TASK_START_PRIO,
                 (CPU_STK      *)&AppTaskStartStk[0u],
                 (CPU_STK_SIZE  )AppTaskStartStk[APP_CFG_TASK_START_STK_SIZE / 10u],
                 (CPU_STK_SIZE  )APP_CFG_TASK_START_STK_SIZE,
                 (OS_MSG_QTY    )0u,
                 (OS_TICK       )0u,
                 (void         *)0u,
                 (OS_OPT        )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR       *)&err);
	
    OSStart(&err);                                              /* Start multitasking (i.e. give control to uC/OS-III). */

    return (0u);
}


/*
*********************************************************************************************************
*                                          STARTUP TASK
*
* Description : This is an example of a startup task.  As mentioned in the book's text, you MUST
*               initialize the ticker only once multitasking has started.
*
* Arguments   : p_arg   is the argument passed to 'AppTaskStart()' by 'OSTaskCreate()'.
*
* Returns     : none
*
* Notes       : 1) The first line of code is used to prevent a compiler warning because 'p_arg' is not
*                  used.  The compiler should not generate any code for this statement.
*********************************************************************************************************
*/

static  void  AppTaskStart (void *p_arg)
{
    CPU_INT32U  cpu_clk_freq;
    CPU_INT32U  cnts;
    OS_ERR      err;


   (void)p_arg;
  
    BSP_Init();                                                 /* Initialize BSP functions                             */
    CPU_Init();                                                 /* Initialize the uC/CPU services                       */

    cpu_clk_freq = BSP_CPU_ClkFreq();                           /* Determine SysTick reference freq.                    */
    cnts         = cpu_clk_freq                                 /* Determine nbr SysTick increments                     */
                 / (CPU_INT32U)OSCfg_TickRate_Hz;

    OS_CPU_SysTickInit(cnts);                                   /* Init uC/OS periodic time src (SysTick).              */

    Mem_Init();                                                 /* Initialize memory managment module                   */
    Math_Init();                                                /* Initialize mathematical module                       */


#if OS_CFG_STAT_TASK_EN > 0u
    OSStatTaskCPUUsageInit(&err);                               /* Compute CPU capacity with no task running            */
#endif

    OSTaskCreate((OS_TCB       *)&Test1TaskStartTCB,              /* Create the start task                                */
                 (CPU_CHAR     *)"Test1_Task",
                 (OS_TASK_PTR   )Test1_Task,
                 (void         *)0u,
                 (OS_PRIO       )TEST1_TASK_START_PRIO,
                 (CPU_STK      *)&Test1TaskStartStk[0u],
                 (CPU_STK_SIZE  )Test1TaskStartStk[TEST1_TASK_START_STK_SIZE / 10u],
                 (CPU_STK_SIZE  )TEST1_TASK_START_STK_SIZE,
                 (OS_MSG_QTY    )0u,
                 (OS_TICK       )0u,
                 (void         *)0u,
                 (OS_OPT        )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR       *)&err);

				 
    OSTaskCreate((OS_TCB       *)&Test2TaskStartTCB,              /* Create the start task                                */
                 (CPU_CHAR     *)"Test2_Task",
                 (OS_TASK_PTR   )Test2_Task,
                 (void         *)0u,
                 (OS_PRIO       )TEST2_TASK_START_PRIO,
                 (CPU_STK      *)&Test2TaskStartStk[0u],
                 (CPU_STK_SIZE  )Test2TaskStartStk[TEST2_TASK_START_STK_SIZE / 10u],
                 (CPU_STK_SIZE  )TEST2_TASK_START_STK_SIZE,
                 (OS_MSG_QTY    )0u,
                 (OS_TICK       )0u,
                 (void         *)0u,
                 (OS_OPT        )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR       *)&err);		

		OSTaskDel(&AppTaskStartTCB,&err);	 
}






