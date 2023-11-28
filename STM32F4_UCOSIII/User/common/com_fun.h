#ifndef COM_FUN_H
#define COM_FUN_H

#include "main.h"

#pragma pack(1)
typedef struct //16字节
{
	u8 aa; //同步字
	u8 type; //1
	u32 can_id	:29; //0~28:can-id
	u32 ext_std	:1; //0标准帧
	u32 rtr		:1; 
	u32 enable	:1;
	u8 data[8];
	u8 len; //数据中有多少有效数据
	u8 sum; //校验和，程序内部使用时，不使用
} CAN_MSG; //can帧
#pragma pack()
#pragma pack(1)
typedef struct //8字节
{
	u16 cmd; //0请求开始，1准许，2不准许，3crc错误，4烧写错误
	u16 id; //目标机id
	u16 len; //数据长度，单位为包，一包8字节，一定是8的整数倍；若是回复，则为版本
	u16 crc; //目标的crc
} CAN_IAP_CMD; //can IAP指令包
#pragma pack()

#ifdef __cplusplus
extern "C"
{
#endif
u8 check_sum(u8 *p,int n);
u16 cal_crc(u8 *ptr,u32 len);
#ifdef __cplusplus
}
#endif

#endif

