#ifndef __PROTOCOLDATA_H__
#define __PROTOCOLDATA_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include "data.h"

#define HTONL(v) ( ((v) << 24) | (((v) >> 24) & 255) | (((v) << 8) & 0xff0000) | (((v) >> 8) & 0xff00) )
#define HTONS(v) ( (((v) << 8) & 0xff00) | (((v) >> 8) & 255) )

uint8_t DelayTimeOut(uint32_t *time, uint32_t delay);
uint32_t DelayGetInterval(uint32_t *nowtime, uint32_t *oldtime);

void Upload_Data_Check(Upload_DATA *data);
void Upload_Data_Check_V_0(Upload_DATA_V1_0 *data) ;

void delay_us(uint32_t nus);
void delay_ms(uint32_t nms);
    
uint32_t MYabs(int32_t a, int32_t b);
uint32_t ModeChange32(uint32_t pdata);
uint16_t ModeChange16(uint16_t pdata);
uint16_t MyStrLen(uint8_t *Dst);
uint16_t IsStartWithSrc(uint8_t* src, uint8_t* s);
uint16_t IsStartWithSrc2(uint8_t* src, uint8_t* s, uint16_t len);
uint16_t IsStartWithSrc3(uint8_t* src, uint8_t* s, uint16_t len);
uint16_t IsStartWithchar(uint8_t * src, uint8_t s);
uint8_t CalU8CRC(uint8_t *pOutBuf, uint16_t OutLen);
uint32_t GetCheckSum32(uint8_t *Src, uint32_t Len);

#ifdef __cplusplus
}
#endif
#endif
