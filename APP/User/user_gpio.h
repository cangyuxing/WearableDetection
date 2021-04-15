#ifndef __USER_GPIO_H__
#define __USER_GPIO_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

void User_AD8223_init(void) ;
uint8_t Get_AD8223_ConnectStatus(void);
void Led_Sate_Run(void);




uint8_t KEY_Scan(void);
void KEY_Do(void);
void LED1_DO(void);
void LED2_DO(void);
#define KEY0 HAL_GPIO_ReadPin(KEY_GPIO_Port,KEY_Pin)  //KEY0°´¼üPC5

#define KEY0_PRES 1

#ifdef __cplusplus
}
#endif
#endif /*__ GPIO_H__ */
