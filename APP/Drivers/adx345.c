#include "adx345.h"

I2C_HandleTypeDef *myadxl345 = &hi2c1;

static int8_t Single_Write_ADXL345(uint8_t addr, uint8_t dat) {
    HAL_I2C_Mem_Write(myadxl345, ADX345_ADDR + 1, addr, I2C_MEMADD_SIZE_8BIT, &dat, 1, 0xFF);
    return 0;
}
static uint8_t Single_Read_ADXL345(uint8_t addr) {
    uint8_t readValue = 0xFF;
    HAL_I2C_Mem_Read(myadxl345, ADX345_ADDR, addr, I2C_MEMADD_SIZE_8BIT, &readValue, 1, 0xff);
    return readValue;
}
//*********************************************************
//��������ADXL345�ڲ����ٶ����ݣ���ַ��Χ0x32~0x37
//*********************************************************
static int8_t Multiple_read_ADXL345(uint8_t *buf) {
    HAL_I2C_Mem_Read(myadxl345, ADX345_ADDR, 0X32, I2C_MEMADD_SIZE_8BIT, &buf[0], 1, 0xFF);
    HAL_I2C_Mem_Read(myadxl345, ADX345_ADDR, 0X33, I2C_MEMADD_SIZE_8BIT, &buf[1], 1, 0xFF);
    HAL_I2C_Mem_Read(myadxl345, ADX345_ADDR, 0X34, I2C_MEMADD_SIZE_8BIT, &buf[2], 1, 0xFF);
    HAL_I2C_Mem_Read(myadxl345, ADX345_ADDR, 0X35, I2C_MEMADD_SIZE_8BIT, &buf[3], 1, 0xFF);
    HAL_I2C_Mem_Read(myadxl345, ADX345_ADDR, 0X36, I2C_MEMADD_SIZE_8BIT, &buf[4], 1, 0xFF);
    HAL_I2C_Mem_Read(myadxl345, ADX345_ADDR, 0X37, I2C_MEMADD_SIZE_8BIT, &buf[5], 1, 0xFF);
    return 0;
}

void Get_ADXL345_State(uint8_t value){
    uint8_t readValue = Single_Read_ADXL345(INT_SOURCE); 
    if(readValue&0x40) {
        MYLOG(WARNING, "ADXL345 SINGLE_TAP = %d %x", value,readValue);
    }
    if(readValue&0x10) {
        MYLOG(WARNING, "ADXL345 Activity = %d %x", value,readValue);
    }
    if(readValue&0x08) {
        MYLOG(WARNING, "ADXL345 Inactivity = %d %x", value,readValue);
    }
    if(readValue&0x04) {
        MYLOG(WARNING, "ADXL345 FREE_FALL = %d %x", value,readValue);
    }
}

//static int8_t Multiple_read_ADXL345(int16_t *xValue, int16_t *yValue, int16_t *zValue) {
//    uint8_t buf[6];
//    HAL_I2C_Mem_Read(myadxl345, ADX345_ADDR, 0X32, I2C_MEMADD_SIZE_8BIT, &buf[0], 1, 0xFF);
//    HAL_I2C_Mem_Read(myadxl345, ADX345_ADDR, 0X33, I2C_MEMADD_SIZE_8BIT, &buf[1], 1, 0xFF);
//    HAL_I2C_Mem_Read(myadxl345, ADX345_ADDR, 0X34, I2C_MEMADD_SIZE_8BIT, &buf[2], 1, 0xFF);
//    HAL_I2C_Mem_Read(myadxl345, ADX345_ADDR, 0X35, I2C_MEMADD_SIZE_8BIT, &buf[3], 1, 0xFF);
//    HAL_I2C_Mem_Read(myadxl345, ADX345_ADDR, 0X36, I2C_MEMADD_SIZE_8BIT, &buf[4], 1, 0xFF);
//    HAL_I2C_Mem_Read(myadxl345, ADX345_ADDR, 0X37, I2C_MEMADD_SIZE_8BIT, &buf[5], 1, 0xFF);
//    *xValue = (short)(((uint16_t)buf[1] << 8) + buf[0]); //DATA_X1Ϊ��λ��Ч�ֽ�
//    *yValue = (short)(((uint16_t)buf[3] << 8) + buf[2]); //DATA_Y1Ϊ��λ��Ч�ֽ�
//    *zValue = (short)(((uint16_t)buf[5] << 8) + buf[4]); //DATA_Z1Ϊ��λ��Ч�ֽ�
//    return 0;
//}
//*****************************************************************
//��ʼ��ADXL345��������Ҫ��ο�pdf�����޸�************************
//void Init_ADXL345(void) {
//    uint8_t readValue = Single_Read_ADXL345(ADX_DEVID);
//    if(readValue == 0xE5) {
//        Single_Write_ADXL345(ADX_DATA_FORMAT, 0x2B); //13λȫ�ֱ���,��������Ҷ���,16g����
//        Single_Write_ADXL345(ADX_BW_RATE, 0x0A);     //��������ٶ�Ϊ100Hz
//        Single_Write_ADXL345(ADX_POWER_CTL, 0x28);   //����ʹ��,����ģʽ
//        Single_Write_ADXL345(ADX_INT_ENABLE, 0x00);  //��ʹ���ж�
//        Single_Write_ADXL345(ADX_OFSX, 0x00);        //�û���ֵ
//        Single_Write_ADXL345(ADX_OFSY, 0x00);        //X��ƫ��
//        Single_Write_ADXL345(ADX_OFSZ, 0x00);        //Y��ƫ��
//        MYLOG(WARNING,"ADXL345_Init OK!");
//        return ;
//    }
//   MYLOG(ERROR,"ADXL345_Init Fail! =%x",readValue);
//}

//void Init_ADXL345(void) {
//    uint8_t readValue = Single_Read_ADXL345(ADX_DEVID);
//    if(readValue == 0xE5) {
//        Single_Write_ADXL345(ADX_INT_ENABLE, 0x00);   //���ж�
//        Single_Write_ADXL345(ADX_DATA_FORMAT, 0x0B);  //������Χ,����16g��10λģʽ
//        Single_Write_ADXL345(ADX_POWER_CTL, 0x28);    //ѡ���Դģʽ   �ο�pdf24ҳ 38�Զ�˯��
//        Single_Write_ADXL345(ADX_BW_RATE, 0x09);      //�����趨Ϊ50HZ �ο�pdf13ҳ
//        
//        Single_Write_ADXL345(FIFO_CTL, 0x9f);         //FIFOģʽ���� 32��fifo�������ж�
//        
//        Single_Write_ADXL345(ADX_OFSX, 0x00);        //Xƫ���� ���ݲ��Դ�������״̬д��pdf29ҳ
//        Single_Write_ADXL345(ADX_OFSY, 0x00);        //Yƫ���� ���ݲ��Դ�������״̬д��pdf29ҳ
//        Single_Write_ADXL345(ADX_OFSZ, 0x00);        //Zƫ���� ���ݲ��Դ�������״̬д��pdf29ҳ
//        
//        Single_Write_ADXL345(THRESH_TAP, 5);          //˯�߼���activity��ֵ�����������ֵ��ʱ���ѣ�����0x10����1g
//        
//        
//        Single_Write_ADXL345(THRESH_ACT, 2);          //˯�߼���activity��ֵ�����������ֵ��ʱ���ѣ�����0x10����1g
//        Single_Write_ADXL345(THRESH_INACT, 1);        //˯�߿�ʼinactivity��ֵ����С�����ֵ��ʱ��˯�ߣ�����02����0.2g
//        Single_Write_ADXL345(TIME_INACT, 2);          //��С��inactivityֵʱ�䳬�����ֵ��ʱ�����˯�ߣ�����02����2��
//        Single_Write_ADXL345(ACT_INACT_CTL, 0xcc);    //ֱ�������������ã�XYZʹ�ܴ������ã��˴�ѡ��X��������
//        
//        Single_Write_ADXL345(INT_MAP, 0x03);          //�ж�ӳ�� 0 INT1 1 INT2
//        Single_Write_ADXL345(ADX_INT_ENABLE, 0x7F);   //ʹ�� DATA_READY �ж�
//        //Single_Write_ADXL345(ADX_INT_ENABLE, 0x83);   //ʹ�� DATA_READY �ж�
//        MYLOG(WARNING, "ADXL345_Init OK!");
//        return ;
//    }
//    MYLOG(ERROR,"ADXL345_Init Fail! =%x",readValue);
//}

void Init_ADXL345(void) {
    uint8_t readValue = Single_Read_ADXL345(ADX_DEVID);
    if(readValue == 0xE5) {
        Single_Write_ADXL345(0x2E,0x00); //�ò�����Ϊ��Ҫ����Ϊ֮ǰ���ù��жϵĻ���������жϴ���Ӧ���������ʹ�� 
        Single_Write_ADXL345(0x31,0x0B); //�ж����͵�ƽ��Ч ������Χ����16g,13λģʽ
        Single_Write_ADXL345(0x2C,0x0A); //�����趨Ϊ12.5�ο�PDF13ҳ 
        Single_Write_ADXL345(0x2D,0x28); //ѡ���Դģʽ �ο�PDF24ҳ
        Single_Write_ADXL345(0x2F,0x00); //�ж������ΪINT1 FREE_FALL�ж����ΪINT1       
        Single_Write_ADXL345(0x24,0x05); //Activity���ֵ  (62.5mg/LSB)*0xxx
        Single_Write_ADXL345(0x25,0x05); //THRESH_INACT�����⾲ֹ��ֵ; (62.5mg/LSB)*0xxx
        Single_Write_ADXL345(0x26,0x02); //TIME_INACT��⾲ֹʱ�䷧ֵ; 2s(1s/LSB)*0xxx
        Single_Write_ADXL345(0x27,0xcc); //ֱ����ϣ����������
        Single_Write_ADXL345(0x28,0x06); //�������巧ֵ 0x05��0x09 300mg-600mg ����������жϼ��ٶ�((62.5mg/LSB))
        Single_Write_ADXL345(0x29,0x30); //��������ʱ�� 0x14��0x46 100ms-350ms (5ms/LSB)
        Single_Write_ADXL345(0x2E,0x14); //�ж�ʹ�ܼĴ���  ʹ��Activity�ж� ʹ��Free_Fall�ж�
//        Single_Write_ADXL345(0x31,0x0b);   //������Χ,����16g��13λģʽ �ߵ�ƽ��Ч
//        Single_Write_ADXL345(0x2C,0x09);   //�����趨Ϊ12.5 �ο�pdf13ҳ
//        Single_Write_ADXL345(0x2D,0x28);   //ѡ���Դģʽ   �ο�pdf24ҳ
//        
//        Single_Write_ADXL345(0x1E,0x00);   //X ƫ���� ���ݲ��Դ�������״̬д��pdf29ҳ
//        Single_Write_ADXL345(0x1F,0x00);   //Y ƫ���� ���ݲ��Դ�������״̬д��pdf29ҳ
//        Single_Write_ADXL345(0x20,0x00);   //Z ƫ���� ���ݲ��Դ�������״̬д��pdf29ҳ


//        Single_Write_ADXL345(0x1D,0x30);        //����THRESH_TAP(�û�����) 62.5mg/LSB   0xff=16g  ��ʱ��ȡ3g   
//        Single_Write_ADXL345(0x21,0x11);        //����DUR���û�����ʱ�䣩 625us/LSB  ��ʱ��ȡ10ms
//        
//        Single_Write_ADXL345(0x2A,0x07);        //���ø��᷽���Ƿ�μ��û����  xyzȫ������
//        
//        Single_Write_ADXL345(0x2E,0x00);        //�����ж�ǰӦ�Ƚ����ж�
//        
//        Single_Write_ADXL345(0x2E,0x40);        //ʹ�� SINGLE_TAP �ж� 
//        Single_Write_ADXL345(0x2F,0x40);        //�����ж�����ӳ�� ������ʱ��ӳ�䵽INT2��
        
        
        
//        Single_Write_ADXL345(ADX_INT_ENABLE, 0x00);   //���ж�
//        Single_Write_ADXL345(ADX_DATA_FORMAT, 0x0B);  //������Χ,����16g��10λģʽ
//        Single_Write_ADXL345(ADX_POWER_CTL, 0x28);    //ѡ���Դģʽ   �ο�pdf24ҳ 38�Զ�˯��
//        Single_Write_ADXL345(ADX_BW_RATE, 0x09);      //�����趨Ϊ50HZ �ο�pdf13ҳ
//        
//        Single_Write_ADXL345(FIFO_CTL, 0x9f);         //FIFOģʽ���� 32��fifo�������ж�
//        
//        Single_Write_ADXL345(ADX_OFSX, 0x00);        //Xƫ���� ���ݲ��Դ�������״̬д��pdf29ҳ
//        Single_Write_ADXL345(ADX_OFSY, 0x00);        //Yƫ���� ���ݲ��Դ�������״̬д��pdf29ҳ
//        Single_Write_ADXL345(ADX_OFSZ, 0x00);        //Zƫ���� ���ݲ��Դ�������״̬д��pdf29ҳ
//        
//        Single_Write_ADXL345(THRESH_TAP, 5);          //˯�߼���activity��ֵ�����������ֵ��ʱ���ѣ�����0x10����1g
//        
//        
//        Single_Write_ADXL345(THRESH_ACT, 2);          //˯�߼���activity��ֵ�����������ֵ��ʱ���ѣ�����0x10����1g
//        Single_Write_ADXL345(THRESH_INACT, 1);        //˯�߿�ʼinactivity��ֵ����С�����ֵ��ʱ��˯�ߣ�����02����0.2g
//        Single_Write_ADXL345(TIME_INACT, 2);          //��С��inactivityֵʱ�䳬�����ֵ��ʱ�����˯�ߣ�����02����2��
//        Single_Write_ADXL345(ACT_INACT_CTL, 0xcc);    //ֱ�������������ã�XYZʹ�ܴ������ã��˴�ѡ��X��������
//        
//        Single_Write_ADXL345(INT_MAP, 0x03);          //�ж�ӳ�� 0 INT1 1 INT2
//        Single_Write_ADXL345(ADX_INT_ENABLE, 0x7F);   //ʹ�� DATA_READY �ж�
        //Single_Write_ADXL345(ADX_INT_ENABLE, 0x83);   //ʹ�� DATA_READY �ж�
        MYLOG(WARNING, "ADXL345_Init OK!");
        return ;
    }
    MYLOG(ERROR,"ADXL345_Init Fail! =%x",readValue);
}

/*------------------------------------------------------------------------------------------------------------------------
*Name:      step_counter()
*Function:  ʵ��Pedometer�Ļ����㷨.
*Input:     void
*Output:    void
*------------------------------------------------------------------------------------------------------------------------*/
uint8_t  adxl345_flag = 0;        //20MS���жϱ�־
uint32_t adxl345_flag_time = 0;   //����5s�ſ�ʼ����

uint8_t  bad_flag[3];                //��Ч��־
uint32_t array0[3] = {1, 1, 1};      //����X�˲�����
uint32_t array1[3] = {1, 1, 1};      //����X�˲�����
uint32_t array2[3] = {0, 0, 0};      //����X�˲�����
uint32_t adresult[3];                //����ƽ��ֵ
uint32_t max[3] = {0, 0, 0};         //���ֵ
uint32_t min[3] = {1000, 1000, 1000};//��С
uint32_t dc[3] = {500, 500, 500};    //��ֵ
uint32_t vpp[3] = {30, 30, 30};      //�����ٶ�
uint32_t precision[3] = {5, 5, 5};   //��̬����
uint32_t old_fixed[3];               //������λ����ֵ
uint32_t new_fixed[3];               //������λ����ֵ
uint32_t STEPS = 0;                  //����

void step_counter(void) {
    static uint8_t sampling_counter = 0; //50��*0.2s = 10s
    uint8_t buf[6], jtemp;
    adxl345_flag = 0;
    Multiple_read_ADXL345(buf);          //�����������ݣ��洢��BUF��
    //------------------------------------------�����˲�----------------------//
    for(jtemp = 0; jtemp <= 2; jtemp++) { //jtemp 0,1,2�ֱ����x��y��z
        array2[jtemp] = array1[jtemp];
        array1[jtemp] = array0[jtemp];
        array0[jtemp] = buf[2 * jtemp] + (buf[2 * jtemp + 1] << 8);
        adresult[jtemp] = array0[jtemp] + array1[jtemp] + array2[jtemp];
        adresult[jtemp] = adresult[jtemp] / 3;
        if(adresult[jtemp] > max[jtemp]) {max[jtemp] = adresult[jtemp];}
        if(adresult[jtemp] < min[jtemp]) {min[jtemp] = adresult[jtemp];}
    }
    sampling_counter = sampling_counter + 1;
    //----------------------------------���㶯̬���޺Ͷ�̬����-----------------------//
    if(sampling_counter >= 50) {
        sampling_counter = 0;
        for(jtemp = 0; jtemp <= 2; jtemp++) {
            vpp[jtemp] = max[jtemp] - min[jtemp];
            dc[jtemp] = min[jtemp] + (vpp[jtemp] >> 1); //dcΪ��ֵ
            max[jtemp] = 0;
            min[jtemp] = 1023;
            bad_flag[jtemp] = 0;
            if(vpp[jtemp] >= 160) {
                precision[jtemp] = vpp[jtemp] / 32; //8
            } else if((vpp[jtemp] >= 50) && (vpp[jtemp] < 160)) {
                precision[jtemp] = 4;
            } else if((vpp[jtemp] >= 15) && (vpp[jtemp] < 50)) {
                precision[jtemp] = 3;
            } else {
                precision[jtemp] = 2;
                bad_flag[jtemp] = 1;
            }
        }
    }
    //--------------------------������λ�Ĵ���--------------------------------------
    for(jtemp = 0; jtemp <= 2; jtemp++) {
        old_fixed[jtemp] = new_fixed[jtemp];
        if(adresult[jtemp] >= new_fixed[jtemp]) {
            if((adresult[jtemp] - new_fixed[jtemp]) >= precision[jtemp])   {new_fixed[jtemp] = adresult[jtemp];}
        } else if(adresult[jtemp] < new_fixed[jtemp]) {
            if((new_fixed[jtemp] - adresult[jtemp]) >= precision[jtemp])   {new_fixed[jtemp] = adresult[jtemp];}
        }
    }
    //------------------------- ��̬�����о� ----------------------------------
    if((vpp[0] >= vpp[1]) && (vpp[0] >= vpp[2])) { //x�����Ծ
        if((old_fixed[0] >= dc[0]) && (new_fixed[0] < dc[0]) && (bad_flag[0] == 0)) {
            STEPS = STEPS + 1;
        }
    } else if((vpp[1] >= vpp[0]) && (vpp[1] >= vpp[2])) { //y�����Ծ
        if((old_fixed[1] >= dc[1]) && (new_fixed[1] < dc[1]) && (bad_flag[1] == 0)) {
            STEPS = STEPS + 1;
        }
    } else if((vpp[2] >= vpp[1]) && (vpp[2] >= vpp[0])) { //z�����Ծ
        if((old_fixed[2] >= dc[2]) && (new_fixed[2] < dc[2]) && (bad_flag[2] == 0)) {
            STEPS = STEPS + 1;
        }
    }
}

void step_counter_run(void) {
    static uint32_t pSTEPS = 0;                  //����
    HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_SET);
    if(DelayTimeOut(&adxl345_flag_time, 20)) {   //0.02s����һ��
        adxl345_flag_time = HAL_GetTick();
        step_counter();
        //MYLOG(WARNING, "step_counter_run = %d", STEPS); //�������
        if(pSTEPS != STEPS) {
            pSTEPS = STEPS;
            MYLOG(WARNING, "step_counter_run = %d", STEPS); //�������
            HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_RESET);
        }
    }
}

//uint8_t SAMPLING_COUNTER = 0;
//SENSOR_DATA_TypeDef MAX_DATA = {0, 0, 0};
//SENSOR_DATA_TypeDef MIN_DATA = {1000, 1000, 1000};
//SENSOR_DATA_TypeDef THRESH   = {35, 225, 225};
//SENSOR_DATA_TypeDef DELTA    = {1000, 1000, 1000};

////max-min to select most active axis
//SENSOR_DATA_TypeDef RES = {1000, 1000, 1000};

//SENSOR_DATA_TypeDef g_last_sample = {0, 0, 0};
//SENSOR_DATA_TypeDef g_cur_sample = {0, 0, 0};

//int g_step_count = 0; //�ǲ���

//#define ADX345_DelayMs(x)     {HAL_Delay(x);}  //��ʱ����
//#define max(a,b) ((a) > (b) ? (a) : (b))
///****************************************************************************
//* Function Name  : ADX345_WriteReg
//* Description    : ����ADX345�Ĵ���
//* Input          : addr���Ĵ�����ַ
//*                * dat����������
//* Output         : None
//* Return         : None
//****************************************************************************/



///****************************************************************************
//* Function Name  : ADX345_ReadReg
//* Description    : ��ȡADX345�Ĵ���
//* Input          : addr����ȡ��ַ
//* Output         : None
//* Return         : ��ȡ����8λ����
//****************************************************************************/



///****************************************************************************
//* Function Name  : ADX345_ReadXYZ
//* Description    : ��ȡX,Y,Z��ADֵ
//* Input          : xValue��X��ı����ַ
//*                * yValue��Y��ı����ַ
//*                * zValue��Z��ı����ַ
//* Output         : None
//* Return         : 0����ȡ�ɹ���0xFF����ȡʧ��
//****************************************************************************/

//static int8_t ADX345_ReadXYZ(int16_t *xValue, int16_t *yValue, int16_t *zValue) {
//    uint8_t buf[6];
//    HAL_I2C_Mem_Read(myadxl345, ADX345_ADDR, 0X32, I2C_MEMADD_SIZE_8BIT, &buf[0], 1, 0xFF);
//    HAL_I2C_Mem_Read(myadxl345, ADX345_ADDR, 0X33, I2C_MEMADD_SIZE_8BIT, &buf[1], 1, 0xFF);
//    HAL_I2C_Mem_Read(myadxl345, ADX345_ADDR, 0X34, I2C_MEMADD_SIZE_8BIT, &buf[2], 1, 0xFF);
//    HAL_I2C_Mem_Read(myadxl345, ADX345_ADDR, 0X35, I2C_MEMADD_SIZE_8BIT, &buf[3], 1, 0xFF);
//    HAL_I2C_Mem_Read(myadxl345, ADX345_ADDR, 0X36, I2C_MEMADD_SIZE_8BIT, &buf[4], 1, 0xFF);
//    HAL_I2C_Mem_Read(myadxl345, ADX345_ADDR, 0X37, I2C_MEMADD_SIZE_8BIT, &buf[5], 1, 0xFF);
//    *xValue = (short)(((uint16_t)buf[1] << 8) + buf[0]); //DATA_X1Ϊ��λ��Ч�ֽ�
//    *yValue = (short)(((uint16_t)buf[3] << 8) + buf[2]); //DATA_Y1Ϊ��λ��Ч�ֽ�
//    *zValue = (short)(((uint16_t)buf[5] << 8) + buf[4]); //DATA_Z1Ϊ��λ��Ч�ֽ�
//    return 0;
//}

///****************************************************************************
//* Function Name  : ADX345_Init
//* Description    : ��ʼ��ADX345�����˶�ADX��ID
//* Input          : None
//* Output         : None
//* Return         : 0���ɹ���0xFF��ʧ��
//****************************************************************************/

//int ADX345_Init(void) {
//    if(ADX345_ReadReg(ADX_DEVID) == 0xE5) {
//        ADX345_WriteReg(ADX_DATA_FORMAT, 0x2B); //13λȫ�ֱ���,��������Ҷ���,16g����
//        ADX345_WriteReg(ADX_BW_RATE, 0x0A);     //��������ٶ�Ϊ100Hz
//        ADX345_WriteReg(ADX_POWER_CTL, 0x28);   //����ʹ��,����ģʽ
//        ADX345_WriteReg(ADX_INT_ENABLE, 0x00);  //��ʹ���ж�
//        ADX345_WriteReg(ADX_OFSX, 0x00);        //�û���ֵ
//        ADX345_WriteReg(ADX_OFSY, 0x00);        //X��ƫ��
//        ADX345_WriteReg(ADX_OFSZ, 0x00);        //Y��ƫ��
//        MYLOG(INFO,"ADXL345_Init OK!");
//        return 0;
//    }
//   MYLOG(ERROR,"ADXL345_Init Fail!");
//    return 0xFF; //���س�ʼ��ʧ��
//}

///****************************************************************************
//* Function Name  : ADX345_Adjust
//* Description    : ADX345����У����
//* Input          : None
//* Output         : None
//* Return         : None
//****************************************************************************/

//void ADX345_Adjust(void) {
//    int32_t offx = 0, offy = 0, offz = 0;
//    int16_t xValue, yValue, zValue;
//    uint8_t i;
//    ADX345_WriteReg(ADX_POWER_CTL, 0x00);    //�Ƚ�������ģʽ.
//    ADX345_DelayMs(100);
//    ADX345_Init();
//    ADX345_DelayMs(20);
//    /* ��ȡʮ����ֵ */
//    for(i = 0; i < 10; i++) {
//        ADX345_ReadXYZ(&xValue, &yValue, &zValue);
//        offx += xValue;
//        offy += yValue;
//        offz += zValue;
//        ADX345_DelayMs(10);   //����Ƶ����100HZ��10ms����һ�����
//    }
//    /* ���ƽ��ֵ */
//    offx /= 10;
//    offy /= 10;
//    offz /= 10;
//    /* ȫ�ֱ����£�ÿ�����LSBΪ3.9 mg��ƫ�ƼĴ���LSB���ķ�֮һ�����Գ���4 */
//    xValue = -(offx / 4);
//    yValue = -(offy / 4);
//    zValue = -((offz - 256) / 4);
//    /* ����ƫ���� */
//    ADX345_WriteReg(ADX_OFSX, xValue);
//    ADX345_WriteReg(ADX_OFSY, yValue);
//    ADX345_WriteReg(ADX_OFSZ, zValue);
//}

///****************************************************************************
//* Function Name  : ADX_GetXYZData
//* Description    : ��ȡADX��XYZ���ֵ�����й����ݴ���
//* Input          : xValue��X��ı����ַ
//*                * yValue��Y��ı����ַ
//*                * zValue��Z��ı����ַ
//* Output         : None
//* Return         : None
//****************************************************************************/

//void ADX_GetXYZData(int16_t *xValue, int16_t *yValue, int16_t *zValue, SENSOR_DATA_TypeDef *axis_converted_avg) {
//    uint8_t i;
//    uint8_t res_div = 20;
//    MAX_DATA.X = 0;
//    MAX_DATA.Y = 0;
//    MAX_DATA.Z = 0;
//    MIN_DATA.X = 1000;
//    MIN_DATA.Y = 1000;
//    MIN_DATA.Z = 1000;
//    for(SAMPLING_COUNTER = 0; SAMPLING_COUNTER < 33; SAMPLING_COUNTER++) { //����50��
//        int32_t xTotal = 0, yTotal = 0, zTotal = 0;
//        /* ��ȡ4�β���ֵ */
//        for(i = 0; i < 4; i++) {
//            *xValue = 0;
//            *yValue = 0;
//            *zValue = 0;
//            ADX345_ReadXYZ(xValue, yValue, zValue);
//            xTotal += *xValue;
//            yTotal += *yValue;
//            zTotal += *zValue;
//            ADX345_DelayMs(10);  //����Ƶ����100HZ��10ms����һ�����
//        }
//        /* ���ƽ��ֵ */
//        *xValue = xTotal / 4;
//        *yValue = yTotal / 4;
//        *zValue = zTotal / 4;
//        axis_converted_avg[SAMPLING_COUNTER].X = *xValue;
//        axis_converted_avg[SAMPLING_COUNTER].Y = *yValue;
//        axis_converted_avg[SAMPLING_COUNTER].Z = *zValue;
//        //find maximum and minimum value of 3 axis
//        if(MAX_DATA.X < *xValue) MAX_DATA.X = *xValue;
//        if(MAX_DATA.Y < *yValue) MAX_DATA.Y = *yValue;
//        if(MAX_DATA.Z < *zValue) MAX_DATA.Z = *zValue;
//        if(MIN_DATA.X > *xValue) MIN_DATA.X = *xValue;
//        if(MIN_DATA.Y > *yValue) MIN_DATA.Y = *yValue;
//        if(MIN_DATA.Z > *zValue) MIN_DATA.Z = *zValue;
//    }
//    THRESH = get_thresh(MAX_DATA, MIN_DATA); //��̬��ֵ
//    DELTA = ADXL345_MINUS(MAX_DATA, MIN_DATA); //�����Сֵ�Ĳ�ֵ   ���ٶȵı仯
//    RES.X = max(MAX_DATA.X - THRESH.X, THRESH.X - MIN_DATA.X) / res_div; //����10,���õ���̬����
//    RES.Y = max(MAX_DATA.Y - THRESH.Y, THRESH.Y - MIN_DATA.Y) / res_div;
//    RES.Z = max(MAX_DATA.Z - THRESH.Z, THRESH.Z - MIN_DATA.Z) / res_div;
//}

//void ADX_GetXYZDataNEW(int16_t *xValue1, int16_t *yValue1, int16_t *zValue1, SENSOR_DATA_TypeDef *buf) {
//    uint8_t i;
//    for(SAMPLING_COUNTER = 0; SAMPLING_COUNTER < 33; SAMPLING_COUNTER++) { //����50��
//        int32_t xTotal = 0, yTotal = 0, zTotal = 0;
//        /* ��ȡ4�β���ֵ */
//        for(i = 0; i < 4; i++) {
//            *xValue1 = 0;
//            *yValue1 = 0;
//            *zValue1 = 0;
//            ADX345_ReadXYZ(xValue1, yValue1, zValue1);
//            xTotal += *xValue1;
//            yTotal += *yValue1;
//            zTotal += *zValue1;
//            ADX345_DelayMs(10);  //����Ƶ����100HZ��10ms����һ�����
//        }
//        /* ���ƽ��ֵ */
//        *xValue1 = xTotal / 4;
//        *yValue1 = yTotal / 4;
//        *zValue1 = zTotal / 4;
//        buf[SAMPLING_COUNTER].X = *xValue1;
//        buf[SAMPLING_COUNTER].Y = *yValue1;
//        buf[SAMPLING_COUNTER].Z = *zValue1;
//    }
//}



////���㲽��
//void ADXL345_STEPCOUNT(SENSOR_DATA_TypeDef NEW_SAMPLE) {
//    //add time window and step model rule
//    if(abs(DELTA.X) >= abs(DELTA.Y) && abs(DELTA.X) >= abs(DELTA.Z)) { //x�����Ծ����
//        //�ҵ����ٶȱ仯�����ᣬ�����㲽��
//        if(NEW_SAMPLE.X - THRESH.X > RES.X)
//            //����ֵ������ֵ�뾫�Ⱥ�
//        {
//            g_last_sample = g_cur_sample;
//            g_cur_sample = NEW_SAMPLE;
//            g_step_count++;
//        } else {
//            g_last_sample = g_cur_sample;
//        }
//    } else if(abs(DELTA.Y) >= abs(DELTA.X) && abs(DELTA.Y) >= abs(DELTA.Z)) { //Y�����Ծ
//        if(NEW_SAMPLE.Y - THRESH.Y > RES.Y) {
//            g_last_sample = g_cur_sample;
//            g_cur_sample = NEW_SAMPLE;
//            g_step_count++;
//        } else {
//            g_last_sample = g_cur_sample;
//        }
//    } else if(abs(DELTA.Z) >= abs(DELTA.X) && abs(DELTA.Z) >= abs(DELTA.X)) { //Z�����Ծ
//        if(NEW_SAMPLE.Z - THRESH.Z > RES.Z) {
//            g_last_sample = g_cur_sample;
//            g_cur_sample = NEW_SAMPLE;
//            g_step_count++;
//        } else {
//            g_last_sample = g_cur_sample;
//        }
//    }
//}

////��̬��ֵ
//SENSOR_DATA_TypeDef get_thresh(SENSOR_DATA_TypeDef data0, SENSOR_DATA_TypeDef data1) {
//    SENSOR_DATA_TypeDef result;
//    result.X = (data0.X + data1.X) / 2;
//    result.Y = (data0.Y + data1.Y) / 2;
//    result.Z = (data0.Z + data1.Z) / 2;
//    return result;
//}
////��
//SENSOR_DATA_TypeDef ADXL345_ADD(SENSOR_DATA_TypeDef data0, SENSOR_DATA_TypeDef data1) {
//    SENSOR_DATA_TypeDef result;
//    result.X = data0.X + data1.X;
//    result.Y = data0.Y + data1.Y;
//    result.Z = data0.Z + data1.Z;
//    return result;
//}

////��
//SENSOR_DATA_TypeDef ADXL345_MINUS(SENSOR_DATA_TypeDef data0, SENSOR_DATA_TypeDef data1) {
//    SENSOR_DATA_TypeDef result;
//    result.X = data0.X - data1.X;
//    result.Y = data0.Y - data1.Y;
//    result.Z = data0.Z - data1.Z;
//    return result;
//}

////��
//SENSOR_DATA_TypeDef ADXL345_TIMES(SENSOR_DATA_TypeDef data0, SENSOR_DATA_TypeDef data1) {
//    SENSOR_DATA_TypeDef result;
//    result.X = data0.X * data1.X;
//    result.Y = data0.Y * data1.Y;
//    result.Z = data0.Z * data1.Z;
//    return result;
//}


////��
//SENSOR_DATA_TypeDef ADXL345_DEVIDE(SENSOR_DATA_TypeDef data0, SENSOR_DATA_TypeDef data1) {
//    SENSOR_DATA_TypeDef result;
//    result.X = data0.X / data1.X;
//    result.Y = data0.Y / data1.Y;
//    result.Z = data0.Z / data1.Z;
//    return result;
//}

//SENSOR_DATA_TypeDef ADXL345_AVERAGE(SENSOR_DATA_TypeDef *data, uint32_t len) {
//    SENSOR_DATA_TypeDef result;
//    uint32_t i;
//    for(i = 0; i < len; i++) {
//        result.X += data[i].X;
//        result.Y += data[i].Y;
//        result.Z += data[i].Z;
//    }
//    result.X /= len;
//    result.Y /= len;
//    result.Z /= len;
//    return result;
//}




///****************************************************************************
//* Function Name  : ADX_GetAngle
//* Description    : ��ADֵת���ɽǶ�ֵ
//* Input          : xValue��x��ֵ
//*                * yValue��y��ֵ
//*                * zValue��z��ֵ
//*                * dir��0����Z��ĽǶ�;1����X��ĽǶ�;2����Y��ĽǶ�.
//* Output         : None
//* Return         : None
//****************************************************************************/

//int16_t ADX_GetAngle(float xValue, float yValue, float zValue, uint8_t dir) {
//    float temp;
//    float res = 0;
//    switch(dir) {
//        /* ����ȻZ��ĽǶ� */
//        case 0:
//            temp = sqrt((xValue * xValue + yValue * yValue)) / zValue;
//            res = atan(temp);
//            break;
//        /* ����ȻX��ĽǶ� */
//        case 1:
//            temp = xValue / sqrt((yValue * yValue + zValue * zValue));
//            res = atan(temp);
//            break;
//        /* ����ȻY��ĽǶ� */
//        case 2:
//            temp = yValue / sqrt((xValue * xValue + zValue * zValue));
//            res = atan(temp);
//            break;
//        default:
//            break;
//    }
//    res = res * 1800 / 3.14;
//    return res;
//}
///*---------------------------------------------------------------------------*/



//#define P_P_DIFF     1000               /* ����-���ȵĲ�ֵ����3D��ֵ 1000*/
//#define RISING_EDGE  1                  /* ������״̬ */
//#define FALLING_EDGE 0                  /* �½���״̬ */
//#define FAST_WALK_TIME_LIMIT_MS 200     /* ms */
//#define SLOW_WALK_TIME_LIMIT_MS 10000   /* 10s ��û����һ�� �����Ʋ� */
//#define STEP_OK 7                       /* 7������ */

//unsigned int lastPos = 0;               /* ������ */
//unsigned int newMax = 0, newMin = 0;    /* ����-���� */

//#define FALSE 0
//#define TRUE 1

//uint8_t walkSta = FALSE;                /* ���һ�η�ֵ״̬ */
//uint8_t walkOkSta = FALSE;              /* ����10s������7�� ��Ч����״̬ */
//uint8_t pSta = RISING_EDGE;             /* 3D���ݲ���״̬ */

//long lastTime = 0;                      /* ��һ�� walkSta ��ʱ�� */
//unsigned char stepOK = 0;               /* ��ʼ�Ʋ�����  �˳����� */
//unsigned long stepCount = 0;            /* ����ֵ */

///*****************************************************************
//** input: 3 axis or angle
//** output: step count
//** user read:
//    3 axis is filter value.
//******************************************************************/
////extern uint32_t Timer_count;
//unsigned long Step_Count(float axis0, float axis1, float axis2) {
//    unsigned int nowPos = 0;
//    int ppDiff = 0;
//    int timeDiff = 0;
//    /* ��ȡ3D IMU */
//    //nowPos = (unsigned int)powf(sqrtf(axis0) + sqrtf(axis1) + sqrtf(axis2), 0.5);
//    nowPos = (unsigned int)sqrtf(axis0*axis0 + axis1*axis1 + axis2*axis2);//��ȡ���ٶ�
//    /* �õ�����Ͳ��� */
//    if((pSta == RISING_EDGE) && (nowPos <= lastPos)) {        //��ʼת����
//        pSta = FALLING_EDGE;
//        newMax = lastPos;                                     //����
//        walkSta = TRUE;
//    } else if((pSta == FALLING_EDGE) && (nowPos > lastPos)) { //��ʼת����
//        pSta = FALLING_EDGE;
//        newMin = lastPos;                                     //����
//        walkSta = TRUE;
//    } else {
//        walkSta = FALSE;                                      //�м�ֵ��������
//    }
//    /* ����3D step״̬���� */
//    lastPos = nowPos;
//    /* �в���򲨹� */
//    if(walkSta == TRUE) {
//        walkSta = FALSE;
//        ppDiff = newMax - newMin; /* �����벨�ȵĲ�ֵ */
//        if(ppDiff > P_P_DIFF) {
//            //timeDiff = HAL_GetTick() - lastTime; /* ��ȡ����Ͳ��ȵ�ʱ��� */
//            timeDiff = HAL_GetTick() - lastTime;     /* ��ȡ����Ͳ��ȵ�ʱ��� */
//            if(timeDiff < FAST_WALK_TIME_LIMIT_MS) { /* ���岨��ʱ���С��200ms��ֱ��ȥ�� */
//                return stepCount;
//            } else if(timeDiff > SLOW_WALK_TIME_LIMIT_MS) { /* ���岨��ʱ������10s����Ϊ��ֹ */
//                walkOkSta = FALSE;
//                stepOK = 0;
//                return stepCount;
//            }
//            stepOK++;
//            if(stepOK >= STEP_OK) {   /* ��7��֮�����״̬ */
//                walkOkSta = TRUE;
//            }
//            //lastTime = HAL_GetTick(); /* ����ʱ�� */
//            lastTime = HAL_GetTick(); /* ����ʱ�� */
//        }
//    }
//    if(walkOkSta == TRUE) {           /* ����10s����7�� */
//        stepCount += stepOK;
//        stepOK = 0;
//    }
//    return stepCount;
//}
//int16_t Xval, Yval, Zval, Xang, Yang, Zang;
//SENSOR_DATA_TypeDef axis_converted_avg2[33];
//unsigned long step = 0;
//void ADX_Test(void) {
//    /* ��ȡX,Y,Z�ļ��ٶ�ֵ */
//    ADX_GetXYZData(&Xval, &Yval, &Zval, axis_converted_avg2);
//    printf(" x:%d \t", Xval);
//    printf(" y:%d \t", Yval);
//    printf(" z:%d \n", Zval);
//    /* ����ȡ���ļ��ٶ�ֵת��Ϊ�Ƕ�ֵ */
//    Xang = ADX_GetAngle(Xval, Yval, Zval, 1);
//    Yang = ADX_GetAngle(Xval, Yval, Zval, 2);
//    Zang = ADX_GetAngle(Xval, Yval, Zval, 0);
//    step = Step_Count(Xang, Yang, Zang);
//    printf("STEP:%lu \n", step);
//}
///*----------------------------------------------------------------------------*/

//int16_t Xval, Yval, Zval;
//int16_t Xval1, Yval1, Zval1;
//SENSOR_DATA_TypeDef axis_converted_avg[33];
//SENSOR_DATA_TypeDef buf[33];
//void ADX_Testxxx(void) {
//    uint32_t i, j;
//    /* ��ȡX,Y,Z�ļ��ٶ�ֵ */
//    ADX_GetXYZData(&Xval, &Yval, &Zval, axis_converted_avg);
////            printf(" x:%d \n", Xval);
////            printf(" y:%d \n", Yval);
////            printf(" z:%d \n", Zval);
//    for(i = 0; i < 33; i++) {
//        //printf(" X:%d \t", axis_converted_avg[i].X);         //���Լ��ٶ�ֵ
//    }
//    ADX_GetXYZDataNEW(&Xval1, &Yval1, &Zval1, buf);
//    for(j = 0; j < 33; j++) {
//        ADXL345_STEPCOUNT(buf[i]);
//    }
//    printf(" step:%d \n", g_step_count);   //��ӡ����
//}
