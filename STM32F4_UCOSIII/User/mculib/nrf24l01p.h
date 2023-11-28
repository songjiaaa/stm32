/*
文件名：nrf24l01p.h
时间：2011-6-11
功能：

*/
#ifndef NRF24L01P_H
#define NRF24L01P_H

#include "main.h"

extern int RF1_inied; //是否初始化成功

void RF1_ini(void);
void RF1_setch(u8 ch); //设置信道
extern u8 RF1_rxbuf[32];
int RF1_poll(void); //返回是否有接收
void RF1_tx(u8 *tx_buf);

extern u8 (*RF1_spi_rw)(u8 a);
extern void (*RF1_spi_cs)(u8 a);
extern void (*RF1_spi_ce)(u8 a);
///////////////////////////////////////////////////////////////

#endif

