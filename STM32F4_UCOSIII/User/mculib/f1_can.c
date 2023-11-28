#include "f1_can.h"

const S_CAN_BAUD_TAB can_baud_tab[]=//can波特率表
{
	1000, 1,3,2,6,
	900,  1,4,3,5,
	800,  1,5,3,5,
	600,  1,6,3,6,
	500,  1,3,2,12,
	400,  1,5,3,10,
	300,  1,3,2,20,
	250,  1,3,2,24,
	200,  1,3,2,30,
	150,  1,3,2,40,
	125,  1,3,2,48,
	100,  1,3,2,60,
	90,   1,4,3,50,
	80,   1,3,2,75,
	60,   1,6,3,60,
	50,   1,3,2,120,
	40,   1,3,2,150,
	30,   1,6,3,120,
	20,   1,3,2,300,
	10,   1,3,2,600,
	5,    2,6,4,600,
	3,    2,6,4,1000,
	2,    2,10,6,1000,
};
int get_can_baud_offset(u16 t) //输入波特率，得到偏移位置
{
	u16 i;
	for(i=0;i<sizeof(can_baud_tab)/sizeof(S_CAN_BAUD_TAB);i++)
	{
		if(can_baud_tab[i].baud==t)
		{
			return i;
		}
	}
	return -1;
}
//CAN初始化
//tsjw:重新同步跳跃时间单元.范围:1~3;
//tbs2:时间段2的时间单元.范围:1~8;
//tbs1:时间段1的时间单元.范围:1~16;
//brp :波特率分频器.范围:1~1024;(实际要加1,也就是1~1024) tq=(brp)*tpclk1
//注意以上参数任何一个都不能设为0,否则会乱.
//波特率=Fpclk1/((tbs1+tbs2+1)*brp);
//mode:0,普通模式;1,回环模式;
//Fpclk1的时钟在初始化的时候设置为36M,如果设置CAN_Normal_Init(1,8,9,4,1);
//则波特率为:36M/((8+9+1)*4)=500Kbps
//返回值:0,初始化OK;
//    其他,初始化失败;
int can1_mode_init(u8 tsjw,u8 tbs2,u8 tbs1,u16 brp)
{
	u8 mode=0;
	u16 i=0;
 	if(tsjw==0||tbs2==0||tbs1==0||brp==0)return 1;
	tsjw-=1;//先减去1.再用于设置
	tbs2-=1;
	tbs1-=1;
	brp-=1;

	RCC->APB2ENR|=1<<2;    	//使能PORTA时钟	 
	GPIOA->CRH&=0XFFF00FFF; 
	GPIOA->CRH|=0X000B8000;	//PA11 RX,PA12 TX推挽输出   	 
    GPIOA->ODR|=3<<11;
					    
	RCC->APB1ENR|=1<<25;	//使能CAN时钟 CAN使用的是APB1的时钟(max:36M)
	CAN1->MCR=0x0000;		//退出睡眠模式(同时设置所有位为0)
	CAN1->MCR|=1<<0;		//请求CAN进入初始化模式
	while((CAN1->MSR&1<<0)==0)
	{
		i++;
		if(i>100)return 2;	//进入初始化模式失败
	}
	CAN1->MCR|=0<<7;		//非时间触发通信模式
	CAN1->MCR|=1<<6;		//软件自动离线管理
	CAN1->MCR|=0<<5;		//睡眠模式通过软件唤醒(清除CAN1->MCR的SLEEP位)
	CAN1->MCR|=1<<4;		//禁止报文自动传送
	CAN1->MCR|=0<<3;		//报文不锁定,新的覆盖旧的
	CAN1->MCR|=1<<2;		//优先级由报文标识符决定
	CAN1->BTR=0x00000000;	//清除原来的设置.
	CAN1->BTR|=mode<<30;	//模式设置 0,普通模式;1,回环模式;
	CAN1->BTR|=tsjw<<24; 	//重新同步跳跃宽度(Tsjw)为tsjw+1个时间单位
	CAN1->BTR|=tbs2<<20; 	//Tbs2=tbs2+1个时间单位
	CAN1->BTR|=tbs1<<16;	//Tbs1=tbs1+1个时间单位
	CAN1->BTR|=brp<<0;  	//分频系数(Fdiv)为brp+1
							//波特率:Fpclk1/((Tbs1+Tbs2+1)*Fdiv)
	CAN1->MCR&=~(1<<0);		//请求CAN退出初始化模式
	while((CAN1->MSR&1<<0)==1)
	{
		i++;
		if(i>0XFFF0)return 3;//退出初始化模式失败
	}
	//过滤器初始化
	CAN1->FMR|=1<<0;		//过滤器组工作在初始化模式
	CAN1->FA1R&=~(1<<0);	//过滤器0不激活
	CAN1->FS1R|=1<<0; 		//过滤器位宽为32位.
	CAN1->FM1R|=0<<0;		//过滤器0工作在标识符屏蔽位模式
	CAN1->FFA1R|=0<<0;		//过滤器0关联到FIFO0
	CAN1->sFilterRegister[0].FR1=0X00000000;//32位ID
	CAN1->sFilterRegister[0].FR2=0X00000000;//32位MASK
	CAN1->FA1R|=1<<0;		//激活过滤器0
	CAN1->FMR&=0<<0;		//过滤器组进入正常模式

#if CAN_RX0_INT_ENABLE
 	//使用中断接收
	CAN1->IER|=1<<1;		//FIFO0消息挂号中断允许.	    
	MY_NVIC_Init(1,0,USB_LP_CAN1_RX0_IRQn,2);//组2
#endif
	return 0;
}   
//得到在FIFO0/FIFO1中接收到的报文个数.
//fifox:0/1.FIFO编号;
//返回值:FIFO0/FIFO1中的报文个数.
u8 CAN_Msg_Pend(u8 fifox)
{
	if(fifox==0)return CAN1->RF0R&0x03; 
	else if(fifox==1)return CAN1->RF1R&0x03; 
	else return 0;
}
#if CAN_RX0_INT_ENABLE	//使能RX0中断
//中断服务函数			    
void USB_LP_CAN1_RX0_IRQHandler(void)
{
}
#endif

/////////////////////////////////////////////////////////////////////
//收发接口
CAN_MSG can1_rx_buf; //接收缓存
int can1_rx(void) //接收，返回收到的数据数量
{
	u8 fifox=0;
	if(CAN_Msg_Pend(0)==0)return 0;	//没有接收到数据,直接退出
	can1_rx_buf.ext_std=CAN1->sFIFOMailBox[fifox].RIR&0x04?1:0;//得到标识符选择位的值
 	if(can1_rx_buf.ext_std==0)//标准标识符
	{
		can1_rx_buf.can_id=CAN1->sFIFOMailBox[fifox].RIR>>21;
	}
	else	   //扩展标识符
	{
		can1_rx_buf.can_id=CAN1->sFIFOMailBox[fifox].RIR>>3;
	}
	can1_rx_buf.rtr=CAN1->sFIFOMailBox[fifox].RIR&0x02?1:0;	//得到远程发送请求值.
	can1_rx_buf.len=CAN1->sFIFOMailBox[fifox].RDTR&0x0F;	//得到DLC
 	//*fmi=(CAN1->sFIFOMailBox[FIFONumber].RDTR>>8)&0xFF;//得到FMI
	//接收数据
	((u32*)(can1_rx_buf.data))[0]=CAN1->sFIFOMailBox[fifox].RDLR;
	((u32*)(can1_rx_buf.data))[1]=CAN1->sFIFOMailBox[fifox].RDHR;
  	if(fifox==0)CAN1->RF0R|=0X20;//释放FIFO0邮箱
	else if(fifox==1)CAN1->RF1R|=0X20;//释放FIFO1邮箱	 
	return can1_rx_buf.len;
}
int can1_can_send(void)
{
	u8 mbox;
	if(CAN1->TSR&(1<<26))mbox=0;		//邮箱0为空
	else if(CAN1->TSR&(1<<27))mbox=1;	//邮箱1为空
	else if(CAN1->TSR&(1<<28))mbox=2;	//邮箱2为空
	else return -1;					//无空邮箱,无法发送 
	return mbox;
}
int can1_tx(CAN_MSG* buf) //发送,返回0成功
{	
	s8 mbox;
	u32 tid=buf->can_id;
	mbox=can1_can_send();
	if(mbox<0) return 1;
	CAN1->sTxMailBox[mbox].TIR=0;		//清除之前的设置
	if(buf->ext_std==0)	//标准帧
	{
		tid&=0x7ff;//取低11位stdid
		tid<<=21;
	}else		//扩展帧
	{
		tid&=0X1FFFFFFF;//取低32位extid
		tid<<=3;
	}
	CAN1->sTxMailBox[mbox].TIR|=tid;
	CAN1->sTxMailBox[mbox].TIR|=buf->ext_std<<2;
	CAN1->sTxMailBox[mbox].TIR|=buf->rtr<<1;
	buf->len&=0X0F;//得到低四位
	CAN1->sTxMailBox[mbox].TDTR&=~(0X0000000F);
	CAN1->sTxMailBox[mbox].TDTR|=buf->len;	//设置DLC.
	//待发送数据存入邮箱.
	CAN1->sTxMailBox[mbox].TDLR=((u32*)(buf->data))[0];
	CAN1->sTxMailBox[mbox].TDHR=((u32*)(buf->data))[1];
	CAN1->sTxMailBox[mbox].TIR|=1<<0; 	//请求发送邮箱数据
	return 0;
}

