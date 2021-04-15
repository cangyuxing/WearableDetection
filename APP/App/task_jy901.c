#include "task_jy901.h"
//**************************************************************************************
TaskHandle_t Task_JY901_Handler;
//QueueHandle_t JY_901_Queue;

//**************************************************************************************
//Task_JY901  任务处理
static void Task_JY901(void *p_arg) {
    Upload_DATA UploadDaTa;
    uint32_t tickstart = HAL_GetTick();
    for(;;) {
        tickstart = HAL_GetTick();
        if(xQueueReceive(JY_901_Queue, UploadDaTa.Data.Data, (TickType_t) 0)) {
            xSemaphoreTake(xSemaphore, portMAX_DELAY);
            Upload_Data_Check((void *)&UploadDaTa);
            NRF24L01_TxPacket((void *)&UploadDaTa);
            //printf("delay:%d  %d\r\n", (HAL_GetTick() - tickstart) / portTICK_RATE_MS, portTICK_RATE_MS);
            xSemaphoreGive(xSemaphore);
        }
        //sprintf((void *)TxData, "NRF24L01_huang%d", count++);
        //NRF24L01_TxPacket(TxData);
        osDelay(5);
    }
}
//**************************************************************************************
//Task_JY901_Create  任务创建
void Task_JY901_Create(void) {
    // 1 > 创建队列
    JY_901_Queue = xQueueCreate(5, sizeof(JY_901_Upload));        // 队列个数  队列大小  加速数据
    // 2 > 创建任务
    xTaskCreate((TaskFunction_t)Task_JY901,                       // 任务函数
                (const char*)"Task_JY901",                        // 任务名称
                (uint16_t)128,                                    // 任务堆栈大小
                (void*)NULL,                                      // 传递给任务函数的参数
                (UBaseType_t)2,                                   // 任务优先级 越大优先级越高
                (TaskHandle_t*)&Task_JY901_Handler);              // 任务句柄
}
