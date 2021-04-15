#ifndef __AXP173_H___
#define __AXP173_H___

/* Includes ------------------------------------------------------------------*/
#include "main.h"

#define I2C1_SLAVE_ADDRESS7     0x68  //�豸��ַΪ 69H(��)�� 68H(д)

#define INPUT_POWER_STATUS      0x00  //��Դ״̬�Ĵ���
#define POWER_OUTPUT_CONTROL    0x12  //DC-DC1/LDO4 & LDO2/3 ���ؿ��ƼĴ���
#define DCDC1_OUTPUT            0x26  //DC-DC1 ��ѹ���üĴ���
#define LDO4_OUTPUT             0x27  //LDO4 ��ѹ���üĴ���
#define LDO2_3_OUTPUT           0x28  //LDO2/3 ��ѹ���üĴ���
#define PEK_STATE               0x36  //PEK �������üĴ���
#define TURN_OFF                0x32  //�ػ�����ؼ�⡢CHGLED ���ƼĴ���
#define ADP_CHAR_ENABLE         0x33  //�����ƼĴ��� 1  
#define ADP_CHAR_2              0x34  //�����ƼĴ��� 2  
#define BAT_ADDRESS_Voltage     0x78  //��ص�ѹ�� 8 λ
#define BAT_ADDRESS_Current     0x7c  //��ص����� 8 λ

#define VOLT_ADC_ENABLE         0x82  //ADC ʹ�����üĴ��� 1

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
