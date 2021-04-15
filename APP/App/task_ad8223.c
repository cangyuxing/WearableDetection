#include "task_ad8223.h"
TaskHandle_t  Task_AD8223_Handler;
TimerHandle_t Time_AD8223_Handler;

//**************************************************************************************
//Task_AD8223  ������
void Task_AD8223(void *p_arg) {
    TickType_t lasttick = xTaskGetTickCount();
    uint8_t pbuf[4] = {0XFE, 0XFE, 0X80, 0};
    uint16_t AdcValue, state;
    MYLOG(INFO, "Task_AD8223 run!");
    for(;;) {
        state = Get_AD8223_ConnectStatus();
        //pbuf[3] = state;
        //printf("��������״̬:%d\r\n", state);
        if(state == 0) {
            AdcValue = Get_Adc(1);
            //AdcValue = kalman_filter(AdcValue);
            //AdcValue = kalman_filter_y(AdcValue);
            pbuf[2] = AdcValue >> 8;
            pbuf[3] = AdcValue;
            //lfADC_Value = ((double)sADC_Value * 3.3) / 4096.0;
            //printf("%d ", AdcValue);
            //printf("�ɼ���ѹ:%04f\r\n", lfADC_Value);
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
//Time_AD822  ��ʱ���ص�
void Time_AD822(TimerHandle_t xTimer) {
    Led_Sate_Run();
    //HAL_IWDG_Refresh(&hiwdg);
}
//**************************************************************************************
//Task_AD8223_Create  ���񴴽�
void Task_AD8223_Create(void) {
    // 1 > ���������ʱ��
    Time_AD8223_Handler = xTimerCreate("Time_AD822",              // ����
                                    100 / portTICK_RATE_MS,       // ������Ҫʱ�� 100MS
                                    pdTRUE,                       // ����ģʽ(�Զ���װ)
                                    0,
                                    Time_AD822);        // �ص�����
    if(Time_AD8223_Handler != NULL) xTimerStart(Time_AD8223_Handler, 0);// ���������ʱ��
    // 2 > ��������
    xTaskCreate((TaskFunction_t)Task_AD8223,                      // ������
                (const char*)"Task_AD8223",                        // ��������
                (uint16_t)128,                                    // �����ջ��С
                (void*)NULL,                                      // ���ݸ��������Ĳ���
                (UBaseType_t)3,                                   // �������ȼ� Խ�����ȼ�Խ��
                (TaskHandle_t*)&Task_AD8223_Handler);                // ������
}
