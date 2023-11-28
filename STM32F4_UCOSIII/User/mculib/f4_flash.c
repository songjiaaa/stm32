#include "f4_flash.h"

#pragma pack(1)
typedef struct
{
	//u32 ACR;
	u8 LATENCY;
	u8 PRFTEN:1; //prefetch
	u8 ICEN:1; //instruction cache
	u8 DCEN:1; //data cache
	u8 ICRST:1; //instruction cache reset
	u8 DCRST:1; //data cache reset
	u8 res0:3;
	u16 res1;
	u32 KEYR;
	u32 OPTKEYR;
	//u32 SR;
	u8 EOP:1; //操作成功（中断使能时）
	u8 OPERR:1; //操作失败（中断使能时）
	u8 res2:2;
	u8 WRPERR:1; //写失败（写1清零）
	u8 PGAERR:1; //失败（写1清零）
	u8 PGPERR:1; //失败（写1清零）
	u8 PGSERR:1; //失败（写1清零）
	u8 res3;
	u16 bsy:1; //忙
	u16 res4:15;
	//u32 CR;
	u8 PG:1; //MFLASH programming activated.
	u8 SER:1; //Sector Erase activated
	u8 MER:1; //Erase activated for all user sectors
	u8 SNB:4; //These bits select the sector to erase
	u8 res5:1; 
	u8 PSIZE; //These bits select the program parallelism.
	u8 STRT:1; //This bit triggers an erase operation when set. It is set only by software and cleared when the BSY bit is cleared.
	u8 res6:7;
	u8 EOPIE:1; //This bit enables the interrupt generation when the EOP bit in the FLASH_SR register goes to 1
	u8 ERRIE:1; //This bit enables the interrupt generation when the OPERR bit in the FLASH_SR register is set to 1.
	u8 res7:5;
	u8 LOCK:1; //Write to 1 only
	u32 OPTCR;
} MFLASH_TypeDef;
#pragma pack()
#define MFLASH               ((MFLASH_TypeDef *) FLASH_R_BASE)

int flash_erase(void *pf) //擦MFLASH扇区
{
	//解锁：
	MFLASH->KEYR=0x45670123;
	MFLASH->KEYR=0xCDEF89AB;
	MFLASH->PSIZE=2; //3.3V供电
	while(MFLASH->bsy);
	MFLASH->SER=1;
	MFLASH->SNB=6; //选择区域(这里选6扇区)
	MFLASH->STRT=1;
	while(MFLASH->bsy);
	MFLASH->SER=0;
	//上锁
	MFLASH->LOCK=1; 
	return 0;
}
int flash_write(u32 *p,int n,void *addr) //直接写入(按4字节)
{
	int i;
	//解锁：
	MFLASH->KEYR=0x45670123;
	MFLASH->KEYR=0xCDEF89AB;

	while(MFLASH->bsy);
	MFLASH->PSIZE=2; //3.3V供电
	MFLASH->PG=1;
	for(i = 0; i < n/4; i++)
	{
		((u32*)addr)[i]=p[i];
	}
	MFLASH->PG=0;
	//上锁
	MFLASH->LOCK=1; 
	return 0;
}
int flash_eraseAwrite(u32 *p,int n,void *pf) //擦除并写入,输入数据指针，数据长度(单位字节)，MFLASH地址
{
	int i;
//擦除
	flash_erase(pf);
//写入
	flash_write(p,n,pf);
//校验
	for(i = 0; i < n/4; i++)
	{
		if(p[i]!=((u32*)pf)[i])
		{
			return 1;
		}
	}
	return 0;
}

