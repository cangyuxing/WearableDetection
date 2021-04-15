#ifndef __USER_I2C_H__
#define __USER_I2C_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

void User_I2C_Init(I2C_TypeDef *pi2c, I2C_HandleTypeDef* hi2c);

#ifdef __cplusplus
}
#endif
#endif /*__ GPIO_H__ */

