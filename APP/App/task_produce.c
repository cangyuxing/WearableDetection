#include "task_produce.h"
TaskHandle_t  produce_Handler;

//启动任务
void Task_produce(void *p_arg) {
    //uint32_t Iwdgtimeout = 0;
    uint8_t RxData[33] = {0};
    uint8_t RxLenth = 0;
    uint8_t serialnumber[4] = {0};
    NRF24L01_RxMode();
    MYLOG(INFO, "Task_produce run!");
    for(;;) {
        RxLenth = NRF24L01_RxPacket(RxData);
        if(RxLenth != 0) { // 收到数据
            if(RxLenth != 33) {
                MYLOG(WARNING, "RxData[%d] Loselen = %d  RxLenth = %d", RxData[0], 33 - RxLenth, RxLenth); //丢包
            }
            if(serialnumber[RxData[0]] == 0) {        //第一包数据
                serialnumber[RxData[0]] = RxData[2];  //第一包同步一下包号
            } else {
                if(serialnumber[RxData[0]] != RxData[2]) {
                    serialnumber[RxData[0]]++;
                    if(serialnumber[RxData[0]] != RxData[2]) {
                        MYLOG(WARNING, "RxData[%d] Losepack = %d %d", RxData[0], serialnumber[RxData[0]], RxData[2]); //丢包
                    }
                }
            }
            NRF24L01_TxPacket_AP(&RxData[1], 32);//响应数据
            //调试
            serialnumber[RxData[0]] = RxData[2];
            MYLOGHEX(INFO, RxData, RxLenth);
            memset(RxData, 0, 33);
            RxLenth = 0;
        } else {
//        if(DelayTimeOut(&Iwdgtimeout, 500)) {
//            Iwdgtimeout = HAL_GetTick();
//            //User_Iwdg_Init(); //500ms定时刷新看门狗
//        }
            //Change_Run_State(1);//不让睡眠
            osDelay(1);
        }
    }
}

void Task_produce_Create(void) {
    xTaskCreate((TaskFunction_t)Task_produce,                 // 任务函数
                (const char*)"Task_produce",                      // 任务名称
                (uint16_t)128,                                    // 任务堆栈大小
                (void*)NULL,                                      // 传递给任务函数的参数
                (UBaseType_t)2,                                   // 任务优先级 越大优先级越高
                (TaskHandle_t*)&produce_Handler);                 // 任务句柄
}
