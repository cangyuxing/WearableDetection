#include "jy901.h"
QueueHandle_t JY_901_Queue;

BSP_UART_JY_901        UART1_JY_901;
BSP_UART_JY_901        UART2_JY_901;
//JY_901_Upload          JY_901;  //���ٶ�����

void BSP_JY_901_Stop_Start(void) {
    uint8_t cmd[]={0xFF,0xAA,0x22,0x01,0x00};
    HAL_UART_Transmit(&huart1, cmd, 5, 0xffff);
}

void BSP_JY_901_save_cfg(uint8_t save) { //���Զ�����
    //save = 0 ����  save = 1 Ĭ�� 
    uint8_t cmd[]={0xFF,0xAA,0x00,save,0x00};
    HAL_UART_Transmit(&huart1, cmd, 5, 0xffff);
}

void BSP_JY_901_data_cfg(uint8_t rswl,uint8_t rswh) {
    //rswl 0x50~0x57  0:�����  1:���
    //rswh 0x58~0x5a  0:�����  1:���
    //��ʱֻ��Ҫ51 53  ������rswh = 0  rswl = 0x0a  ����
    uint8_t cmd[]={0xFF,0xAA,0x02,rswl,rswh};
    HAL_UART_Transmit(&huart1, cmd, 5, 0xffff);
}

void BSP_JY_901_data_hz(uint8_t rate) {
    //0x06��10Hz��Ĭ�ϣ�
    uint8_t cmd[]={0xFF,0xAA,0x03,rate,0x00};
    HAL_UART_Transmit(&huart1, cmd, 5, 0xffff);
}

void BSP_JY_901_baud(uint8_t baud) {
    //0x02��9600��Ĭ�ϣ� 0x06��115200
    uint8_t cmd[]={0xFF,0xAA,0x04,baud,0x00};
    HAL_UART_Transmit(&huart1, cmd, 5, 0xffff);
}

void BSP_JY_901_Init(void) {
    globalvariable.EquipmentFlag = ClrBit(globalvariable.EquipmentFlag,jiy901_flag);
    MYLOG(INFO,"BSP_JY_901_Init Start!= %x",globalvariable.EquipmentFlag);
    ldo2_enable();//��JY901
    HAL_Delay(100);
//    BSP_JY_901_data_cfg(0x0a,0x00);  //�����������
//    HAL_Delay(10);
//    BSP_JY_901_data_hz(0x08);        //����Ϊ50hz
//    HAL_Delay(10);
//    BSP_JY_901_save_cfg(0x00);       //��������
//    HAL_Delay(50);
//    BSP_JY_901_baud(6);            //����Ϊ115200
//    HAL_Delay(50);
    MYLOG(INFO,"BSP_JY_901_Init OK! = %x",globalvariable.EquipmentFlag);
}

//0 �����κδ��� 1ֻ��ʾ 2ֻ��� 3��ʾ���
void BSP_JY_901_printf(BSP_UART_JY_901 * receivedata, uint8_t state) {
    if((state == 1) || (state == 3)) {
        uint8_t *pbuf = (uint8_t *)receivedata;
        MYLOGHEX(INFO, pbuf,(receivedata->DataSta & 0X3FFF));
    }
    if((state == 2) || (state == 3)) {
        //����������ݽ�������
        receivedata->DataSta = 0;//����Ϊ1������ձ�׼
    }
}

uint8_t BSP_Cheak_JY_901(BSP_UART_JY_901 * receivedata) {
    uint16_t i = 0, sum = 0;
    for(i = 0; i < 10; i++) {
        sum += receivedata->Data.Data[i];
    }
    return sum;
}

void BSP_GET_HKJ_JY_901(BSP_UART_JY_901 * receivedata,void (*send)(QueueHandle_t Handle,uint8_t *pbuf)) {
    static JY_901_Upload  JY_901;  //���ٶ�����
    static uint8_t state = 0;
    if(receivedata->Data.JY_901.cmd == 0x51) {
        memcpy((void*)&JY_901.Ax, (void*)&receivedata->Data.JY_901.data[0], 6); //���ٶ�
        state |= 0x01;
    }
    if(receivedata->Data.JY_901.cmd == 0x52) {
        memcpy((void*)&JY_901.Wx, (void*)&receivedata->Data.JY_901.data[0], 6); //���ٶ�
        state |= 0x02;
    }
    if(receivedata->Data.JY_901.cmd == 0x53) {
        memcpy((void*)&JY_901.Rx, (void*)&receivedata->Data.JY_901.data[0], 6); //ŷ����
        state |= 0x04;
    }
    if(receivedata->Data.JY_901.cmd == 0x54) {
        memcpy((void*)&JY_901.Hx, (void*)&receivedata->Data.JY_901.data[0], 6); //�ų�
        state |= 0x08;
    }
    if((state & 0x0f) == 0x0f) {
        send(JY_901_Queue,(void*)&JY_901);
        state = 0;
    }
}
void BSP_GET_HKJ_JY_901_V1_0(BSP_UART_JY_901 * receivedata,void (*send)(QueueHandle_t Handle,uint8_t *pbuf)) {
    static JY_901_Upload  JY_901;  //���ٶ�����
    static uint8_t state = 0;
    if(receivedata->Data.JY_901.cmd == 0x51) {
        //MYLOG(INFO,"BSP_GET_HKJ_JY_901_V1_0  0x51");
        memcpy((void*)&JY_901.Ax, (void*)&receivedata->Data.JY_901.data[0], 6); //���ٶ�
        uint32_t temp = JY_901.Ax * JY_901.Ax + JY_901.Ay * JY_901.Ay + JY_901.Az * JY_901.Az;
        if(temp>UploadRx.A_max){//ȡ���ֵ
            UploadRx.A_max = temp;
            UploadRx.upload.Ax_max = JY_901.Ax;
            UploadRx.upload.Ay_max = JY_901.Ay;
            UploadRx.upload.Az_max = JY_901.Az;
        }
        if(temp<UploadRx.A_min){//ȡ��Сֵ
            UploadRx.A_max = temp;
            UploadRx.upload.Ax_min = JY_901.Ax;
            UploadRx.upload.Ay_min = JY_901.Ay;
            UploadRx.upload.Az_min = JY_901.Az;
        }
        state |= 0x01;
        //�Ʋ��㷨  /8��λ�ͺ�adxl345��ͬ  ��ʱͳһ��λ����һ��
        //MYLOG(WARNING, "<x y z %5.2f><%04d %04d %04d>",sqrt((JY_901.Ax/2)*(JY_901.Ax/2)+(JY_901.Ay/2)*(JY_901.Ay/2)+(JY_901.Az/2)*(JY_901.Az/2)),JY_901.Ax/2,JY_901.Ay/2,JY_901.Az/2);
        //Pedometer((JY_901.Ax*1000)/2048, (JY_901.Ay*1000)/2048, (JY_901.Az*1000)/2048);
        int16_t buf[3];
        buf[0] = (JY_901.Ax*1000)/2048;
        buf[1] = (JY_901.Ay*1000)/2048;
        buf[2] = (JY_901.Az*1000)/2048;
        if(information.algorithm==3) ADXL345_Step2(buf);
        if(information.algorithm==4) Pedometer(buf[0], buf[1], buf[2]);
    }
    if(receivedata->Data.JY_901.cmd == 0x53) {
        //MYLOG(INFO,"BSP_GET_HKJ_JY_901_V1_0  0x53");
        memcpy((void*)&JY_901.Rx, (void*)&receivedata->Data.JY_901.data[0], 6); //ŷ����
        if(UploadRx.R_count==0){
            UploadRx.Rx_add = JY_901.Rx;
            UploadRx.Ry_add = JY_901.Ry;
            UploadRx.Rz_add = JY_901.Rz;
        } else {
            UploadRx.Rx_add = (UploadRx.Rx_add + JY_901.Rx*2)/3;
            UploadRx.Ry_add = (UploadRx.Ry_add + JY_901.Ry*2)/3;
            UploadRx.Rz_add = (UploadRx.Rz_add + JY_901.Rz*2)/3;
        }
//        UploadRx.Rx_add += JY_901.Rx;
//        UploadRx.Ry_add += JY_901.Ry;
//        UploadRx.Rz_add += JY_901.Rz;
        UploadRx.R_count++; //����Ϳ��Դ���0.1s�ڽ��յĴ���
        
//        UploadRx.Rx_add += JY_901.Rx;
//        UploadRx.Ry_add += JY_901.Ry;
//        UploadRx.Rz_add += JY_901.Rz;
//        UploadRx.R_count++; //����Ϳ��Դ���0.1s�ڽ��յĴ���
        state |= 0x04;
    }
    if((state & 0x0f) == 0x05) {
        //MYLOG(INFO,"BSP_GET_HKJ_JY_901_V1_0  0x51 0x53");
        globalvariable.EquipmentFlag = SetBit(globalvariable.EquipmentFlag,jiy901_flag);
        UploadRx.Flag = (UploadRx.Flag & 0xf0) | 0x05; //���յ�����
        state = 0;
    }
}

void BSP_Receive_JY_901(uint8_t pData, BSP_UART_JY_901 * receivedata) {
    if((receivedata->DataSta & 0x8000) == 0) {         //����δ���
        if((receivedata->DataSta & 0X3FFF) > 10)receivedata->DataSta = 0; //�������ݴ���,���¿�ʼ����
        receivedata->Data.Data[receivedata->DataSta & 0X3FFF] = pData ;
        receivedata->DataSta++;
        if((receivedata->DataSta & 0X3FFF) == 1) {
            if(receivedata->Data.JY_901.head != 0x55) {
                receivedata->DataSta = 0; //�������ݴ���,���¿�ʼ����
            }
        } else if((receivedata->DataSta & 0X3FFF) == 2) {
            if((receivedata->Data.JY_901.cmd & 0xf0) != 0x50) {
                receivedata->DataSta = 0; //�������ݴ���,���¿�ʼ����
            }
        } else if((receivedata->DataSta & 0X3FFF) == 11) {
            if(BSP_Cheak_JY_901(receivedata) == receivedata->Data.JY_901.sum) {
                receivedata->DataSta |= 0x8000;        //���������
                //BSP_GET_HKJ_JY_901(receivedata,BSP_QueueSend);
                BSP_GET_HKJ_JY_901_V1_0(receivedata,BSP_QueueSend);
                BSP_JY_901_printf(receivedata, 2);
            } else {
                receivedata->DataSta = 0; //�������ݴ���,���¿�ʼ����
            }
        }
    }
}
