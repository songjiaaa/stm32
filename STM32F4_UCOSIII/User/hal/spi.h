#ifndef SPI_H_
#define SPI_H_
#include "stm32f4xx.h"
#include "stm32f4_sys.h"

#define SPI1_CS_SCK  GPIOB
#define SPI1_CS_Pin  GPIO_Pin_0
#define SPI1_CS 	 PBout(0)

void spi1_config(void);
u8 SPI1_ReadWriteByte(u8 TxData);




#define SPI2_CS 	PBout(12)
#define BUFFERSIZE  (1024)

extern u8 aTxBuffer[BUFFERSIZE];
extern u8 aRxBuffer[BUFFERSIZE];
void spi2_dma_config(void);
void spi2_init_master(void);
void spi2_init_slave(void);
//void set_spi2_dma(u32 buf_size);
void set_spi2_dma(u8 *addr,u32 buf_size);
#endif


