#include "user_iwdg.h"

void User_Iwdg_Init(void){
  hiwdg.Instance = IWDG;
  hiwdg.Init.Prescaler = IWDG_PRESCALER_32;
  hiwdg.Init.Reload = 1000; //4S
  HAL_IWDG_Init(&hiwdg);
  __HAL_IWDG_START(&hiwdg);
}
