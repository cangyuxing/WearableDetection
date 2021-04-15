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
    //����ͨ��Э��
    //���ò������� 
    //��λ���������� 0xFF 0XC6 0x04 CKSUM 0xA9 x
    //����������λ�� 0xFF 0xC6 0x03 CKSUM 0xA9
    //˵���������� 
    //T=0.5ms*x��x�ķ�Χ��1-255�� ��AD��
    //<cksum = 0XAF x=0X02>   1ms
    //<cksum = 0XB1 x=0X04>   2ms ��ʱ5ms   25*n<100
    //<cksum = 0XB7 x=0X0A>   5ms
    //<cksum = 0XBD x=0X10>   8ms
    uint8_t cmd[]={0xFF,0XC6,0x04,cksum,0xA9,x}; //0xA5
    HAL_UART_Transmit(&huart2, cmd, 6, 0xffff);
}

void BSP_HKJ_15C_Init(void) {
    globalvariable.EquipmentFlag = ClrBit(globalvariable.EquipmentFlag,hkj15c_flag);
    MYLOG(INFO,"BSP_HKJ_15C_Init Start!= %x",globalvariable.EquipmentFlag);
    ldo4_enable();//��JY901
    HAL_Delay(50);
    BSP_HKJ_15C_ID(0xA5);
    HAL_Delay(10);
    BSP_HKJ_15C_Date(0xA6);
    HAL_Delay(10);
    BSP_HKJ_15C_Cfg(0xB1,0X04);
    HAL_Delay(5);
    BSP_HKJ_15C_Start(0xA3);//��������
    HAL_Delay(5);
    MYLOG(INFO,"BSP_HKJ_15C_Init OK! = %x",globalvariable.EquipmentFlag);
}

//HKJ_15C_Upload HKJ_15C; //��������
//0 �����κδ��� 1ֻ��ʾ 2ֻ��� 3��ʾ���
void BSP_HKJ_15C_printf(BSP_UART_HKJ_15C * receivedata, uint8_t state) {
    if((state == 1) || (state == 3)) {
        uint8_t *pbuf = (uint8_t *)receivedata;
        MYLOGHEX(INFO, pbuf,(receivedata->DataSta & 0X3FFF));
    }
    if((state == 2) || (state == 3)) {
        //����������ݽ�������
        receivedata->DataSta = 0;//����Ϊ1������ձ�׼
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
    if(receivedata->Data.HKJ_15C.cmd == 0xa0) {//��ʼ����  ��������
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        send(HKJ_15C_Queue,(void*)&receivedata->Data.HKJ_15C.data[0]);
        send(HKJ_15C_Queue,(void*)&receivedata->Data.HKJ_15C.data[12]);
//        xQueueSendToFrontFromISR(HKJ_15C_Queue, (void*)&receivedata->Data.HKJ_15C.data[0],  &xHigherPriorityTaskWoken);  //0��12
//        xQueueSendToFrontFromISR(HKJ_15C_Queue, (void*)&receivedata->Data.HKJ_15C.data[12], &xHigherPriorityTaskWoken);  //12��24
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        
    }
    if(receivedata->Data.HKJ_15C.cmd == 0xa1) {//ֹͣ����
        globalvariable.EquipmentFlag = SetBit(globalvariable.EquipmentFlag,hkj15c_flag);
        MYLOG(INFO,"HKJ_15C stop = %x",globalvariable.EquipmentFlag);
    }
    if(receivedata->Data.HKJ_15C.cmd == 0xa2) {//���豸��
        MYLOG(INFO,"HKJ_15C id");
    }
    if(receivedata->Data.HKJ_15C.cmd == 0xa3) {//��������
        MYLOG(INFO,"HKJ_15C date");
    }
    if(receivedata->Data.HKJ_15C.cmd == 0xa9) {//������Ϣ
        MYLOG(INFO,"HKJ_15C cfg");
    }
}
void BSP_GET_HKJ_15C_V1_0(BSP_UART_HKJ_15C * receivedata,void (*send)(QueueHandle_t Handle,uint8_t *pbuf)) {
    if(receivedata->Data.HKJ_15C.cmd == 0xa0) {//��ʼ����  ��������
        uint16_t i,temp;
        for(i=0;i<25;i++){
            temp = HTONS(receivedata->Data.HKJ_15C.data[i]);//ת��ΪС��
            if(temp>UploadRx.J_max){//ȡ���ֵ
                UploadRx.J_max = temp;
                //UploadRx.upload.JD_max = receivedata->Data.HKJ_15C.data[i];
            }
            if(temp<UploadRx.J_min){//ȡ��Сֵ
                UploadRx.J_min = temp;
                //UploadRx.upload.JD_min = receivedata->Data.HKJ_15C.data[i];
            }
        }
        UploadRx.Flag = (UploadRx.Flag & 0x0f) | 0x10;
    }
    if(receivedata->Data.HKJ_15C.cmd == 0xa1) {//ֹͣ����
        globalvariable.EquipmentFlag = SetBit(globalvariable.EquipmentFlag,hkj15c_flag);
        MYLOG(INFO,"HKJ_15C stop");
    }
    if(receivedata->Data.HKJ_15C.cmd == 0xa2) {//���豸��
        globalvariable.EquipmentFlag = SetBit(globalvariable.EquipmentFlag,hkj15c_flag);
        MYLOG(INFO,"HKJ_15C id");
    }
    if(receivedata->Data.HKJ_15C.cmd == 0xa3) {//��������
        globalvariable.EquipmentFlag = SetBit(globalvariable.EquipmentFlag,hkj15c_flag);
        MYLOG(INFO,"HKJ_15C date");
    }
    if(receivedata->Data.HKJ_15C.cmd == 0xa9) {//���ò���
        globalvariable.EquipmentFlag = SetBit(globalvariable.EquipmentFlag,hkj15c_flag);
        MYLOG(INFO,"HKJ_15C cfg");
    }
}
void BSP_Receive_HKJ_15C(uint8_t pData, BSP_UART_HKJ_15C * receivedata) {
    if((receivedata->DataSta & 0x8000) == 0) {                            //����δ���
        if((receivedata->DataSta & 0X3FFF) > 54)receivedata->DataSta = 0; //�������ݴ���,���¿�ʼ����
        receivedata->Data.Data[receivedata->DataSta & 0X3FFF] = pData ;
        receivedata->DataSta++;
        if((receivedata->DataSta & 0X3FFF) == 1) {
            if(receivedata->Data.HKJ_15C.head != 0xff) {
                receivedata->DataSta = 0; //�������ݴ���,���¿�ʼ����
            }
        } else if((receivedata->DataSta & 0X3FFF) == 2) {
            if((receivedata->Data.HKJ_15C.dev) != 0xc6) {
                receivedata->DataSta = 0; //�������ݴ���,���¿�ʼ����
            }
        } else if((receivedata->DataSta & 0X3FFF) == 3) {
        } else if((receivedata->DataSta & 0X3FFF) == (receivedata->Data.HKJ_15C.len + 2)) {
            if(BSP_Cheak_HKJ_15C(receivedata) == receivedata->Data.HKJ_15C.sum) {
                receivedata->DataSta |= 0x8000;        //���������
                //BSP_GET_HKJ_15C(receivedata,BSP_QueueSend);
                BSP_GET_HKJ_15C_V1_0(receivedata,BSP_QueueSend);
                BSP_HKJ_15C_printf(receivedata, 2);
            } else {
                receivedata->DataSta = 0; //�������ݴ���,���¿�ʼ����
            }
        }
    }
}
