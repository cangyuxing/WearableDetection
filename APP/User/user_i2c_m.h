#ifndef _MYIIC_H
#define _MYIIC_H
#include "main.h"

void    IIC1_MYTest(void);
void    IIC1_Init(void);                //初始化IIC的IO口
void    IIC1_WriteReg(uint8_t SlaveAddress, uint8_t WriteAddr, uint8_t DataToWrite);
uint8_t IIC1_ReadReg(uint8_t SlaveAddress, uint8_t ReadAddr);

void    IIC2_MYTest(void);
void    IIC2_Init(void);                //初始化IIC的IO口
void    IIC2_WriteReg(uint8_t SlaveAddress, uint8_t WriteAddr, uint8_t DataToWrite);
uint8_t IIC2_ReadReg(uint8_t SlaveAddress, uint8_t ReadAddr);
#endif

