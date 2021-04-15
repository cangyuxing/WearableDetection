#include "task_jy901.h"
//**************************************************************************************
TaskHandle_t Task_JY901_Handler;
//QueueHandle_t JY_901_Queue;

//**************************************************************************************
//Task_JY901  ������
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
//Task_JY901_Create  ���񴴽�
void Task_JY901_Create(void) {
    // 1 > ��������
    JY_901_Queue = xQueueCreate(5, sizeof(JY_901_Upload));        // ���и���  ���д�С  ��������
    // 2 > ��������
    xTaskCreate((TaskFunction_t)Task_JY901,                       // ������
                (const char*)"Task_JY901",                        // ��������
                (uint16_t)128,                                    // �����ջ��С
                (void*)NULL,                                      // ���ݸ��������Ĳ���
                (UBaseType_t)2,                                   // �������ȼ� Խ�����ȼ�Խ��
                (TaskHandle_t*)&Task_JY901_Handler);              // ������
}
