#include "task_upload.h"
TaskHandle_t  upload_Handler;
QueueHandle_t upload_Queue;

Upload_calculation UploadRx;  //接收数据

void Upload_calculation_init(void) {
    UploadRx.A_max   = 0;
    UploadRx.A_min   = 0xffffffff;
    UploadRx.J_max   = 0;
    UploadRx.J_min   = 0xffff;
    UploadRx.Flag    = 0;
    UploadRx.R_count = 0;
    UploadRx.Rx_add  = 0;
    UploadRx.Ry_add  = 0;
    UploadRx.Rz_add  = 0;
    UploadRx.vbat    = 0;
}

void upload_Queue_send(void) {
    if(UploadRx.R_count > 0) {
        uint16_t Current, Voltage;
        Upload upload;
        upload.Ax_max = HTONS(UploadRx.upload.Ax_max);
        upload.Ay_max = HTONS(UploadRx.upload.Ay_max);
        upload.Az_max = HTONS(UploadRx.upload.Az_max);
        upload.Ax_min = HTONS(UploadRx.upload.Ax_min);
        upload.Ay_min = HTONS(UploadRx.upload.Ay_min);
        upload.Az_min = HTONS(UploadRx.upload.Az_min);
        upload.Rx_mid = HTONS(UploadRx.Rx_add);
        upload.Ry_mid = HTONS(UploadRx.Ry_add);
        upload.Rz_mid = HTONS(UploadRx.Rz_add);
//        upload.Rx_mid = HTONS(UploadRx.Rx_add / UploadRx.R_count);
//        upload.Ry_mid = HTONS(UploadRx.Ry_add / UploadRx.R_count);
//        upload.Rz_mid = HTONS(UploadRx.Rz_add / UploadRx.R_count);
        if((information.EquipmentType == HOPEN_DL) || (information.EquipmentType == HOPEN_DR) || (information.EquipmentType == HOPEN_DX)) {
            if((UploadRx.Flag & 0xf0) != 0x10) return;
            upload.JD_max = HTONS(UploadRx.J_max);
            upload.JD_min = HTONS(UploadRx.J_min);
        } else {
            upload.JD_max = 0;
            upload.JD_min = 0;
        }
        if((UploadRx.Flag & 0x0f) != 0x05) return;
        Voltage = getVoltage();
        Current = getCurrent();
        MYLOG(DEBUG, "Voltage = %d Current = %d", Voltage, Current);
        upload.vbat = Voltage / 100;
        //upload.StepbyStep = HTONS(UploadRx.StepbyStep);//globalvariable.steps
        upload.StepbyStep = HTONS(globalvariable.steps);//globalvariable.steps
        BSP_QueueSend(upload_Queue, (void *)&upload);
        Upload_calculation_init();
    }
}

void Upload_pack(Upload_DATA_V1_0 *UploadDaTa) {
    uint8_t times = 0, state = 0, *p = (uint8_t *)UploadDaTa;
    UploadDaTa->head = 0xaa;     //0xaa
    UploadDaTa->id = 1 ;         //需要设置同套固定
    UploadDaTa->id = HTONS(UploadDaTa->id);
    UploadDaTa->cmd = 0x01;
    UploadDaTa->tail = 0x55;
    Upload_Data_Check_V_0((void *)UploadDaTa);
    if(globalvariable.SleepFlag == 1) return; //睡眠状态
    if(globalvariable.RunFlag   == 2) return; //静止状态
    while(1) {
        state = NRF24L01_TxPacket((void *)UploadDaTa, 32);
        //MYLOG(WARNING, "Upload_pack=%x",state);
        if(state == NRF24L01_STATUS_TX_DS) {
            Change_Run_State(1);//只要主机在线，就不睡眠
            break;
        }
        if(times++ > 3) {
            MYLOG(INFO, "Upload_pack no ack");
            break;
        }
        //osDelay(1);
        osDelay(information.EquipmentType);//延时重发时间
    }
    MYLOGHEX(INFO, p, 32);
    UploadDaTa->serialnumber++;
    //添加接收部分函数
    #if (NRF24L01_Mode == 1)
    uint8_t RxData[33] = {0};
    uint8_t RxLenth = 0;
    RxLenth = NRF24L01_RxPacket(RxData);
    if(RxLenth != 0) {
        MYLOGHEX(WARNING, RxData, RxLenth);
        memset(RxData, 0, 33);
        RxLenth = 0;
    }
    #endif
}

//**************************************************************************************
uint8_t Get_random(uint8_t Type) {
    if(Type > 4) return 50;
    uint8_t Get_random[] = {59, 71, 79, 89, 97};
    return Get_random[Type];
}
//Task_JY901  任务处理
static void Task_upload(void *p_arg) {
    Upload_DATA_V1_0 UploadDaTa;
    NRF24L01_TxMode();
    MYLOG(INFO, "Task_upload run!");
    for(;;) {
        if(xQueueReceive(upload_Queue, UploadDaTa.Data.Data, (TickType_t) 50)) {
            uint32_t tickstart = HAL_GetTick();
            //xSemaphoreTake(xSemaphore, portMAX_DELAY);
            //taskENTER_CRITICAL();
            Upload_pack(&UploadDaTa);
            //taskEXIT_CRITICAL();
//            NRF24L01_Write_Reg(NRF24L01_WRITE_REG | NRF24L01_RF_CH, 40);
//            Upload_pack(&UploadDaTa);
//            NRF24L01_Write_Reg(NRF24L01_WRITE_REG | NRF24L01_RF_CH, 60);
//            Upload_pack(&UploadDaTa);
//            UploadDaTa.serialnumber++;
//            MYLOG(WARNING, "delay:%d  %d", (HAL_GetTick() - tickstart) / portTICK_RATE_MS, portTICK_RATE_MS);
            //xSemaphoreGive(xSemaphore);
        }
        //HAL_IWDG_Refresh(&hiwdg);
        //osDelay(50);
        HAL_IWDG_Refresh(&hiwdg);
        osDelay(Get_random(information.EquipmentType));
    }
}
//**************************************************************************************
//Task_JY901_Create  任务创建
void Task_upload_Create(void) {
    // 1 > 创建队列
    upload_Queue = xQueueCreate(5, sizeof(Upload));               // 队列个数  队列大小  加速数据
    // 2 > 创建任务
    xTaskCreate((TaskFunction_t)Task_upload,                      // 任务函数
                (const char*)"Task_JY901",                        // 任务名称
                (uint16_t)256,                                    // 任务堆栈大小
                (void*)NULL,                                      // 传递给任务函数的参数
                (UBaseType_t)2,                                   // 任务优先级 越大优先级越高
                (TaskHandle_t*)&upload_Handler);                  // 任务句柄
}
