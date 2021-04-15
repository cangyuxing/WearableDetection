#ifndef __hkj15c_H
#define __hkj15c_H

#include "main.h"

extern QueueHandle_t HKJ_15C_Queue;

extern BSP_UART_HKJ_15C       UART1_HKJ_15C;
extern BSP_UART_HKJ_15C       UART2_HKJ_15C;

uint8_t BSP_Cheak_HKJ_15C(BSP_UART_HKJ_15C * receivedata);
void BSP_HKJ_15C_printf(BSP_UART_HKJ_15C * receivedata, uint8_t state);
void BSP_Receive_HKJ_15C(uint8_t pData, BSP_UART_HKJ_15C * receivedata);

void BSP_HKJ_15C_Init(void);
#endif
