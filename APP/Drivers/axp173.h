#ifndef __AXP173_H___
#define __AXP173_H___

/* Includes ------------------------------------------------------------------*/
#include "main.h"

#define I2C1_SLAVE_ADDRESS7     0x68  //设备地址为 69H(读)和 68H(写)

#define INPUT_POWER_STATUS      0x00  //电源状态寄存器
#define POWER_OUTPUT_CONTROL    0x12  //DC-DC1/LDO4 & LDO2/3 开关控制寄存器
#define DCDC1_OUTPUT            0x26  //DC-DC1 电压设置寄存器
#define LDO4_OUTPUT             0x27  //LDO4 电压设置寄存器
#define LDO2_3_OUTPUT           0x28  //LDO2/3 电压设置寄存器
#define PEK_STATE               0x36  //PEK 参数设置寄存器
#define TURN_OFF                0x32  //关机、电池检测、CHGLED 控制寄存器
#define ADP_CHAR_ENABLE         0x33  //充电控制寄存器 1  
#define ADP_CHAR_2              0x34  //充电控制寄存器 2  
#define BAT_ADDRESS_Voltage     0x78  //电池电压高 8 位
#define BAT_ADDRESS_Current     0x7c  //电池电流高 8 位

#define VOLT_ADC_ENABLE         0x82  //ADC 使能设置寄存器 1

void ldo2_enable(void);
void ldo2_disable(void);
void ldo3_enable(void);
void ldo3_disable(void);
void ldo4_enable(void);
void ldo4_disable(void);
uint8_t is_charging(void);
uint8_t axp173_init(void);

uint16_t getVoltage(void);
uint16_t getCurrent(void);

void axp173_poweroff(void) ;
#endif 
