#include "task_ad8223.h"
TaskHandle_t  Task_AD8223_Handler;
TimerHandle_t Time_AD8223_Handler;

//**************************************************************************************
//Task_AD8223  任务处理
void Task_AD8223(void *p_arg) {
    TickType_t lasttick = xTaskGetTickCount();
    uint8_t pbuf[4] = {0XFE, 0XFE, 0X80, 0};
    uint16_t AdcValue, state;
    MYLOG(INFO, "Task_AD8223 run!");
    for(;;) {
        state = Get_AD8223_ConnectStatus();
        //pbuf[3] = state;
        //printf("导联连接状态:%d\r\n", state);
        if(state == 0) {
            AdcValue = Get_Adc(1);
            //AdcValue = kalman_filter(AdcValue);
            //AdcValue = kalman_filter_y(AdcValue);
            pbuf[2] = AdcValue >> 8;
            pbuf[3] = AdcValue;
            //lfADC_Value = ((double)sADC_Value * 3.3) / 4096.0;
            //printf("%d ", AdcValue);
            //printf("采集电压:%04f\r\n", lfADC_Value);
        } else {
            pbuf[2] = 0X80;
            pbuf[3] = state;
        }
        //USART_SendArray(USART2, &pbuf[0], 4);
        
        HAL_UART_Transmit(&huart2, &pbuf[0], 4, 200);
        //HAL_UART_Transmit(&huart2, &pbuf[2], 2, 200);
        HAL_IWDG_Refresh(&hiwdg);
        vTaskDelayUntil(&lasttick, 2);//250HZ 500HZ
        //osDelay(2);
    }
}
//**************************************************************************************
//Time_AD822  定时器回调
void Time_AD822(TimerHandle_t xTimer) {
    Led_Sate_Run();
    //HAL_IWDG_Refresh(&hiwdg);
}
//**************************************************************************************
//Task_AD8223_Create  任务创建
void Task_AD8223_Create(void) {
    // 1 > 创建软件定时器
    Time_AD8223_Handler = xTimerCreate("Time_AD822",              // 名字
                                    100 / portTICK_RATE_MS,       // 重载需要时间 100MS
                                    pdTRUE,                       // 重载模式(自动重装)
                                    0,
                                    Time_AD822);        // 回调函数
    if(Time_AD8223_Handler != NULL) xTimerStart(Time_AD8223_Handler, 0);// 启动软件定时器
    // 2 > 创建任务
    xTaskCreate((TaskFunction_t)Task_AD8223,                      // 任务函数
                (const char*)"Task_AD8223",                        // 任务名称
                (uint16_t)128,                                    // 任务堆栈大小
                (void*)NULL,                                      // 传递给任务函数的参数
                (UBaseType_t)3,                                   // 任务优先级 越大优先级越高
                (TaskHandle_t*)&Task_AD8223_Handler);                // 任务句柄
}
