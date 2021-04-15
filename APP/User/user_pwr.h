#ifndef __USER_RWR_H__
#define __USER_RWR_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"


void enter_stop_rtc(uint32_t stoptime);
void enter_standby_rtc(uint32_t standbytime);
void enter_sleep_rtc(uint32_t sleeptime);
void system_power_config(void);
void system_clock_config(void);

void Flash_EnableReadProtection(void);
void Flash_DisableReadProtection(void);

void User_pwr_enter_lowpower(uint8_t mode);
void User_pwr_test(void);

#ifdef __cplusplus
}
#endif
#endif /*__ GPIO_H__ */
