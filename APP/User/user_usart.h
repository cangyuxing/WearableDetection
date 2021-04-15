#ifndef __USER_USART_H__
#define __USER_USART_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

#define  USART_HAL_LEN   1
extern __IO uint8_t  Uart1_HAL_Buffer[USART_HAL_LEN];//HAL库使用的串口接收缓冲
extern __IO uint8_t  Uart2_HAL_Buffer[USART_HAL_LEN];//HAL库使用的串口接收缓冲

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */
void MX_Struct_Test(void);

void User_USART_Init(USART_TypeDef *puart, UART_HandleTypeDef *huart, uint32_t pbaudrate);
    
void BSP_QueueSend(QueueHandle_t Handle,uint8_t *pbuf);

void BSP_UART_IRQHandler(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size);

#ifdef __cplusplus
}
#endif
#endif /*__ GPIO_H__ */
