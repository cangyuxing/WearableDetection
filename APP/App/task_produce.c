#include "task_produce.h"
TaskHandle_t  produce_Handler;

//��������
void Task_produce(void *p_arg) {
    //uint32_t Iwdgtimeout = 0;
    uint8_t RxData[33] = {0};
    uint8_t RxLenth = 0;
    uint8_t serialnumber[4] = {0};
    NRF24L01_RxMode();
    MYLOG(INFO, "Task_produce run!");
    for(;;) {
        RxLenth = NRF24L01_RxPacket(RxData);
        if(RxLenth != 0) { // �յ�����
            if(RxLenth != 33) {
                MYLOG(WARNING, "RxData[%d] Loselen = %d  RxLenth = %d", RxData[0], 33 - RxLenth, RxLenth); //����
            }
            if(serialnumber[RxData[0]] == 0) {        //��һ������
                serialnumber[RxData[0]] = RxData[2];  //��һ��ͬ��һ�°���
            } else {
                if(serialnumber[RxData[0]] != RxData[2]) {
                    serialnumber[RxData[0]]++;
                    if(serialnumber[RxData[0]] != RxData[2]) {
                        MYLOG(WARNING, "RxData[%d] Losepack = %d %d", RxData[0], serialnumber[RxData[0]], RxData[2]); //����
                    }
                }
            }
            NRF24L01_TxPacket_AP(&RxData[1], 32);//��Ӧ����
            //����
            serialnumber[RxData[0]] = RxData[2];
            MYLOGHEX(INFO, RxData, RxLenth);
            memset(RxData, 0, 33);
            RxLenth = 0;
        } else {
//        if(DelayTimeOut(&Iwdgtimeout, 500)) {
//            Iwdgtimeout = HAL_GetTick();
//            //User_Iwdg_Init(); //500ms��ʱˢ�¿��Ź�
//        }
            //Change_Run_State(1);//����˯��
            osDelay(1);
        }
    }
}

void Task_produce_Create(void) {
    xTaskCreate((TaskFunction_t)Task_produce,                 // ������
                (const char*)"Task_produce",                      // ��������
                (uint16_t)128,                                    // �����ջ��С
                (void*)NULL,                                      // ���ݸ��������Ĳ���
                (UBaseType_t)2,                                   // �������ȼ� Խ�����ȼ�Խ��
                (TaskHandle_t*)&produce_Handler);                 // ������
}
