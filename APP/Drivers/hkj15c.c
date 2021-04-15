#include "hkj15c.h"
QueueHandle_t HKJ_15C_Queue;

BSP_UART_HKJ_15C       UART1_HKJ_15C;
BSP_UART_HKJ_15C       UART2_HKJ_15C;


void BSP_HKJ_15C_Start(uint8_t cksum ) {
    uint8_t cmd[]={0xFF,0xC6,0x03,cksum,0xA0}; //0xA3
    HAL_UART_Transmit(&huart2, cmd, 5, 0xffff);
}

void BSP_HKJ_15C_Stop(uint8_t cksum) {
    uint8_t cmd[]={0xFF,0xC6,0x03,cksum,0xA1}; //0xA4
    HAL_UART_Transmit(&huart2, cmd, 5, 0xffff);
}

void BSP_HKJ_15C_ID(uint8_t cksum ) {
    uint8_t cmd[]={0xFF,0xC6,0x03,cksum,0xA2}; //0xA5
    HAL_UART_Transmit(&huart2, cmd, 5, 0xffff);
}

void BSP_HKJ_15C_Date(uint8_t cksum ) {
    uint8_t cmd[]={0xFF,0xC6,0x03,cksum,0xA3}; //0xA6
    HAL_UART_Transmit(&huart2, cmd, 5, 0xffff);
}

void BSP_HKJ_15C_Cfg(uint8_t cksum, uint8_t x) {
    //补充通信协议
    //设置采样周期 
    //上位机至传感器 0xFF 0XC6 0x04 CKSUM 0xA9 x
    //传感器至上位机 0xFF 0xC6 0x03 CKSUM 0xA9
    //说明采样周期 
    //T=0.5ms*x（x的范围是1-255） 《AD》
    //<cksum = 0XAF x=0X02>   1ms
    //<cksum = 0XB1 x=0X04>   2ms 暂时5ms   25*n<100
    //<cksum = 0XB7 x=0X0A>   5ms
    //<cksum = 0XBD x=0X10>   8ms
    uint8_t cmd[]={0xFF,0XC6,0x04,cksum,0xA9,x}; //0xA5
    HAL_UART_Transmit(&huart2, cmd, 6, 0xffff);
}

void BSP_HKJ_15C_Init(void) {
    globalvariable.EquipmentFlag = ClrBit(globalvariable.EquipmentFlag,hkj15c_flag);
    MYLOG(INFO,"BSP_HKJ_15C_Init Start!= %x",globalvariable.EquipmentFlag);
    ldo4_enable();//打开JY901
    HAL_Delay(50);
    BSP_HKJ_15C_ID(0xA5);
    HAL_Delay(10);
    BSP_HKJ_15C_Date(0xA6);
    HAL_Delay(10);
    BSP_HKJ_15C_Cfg(0xB1,0X04);
    HAL_Delay(5);
    BSP_HKJ_15C_Start(0xA3);//启动测量
    HAL_Delay(5);
    MYLOG(INFO,"BSP_HKJ_15C_Init OK! = %x",globalvariable.EquipmentFlag);
}

//HKJ_15C_Upload HKJ_15C; //肌肉数据
//0 不做任何处理 1只显示 2只清除 3显示清除
void BSP_HKJ_15C_printf(BSP_UART_HKJ_15C * receivedata, uint8_t state) {
    if((state == 1) || (state == 3)) {
        uint8_t *pbuf = (uint8_t *)receivedata;
        MYLOGHEX(INFO, pbuf,(receivedata->DataSta & 0X3FFF));
    }
    if((state == 2) || (state == 3)) {
        //这里添加数据解析处理
        receivedata->DataSta = 0;//设置为1清除接收标准
    }
}
uint8_t BSP_Cheak_HKJ_15C(BSP_UART_HKJ_15C * receivedata) {
    uint16_t i = 0, sum = 0;
    uint16_t len = receivedata->Data.HKJ_15C.len;
    sum = len;
    for(i = 4; i < len + 2; i++) {
        sum += receivedata->Data.Data[i];
    }
    return sum;
}

void BSP_GET_HKJ_15C(BSP_UART_HKJ_15C * receivedata,void (*send)(QueueHandle_t Handle,uint8_t *pbuf)) {
    if(receivedata->Data.HKJ_15C.cmd == 0xa0) {//开始采样  肌电数据
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        send(HKJ_15C_Queue,(void*)&receivedata->Data.HKJ_15C.data[0]);
        send(HKJ_15C_Queue,(void*)&receivedata->Data.HKJ_15C.data[12]);
//        xQueueSendToFrontFromISR(HKJ_15C_Queue, (void*)&receivedata->Data.HKJ_15C.data[0],  &xHigherPriorityTaskWoken);  //0到12
//        xQueueSendToFrontFromISR(HKJ_15C_Queue, (void*)&receivedata->Data.HKJ_15C.data[12], &xHigherPriorityTaskWoken);  //12到24
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        
    }
    if(receivedata->Data.HKJ_15C.cmd == 0xa1) {//停止采样
        globalvariable.EquipmentFlag = SetBit(globalvariable.EquipmentFlag,hkj15c_flag);
        MYLOG(INFO,"HKJ_15C stop = %x",globalvariable.EquipmentFlag);
    }
    if(receivedata->Data.HKJ_15C.cmd == 0xa2) {//读设备号
        MYLOG(INFO,"HKJ_15C id");
    }
    if(receivedata->Data.HKJ_15C.cmd == 0xa3) {//生产日期
        MYLOG(INFO,"HKJ_15C date");
    }
    if(receivedata->Data.HKJ_15C.cmd == 0xa9) {//配置信息
        MYLOG(INFO,"HKJ_15C cfg");
    }
}
void BSP_GET_HKJ_15C_V1_0(BSP_UART_HKJ_15C * receivedata,void (*send)(QueueHandle_t Handle,uint8_t *pbuf)) {
    if(receivedata->Data.HKJ_15C.cmd == 0xa0) {//开始采样  肌电数据
        uint16_t i,temp;
        for(i=0;i<25;i++){
            temp = HTONS(receivedata->Data.HKJ_15C.data[i]);//转化为小端
            if(temp>UploadRx.J_max){//取最大值
                UploadRx.J_max = temp;
                //UploadRx.upload.JD_max = receivedata->Data.HKJ_15C.data[i];
            }
            if(temp<UploadRx.J_min){//取最小值
                UploadRx.J_min = temp;
                //UploadRx.upload.JD_min = receivedata->Data.HKJ_15C.data[i];
            }
        }
        UploadRx.Flag = (UploadRx.Flag & 0x0f) | 0x10;
    }
    if(receivedata->Data.HKJ_15C.cmd == 0xa1) {//停止采样
        globalvariable.EquipmentFlag = SetBit(globalvariable.EquipmentFlag,hkj15c_flag);
        MYLOG(INFO,"HKJ_15C stop");
    }
    if(receivedata->Data.HKJ_15C.cmd == 0xa2) {//读设备号
        globalvariable.EquipmentFlag = SetBit(globalvariable.EquipmentFlag,hkj15c_flag);
        MYLOG(INFO,"HKJ_15C id");
    }
    if(receivedata->Data.HKJ_15C.cmd == 0xa3) {//生产日期
        globalvariable.EquipmentFlag = SetBit(globalvariable.EquipmentFlag,hkj15c_flag);
        MYLOG(INFO,"HKJ_15C date");
    }
    if(receivedata->Data.HKJ_15C.cmd == 0xa9) {//配置参数
        globalvariable.EquipmentFlag = SetBit(globalvariable.EquipmentFlag,hkj15c_flag);
        MYLOG(INFO,"HKJ_15C cfg");
    }
}
void BSP_Receive_HKJ_15C(uint8_t pData, BSP_UART_HKJ_15C * receivedata) {
    if((receivedata->DataSta & 0x8000) == 0) {                            //接收未完成
        if((receivedata->DataSta & 0X3FFF) > 54)receivedata->DataSta = 0; //接收数据错误,重新开始接收
        receivedata->Data.Data[receivedata->DataSta & 0X3FFF] = pData ;
        receivedata->DataSta++;
        if((receivedata->DataSta & 0X3FFF) == 1) {
            if(receivedata->Data.HKJ_15C.head != 0xff) {
                receivedata->DataSta = 0; //接收数据错误,重新开始接收
            }
        } else if((receivedata->DataSta & 0X3FFF) == 2) {
            if((receivedata->Data.HKJ_15C.dev) != 0xc6) {
                receivedata->DataSta = 0; //接收数据错误,重新开始接收
            }
        } else if((receivedata->DataSta & 0X3FFF) == 3) {
        } else if((receivedata->DataSta & 0X3FFF) == (receivedata->Data.HKJ_15C.len + 2)) {
            if(BSP_Cheak_HKJ_15C(receivedata) == receivedata->Data.HKJ_15C.sum) {
                receivedata->DataSta |= 0x8000;        //接收完成了
                //BSP_GET_HKJ_15C(receivedata,BSP_QueueSend);
                BSP_GET_HKJ_15C_V1_0(receivedata,BSP_QueueSend);
                BSP_HKJ_15C_printf(receivedata, 2);
            } else {
                receivedata->DataSta = 0; //接收数据错误,重新开始接收
            }
        }
    }
}
