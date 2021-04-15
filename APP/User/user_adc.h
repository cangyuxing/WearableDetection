#ifndef __USER_ADC_H__
#define __USER_ADC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

void User_Adc_Init(ADC_TypeDef *padc, ADC_HandleTypeDef *hadc);
uint16_t Get_Adc(uint32_t ch);
uint16_t Get_Adc_Average(uint32_t ch, uint8_t times);

#ifdef __cplusplus
}
#endif
#endif /*__ GPIO_H__ */

