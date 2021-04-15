#include "app.h"
//**************************************************************************************
TaskHandle_t  Task_Base_Handler; //任务句柄
TimerHandle_t Time_Base_Handler; //定时器
//SemaphoreHandle_t xSemaphore;  //互斥信号量句柄
//**************************************************************************************
GlobalVariable globalvariable;
DeviceInformation information;
DeviceInformation informationdef  = {
    .EquipmentName       = "HOPEN_Wearable", //R  设备名称：HOPEN_Wearable
    .SoftwareVersion     = "V1.0",           //R  软件版本: V1.0
    .SoftwareReleaseDate = "20210203",       //R  软件发布日期 20201215
    .EquipmentID         = "HOPEN_1234",     //RW 设备ID：HOPEN_1234
    .Password            = "admin",          //RW 调试密码：admin
    .EquipmentType       = HOPEN_UL,         //RW 设备类型 HOPEN_DEF HOPEN_ECG
    .Channel             = 40,               //R  发送频率255
    .UpId                = 1,                //RW 上传通讯ID
    .Debugbak            = LOG_OFF,          //RW 缓存debug等级 重启生效

    .threshold           = 200,             //RW 动态阀值
    .accuracy            = 30,              //RW 动态精度
    .algorithm           = 1,               //RW 计步算法
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
        globalvariable.Debug     = information.Debugbak; // 调试等级 LOG_WARNING
        MYLOG(INFO, "Write informationdef <%s><%x>", information.SoftwareVersion, information.crc);
    }
    globalvariable.Debug         = information.Debugbak; // 调试等级 LOG_WARNING
    globalvariable.CfgFlag       = 0;        //T 调试开关
    globalvariable.RestFlag      = 0;        //T 复位
    globalvariable.SleepFlag     = 0;        //O 复位标
    globalvariable.EquipmentFlag = 0;        //R 模板标志位
    globalvariable.SleepFlag     = 0;        //睡眠状态
    globalvariable.RunFlag       = 0;        //开机状态   1活动   2静止
    globalvariable.RunTime       = 0;        //开机状态时间
    globalvariable.steps         = 0;        //步数
    globalvariable.HKJ_15C       = 0;        //HKJ_15C 初始化标志
    globalvariable.ADXL345_adj   = 1;        //开机需要校准一下
    //生产调试设备
//    information.EquipmentType    = HOPEN_DEF; // 心电设置 LOG_WARNING
//    globalvariable.Debug         = LOG_INFO;  // 调试等级 LOG_WARNING
    //心电设备
//    information.EquipmentType    = HOPEN_ECG; // 心电设置 LOG_WARNING
//    globalvariable.Debug         = LOG_OFF;   // 调试等级 LOG_WARNING
//    globalvariable.Debug         = LOG_OFF;   // 调试等级 LOG_WARNING
}

//**************************************************************************************
//Time_Base  任务处理
void Task_Base(void *p_arg) {
    TickType_t lasttick = xTaskGetTickCount();
    //RTC_TimeTypeDef stimestructure;
    //RTC_DateTypeDef sdatestructure;
    //uint8_t *buf = pvPortMalloc(10);  //申请内存
    //uint16_t adcx;
    MX_Struct_Test();
    //FLASH_EEPROM_Test();
    //Parameter_Init();
    HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET);//统一关灯
    HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_SET);//统一关灯
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
            NVIC_SystemReset(); //软件复位
        }
        if((information.EquipmentType >= HOPEN_UL) && (information.EquipmentType <= HOPEN_DX)) {
            if(globalvariable.CfgFlag == 0) {//配置参数的时候，设备不进去低功耗
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
                    //相应参数初始化
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
            if(globalvariable.RunFlag == 2) { //静止
                if(DelayTimeOut(&globalvariable.RunTime, 1000 * 1)) { //10s以后再睡眠
                    globalvariable.SleepFlag = 1;
                    globalvariable.RunFlag = 0; //清除静止状态
                    globalvariable.RunTime = HAL_GetTick();
                    MYLOG(WARNING, "Equipment stationary");//差不多20s没有运动睡眠
                    
                    HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET);//统一开灯 进入静止状态
                    HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_RESET);//统一开灯 进入静止状态
                }
            } else {                           //检测步伐
                if(DelayTimeOut(&globalvariable.RunTime, 1000 * 9)) { //10s以后再睡眠
                    globalvariable.RunFlag = 2; //清除静止状态
                    globalvariable.RunTime = HAL_GetTick();
                    MYLOG(WARNING, "Equipment time out");
                }
            }
            if((information.EquipmentType >= HOPEN_UL) && (information.EquipmentType <= HOPEN_DX)) {
                if(globalvariable.EquipmentFlag & adx345_flag) { //加速度好使才做相应的操作
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
        //adcx=Get_Adc_Average(ADC_CHANNEL_1,10);//获取通道1的转换值，10次取平均
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
//Time_Base  定时器回调
uint32_t Timer_count;
void Time_Base(TimerHandle_t xTimer) {
    UBaseType_t uxSavedInterruptStatus;
    uxSavedInterruptStatus = portSET_INTERRUPT_MASK_FROM_ISR();
    Timer_count ++;
    upload_Queue_send();
    //printf("Timer_count = %d\r\n", Timer_count);
//    printf("\r\n");
//    printf("Task Start   = %ld\r\n", uxTaskGetStackHighWaterMark(Task_One_Handler));     //剩余堆栈空间
//    printf("Task Collect = %ld\r\n", uxTaskGetStackHighWaterMark(Task_Two_Handler));     //剩余堆栈空间
//    printf("Task func    = %ld\r\n", uxTaskGetStackHighWaterMark(Task_Three_Handler));   //剩余堆栈空间
//    printf("\r\n");
//    HAL_IWDG_Refresh(&hiwdg);
    portCLEAR_INTERRUPT_MASK_FROM_ISR(uxSavedInterruptStatus);
}
//**************************************************************************************
//Drivers_Init_AD8223  心电采集设备初始化
void Drivers_Init_AD8223(void) {
    User_USART_Init(USART2, &huart2, 115200);
    User_Adc_Init(ADC1, &hadc);
    User_AD8223_init();
    User_Iwdg_Init();
}
//**************************************************************************************
//Drivers_Init  上下肢设备初始化
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
//Task_Base_Create  任务创建
void Task_Base_Create(void) {
    // 1 > 创建互斥信号量
    //xSemaphore = xSemaphoreCreateMutex();
    // 2 > 创建软件定时器
    Time_Base_Handler = xTimerCreate("Timer Auto",                   // 名字
                                     100 / portTICK_RATE_MS,         // 重载需要时间 100MS 中断一次
                                     pdTRUE,                         // 重载模式(自动重装)
                                     0,
                                     Time_Base);  // 回调函数
    if(Time_Base_Handler != NULL) xTimerStart(Time_Base_Handler, 0);// 启动软件定时器
    // 2 > 创建任务
    xTaskCreate((TaskFunction_t)Task_Base,                          // 任务函数
                (const char*)"Task func",                           // 任务名称
                (uint16_t)256,                                      // 任务堆栈大小
                (void*)NULL,                                        // 传递给任务函数的参数
                (UBaseType_t)1,                                     // 任务优先级 越大优先级越高
                (TaskHandle_t*)&Task_Base_Handler);                 // 任务句柄
    //vTaskStartScheduler();                                        // 开启任务调度
}
//**************************************************************************************
//Task_Create 总任务创建
void Task_Create(void) {
    Parameter_Init();
    switch(information.EquipmentType) {
        case HOPEN_DEF://生产模式  默认模式《初始化2个串口和LED灯即可 串口1：9600 串口2：115200》
            Drivers_Init();
            Task_Base_Create();     //基础任务
            Task_produce_Create();  //生产配置任务
            break;
        case HOPEN_UL://上肢设备L
        case HOPEN_UR://上肢设备R
        case HOPEN_DL://下肢设备L
        case HOPEN_DR://下肢设备R
        case HOPEN_DX://预留设备
            Drivers_Init();
            Task_Base_Create();     //基础任务
            Task_upload_Create();
            break;
        case HOPEN_ECG://心电设备          《串口2 ADC LED即可 串口1：9600 串口2：115200》
            Drivers_Init_AD8223();
            Task_AD8223_Create();   //心电采集任务
            break;
        default:
            while(1) {};
    }
}
//**************************************************************************************
