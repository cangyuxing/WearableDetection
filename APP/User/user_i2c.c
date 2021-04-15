#include "user_i2c.h"

void User_I2C_Init(I2C_TypeDef *pi2c, I2C_HandleTypeDef* hi2c) {
    hi2c->Instance = pi2c;
    hi2c->Init.ClockSpeed = 50000;
    hi2c->Init.DutyCycle = I2C_DUTYCYCLE_2;
    hi2c->Init.OwnAddress1 = 0;
    hi2c->Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    hi2c->Init.DualAddressMode = I2C_DUALADDRESS_DISABLED;
    hi2c->Init.OwnAddress2 = 0;
    hi2c->Init.GeneralCallMode = I2C_GENERALCALL_DISABLED;
    hi2c->Init.NoStretchMode = I2C_NOSTRETCH_DISABLED;
    HAL_I2C_Init(hi2c);
}
