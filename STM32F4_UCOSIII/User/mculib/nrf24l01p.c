/*
文件名：nrf24l01p.c
时间：2011-6-11
功能：

*/

#include "nrf24l01p.h"
#include "spi_rf.h"

u8 (*RF1_spi_rw)(u8 a);
void (*RF1_spi_cs)(u8 a);
void (*RF1_spi_ce)(u8 a);

int RF1_inied=0; //是否初始化成功

#define RX_DR	(1<<6)
#define TX_DS	(1<<5)
#define MAX_RT	(1<<4)

#define TX_ADR_WIDTH    3   	// 5 u16s TX address width
#define RX_ADR_WIDTH    3   	// 5 u16s RX address width
#define TX_PLOAD_WIDTH  32  	// 20 u16s TX payload
#define RX_PLOAD_WIDTH  32  	// 20 u16s TX payload
u8 const TX_ADDRESS[TX_ADR_WIDTH]= {0,1,2};	//本地地址
u8 const RX_ADDRESS[RX_ADR_WIDTH]= {0,1,2};	//接收地址
//******************NRF24L01寄存器指令*****************************
#define NRF2401_READ_REG        0x00  	// 读寄存器指令
#define NRF2401_WRITE_REG       0x20 	// 写寄存器指令
#define RD_RX_PLOAD     0x61  	// 读取接收数据指令
#define WR_TX_PLOAD     0xa0  	// 写待发数据指令
#define FLUSH_TX        0xE1 	// 冲洗发送 FIFO指令
#define FLUSH_RX        0xE2  	// 冲洗接收 FIFO指令
#define REUSE_TX_PL     0xE3  	// 定义重复装载数据指令
#define NOP             0xFF  	// 保留
//******************SPI(nRF24L01)寄存器地址************************************
#define CONFIG          0x00  // 配置收发状态，CRC校验模式以及收发状态响应方式
#define EN_AA           0x01  // 自动应答功能设置
#define EN_RXADDR       0x02  // 可用信道设置
#define SETUP_AW        0x03  // 收发地址宽度设置
#define SETUP_RETR      0x04  // 自动重发功能设置
#define RF_CH           0x05  // 工作频率设置
#define RF_SETUP        0x06  // 发射速率、功耗功能设置
#define STATUS          0x07  // 状态寄存器
#define OBSERVE_TX      0x08  // 发送监测功能
#define CD              0x09  // 地址检测
#define RX_ADDR_P0      0x0A  // 频道0接收数据地址
#define RX_ADDR_P1      0x0B  // 频道1接收数据地址
#define RX_ADDR_P2      0x0C  // 频道2接收数据地址
#define RX_ADDR_P3      0x0D  // 频道3接收数据地址
#define RX_ADDR_P4      0x0E  // 频道4接收数据地址
#define RX_ADDR_P5      0x0F  // 频道5接收数据地址
#define TX_ADDR         0x10  // 发送地址寄存器
#define RX_PW_P0        0x11  // 接收频道0接收数据长度
#define RX_PW_P1        0x12  // 接收频道0接收数据长度
#define RX_PW_P2        0x13  // 接收频道0接收数据长度
#define RX_PW_P3        0x14  // 接收频道0接收数据长度
#define RX_PW_P4        0x15  // 接收频道0接收数据长度
#define RX_PW_P5        0x16  // 接收频道0接收数据长度
#define FIFO_STATUS     0x17  // FIFO栈入栈出状态寄存器设置
////////////////////////////////////////////////////////////////////////
//				NRF24L01基础读写
////////////////////////////////////////////////////////////////////////
//读单个寄存器
u8 RF1_r_reg(u8 reg) //输入要读的寄存器
{
	u8 tmp;
	RF1_spi_cs(0);                // CSN low, initialize SPI communication...
	RF1_spi_rw(reg);            // Select register to read from..
	tmp = RF1_spi_rw(0);    // ..then read registervalue
	RF1_spi_cs(1);                // CSN high, terminate SPI communication
	return tmp;        // return register value
}
//写单寄存器
u8 RF1_w_reg(u8 reg, u8 v)
{
	u8 status;
	RF1_spi_cs(0);                   // CSN low, init SPI transaction
	status = RF1_spi_rw(reg | NRF2401_WRITE_REG);      // select register
	RF1_spi_rw(v);             // ..and write value to it..
	RF1_spi_cs(1);                   // CSN high again
	return(status);            // return nRF24L01 status uchar
}
u8 RF1_r_buf(u8 reg, u8 *p, u8 n)
{
	u8 status,i;
	RF1_spi_cs(0);
	status = RF1_spi_rw(reg);
	for(i=0;i<n;i++)
	{
		p[i] = RF1_spi_rw(0);
	}
	RF1_spi_cs(1);
	return(status);
}
u8 RF1_w_buf(u8 reg, u8 *p, u8 n)
{
	u8 status,i;
	RF1_spi_cs(0);
	status = RF1_spi_rw(reg | NRF2401_WRITE_REG);
	for(i=0; i<n; i++) //
	{
		RF1_spi_rw(*p++);
	}
	RF1_spi_cs(1);
	return(status);
}
//void EXTI1_IRQHandler(void)
//{
	//EXTI->PR=EXTI_Line1;	//清中断标志位
//}
////////////////////////////////////////////////////////////////////////
//				NRF24L01初始化和接口函数
////////////////////////////////////////////////////////////////////////
void print_rf_regs(void)
{
	u8 t=0;
	u8 cbuf[32]={0};
	t=RF1_r_reg(0); printf("00:	config:		%02X\r\n",t);
	t=RF1_r_reg(1); printf("01:	EN_AA:		%02X\r\n",t);
	t=RF1_r_reg(2); printf("02:	EN_RXADDR:	%02X\r\n",t);
	t=RF1_r_reg(3); printf("03:	SETUP_AW:	%02X\r\n",t);
	t=RF1_r_reg(4); printf("04:	SETUP_RETR:	%02X\r\n",t);
	t=RF1_r_reg(5); printf("05:	RF_CH:		%02X\r\n",t);
	t=RF1_r_reg(6); printf("06:	RF_SETUP:	%02X\r\n",t);
	t=RF1_r_reg(7); printf("07:	STATUS:		%02X\r\n",t);
	t=RF1_r_reg(9); printf("09:	CD:			%02X\r\n",t);
	RF1_r_buf(0x0a,cbuf,5); printf("0x0a:RX_ADDR_P0:	%02X,%02X,%02X,%02X,%02X\r\n",cbuf[0],cbuf[1],cbuf[2],cbuf[3],cbuf[4]);
	RF1_r_buf(0x0b,cbuf,5); printf("0x0b:RX_ADDR_P1:	%02X,%02X,%02X,%02X,%02X\r\n",cbuf[0],cbuf[1],cbuf[2],cbuf[3],cbuf[4]);
	RF1_r_buf(0x10,cbuf,5); printf("0x10:TX_ADDR:	%02X,%02X,%02X,%02X,%02X\r\n",cbuf[0],cbuf[1],cbuf[2],cbuf[3],cbuf[4]);
	t=RF1_r_reg(0x11); printf("0x11:	RX_PW_P0:		%02X\r\n",t);
	t=RF1_r_reg(0x12); printf("0x12:	RX_PW_P1:		%02X\r\n",t);
	t=RF1_r_reg(0x17); printf("0x17:	FIFO_STATUS:	%02X\r\n",t);
}
void RF1_ini(void)
{
	u8 t,i;
	RF1_inied=0;
//开始模块初始化
 	RF1_spi_ce(0);    // chip enable
 	RF1_spi_cs(1);   // Spi  disable
	for(i=0;i<5;i++)
	{
		t=RF1_r_reg(0);
		RF1_w_reg(0,0x0b); //8crc,上电，接收
		wait(2000000);	//足够长的延时
		printf("the EN_AA register is:%d\r\n",(u16)RF1_r_reg(1));
		if(RF1_r_reg(0)==0x0b)
		{
			break;
		}
	}
	if(i==5)	//若超时
	{
		printf("nRF24L01 test failed\r\n");
		return ;
	}
	//printf("nRF24L01 is OK");
	RF1_w_reg(EN_AA, 0x00);      //频道0自动	ACK应答禁止	
	RF1_w_reg(EN_RXADDR, 0x01);
	RF1_w_reg(SETUP_AW, 0x01);  //地址宽度
	RF1_w_reg(SETUP_RETR, 0x00);  //禁止自动重发
	RF1_w_reg(RF_CH, 1);        //   设置信道工作为2.4GHZ，收发必须一致
	RF1_w_reg(RF_SETUP, 0x0f);	//设置发射速率为2MHZ，发射功率为最大值0dB
	RF1_w_reg(RX_PW_P0, RX_PLOAD_WIDTH); //设置接收数据长度，本次设置为32字节
	RF1_w_buf(TX_ADDR, (u8 *)TX_ADDRESS, TX_ADR_WIDTH);    // 写本地地址	
	RF1_w_buf(RX_ADDR_P0, (u8 *)RX_ADDRESS, RX_ADR_WIDTH); // 写接收端地址
	
	RF1_spi_ce(1);
	//if(RF1_r_reg(EN_AA)==0x00)	//测试是否写入
	//{
		//printf("nRF24L01 initialed sucessfully\r\n");
	//}
	//else
	//{
		//return ;
	//}
	RF1_w_reg(STATUS,0x70);	//清空寄存器
	//读取寄存器
	//print_rf_regs();
	RF1_inied=1;
}
void RF1_setch(u8 ch) //设置信道
{
	RF1_w_reg(RF_CH, ch);        //   设置信道工作为2.4GHZ，收发必须一致
}
u8 RF1_rxbuf[32];
void RF1_read_rx(void) //从模块中读取数据
{
	RF1_r_buf(RD_RX_PLOAD,RF1_rxbuf,RX_PLOAD_WIDTH);
	RF1_spi_rw(FLUSH_RX);
}
int RF1_poll(void) //返回是否有接收
{
	u8 sta=0;
	u8 rx_len=0;

	if(RF1_inied==0) return 0;

	sta=RF1_r_reg(STATUS);	// 读取状态寄存其来判断数据接收状况
	if(sta==0xff) return 0;
	if((sta & RX_DR) || (sta & 0x0e)!=0x0e)	//不能只收一次,FIFO里都要拿出来
	{
		RF1_read_rx();
		rx_len=RX_PLOAD_WIDTH;

		RF1_w_reg(STATUS,0x50);//接收到数据后RX_DR,TX_DS,MAX_PT都置高为1，通过写1来清除3中断标志
	}
	if(sta & TX_DS) //发送完成，切换为接收模式
	{
		RF1_spi_ce(0);
		RF1_w_reg(CONFIG, 0x0b);  	//8位CRC，主接收
		RF1_spi_ce(1);

		RF1_w_reg(STATUS,0x30);//接收到数据后RX_DR,TX_DS,MAX_PT都置高为1，通过写1来清除3中断标志
	}
	return rx_len;
}
void RF1_tx(u8 *tx_buf)
{
	RF1_spi_ce(0);			//StandBy I模式	
	RF1_spi_rw(FLUSH_TX);
	RF1_w_buf(WR_TX_PLOAD, tx_buf, TX_PLOAD_WIDTH); 		 // 装载数据	
	RF1_w_reg(CONFIG, 0x0a);  		 //8位CRC，主发送
	RF1_spi_ce(1);		 //置高CE，激发数据发送
}

