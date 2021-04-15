/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l1xx_hal.h"
#include "stm32l1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "stdio.h"
#include "string.h"
#include "math.h"
#include "time.h"
#include "stdint.h"
#include "stdlib.h"

#include "FreeRTOS.h"
#include "cmsis_os.h"
#include "gpio.h"
#include "adc.h"
#include "iwdg.h"
#include "rtc.h"
#include "i2c.h"
#include "spi.h"
#include "usart.h"


#include "app.h"
#include "data.h"
//#include "task_jy901.h"
//#include "task_hkj15c.h"
#include "task_ad8223.h"
#include "task_produce.h"
#include "protocoldata.h"
#include "task_upload.h"

//#include "adx345.h"
#include "axp173.h"
#include "hkj15c.h"
#include "jy901.h"
#include "NRF24L01.h"
#include "ADXL345.h"
//#include "nrf24l01plus.h"
#include "filter.h"

#include "user_gpio.h"
#include "user_adc.h"
#include "user_i2c.h"
#include "user_iwdg.h"
#include "user_rtc.h"
#include "user_spi.h"
#include "user_usart.h"
#include "user_stmflash.h"
#include "user_pwr.h"

#include "user_i2c_m.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
void SystemClock_Config(void);
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define ADXL345_CS_Pin GPIO_PIN_13
#define ADXL345_CS_GPIO_Port GPIOC
#define ADC1_Pin GPIO_PIN_1
#define ADC1_GPIO_Port GPIOA
#define UART2_XT_Pin GPIO_PIN_2
#define UART2_XT_GPIO_Port GPIOA
#define UART2_RX_Pin GPIO_PIN_3
#define UART2_RX_GPIO_Port GPIOA
#define SPI1_CS_Pin GPIO_PIN_4
#define SPI1_CS_GPIO_Port GPIOA
#define SPI1_SCK_Pin GPIO_PIN_5
#define SPI1_SCK_GPIO_Port GPIOA
#define SPI1_MISO_Pin GPIO_PIN_6
#define SPI1_MISO_GPIO_Port GPIOA
#define SPI1_MOSI_Pin GPIO_PIN_7
#define SPI1_MOSI_GPIO_Port GPIOA
#define NRF24L01_CE_Pin GPIO_PIN_0
#define NRF24L01_CE_GPIO_Port GPIOB
#define LED1_Pin GPIO_PIN_1
#define LED1_GPIO_Port GPIOB
#define IIC2_CLK_Pin GPIO_PIN_10
#define IIC2_CLK_GPIO_Port GPIOB
#define IIC2_SDA_Pin GPIO_PIN_11
#define IIC2_SDA_GPIO_Port GPIOB
#define NRF24L01_IRQ_Pin GPIO_PIN_12
#define NRF24L01_IRQ_GPIO_Port GPIOB
#define NRF24L01_IRQ_EXTI_IRQn EXTI15_10_IRQn
#define JY901_X_N_Pin GPIO_PIN_13
#define JY901_X_N_GPIO_Port GPIOB
#define JY901_Y_N_Pin GPIO_PIN_14
#define JY901_Y_N_GPIO_Port GPIOB
#define JY901_Y_P_Pin GPIO_PIN_15
#define JY901_Y_P_GPIO_Port GPIOB
#define JY901_X_P_Pin GPIO_PIN_8
#define JY901_X_P_GPIO_Port GPIOA
#define UART1_TX_Pin GPIO_PIN_9
#define UART1_TX_GPIO_Port GPIOA
#define UART1_RX_Pin GPIO_PIN_10
#define UART1_RX_GPIO_Port GPIOA
#define LED2_Pin GPIO_PIN_15
#define LED2_GPIO_Port GPIOA
#define AXP173_VBUSEN_Pin GPIO_PIN_3
#define AXP173_VBUSEN_GPIO_Port GPIOB
#define AXP173_INT1_Pin GPIO_PIN_4
#define AXP173_INT1_GPIO_Port GPIOB
#define AXP173_INT1_EXTI_IRQn EXTI4_IRQn
#define KEY_Pin GPIO_PIN_5
#define KEY_GPIO_Port GPIOB
#define KEY_EXTI_IRQn EXTI9_5_IRQn
#define IIC1_CLK_Pin GPIO_PIN_6
#define IIC1_CLK_GPIO_Port GPIOB
#define IIC1_SDA_Pin GPIO_PIN_7
#define IIC1_SDA_GPIO_Port GPIOB
#define ADXL345_INT2_Pin GPIO_PIN_8
#define ADXL345_INT2_GPIO_Port GPIOB
#define ADXL345_INT2_EXTI_IRQn EXTI9_5_IRQn
#define ADXL345_INT1_Pin GPIO_PIN_9
#define ADXL345_INT1_GPIO_Port GPIOB
#define ADXL345_INT1_EXTI_IRQn EXTI9_5_IRQn
/* USER CODE BEGIN Private defines */
//led π‹Ω≈∂®“Â
#define LED_port              GPIOB
#define LED_pin               GPIO_PIN_1
#define AD8232_SDN_port       GPIOA
#define AD8232_SDN_pin        GPIO_PIN_8
#define AD8232_SDN_LO_P_port  GPIOA
#define AD8232_SDN_LO_P_pin   GPIO_PIN_9
#define AD8232_SDN_LO_N_port  GPIOA
#define AD8232_SDN_LO_N_pin   GPIO_PIN_10
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
