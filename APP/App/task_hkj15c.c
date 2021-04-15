#include "task_hkj15c.h"
TaskHandle_t  Task_HKJ15C_Handler;
//QueueHandle_t HKJ_15C_Queue;

//**************************************************************************************
//Task_HKJ15C  任务处理
static void Task_HKJ15C(void *p_arg) {
    Upload_DATA UploadDaTa;
    uint32_t tickstart = HAL_GetTick();
    for(;;) {
        tickstart = HAL_GetTick();
        if(xQueueReceive(HKJ_15C_Queue, UploadDaTa.Data.Data, (TickType_t) 0)) {
            xSemaphoreTake(xSemaphore, portMAX_DELAY);
            Upload_Data_Check((void *)&UploadDaTa);
            NRF24L01_TxPacket((void *)&UploadDaTa);
            //printf("delay:%d  %d\r\n", (HAL_GetTick() - tickstart) / portTICK_RATE_MS, portTICK_RATE_MS);
            xSemaphoreGive(xSemaphore);
        }
        osDelay(5);
    }
}
//**************************************************************************************
//Task_HKJ15C_Create  任务创建
void Task_HKJ15C_Create(void) {
    // 1 > 创建队列
    HKJ_15C_Queue = xQueueCreate(5, sizeof(HKJ_15C_Upload));      // 队列个数  队列大小  肌肉数据
    // 2 > 创建任务
    xTaskCreate((TaskFunction_t)Task_HKJ15C,                         // 任务函数
                (const char*)"Task_HKJ15C",                      // 任务名称
                (uint16_t)256,                                    // 任务堆栈大小
                (void*)NULL,                                      // 传递给任务函数的参数
                (UBaseType_t)2,                                   // 任务优先级 越大优先级越高
                (TaskHandle_t*)&Task_HKJ15C_Handler);                // 任务句柄
}
