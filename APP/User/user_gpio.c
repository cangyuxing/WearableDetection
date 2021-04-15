#include "user_gpio.h"

/*------------------------- �ĵ粿�ִ��� -----------------------------------------------
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
    if(statetemp == value) { //���ڿ����л��Ŀ�����
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
    static uint32_t connect_times = 0; //����ʱ��
    static uint8_t stateback = 0;
    uint8_t state = 0, statetemp = 0;
    uint8_t state_p = HAL_GPIO_ReadPin(AD8232_SDN_LO_P_port, AD8232_SDN_LO_P_pin);
    uint8_t state_n = HAL_GPIO_ReadPin(AD8232_SDN_LO_N_port, AD8232_SDN_LO_N_pin);
    statetemp |= state_p;
    statetemp <<= 1;
    statetemp |= state_n;
    //����״̬�л����  ��Ҫ���˵�����
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
        if(state != 0) led_on(); //����led
        else           led_off();//Ϩ��led
    }
    AD8223_ConnectStatus = state;
    stateback = statetemp; //������һ�λ���״̬
    //state = state_p + state_n (ȡֵ��Χ��0��3)
    //state = 0 ����
    //state = 1 ���ֵ缫�Ͽ� state_n
    //state = 2 ���ֵ缫�Ͽ� state_p
    //state = 3 ��/���ֵ缫ͬʱ�Ͽ� state_n state_p
    //state = 4 ���Ȳο��缫�Ͽ�
    return state;
}
void Led_Run_times(uint8_t state, uint8_t times) {
    const uint8_t count = 8;           //����ʱ��
    if(Led_Sate_cout > count - times) { //�������� 5 = times+ʣ��
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
    if(AD8223_ConnectStatus == 0) { //��ת��˸
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

/*------------------------- ����֫��ش��� -----------------------------------------------
2020-12-115
---------------------------------------------------------------------------------------*/
//�ж�
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
        if(KEY0 == 0) { //��ⰴ������
            downtime = HAL_GetTick();//����ʱ��
            //printf("downtime =%d\r\n",downtime);
        }
    } else {           //��ⰴ���ɿ�
        if(DelayTimeOut(&downtime, 50)) { //��ʱ50ms�˲�����
            if(uptime == 0) {
                if(KEY0 == 1) { //��ⰴ���ɿ�
                    uptime = HAL_GetTick();//̧��ʱ��
                    //printf("uptime =%d\r\n",uptime);
                }
            } else {
                if(DelayTimeOut(&uptime, 50)) { //��ʱ50ms�˲�����
                    uint8_t state = 0;
                    uint32_t time = DelayGetInterval(&uptime, &downtime);
                    if(time > 1000) {
                        state = 2; //����
                    } else {
                        state = 1; //�̰�
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


void KEY_Do(void) { //��������
    static uint8_t  keycount = 0;
    static uint32_t keytime = 0;
    const  uint8_t  mode = 0; //0����ͨ����ģʽ 1��֧������ģʽ ��ʱʱ��1S
    uint8_t keyvalue = KEY_Scan();
    if(keyvalue) {
        if(keyvalue == 1) { //�̰���֧�ֶఴ�����
            if(keytime == 0) {
                if(mode) {
                    keytime = HAL_GetTick(); //��һ�ΰ���
                    keycount = 1;
                } else {
                    MYLOG(INFO, "short key");
                    //��Ӵ�����
                    //User_pwr_test();
                    //Flash_DisableReadProtection();
                }
                return;
            } else { //����ģʽ
                uint32_t time = HAL_GetTick();//��ǰʱ��
                if(DelayGetInterval(&time, &keytime) < 1000) {
                    keytime = time; //����ʱ��
                    keycount ++;    //�������ޣ�ֱ�Ӽ���
                }
            }
        }
        if(keyvalue == 2) { //������������
            MYLOG(INFO, "long key");
            //��Ӵ�����
            //Flash_EnableReadProtection();
        }
    }
    if(keytime) {
        if(DelayTimeOut(&keytime, 1000)) { //��ʱ1s���������
            if(keycount) {
                MYLOG(INFO, "more key keycount =%d", keycount);
                keytime  = 0;
                keycount = 0;
                //��Ӵ�����
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
//�豸��
void Led1_Sate_Run(uint8_t Status) {
    const uint8_t cycle = 10;            //����ʱ��N+1+N
    static uint8_t Led_cout = 0;
    Led_cout ++;
    if(Status == 0) {                    //��ת��˸
        if(Led_cout > 8) {
            Led_cout = 0;
            HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
        }
    } else if(Status > HOPEN_DX) {       //�޶����豸
        HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
    } else {
        if(Led_cout <= cycle - Status) { //��ʼ�հ�ʱ��
            HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET);//�ص�
        }
        if(Led_cout > cycle - Status) {  //��˸ʱ��
            HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);             //��˸
        }
        if(Led_cout >= cycle + Status) { //����ʱ��
            HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET);//�ص�
        }
        if(Led_cout >= cycle * 2) {     //һ����˸���ڽ���
            Led_cout = 0;
        }
    }
}
//״̬��
void Led2_Sate_Run(uint8_t Status) {
    const uint8_t cycle = 10;            //����ʱ��N+1+N
    static uint8_t Led_cout = 0;
    Led_cout ++;
    if(Status == 0) {                    //��ת��˸
        if(Led_cout > 8) {
            Led_cout = 0;
            HAL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin);
        }
    } else if(Status == 0Xff) {
        HAL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin);
    } else {
        if(Led_cout <= cycle - Status) { //��ʼ�հ�ʱ��
            HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_SET);//�ص�
        }
        if(Led_cout > cycle - Status) {//��˸ʱ��
            HAL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin);             //��˸
        }
        if(Led_cout >= cycle + Status) { //����ʱ��
            HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_SET);//�ص�
        }
        if(Led_cout >= cycle * 2) {     //һ����˸���ڽ���
            Led_cout = 0;
        }
    }
}

void LED1_DO(void) {  //�豸
    //HOPEN_DEF = 0,  //Ĭ�ϣ�      ����
    //HOPEN_UL,       //��֫�� 1    ��˸1��
    //HOPEN_UR,       //��֫�� 2    ��˸2��
    //HOPEN_DL,       //��֫�� 3    ��˸3��
    //HOPEN_DR,       //��֫�� 4    ��˸4��
    static uint32_t led1time = 0;
    if(DelayTimeOut(&led1time, 50)) {
        led1time = HAL_GetTick();
        Led1_Sate_Run(information.EquipmentType);
    }
}

void LED2_DO(void) { //״̬
    //��������״̬  1��1��
    //����ģ���쳣��N��һ��
    //���ģ���쳣��������˸
    //�񶯿�����˸һ��
    static uint32_t led1time = 0;
    if(DelayTimeOut(&led1time, 50)) {
        led1time = HAL_GetTick();
        Led2_Sate_Run(Get_RunStatus());
    }
}
