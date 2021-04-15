#include "app.h"
//**************************************************************************************
TaskHandle_t  Task_Base_Handler; //������
TimerHandle_t Time_Base_Handler; //��ʱ��
//SemaphoreHandle_t xSemaphore;  //�����ź������
//**************************************************************************************
GlobalVariable globalvariable;
DeviceInformation information;
DeviceInformation informationdef  = {
    .EquipmentName       = "HOPEN_Wearable", //R  �豸���ƣ�HOPEN_Wearable
    .SoftwareVersion     = "V1.0",           //R  ����汾: V1.0
    .SoftwareReleaseDate = "20210203",       //R  ����������� 20201215
    .EquipmentID         = "HOPEN_1234",     //RW �豸ID��HOPEN_1234
    .Password            = "admin",          //RW �������룺admin
    .EquipmentType       = HOPEN_UL,         //RW �豸���� HOPEN_DEF HOPEN_ECG
    .Channel             = 40,               //R  ����Ƶ��255
    .UpId                = 1,                //RW �ϴ�ͨѶID
    .Debugbak            = LOG_OFF,          //RW ����debug�ȼ� ������Ч

    .threshold           = 200,             //RW ��̬��ֵ
    .accuracy            = 30,              //RW ��̬����
    .algorithm           = 1,               //RW �Ʋ��㷨
};

void Parameter_Save(void) {
    information.crc = CalU8CRC((void *)&information, sizeof(DeviceInformation) - 1);
    STMEEPROM_Write(0, (void *)&information, sizeof(DeviceInformation));
    MYLOG(INFO, "Parameter_Save <%s><%x>", information.SoftwareVersion, information.crc);
}

void Parameter_Init(void) {
    User_USART_Init(USART2, &huart2, 115200);
    STMEEPROM_Read(0, (void *)&information, sizeof(DeviceInformation));
    if((CalU8CRC((void *)&information, sizeof(DeviceInformation)) != 0) || (information.SoftwareVersion[0] != 'V') || (information.algorithm == 0)) {
        information = informationdef;
        Parameter_Save();
        globalvariable.Debug     = information.Debugbak; // ���Եȼ� LOG_WARNING
        MYLOG(INFO, "Write informationdef <%s><%x>", information.SoftwareVersion, information.crc);
    }
    globalvariable.Debug         = information.Debugbak; // ���Եȼ� LOG_WARNING
    globalvariable.CfgFlag       = 0;        //T ���Կ���
    globalvariable.RestFlag      = 0;        //T ��λ
    globalvariable.SleepFlag     = 0;        //O ��λ��
    globalvariable.EquipmentFlag = 0;        //R ģ���־λ
    globalvariable.SleepFlag     = 0;        //˯��״̬
    globalvariable.RunFlag       = 0;        //����״̬   1�   2��ֹ
    globalvariable.RunTime       = 0;        //����״̬ʱ��
    globalvariable.steps         = 0;        //����
    globalvariable.HKJ_15C       = 0;        //HKJ_15C ��ʼ����־
    globalvariable.ADXL345_adj   = 1;        //������ҪУ׼һ��
    //���������豸
//    information.EquipmentType    = HOPEN_DEF; // �ĵ����� LOG_WARNING
//    globalvariable.Debug         = LOG_INFO;  // ���Եȼ� LOG_WARNING
    //�ĵ��豸
//    information.EquipmentType    = HOPEN_ECG; // �ĵ����� LOG_WARNING
//    globalvariable.Debug         = LOG_OFF;   // ���Եȼ� LOG_WARNING
//    globalvariable.Debug         = LOG_OFF;   // ���Եȼ� LOG_WARNING
}

//**************************************************************************************
//Time_Base  ������
void Task_Base(void *p_arg) {
    TickType_t lasttick = xTaskGetTickCount();
    //RTC_TimeTypeDef stimestructure;
    //RTC_DateTypeDef sdatestructure;
    //uint8_t *buf = pvPortMalloc(10);  //�����ڴ�
    //uint16_t adcx;
    MX_Struct_Test();
    //FLASH_EEPROM_Test();
    //Parameter_Init();
    HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET);//ͳһ�ص�
    HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_SET);//ͳһ�ص�
    ADXL345_Get_State(1);
    MYLOG(INFO, "Task_Base run!");
//    BSP_JY_901_Init();
    //information.RestFlag = 1;
    //User_pwr_test();
    Change_Run_State(1);
    User_Iwdg_Init();
    for(;;) {
        HAL_IWDG_Refresh(&hiwdg);
        if(globalvariable.RestFlag == 1) {
            globalvariable.RestFlag = 0;
            NVIC_SystemReset(); //�����λ
        }
        if((information.EquipmentType >= HOPEN_UL) && (information.EquipmentType <= HOPEN_DX)) {
            if(globalvariable.CfgFlag == 0) {//���ò�����ʱ���豸����ȥ�͹���
                if(globalvariable.SleepFlag == 1) {
                    osDelay(50);
                    MYLOG(WARNING, "Equipment enter sleep");
                    osDelay(50);
                    while(1) {
                        axp173_poweroff();
                        //User_pwr_enter_lowpower(0);
                        osDelay(500);
                        HAL_IWDG_Refresh(&hiwdg);
                    }
                    MYLOG(WARNING, "Equipment exit sleep");
                    //��Ӧ������ʼ��
                    osDelay(50);
                    ADXL345_clean_falg();
                    Change_Run_State(1);
                    globalvariable.SleepFlag = 0;
                }
            }
            if(globalvariable.HKJ_15C == 1) {
                globalvariable.HKJ_15C = 0;
                if((information.EquipmentType == HOPEN_DL) || (information.EquipmentType == HOPEN_DR) || (information.EquipmentType == HOPEN_DX)) {
                    globalvariable.Debug = 0;
                    BSP_HKJ_15C_Init();
                }
            }
            if(globalvariable.RunFlag == 2) { //��ֹ
                if(DelayTimeOut(&globalvariable.RunTime, 1000 * 1)) { //10s�Ժ���˯��
                    globalvariable.SleepFlag = 1;
                    globalvariable.RunFlag = 0; //�����ֹ״̬
                    globalvariable.RunTime = HAL_GetTick();
                    MYLOG(WARNING, "Equipment stationary");//���20sû���˶�˯��
                    
                    HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET);//ͳһ���� ���뾲ֹ״̬
                    HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_RESET);//ͳһ���� ���뾲ֹ״̬
                }
            } else {                           //��ⲽ��
                if(DelayTimeOut(&globalvariable.RunTime, 1000 * 9)) { //10s�Ժ���˯��
                    globalvariable.RunFlag = 2; //�����ֹ״̬
                    globalvariable.RunTime = HAL_GetTick();
                    MYLOG(WARNING, "Equipment time out");
                }
            }
            if((information.EquipmentType >= HOPEN_UL) && (information.EquipmentType <= HOPEN_DX)) {
                if(globalvariable.EquipmentFlag & adx345_flag) { //���ٶȺ�ʹ������Ӧ�Ĳ���
                    if(globalvariable.ADXL345_adj) {
                        ADXL345_Auto_Adjust();
                        globalvariable.ADXL345_adj = 0;
                    } else {
                        ADXL345_Step_Run();
                    }
                }
            }
        }
        LED1_DO();
        LED2_DO();
        KEY_Do();
        //ADXL345_Test();
        //ADX_Test();
        //ADXL345_See();
        //ADXL345_Test();
        //adcx=Get_Adc_Average(ADC_CHANNEL_1,10);//��ȡͨ��1��ת��ֵ��10��ȡƽ��
        //printf("adc=%d\r\n",adcx);
        //HAL_RTC_GetTime(&hrtc, &stimestructure, RTC_FORMAT_BIN);
        //HAL_RTC_GetDate(&hrtc, &sdatestructure, RTC_FORMAT_BIN);
        //printf("%02d-%02d-%02d ", 2000 + sdatestructure.Year, sdatestructure.Month, sdatestructure.Date);
        //printf("%02d:%02d:%02d\r\n", stimestructure.Hours, stimestructure.Minutes, stimestructure.Seconds);
        osDelay(20);
        //vTaskDelayUntil(&lasttick, 10);//250HZ
    }
}
//**************************************************************************************
//Time_Base  ��ʱ���ص�
uint32_t Timer_count;
void Time_Base(TimerHandle_t xTimer) {
    UBaseType_t uxSavedInterruptStatus;
    uxSavedInterruptStatus = portSET_INTERRUPT_MASK_FROM_ISR();
    Timer_count ++;
    upload_Queue_send();
    //printf("Timer_count = %d\r\n", Timer_count);
//    printf("\r\n");
//    printf("Task Start   = %ld\r\n", uxTaskGetStackHighWaterMark(Task_One_Handler));     //ʣ���ջ�ռ�
//    printf("Task Collect = %ld\r\n", uxTaskGetStackHighWaterMark(Task_Two_Handler));     //ʣ���ջ�ռ�
//    printf("Task func    = %ld\r\n", uxTaskGetStackHighWaterMark(Task_Three_Handler));   //ʣ���ջ�ռ�
//    printf("\r\n");
//    HAL_IWDG_Refresh(&hiwdg);
    portCLEAR_INTERRUPT_MASK_FROM_ISR(uxSavedInterruptStatus);
}
//**************************************************************************************
//Drivers_Init_AD8223  �ĵ�ɼ��豸��ʼ��
void Drivers_Init_AD8223(void) {
    User_USART_Init(USART2, &huart2, 115200);
    User_Adc_Init(ADC1, &hadc);
    User_AD8223_init();
    User_Iwdg_Init();
}
//**************************************************************************************
//Drivers_Init  ����֫�豸��ʼ��
void Drivers_Init(void) {
    User_USART_Init(USART1, &huart1, 115200);
    #if AnalogHardware
    User_I2C_Init(I2C1, &hi2c1);
    User_I2C_Init(I2C2, &hi2c2);
    #else
    IIC1_Init();
    IIC2_Init();
    #endif
    User_SPI_Init(SPI1, &hspi1);
    User_RTC_Init();
    axp173_init();
    ADXL345_Init();
    NRF24L_Init();
    if((information.EquipmentType >= HOPEN_UL) && (information.EquipmentType <= HOPEN_DX)) {
        BSP_JY_901_Init();
        if((information.EquipmentType == HOPEN_DL) || (information.EquipmentType == HOPEN_DR) || (information.EquipmentType == HOPEN_DX)) {
            BSP_HKJ_15C_Init();
        }
    }
    MYLOG(INFO, "Hardware initialization completed!");
}
//**************************************************************************************
//Task_Base_Create  ���񴴽�
void Task_Base_Create(void) {
    // 1 > ���������ź���
    //xSemaphore = xSemaphoreCreateMutex();
    // 2 > ���������ʱ��
    Time_Base_Handler = xTimerCreate("Timer Auto",                   // ����
                                     100 / portTICK_RATE_MS,         // ������Ҫʱ�� 100MS �ж�һ��
                                     pdTRUE,                         // ����ģʽ(�Զ���װ)
                                     0,
                                     Time_Base);  // �ص�����
    if(Time_Base_Handler != NULL) xTimerStart(Time_Base_Handler, 0);// ���������ʱ��
    // 2 > ��������
    xTaskCreate((TaskFunction_t)Task_Base,                          // ������
                (const char*)"Task func",                           // ��������
                (uint16_t)256,                                      // �����ջ��С
                (void*)NULL,                                        // ���ݸ��������Ĳ���
                (UBaseType_t)1,                                     // �������ȼ� Խ�����ȼ�Խ��
                (TaskHandle_t*)&Task_Base_Handler);                 // ������
    //vTaskStartScheduler();                                        // �����������
}
//**************************************************************************************
//Task_Create �����񴴽�
void Task_Create(void) {
    Parameter_Init();
    switch(information.EquipmentType) {
        case HOPEN_DEF://����ģʽ  Ĭ��ģʽ����ʼ��2�����ں�LED�Ƽ��� ����1��9600 ����2��115200��
            Drivers_Init();
            Task_Base_Create();     //��������
            Task_produce_Create();  //������������
            break;
        case HOPEN_UL://��֫�豸L
        case HOPEN_UR://��֫�豸R
        case HOPEN_DL://��֫�豸L
        case HOPEN_DR://��֫�豸R
        case HOPEN_DX://Ԥ���豸
            Drivers_Init();
            Task_Base_Create();     //��������
            Task_upload_Create();
            break;
        case HOPEN_ECG://�ĵ��豸          ������2 ADC LED���� ����1��9600 ����2��115200��
            Drivers_Init_AD8223();
            Task_AD8223_Create();   //�ĵ�ɼ�����
            break;
        default:
            while(1) {};
    }
}
//**************************************************************************************
