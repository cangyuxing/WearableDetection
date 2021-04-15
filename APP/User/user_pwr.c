#include "user_pwr.h"
/*************************************************************************************************
STM32L151ϵ�е͹���ģʽ��Stopģʽ��Stop+RTCģʽ��Sleepģʽ
STM32Lϵ�������ֵ͹���ģʽ���ֱ���standby,stop,sleep,low power sleep,low power run.
����standbyģʽ�¹�����ͣ�ʵ�����Ϊ1uA������ģʽ�����ݲ����档
stopģʽʵ�⹦��Ϊ1.3uA������ģʽ�����ݱ��档
sleepģʽ�¹���Ϊ1mA���ң���Ҫ��Ƶ�ʣ��¶ȵ�Ӱ�죬���ķ����仯��

�˴���ʹ��RTC���ѣ�ʹ��������ʽ���ѷ�ʽ��Լ򵥣����Բο�st�ٷ����̡�
**************************************************************************************************/

/**********************************************************
** Filename: stop_mode.c
** Abstract: ʹ��STM32L151C8T6MCU��ʹ��RTC����STOP��STANDBYģʽ�µĵ͹���,�͹���ʱ��������Ϊ�Ľ�������
**           ���õ͹���ʱ��ʱ��ο�ͷ�ļ��й���ʱ���ĺ궨��
** ʹ��ע�����ʹ��CubeMX���ɺ���ʱ����main()��������Զ�����SystemClock_Config()�������˳����е����˸ú�����
**               ����ú����������ļ��У��뽫��.h�ļ����룬���ⷢ������
** Date : 2018-01-04
** Author:������
*********************************************************/
//RTC_HandleTypeDef RTCHandle;        //RTC�ṹ�����
//����SLEEPģʽ�͹��ģ�ʹ��RTC���ܻ��ѣ�����sleeptime��λΪS��������1���͹���1�����
void enter_sleep_rtc(uint32_t sleeptime) {
    uint32_t i;                //�ֲ����������ڼ���͹���ʱ��
    system_power_config();
    /* Disable Wakeup Counter */
    HAL_RTCEx_DeactivateWakeUpTimer(&hrtc);
    /*To configure the wake up timer to 4s the WakeUpCounter is set to 0x242B:
    RTC_WAKEUPCLOCK_RTCCLK_DIV = RTCCLK_Div16 = 16
    Wakeup Time Base = 16 /(~37KHz) = ~0,432 ms
    Wakeup Time = ~5s = 0,432ms  * WakeUpCounter
    ==> WakeUpCounter = ~5s/0,432ms = 11562 */
    i = sleeptime * 2396;
    HAL_RTCEx_SetWakeUpTimer_IT(&hrtc, i, RTC_WAKEUPCLOCK_RTCCLK_DIV16);
    /*Suspend Tick increment to prevent wakeup by Systick interrupt.
    Otherwise the Systick interrupt will wake up the device within 1ms (HAL time base)*/
    HAL_SuspendTick();
    /* Enter Sleep Mode , wake up is done once Key push button is pressed */
    HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
    /* Resume Tick interrupt if disabled prior to sleep mode entry*/
    HAL_ResumeTick();
    /* Disable Wakeup Counter */
    HAL_RTCEx_DeactivateWakeUpTimer(&hrtc);
}
//����STOPģʽ�͹��ģ�ʹ��RTC���ܻ��ѣ�����stoptime��λΪS��������1���͹���1�����
void enter_stop_rtc(uint32_t stoptime) {
    uint32_t i;                //�ֲ����������ڼ���͹���ʱ��
    system_power_config();
    /* Disable Wakeup Counter */
    HAL_RTCEx_DeactivateWakeUpTimer(&hrtc);
    /*To configure the wake up timer to 4s the WakeUpCounter is set to 0x242B:
    RTC_WAKEUPCLOCK_RTCCLK_DIV = RTCCLK_Div16 = 16
    Wakeup Time Base = 16 /(~37KHz) = ~0,432 ms
    Wakeup Time = ~5s = 0,432ms  * WakeUpCounter
    ==> WakeUpCounter = ~5s/0,432ms = 11562 */
    i = stoptime * 2396;
    HAL_RTCEx_SetWakeUpTimer_IT(&hrtc, i, RTC_WAKEUPCLOCK_RTCCLK_DIV16);
    /*Suspend Tick increment to prevent wakeup by Systick interrupt.
    Otherwise the Systick interrupt will wake up the device within 1ms (HAL time base)*/
    HAL_SuspendTick();
    /* Enter Stop Mode */
    HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);
    /* Resume Tick interrupt if disabled prior to sleep mode entry*/
    HAL_ResumeTick();
    SystemClock_Config();
    /*��Ҫ��ʼ�����Ӳ���豸 */
    /* Disable Wakeup Counter */
    HAL_RTCEx_DeactivateWakeUpTimer(&hrtc);
}

//����STANDBYģʽ�͹��ģ�ʹ��RTC���ܻ��ѣ�����standbytime��λΪS��������1���͹���1�����  ����������λ
void enter_standby_rtc(uint32_t standbytime) {
    uint32_t i;                //�ֲ����������ڼ���͹���ʱ��
    system_power_config();
    if(__HAL_PWR_GET_FLAG(PWR_FLAG_SB) != RESET) { //��鲢���ϵͳ�Ƿ�Ӵ���ģʽ�ָ�
        /* Clear Standby flag */
        __HAL_PWR_CLEAR_FLAG(PWR_FLAG_SB);
    }
    HAL_PWR_DisableWakeUpPin(PWR_WAKEUP_PIN1);//��������ʹ�õĻ���Դ:PWR_WAKEUP_PIN1 connected to PA.00
    HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1); //�������ӵ�PA.00��WakeUp Pin
    /* Disable Wakeup Counter */
    HAL_RTCEx_DeactivateWakeUpTimer(&hrtc);
    /*To configure the wake up timer to 4s the WakeUpCounter is set to 0x242B:
    RTC_WAKEUPCLOCK_RTCCLK_DIV = RTCCLK_Div16 = 16
    Wakeup Time Base = 16 /(~37KHz) = ~0,432 ms
    Wakeup Time = ~5s = 0,432ms  * WakeUpCounter
    ==> WakeUpCounter = ~5s/0,432ms = 11562 */
    i = standbytime * 2396;
    HAL_RTCEx_SetWakeUpTimer_IT(&hrtc, i, RTC_WAKEUPCLOCK_RTCCLK_DIV16);
    /* Clear all related wakeup flags */
    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);
    /* Enter the Standby mode */
    HAL_PWR_EnterSTANDBYMode();
}

//�͹��Ĺر���
void system_power_config(void) {
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    //�ر���������ʱ��
    /* Enable Power Control clock */
    __HAL_RCC_PWR_CLK_ENABLE();
    /* Enable Ultra low power mode */
    HAL_PWREx_EnableUltraLowPower();
    /* Enable the fast wake up from Ultra low power mode */
    HAL_PWREx_EnableFastWakeUp();
    /* ������stopģʽ�½��е��� */
    //HAL_DBGMCU_EnableDBGStopMode();
    /* Enable GPIOs clock */
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOH_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();
    /* Configure all GPIO port pins in Analog Input mode (floating input trigger OFF) */
    /* Note: Debug using ST-Link is not possible during the execution of this   */
    /*       example because communication between ST-link and the device       */
    /*       under test is done through UART. All GPIO pins are disabled (set   */
    /*       to analog input mode) including  UART I/O pins.           */
    GPIO_InitStructure.Pin = GPIO_PIN_All;
    GPIO_InitStructure.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStructure.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);
    HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);
    HAL_GPIO_Init(GPIOC, &GPIO_InitStructure);
    HAL_GPIO_Init(GPIOD, &GPIO_InitStructure);
    HAL_GPIO_Init(GPIOH, &GPIO_InitStructure);
    HAL_GPIO_Init(GPIOE, &GPIO_InitStructure);
    /* Disable GPIOs clock */
    __HAL_RCC_GPIOA_CLK_DISABLE();
    __HAL_RCC_GPIOB_CLK_DISABLE();
    __HAL_RCC_GPIOC_CLK_DISABLE();
    __HAL_RCC_GPIOD_CLK_DISABLE();
    __HAL_RCC_GPIOH_CLK_DISABLE();
    __HAL_RCC_GPIOE_CLK_DISABLE();
    //RTCʱ�ӳ�ʼ����Ҫ��ʵ�ֶ�ʱ���ѱ��뿪�������ж�
    User_RTC_Init();
}

void Flash_EnableReadProtection(void) {
    HAL_StatusTypeDef status = HAL_ERROR;
    FLASH_OBProgramInitTypeDef OBInit;
    __HAL_FLASH_PREFETCH_BUFFER_DISABLE();
    HAL_FLASHEx_OBGetConfig(&OBInit);
    MYLOG(INFO, "OBInit.RDPLevel = %x", OBInit.RDPLevel);
    if(OBInit.RDPLevel != OB_RDP_LEVEL_1) {
        OBInit.OptionType = OPTIONBYTE_RDP;
        OBInit.RDPLevel = OB_RDP_LEVEL_1;
        HAL_FLASH_Unlock();
        HAL_FLASH_OB_Unlock();
        HAL_FLASHEx_OBProgram(&OBInit);
        status = HAL_FLASHEx_OBProgram(&OBInit);
        if(status != HAL_OK) {
            MYLOG(INFO, "Flash_EnableReadProtection fail = %x", status);
        } else {
            MYLOG(INFO, "Flash_EnableReadProtection OK");
            HAL_FLASH_OB_Launch();//���д��ɹ�����λ����ѡ���ֽ�
            NVIC_SystemReset();
        }
        HAL_FLASH_OB_Lock();
        HAL_FLASH_Lock();
    }
    __HAL_FLASH_PREFETCH_BUFFER_ENABLE();
}

void Flash_DisableReadProtection(void) {
    HAL_StatusTypeDef status = HAL_ERROR;
    FLASH_OBProgramInitTypeDef OBInit;
    __HAL_FLASH_PREFETCH_BUFFER_DISABLE();
    HAL_FLASHEx_OBGetConfig(&OBInit);
    MYLOG(INFO, "OBInit.RDPLevel = %x", OBInit.RDPLevel);
    if(OBInit.RDPLevel != OB_RDP_LEVEL_0) {
        OBInit.OptionType = OPTIONBYTE_RDP;
        OBInit.RDPLevel = OB_RDP_LEVEL_0;
        HAL_FLASH_Unlock();
        HAL_FLASH_OB_Unlock();
        status = HAL_FLASHEx_OBProgram(&OBInit);
        if(status != HAL_OK) {
            MYLOG(INFO, "Flash_DisableReadProtection fail = %x", status);
        } else {
            MYLOG(INFO, "Flash_DisableReadProtection OK");
            HAL_FLASH_OB_Launch();//���д��ɹ�����λ����ѡ���ֽ�
            NVIC_SystemReset();
        }
        HAL_FLASH_OB_Lock();
        HAL_FLASH_Lock();
    }
    __HAL_FLASH_PREFETCH_BUFFER_ENABLE();
}


//�͹��Ĺر���
void system_power_config2(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    //�ر���������ʱ��
    /* Enable Power Control clock */
    __HAL_RCC_PWR_CLK_ENABLE();
    /* Enable Ultra low power mode */
    HAL_PWREx_EnableUltraLowPower();
    /* Enable the fast wake up from Ultra low power mode */
    HAL_PWREx_EnableFastWakeUp();
    /* ������stopģʽ�½��е��� */
    //HAL_DBGMCU_EnableDBGStopMode();
    /* Enable GPIOs clock */
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOH_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();
    /* Configure all GPIO port pins in Analog Input mode (floating input trigger OFF) */
    /* Note: Debug using ST-Link is not possible during the execution of this   */
    /*       example because communication between ST-link and the device       */
    /*       under test is done through UART. All GPIO pins are disabled (set   */
    /*       to analog input mode) including  UART I/O pins.           */
    GPIO_InitStruct.Pin = GPIO_PIN_All;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
    HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
    /* Disable GPIOs clock */
//    __HAL_RCC_GPIOA_CLK_DISABLE();
//    __HAL_RCC_GPIOB_CLK_DISABLE();
    __HAL_RCC_GPIOC_CLK_DISABLE();
    __HAL_RCC_GPIOD_CLK_DISABLE();
    __HAL_RCC_GPIOH_CLK_DISABLE();
    __HAL_RCC_GPIOE_CLK_DISABLE();
    //RTCʱ�ӳ�ʼ����Ҫ��ʵ�ֶ�ʱ���ѱ��뿪�������ж�
    //User_RTC_Init();
    HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET);//�ص�
    HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_SET);//�ص�
    HAL_GPIO_WritePin(GPIOB, AXP173_VBUSEN_Pin, GPIO_PIN_RESET);
    /*Configure GPIO pins : PBPin PBPin */
    GPIO_InitStruct.Pin =  LED1_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    /*Configure GPIO pins : PAPin PAPin */
    GPIO_InitStruct.Pin =  LED2_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    /*Configure GPIO pin : PtPin */
    GPIO_InitStruct.Pin = AXP173_VBUSEN_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(AXP173_VBUSEN_GPIO_Port, &GPIO_InitStruct);
    /*Configure GPIO pins : PBPin PBPin PBPin */
    GPIO_InitStruct.Pin =  ADXL345_INT2_Pin | ADXL345_INT1_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    /*Configure GPIO pin : PtPin */
    GPIO_InitStruct.Pin = KEY_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(KEY_GPIO_Port, &GPIO_InitStruct);
    /* EXTI interrupt init*/
    HAL_NVIC_SetPriority(EXTI9_5_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
    HAL_NVIC_SetPriority(EXTI15_10_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
}

void User_pwr_enter_lowpower(uint8_t mode) {
    if(mode == 0) { //SLEEPģʽ ���ڿ��Ի���
        ldo2_disable();//�ر�JY901
        ldo3_disable();//�ر�24L01
        ldo4_disable();//��hkj15c
        ADXL345_Get_State(3);
        ADXL345_Get_State(3);
        __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_4);
        __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_5);
        __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_8);
        __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_9);
//        HAL_UART_DeInit(&huart1);//����ʼ��
//        HAL_UART_DeInit(&huart2);//����ʼ��
//        HAL_I2C_DeInit(&hi2c1);  //����ʼ��
//        HAL_I2C_DeInit(&hi2c2);  //����ʼ��
//        HAL_SPI_DeInit(&hspi1);  //����ʼ��
        //system_power_config2();  //���³�ʼ��ʱ��
        HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET);//�ص�
        HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_SET);//�ص�
        osDelay(50);
        HAL_SuspendTick();
        HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
        HAL_ResumeTick();
//        User_USART_Init(USART1, &huart1, 9600);
//        User_USART_Init(USART1, &huart1, 115200);
        osDelay(50);
        HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET);//����
        HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_RESET);//����
        //User_SPI_Init(SPI1, &hspi1);
        NRF24L_Init();
        if((information.EquipmentType >= HOPEN_UL) && (information.EquipmentType <= HOPEN_DX)) {
            BSP_JY_901_Init();
            if((information.EquipmentType == HOPEN_DL) || (information.EquipmentType == HOPEN_DR || (information.EquipmentType == HOPEN_DX))) {
                BSP_HKJ_15C_Init();
            }
            NRF24L01_TxMode();
        }
        __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_4);
        __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_5);
        __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_8);
        __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_9);
        ADXL345_Get_State(3);
        ADXL345_Get_State(3);
    } else {        //STOPģʽ  �����޷����� �����ʼ��ʱ�� ��������
        //�رյ�Դ
        ldo2_disable();//�ر�JY901
        ldo3_disable();//�ر�24L01
        ldo4_disable();//��hkj15c
        ADXL345_Get_State(1);
        ADXL345_Get_State(0);
        HAL_UART_DeInit(&huart1);//����ʼ��
        HAL_UART_DeInit(&huart2);//����ʼ��
        HAL_I2C_DeInit(&hi2c1);  //����ʼ��
        HAL_I2C_DeInit(&hi2c2);  //����ʼ��
        HAL_SPI_DeInit(&hspi1);  //����ʼ��
        system_power_config2();  //���³�ʼ��ʱ��
        HAL_SuspendTick();
        HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);
        HAL_ResumeTick();
        NVIC_SystemReset();//ֱ�Ӹ�λ
    }
}

void User_pwr_test(void) {
    //NVIC_SystemReset();
    MYLOG(WARNING, "enter_sleep_rtc(5)");
    //enter_sleep_rtc(10);
    //enter_stop_rtc(10);
    //enter_standby_rtc(10);
    User_pwr_enter_lowpower(0);
    MYLOG(WARNING, "exit_sleep_rtc(5)");
}
/************************ Johnking *****END OF FILE****/
