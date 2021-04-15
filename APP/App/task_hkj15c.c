#include "task_hkj15c.h"
TaskHandle_t  Task_HKJ15C_Handler;
//QueueHandle_t HKJ_15C_Queue;

//**************************************************************************************
//Task_HKJ15C  ������
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
//Task_HKJ15C_Create  ���񴴽�
void Task_HKJ15C_Create(void) {
    // 1 > ��������
    HKJ_15C_Queue = xQueueCreate(5, sizeof(HKJ_15C_Upload));      // ���и���  ���д�С  ��������
    // 2 > ��������
    xTaskCreate((TaskFunction_t)Task_HKJ15C,                         // ������
                (const char*)"Task_HKJ15C",                      // ��������
                (uint16_t)256,                                    // �����ջ��С
                (void*)NULL,                                      // ���ݸ��������Ĳ���
                (UBaseType_t)2,                                   // �������ȼ� Խ�����ȼ�Խ��
                (TaskHandle_t*)&Task_HKJ15C_Handler);                // ������
}
