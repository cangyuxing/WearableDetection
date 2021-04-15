#include "user_gpio.h"

/*------------------------- 心电部分代码 -----------------------------------------------
2020-12-115
---------------------------------------------------------------------------------------*/
void led_on(void) {
    HAL_GPIO_WritePin(LED_port, LED_pin, GPIO_PIN_RESET);
}
void led_off(void) {
    HAL_GPIO_WritePin(LED_port, LED_pin, GPIO_PIN_SET);
}
void User_AD8223_init(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    __HAL_RCC_GPIOH_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    HAL_GPIO_WritePin(AD8232_SDN_port, AD8232_SDN_pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(LED_port, LED_pin, GPIO_PIN_RESET);
    GPIO_InitStruct.Pin = AD8232_SDN_pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(AD8232_SDN_port, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = LED_pin;
    HAL_GPIO_Init(LED_port, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = AD8232_SDN_LO_N_pin | AD8232_SDN_LO_P_pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(AD8232_SDN_LO_N_port, &GPIO_InitStruct);
}
uint8_t AD8223_ConnectStatus;
uint8_t Led_Sate_cout;
uint8_t StatusChange(uint8_t *pstate, uint32_t *ptime, uint8_t statetemp, uint8_t value) {
    if(statetemp == value) { //存在快速切换的可能性
        if(*ptime < 100) {
            *pstate = 0x04;
        } else {
            *pstate = statetemp;
        }
        return 0;
    } else {
        *pstate = statetemp;
        return 1;
    }
}

uint8_t Get_AD8223_ConnectStatus(void) {
    static uint32_t connect_times = 0; //持续时间
    static uint8_t stateback = 0;
    uint8_t state = 0, statetemp = 0;
    uint8_t state_p = HAL_GPIO_ReadPin(AD8232_SDN_LO_P_port, AD8232_SDN_LO_P_pin);
    uint8_t state_n = HAL_GPIO_ReadPin(AD8232_SDN_LO_N_port, AD8232_SDN_LO_N_pin);
    statetemp |= state_p;
    statetemp <<= 1;
    statetemp |= state_n;
    //快速状态切换检测  需要过滤掉干扰
    connect_times++;
    switch(AD8223_ConnectStatus) {
        case 1:
        case 2:
            state = statetemp;
            break;
        case 0:
            StatusChange(&state, &connect_times, statetemp, 3);
            break;
        case 3:
            StatusChange(&state, &connect_times, statetemp, 0);
            break;
        case 4:
            if(connect_times < 100) {
                state = 0x04;
            } else {
                state = statetemp;
            }
            break;
        default: break;
    }
    if(stateback != statetemp) {
        //printf("stateback = %d statetemp=%d runStatus = %d AD8223_ConnectStatus_time =%d\r\n", stateback, statetemp, AD8223_ConnectStatus, AD8223_ConnectStatus_time);
        connect_times = 0;
    }
    if(AD8223_ConnectStatus != state) {
        Led_Sate_cout = 0;
        connect_times = 0;
        //printf("AD8223_ConnectStatus=%d state=%d\r\n",AD8223_ConnectStatus,state);
        if(state != 0) led_on(); //点亮led
        else           led_off();//熄灭led
    }
    AD8223_ConnectStatus = state;
    stateback = statetemp; //备份上一次基础状态
    //state = state_p + state_n (取值范围：0到3)
    //state = 0 正常
    //state = 1 左手电极断开 state_n
    //state = 2 右手电极断开 state_p
    //state = 3 左/右手电极同时断开 state_n state_p
    //state = 4 右腿参考电极断开
    return state;
}
void Led_Run_times(uint8_t state, uint8_t times) {
    const uint8_t count = 8;           //计数时间
    if(Led_Sate_cout > count - times) { //次数计算 5 = times+剩余
        if(state)
            led_on();
        else
            led_off();
    }
    if(Led_Sate_cout > count + times) {
        led_on();
    }
    if(Led_Sate_cout > count * 2) {
        Led_Sate_cout = 0;
    }
}
void Led_Sate_Run(void) {
    uint8_t state = HAL_GPIO_ReadPin(LED_port, LED_pin);
    Led_Sate_cout++;
    //Led_Run_times(state, AD8223_ConnectStatus);
    if(AD8223_ConnectStatus == 0) { //翻转闪烁
        if(Led_Sate_cout > 8) {
            Led_Sate_cout = 0;
            if(state)
                led_on();
            else
                led_off();
        }
    } else {
        Led_Run_times(state, AD8223_ConnectStatus);
    }
}

/*------------------------- 上下肢相关代码 -----------------------------------------------
2020-12-115
---------------------------------------------------------------------------------------*/
//中断
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    //printf("EXTI  GPIO_Pin = %d/r/n",GPIO_Pin);
    if(GPIO_Pin == KEY_Pin) {
        MYLOG(INFO, "EXTI  KEY_Pin = %x", GPIO_Pin);
    }
    if(GPIO_Pin == NRF24L01_IRQ_Pin) {
        //printf("EXTI  NRF24L01_IRQ_Pin = %x\r\n",GPIO_Pin);
        //NRF24L01_Handler();
    }
    if(GPIO_Pin == ADXL345_INT1_Pin) {
        ADXL345_Get_State(1);
        //Get_ADXL345_State(1);
        //MYLOG(DEBUG, "EXTI  ADXL345_INT1_Pin = %x", GPIO_Pin);
    }
    if(GPIO_Pin == ADXL345_INT2_Pin) {
        ADXL345_Get_State(2);
        //Get_ADXL345_State(2);
        //MYLOG(DEBUG, "EXTI  ADXL345_INT2_Pin = %x", GPIO_Pin);
    }
    if(GPIO_Pin == AXP173_INT1_Pin) {
        MYLOG(INFO, "EXTI  AXP173_INT1_Pin = %x", GPIO_Pin);
    }
}

uint8_t KEY_Scan(void) {
    static uint32_t downtime = 0;
    static uint32_t uptime = 0;
    if(downtime == 0) {
        if(KEY0 == 0) { //检测按键按下
            downtime = HAL_GetTick();//按下时间
            //printf("downtime =%d\r\n",downtime);
        }
    } else {           //检测按键松开
        if(DelayTimeOut(&downtime, 50)) { //超时50ms滤波处理
            if(uptime == 0) {
                if(KEY0 == 1) { //检测按键松开
                    uptime = HAL_GetTick();//抬起时间
                    //printf("uptime =%d\r\n",uptime);
                }
            } else {
                if(DelayTimeOut(&uptime, 50)) { //超时50ms滤波处理
                    uint8_t state = 0;
                    uint32_t time = DelayGetInterval(&uptime, &downtime);
                    if(time > 1000) {
                        state = 2; //长按
                    } else {
                        state = 1; //短按
                    }
                    uptime = 0;
                    downtime = 0;
                    MYLOG(DEBUG, "time =%d", time);
                    return state;
                }
            }
        }
    }
    return 0;
}


void KEY_Do(void) { //按键处理
    static uint8_t  keycount = 0;
    static uint32_t keytime = 0;
    const  uint8_t  mode = 0; //0：普通单击模式 1：支持连击模式 超时时间1S
    uint8_t keyvalue = KEY_Scan();
    if(keyvalue) {
        if(keyvalue == 1) { //短按才支持多按键编程
            if(keytime == 0) {
                if(mode) {
                    keytime = HAL_GetTick(); //第一次按下
                    keycount = 1;
                } else {
                    MYLOG(INFO, "short key");
                    //添加处理函数
                    //User_pwr_test();
                    //Flash_DisableReadProtection();
                }
                return;
            } else { //连击模式
                uint32_t time = HAL_GetTick();//当前时间
                if(DelayGetInterval(&time, &keytime) < 1000) {
                    keytime = time; //更新时间
                    keycount ++;    //连击有限，直接计数
                }
            }
        }
        if(keyvalue == 2) { //长按处理流程
            MYLOG(INFO, "long key");
            //添加处理函数
            //Flash_EnableReadProtection();
        }
    }
    if(keytime) {
        if(DelayTimeOut(&keytime, 1000)) { //超时1s清零击功能
            if(keycount) {
                MYLOG(INFO, "more key keycount =%d", keycount);
                keytime  = 0;
                keycount = 0;
                //添加处理函数
            }
        }
    }
}

uint8_t Get_RunStatus(void) {
    if((information.EquipmentType == HOPEN_UL) || (information.EquipmentType == HOPEN_UR)) {
        if(globalvariable.EquipmentFlag == 0x0f) {
            return 0;
        }
    }
    if((information.EquipmentType == HOPEN_DL) || (information.EquipmentType == HOPEN_DR) || (information.EquipmentType == HOPEN_DX)) {
        if(globalvariable.EquipmentFlag == 0x1f) {
            return 0;
        }
        if(globalvariable.EquipmentFlag == (0x1f & (~hkj15c_flag))) {
            return 5;
        }
    }
    if(information.EquipmentType == HOPEN_DEF) {
        if(globalvariable.EquipmentFlag == 0x07) {
            return 0;
        }
    }
    if(globalvariable.EquipmentFlag == (0x1f & (~axp173_flag))) {
        return 1;
    }
    if(globalvariable.EquipmentFlag == (0x1f & (~adx345_flag))) {
        return 2;
    }
    if(globalvariable.EquipmentFlag == (0x1f & (~nrf24l_flag))) {
        return 3;
    }
    if(globalvariable.EquipmentFlag == (0x1f & (~jiy901_flag))) {
        return 4;
    }
    return 0xff;
}
//设备灯
void Led1_Sate_Run(uint8_t Status) {
    const uint8_t cycle = 10;            //计数时间N+1+N
    static uint8_t Led_cout = 0;
    Led_cout ++;
    if(Status == 0) {                    //翻转闪烁
        if(Led_cout > 8) {
            Led_cout = 0;
            HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
        }
    } else if(Status > HOPEN_DX) {       //无定义设备
        HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
    } else {
        if(Led_cout <= cycle - Status) { //开始空白时间
            HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET);//关灯
        }
        if(Led_cout > cycle - Status) {  //闪烁时间
            HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);             //闪烁
        }
        if(Led_cout >= cycle + Status) { //结束时间
            HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET);//关灯
        }
        if(Led_cout >= cycle * 2) {     //一个闪烁周期结束
            Led_cout = 0;
        }
    }
}
//状态灯
void Led2_Sate_Run(uint8_t Status) {
    const uint8_t cycle = 10;            //计数时间N+1+N
    static uint8_t Led_cout = 0;
    Led_cout ++;
    if(Status == 0) {                    //翻转闪烁
        if(Led_cout > 8) {
            Led_cout = 0;
            HAL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin);
        }
    } else if(Status == 0Xff) {
        HAL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin);
    } else {
        if(Led_cout <= cycle - Status) { //开始空白时间
            HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_SET);//关灯
        }
        if(Led_cout > cycle - Status) {//闪烁时间
            HAL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin);             //闪烁
        }
        if(Led_cout >= cycle + Status) { //结束时间
            HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_SET);//关灯
        }
        if(Led_cout >= cycle * 2) {     //一个闪烁周期结束
            Led_cout = 0;
        }
    }
}

void LED1_DO(void) {  //设备
    //HOPEN_DEF = 0,  //默认：      常量
    //HOPEN_UL,       //上肢左 1    闪烁1下
    //HOPEN_UR,       //上肢右 2    闪烁2下
    //HOPEN_DL,       //下肢左 3    闪烁3下
    //HOPEN_DR,       //下肢右 4    闪烁4下
    static uint32_t led1time = 0;
    if(DelayTimeOut(&led1time, 50)) {
        led1time = HAL_GetTick();
        Led1_Sate_Run(information.EquipmentType);
    }
}

void LED2_DO(void) { //状态
    //正常工作状态  1闪1闪
    //单个模块异常：N闪一下
    //多个模块异常：快速闪烁
    //振动快速闪烁一下
    static uint32_t led1time = 0;
    if(DelayTimeOut(&led1time, 50)) {
        led1time = HAL_GetTick();
        Led2_Sate_Run(Get_RunStatus());
    }
}
