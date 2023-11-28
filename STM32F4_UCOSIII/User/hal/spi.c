#include "spi.h"
#include "w25qxx.h"

u8 aTxBuffer[BUFFERSIZE] = "SPI Master/Slave : Communication between two SPI using DMA";
u8 aRxBuffer[BUFFERSIZE];

void spi1_config(void)
{	 
	GPIO_InitTypeDef  GPIO_InitStructure;
	SPI_InitTypeDef  SPI_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);

	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin   = SPI1_CS_Pin;           //CS����
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
	GPIO_Init(SPI1_CS_SCK,&GPIO_InitStructure); 
	SPI1_CS = 1; 

	GPIO_PinAFConfig(GPIOB,GPIO_PinSource3,GPIO_AF_SPI1); //PB3����Ϊ SPI1
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource4,GPIO_AF_SPI1); //PB4����Ϊ SPI1
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource5,GPIO_AF_SPI1); //PB5����Ϊ SPI1

	//����ֻ���SPI�ڳ�ʼ��
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_SPI1,ENABLE);//��λSPI1
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_SPI1,DISABLE);//ֹͣ��λSPI1

	SPI_I2S_DeInit(SPI1);
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7; 
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;			
	SPI_Init(SPI1, &SPI_InitStructure);  

	SPI_Cmd(SPI1, ENABLE); 
}  

u8 SPI1_ReadWriteByte(u8 TxData)
{		 			 
	u16 retry=0;
	while((SPI1->SR&1<<1)==0)
	{
		retry++;
		if(retry>=0XFFFE)return 0; 	
	}
	SPI1->DR=TxData;	 	  		
	retry=0;
	while((SPI1->SR&1<<0)==0)		 
	{
		retry++;
		if(retry>=0XFFFE)return 0; 	
	}	
 	return SPI1->DR;          						    
}



/***************************************************************/
SPI_InitTypeDef  SPI_InitStructure;
DMA_InitTypeDef DMA_InitStructure;
void spi2_dma_config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;	
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_12;           //CS����
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
	GPIO_Init(GPIOB,&GPIO_InitStructure); 
	SPI2_CS = 1; 
	
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource13, GPIO_AF_SPI2);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource14, GPIO_AF_SPI2);    
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource15, GPIO_AF_SPI2);
	
	SPI_I2S_DeInit(SPI2);
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	
	DMA_DeInit(DMA1_Stream4);
    DMA_DeInit(DMA1_Stream3);
	/* Configure DMA Initialization Structure */
	DMA_InitStructure.DMA_BufferSize = 20;
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable ;
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull ;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single ;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_PeripheralBaseAddr =(uint32_t) (&(SPI2->DR)) ;
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	/* Configure TX DMA */
	DMA_InitStructure.DMA_Channel = DMA_Channel_0 ;
	DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral ;
	DMA_InitStructure.DMA_Memory0BaseAddr =(uint32_t)aTxBuffer ;
	DMA_Init(DMA1_Stream4, &DMA_InitStructure);
	/* Configure RX DMA */
	DMA_InitStructure.DMA_Channel = DMA_Channel_0 ;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory ;
	DMA_InitStructure.DMA_Memory0BaseAddr =(uint32_t)aRxBuffer ; 
	DMA_Init(DMA1_Stream3, &DMA_InitStructure);
	

  	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream3_IRQn;			//����ͨ���ж�
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;	 
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;					
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;							
  	NVIC_Init(&NVIC_InitStructure);

    DMA_ITConfig(DMA1_Stream3, DMA_IT_TC, ENABLE); //ʹ�ܴ�������ж�
}

void spi2_init_master(void)
{
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_Init(SPI2, &SPI_InitStructure);
	SPI_Cmd(SPI2, DISABLE); //ʧ��
}

void spi2_init_slave(void)
{
    SPI_InitStructure.SPI_Mode = SPI_Mode_Slave;
	SPI_Init(SPI2, &SPI_InitStructure);
	SPI_Cmd(SPI2, DISABLE);
}

//addr �����ַ�� ���䳤��
void set_spi2_dma(u8 *addr,u32 buf_size)
{
	DMA_InitStructure.DMA_BufferSize = buf_size;
	memcpy(aTxBuffer,addr,buf_size);
	DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral ;
	DMA_InitStructure.DMA_Memory0BaseAddr =(uint32_t)aTxBuffer ;
	DMA_Init(DMA1_Stream4, &DMA_InitStructure);
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory ;
	DMA_InitStructure.DMA_Memory0BaseAddr =(uint32_t)aRxBuffer ; 
	DMA_Init(DMA1_Stream3, &DMA_InitStructure);
	
	DMA_Cmd(DMA1_Stream4,ENABLE);
	/* Enable DMA SPI RX Stream */
	DMA_Cmd(DMA1_Stream3,ENABLE);
	/* Enable SPI DMA TX Requsts */
	SPI_I2S_DMACmd(SPI2, SPI_I2S_DMAReq_Tx, ENABLE);
	/* Enable SPI DMA RX Requsts */
	SPI_I2S_DMACmd(SPI2, SPI_I2S_DMAReq_Rx, ENABLE);

	SPI_Cmd(SPI2, ENABLE);
	SPI2_CS = 0;	                           //��������
}


void DMA1_Stream3_IRQHandler(void)
{		
	if(DMA_GetITStatus(DMA1_Stream3,DMA_IT_TCIF3) != RESET)    //������ɱ�־
	{			
		DMA_ClearFlag(DMA1_Stream3, DMA_FLAG_TCIF3);
		DMA_Cmd(DMA1_Stream4, DISABLE);               //��������
		DMA_Cmd(DMA1_Stream3, DISABLE);               //��������
		SPI2_CS=1;		                        //��������
	}
}


