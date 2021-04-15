#ifndef __jy901_H
#define __jy901_H

#include "main.h"

extern BSP_UART_JY_901        UART1_JY_901;
extern BSP_UART_JY_901        UART2_JY_901;

void BSP_JY_901_Init(void);
uint8_t BSP_Cheak_JY_901(BSP_UART_JY_901 * receivedata);
void BSP_Receive_JY_901(uint8_t pData, BSP_UART_JY_901 * receivedata);
void BSP_Receive_JY_901(uint8_t pData, BSP_UART_JY_901 * receivedata);
#endif
