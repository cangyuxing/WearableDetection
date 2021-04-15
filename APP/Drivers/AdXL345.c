#include "ADXL345.h"

I2C_HandleTypeDef *adxl345 = &hi2c1;
unsigned char adxl345_read(unsigned char id, unsigned char addr) {
    uint8_t readValue = 0xFF;
    #if AnalogHardware
    HAL_I2C_Mem_Read(adxl345, id, addr, I2C_MEMADD_SIZE_8BIT, &readValue, 1, 0xff);
    #else
    readValue = IIC1_ReadReg(id - 1, addr);
    #endif
    return readValue;
}

void adxl345_write(unsigned char id, unsigned char addr, unsigned char dat) {
    #if AnalogHardware
    HAL_I2C_Mem_Write(adxl345, id, addr, I2C_MEMADD_SIZE_8BIT, &dat, 1, 0xFF);
    #else
    IIC1_WriteReg(id, addr, dat);
    #endif
}

/****************************************************************************
* Function Name  : ADX345_Init
* Description    : ��ʼ��ADX345�����˶�ADX��ID
* Input          : None
* Output         : None
* Return         : 0���ɹ���0xFF��ʧ��
****************************************************************************/
void ADXL345_Auto_Adjust(void) {
    int8_t xval, yval, zval;
    short tx, ty, tz;
    uint8_t i;
    short offx = 0, offy = 0, offz = 0;
    adxl345_write(ADXL_WRITE, POWER_CTL, 0x00);     //�Ƚ�������ģʽ
    HAL_Delay(100);
    adxl345_write(ADXL_WRITE, INT_ENABLE, 0x00);    //��ʹ���ж�
    adxl345_write(ADXL_WRITE, DATA_FORMAT, 0x2B);   //�͵�ƽ����,13λȫ�ֱ���,��������Ҷ���,16g����  0�ж����ߵ�ƽ��Ч�������� 1���ж����͵�ƽ��Ч���½���
    adxl345_write(ADXL_WRITE, BW_RATE, 0x09);       //��������:�͹��Ĳ�����100Hz���  ��Ƶ�� 1hz   50hz  1��  50������  200ms��һ��
    adxl345_write(ADXL_WRITE, POWER_CTL, 0x28);     //����ģʽ ���Զ�˯��    08
    for(i = 0; i < 10; i++) {
        ADXL345_Read_Average(&tx, &ty, &tz, 3);
        offx += tx; offy += ty; offz += tz;
    }
    offx /= 10; offy /= 10; offz /= 10;
    xval = -offx / 4;
    yval = -offy / 4;
    zval = -(offz - 256) / 4;
    adxl345_write(ADXL_WRITE, OFSX, xval);      //��ʹ���ж�
    adxl345_write(ADXL_WRITE, OFSY, yval);      //��ʹ���ж�
    adxl345_write(ADXL_WRITE, OFSZ, zval);      //��ʹ���ж�
    HAL_Delay(20);
    adxl345_write(ADXL_WRITE, INT_MAP, 0x5c);       //�ж�����ѡ�񣬴˴����ǽ�activity��Inactivityӳ�䵽INT2
    adxl345_write(ADXL_WRITE, INT_ENABLE, 0x5c);    //���ж� �˴����Ǵ�activity��Inactivity�ж�
    adxl345_read(ADXL_READ, INT_SOURC);             //��ʵû�йػ�һֱ����
    MYLOG(WARNING, "ADXL345_Auto_Adjust %d %d %d>", xval, yval, zval);
}

uint8_t ADXL345_Init(void) {
    uint8_t id, Data;
    id = adxl345_read(ADXL_READ, DEVICE_ID);            //��ȡ�豸ID
    if(id != 0XE5) {
        HAL_Delay(20);
        id = adxl345_read(ADXL_READ, DEVICE_ID);        //��ȡ�豸ID
    }
    if(id == 0XE5) {                                    //������ID��ADXL345������IDΪ0XE5
        adxl345_write(ADXL_WRITE, POWER_CTL, 0x00);     //�Ƚ�������ģʽ
        HAL_Delay(100);
        adxl345_write(ADXL_WRITE, INT_ENABLE, 0x00);    //�ȹ��ж�
        adxl345_write(ADXL_WRITE, DATA_FORMAT, 0x2B);   //�͵�ƽ����,13λȫ�ֱ���,��������Ҷ���,16g����  0�ж����ߵ�ƽ��Ч�������� 1���ж����͵�ƽ��Ч���½���
        adxl345_write(ADXL_WRITE, BW_RATE, 0x09);       //��������:�͹��Ĳ�����100Hz���  ��Ƶ�� 1hz   50hz  1��  50������  200ms��һ��
        adxl345_write(ADXL_WRITE, POWER_CTL, 0x28);     //����ģʽ ���Զ�˯��    08
        adxl345_write(ADXL_WRITE, FIFO_CTL, 0x8a);      //FIFO ��10��fifo
        adxl345_write(ADXL_WRITE, THRESH_TAP, 0x30);    //����THRESH_TAP(�û�����) 62.5mg/LSB   0xff=16g  ��ʱ��ȡ2g
        adxl345_write(ADXL_WRITE, DUR, 0x0F);           //����DUR���û�����ʱ�䣩  625us/LSB  ��ʱ��ȡ10ms
        adxl345_write(ADXL_WRITE, TAP_AXES, 0x07);      //���ø��᷽���Ƿ�μ��û����  xyzȫ������
        adxl345_write(ADXL_WRITE, THRESH_FF, 0x09);     //�������巧ֵ  0x05��0x09 300mg-600mg ����������жϼ��ٶ�((62.5mg/LSB)) ����ʱû����֤������
        adxl345_write(ADXL_WRITE, TIME_FF, 0x14);       //��������ʱ��  0x14��0x46 100ms-350ms (5ms/LSB)  ֱ�����
        adxl345_write(ADXL_WRITE, THRESH_ACT, 0x1a);    //˯�߼���activity��ֵ��  ���������ֵ��ʱ���ѣ�����10����1g    Activity���ֵ              (62.5mg/LSB)*0xxx
        adxl345_write(ADXL_WRITE, THRESH_INACT, 0x06);  //˯�߿�ʼinactivity��ֵ����С�����ֵ��ʱ��˯�ߣ�����02����0.2g  THRESH_INACT�����⾲ֹ��ֵ; (62.5mg/LSB)*0xxx
        adxl345_write(ADXL_WRITE, TIME_INACT, 0x05);   //��С��inactivityֵʱ�䳬�����ֵ��ʱ�����˯�ߣ�����02����2��   TIME_INACT��⾲ֹʱ�䷧ֵ; 2s(1s/LSB)*0xxx
        adxl345_write(ADXL_WRITE, ACT_INACT_CTL, 0xee); //������ϣ����������  �����и���û���ý���
        adxl345_write(ADXL_WRITE, INT_MAP, 0x5c);       //�ж�����ѡ�񣬴˴����ǽ�activity��Inactivityӳ�䵽INT2
        adxl345_write(ADXL_WRITE, INT_ENABLE, 0x5c);    //���ж� �˴����Ǵ�activity��Inactivity�ж�
        Data = adxl345_read(ADXL_READ, INT_SOURC);      //��ʵû�йػ�һֱ����
        globalvariable.EquipmentFlag = SetBit(globalvariable.EquipmentFlag, adx345_flag);
        MYLOG(INFO, "ADXL345_Init OK! = %x %x", Data, globalvariable.EquipmentFlag);
        return 0;
    }
    globalvariable.EquipmentFlag = ClrBit(globalvariable.EquipmentFlag, adx345_flag);
    MYLOG(ERROR, "ADXL345_Init Fail! = %x %x", id, globalvariable.EquipmentFlag);
    return 1;
}

/****************************************************************************
* Function Name  : ADX345_ReadXYZ
* Description    : ��ȡX,Y,Z��ADֵ
* Input          : xValue��X��ı����ַ
*                * yValue��Y��ı����ַ
*                * zValue��Z��ı����ַ
* Output         : None
* Return         : 0����ȡ�ɹ���0xFF����ȡʧ��
****************************************************************************/
/*��ȡADXL345�����������*/
#include "math.h"
void ADXL345_RD_XYZ(short *x, short *y, short *z) {
    static short xf = 0, yf = 0, zf = 256;
    short xt = 0, yt = 0, zt = 0;
    const  short acceleration = 200; //ͻ����ٶ� 256=1G
    float  scale = 3.9;              //����10��
    uint8_t buf[6];
    buf[0] = adxl345_read(ADXL_READ, DATA_X0);
    buf[1] = adxl345_read(ADXL_READ, DATA_X1);
    buf[2] = adxl345_read(ADXL_READ, DATA_Y0);
    buf[3] = adxl345_read(ADXL_READ, DATA_Y1);
    buf[4] = adxl345_read(ADXL_READ, DATA_Z0);
    buf[5] = adxl345_read(ADXL_READ, DATA_Z1);
    xt = (short)(((uint16_t)buf[1] << 8) + buf[0]); //DATA_X1Ϊ��λ��Ч�ֽ�
    yt = (short)(((uint16_t)buf[3] << 8) + buf[2]); //DATA_Y1Ϊ��λ��Ч�ֽ�
    zt = (short)(((uint16_t)buf[5] << 8) + buf[4]); //DATA_Z1Ϊ��λ��Ч�ֽ�
    //MYLOG(ERROR, "<x y z %04.f><%04.f %04.f %04.f>", sqrt((xt * scale) * (xt * scale) + (yt * scale) * (yt * scale) + (zt * scale) * (zt * scale)), xt * scale, yt * scale, zt * scale);
    //����������
    if(MYabs(xf, xt) < acceleration)  xf = (xf + 2 * xt) / 3;
    if(MYabs(yf, yt) < acceleration)  yf = (yf + 2 * yt) / 3;
    if(MYabs(zf, zt) < acceleration)  zf = (zf + 2 * zt) / 3;
    //�������˲� ����ƽ���ȱ��
    //xf = kalmanFilter(&KFP_adxl345_x,xf);
    //yf = kalmanFilter(&KFP_adxl345_y,yf);
    //zf = kalmanFilter(&KFP_adxl345_z,zf);
    *x = xf;//ת��Ϊ���ǵ�mg
    *y = yf;//ת��Ϊ���ǵ�mg
    *z = zf;//ת��Ϊ���ǵ�mg
//    HAL_UART_Transmit(&huart2, (uint8_t *)&zf, 2, 200);
//    HAL_UART_Transmit(&huart2, (uint8_t *)&xf, 2, 200);
//    HAL_UART_Transmit(&huart2, (uint8_t *)&yt, 2, 200);
//    HAL_UART_Transmit(&huart2, (uint8_t *)&yf, 2, 200);
//    HAL_UART_Transmit(&huart2, (uint8_t *)&zt, 2, 200);
//    HAL_UART_Transmit(&huart2, (uint8_t *)&zf, 2, 200);
    //LOG_WARNING  DEBUG
    //MYLOG(WARNING, "<x y z %5.2f><%04d %04d %04d>",sqrt((*x*4)*(*x*4)+(*y*4)*(*y*4)+(*z*4)*(*z*4)),*x*4,*y*4,*z*4);
    //MYLOG(DEBUG, "<x y z %04.f><%04.f %04.f %04.f>", sqrt((xf * scale) * (xf * scale) + (yf * scale) * (yf * scale) + (zf * scale) * (zf * scale)), xf * scale, yf * scale, zf * scale);
}
void ADXL345_RD_XYZ_Buf(uint8_t *buf) {
    buf[0] = adxl345_read(ADXL_READ, DATA_X0);
    buf[1] = adxl345_read(ADXL_READ, DATA_X1);
    buf[2] = adxl345_read(ADXL_READ, DATA_Y0);
    buf[3] = adxl345_read(ADXL_READ, DATA_Y1);
    buf[4] = adxl345_read(ADXL_READ, DATA_Z0);
    buf[5] = adxl345_read(ADXL_READ, DATA_Z1);
}
/*��ȡADXL345�����ݲ����˲�������times����ȡƽ��ֵ*/
void ADXL345_Read_Average(short *x, short *y, short *z, uint8_t times) {
    uint8_t i;
    short tx, ty, tz;
    *x = 0; *y = 0; *z = 0;
    if(times) {
        for(i = 0; i < times; i++) {
            ADXL345_RD_XYZ(&tx, &ty, &tz);
            *x += tx; *y += ty; *z += tz;
            HAL_Delay(20);
        }
        *x /= times; *y /= times; *z /= times;
    }
}

/*ADXL345�Զ�У׼����*/
void ADXL345_AUTO_Adjust(char *xval, char *yval, char *zval) {
    short tx, ty, tz;
    uint8_t i;
    short offx = 0, offy = 0, offz = 0;
    adxl345_write(ADXL_WRITE, POWER_CTL, 0x00); //�Ƚ�������ģʽ
    HAL_Delay(100);
    adxl345_write(ADXL_WRITE, DATA_FORMAT, 0x08); //�͵�ƽ�ж������13λȫ�ֱ��ʣ���������Ҷ��룬16g����
    adxl345_write(ADXL_WRITE, BW_RATE, 0x09); //��������ٶ�Ϊ100Hz
    adxl345_write(ADXL_WRITE, POWER_CTL, 0x28); //����ʹ�ܣ�����ģʽ
    adxl345_write(ADXL_WRITE, INT_ENABLE, 0x00); //��ʹ���ж�
    adxl345_write(ADXL_WRITE, OFSX, 0x00);
    adxl345_write(ADXL_WRITE, OFSY, 0x00);
    adxl345_write(ADXL_WRITE, OFSZ, 0x00);
    HAL_Delay(20);
    for(i = 0; i < 10; i++) {
        ADXL345_Read_Average(&tx, &ty, &tz, 10);
        offx += tx; offy += ty; offz += tz;
    }
    offx /= 10; offy /= 10; offz /= 10;
    *xval = -offx / 4;
    *yval = -offy / 4;
    *zval = -(offz - 256) / 4;
    adxl345_write(ADXL_WRITE, OFSX, *xval);
    adxl345_write(ADXL_WRITE, OFSY, *yval);
    adxl345_write(ADXL_WRITE, OFSZ, *zval);
}

/*����ADXL345�Ƕȣ�x/y/��ʾ�������ϵļ��ٶȷ�����dir��ʾҪ��õĽǶ�*/
short ADXL345_Get_Angle(float x, float y, float z, uint8_t dir) {
    float temp;
    float res = 0;  //����ֵ
    switch(dir) {
        case 0:     //0��ʾ��Z��ĽǶ�
            temp = sqrt((x * x + y * y)) / z;
            res = atan(temp);
            break;
        case 1:     //1��ʾ��X��ĽǶ�
            temp = x / sqrt((y * y + z * z));
            res = atan(temp);
            break;
        case 2:     //2��ʾ��Y��ĽǶ�
            temp = y / sqrt((x * x + z * z));
            res = atan(temp);
            break;
    }
    return res * 180 / 3.14; //���ؽǶ�ֵ
}

void ADXL345_Get_State(uint8_t value) {
    static uint8_t flag = 0;
    uint8_t readValue;
    if((value == 1) || (value == 2) || (value == 3)) {
//      readValue = adxl345_read(ADXL_READ, INT_SOURC);
        flag = value;
        return ;
    }
    if(flag == 0) return;
    readValue = adxl345_read(ADXL_READ, INT_SOURC);
    if(flag == 3) return;
    if(readValue & 0x08) {
        //Change_Run_State(2);  ��ִ���κβ���
        MYLOG(WARNING, "ADXL345 Inactivity = %d %x", flag, readValue);
    }
    if(readValue & 0x40) {
        Change_Run_State(1);
        MYLOG(WARNING, "ADXL345 SINGLE_TAP = %d %x", flag, readValue);
    }
    if(readValue & 0x10) {
        Change_Run_State(1);
        MYLOG(WARNING, "ADXL345 Activity = %d %x", flag, readValue);
    }
    if(readValue & 0x04) {
        Change_Run_State(1);
        MYLOG(WARNING, "ADXL345 FREE_FALL = %d %x", flag, readValue);
    }
    flag--;
}
/*------------------------------------------------------------------------------------------------------------------------
*Name:      step_counter()
*Function:  ʵ��Pedometer�Ļ����㷨.
*Input:     void
*Output:    void
*------------------------------------------------------------------------------------------------------------------------*/
typedef struct {
    uint8_t bad_flag;   //��Ч��־
    int16_t data[3];    //ADXL345 ԭʼ����
    int16_t adresult;   //����ƽ��ֵ
    int16_t max;        //���ֵ
    int16_t min;        //��С
    int16_t dc;         //��ֵ
    int16_t vpp;        //�����ٶ�
    int16_t precision;  //��̬����
    int16_t old_fixed;  //������λ����ֵ
    int16_t new_fixed;  //������λ����ֵ
} ADXL345_Base;

typedef struct {
    uint8_t  flag;               //��ʼ������־
    uint32_t time;               //��ʱ����ʱ��
    uint32_t steps;              //�Ʋ�����
    const uint8_t samplingtimes; //��������
    uint8_t  sampling;           //��������
    ADXL345_Base base[3];        //X,Y,Z��������
} ADXL345_Counter;

ADXL345_Counter Counter = {
    .flag  = 0,
    .time  = 0,
    .steps = 0,
    .samplingtimes = 50,
    .sampling = 0,
    .base[0] = {.bad_flag  = 0, .data = {1, 1, 0}, .adresult = 0, .max = -1000, .min = 1000, .dc = 200, .vpp = 30, .precision = 5, .old_fixed = 0, .new_fixed = 0,},
    .base[1] = {.bad_flag  = 0, .data = {1, 1, 0}, .adresult = 0, .max = -1000, .min = 1000, .dc = 200, .vpp = 30, .precision = 5, .old_fixed = 0, .new_fixed = 0,},
    .base[2] = {.bad_flag  = 0, .data = {1, 1, 0}, .adresult = 0, .max = -1000, .min = 1000, .dc = 200, .vpp = 30, .precision = 5, .old_fixed = 0, .new_fixed = 0,},
};

void ADXL345_Counter_steps(void) {
    Counter.steps++;
}

void ADXL345_clean_falg(void) {
    Counter.steps = 0;
    Counter.flag  = 0;
    Counter.time  = HAL_GetTick();
}

void Change_Run_State(uint8_t state) {
    globalvariable.RunFlag = state;
    globalvariable.RunTime = HAL_GetTick();
}

void ADXL345_filter(int16_t *pbuf) {
    //------------------------------------------�����˲�----------------------//
    int16_t i;
    for(i = 0; i < 3; i++) {
        Counter.base[i].data[2] = Counter.base[i].data[1];
        Counter.base[i].data[1] = Counter.base[i].data[0];
        Counter.base[i].data[0] = pbuf[i];
        Counter.base[i].adresult = (Counter.base[i].data[0] + Counter.base[i].data[1] + Counter.base[i].data[2]) / 3;
        if(Counter.base[i].adresult > Counter.base[i].max) {Counter.base[i].max = Counter.base[i].adresult;}  //���
        if(Counter.base[i].adresult < Counter.base[i].min) {Counter.base[i].min = Counter.base[i].adresult;}  //��С
    }
}
void ADXL345_peak_update(void) {
    //----------------------------------���㶯̬���޺Ͷ�̬����-----------------------//
    //ÿ�����ٶȱ仯Լ 55 mg
    int16_t i;
    if(Counter.sampling++ >= 50) {
        Counter.sampling = 0;
        for(i = 0; i < 3; i++) {
            Counter.base[i].vpp =  Counter.base[i].max - Counter.base[i].min;         //�����ٶ�
            Counter.base[i].dc  = (Counter.base[i].max + Counter.base[i].min) / 2;    //dcΪ��ֵ
            Counter.base[i].precision = information.accuracy;     //�������������ֵ����������̫����
            Counter.base[i].max = -1000;        //ÿsamplingtimes���ڸ���  16G <4096= 0X0FFF>
            Counter.base[i].min =  1000;        //ÿsamplingtimes���ڸ���
        }
    }
}
int16_t ADXL345_slid_update(void) {
    //--------------------------������λ�Ĵ���--------------------------------------
    int16_t i, res = 0;
    for(i = 0; i < 3; i++) {
        Counter.base[i].old_fixed = Counter.base[i].new_fixed;
        if(Counter.base[i].adresult >= Counter.base[i].new_fixed) {
            if((Counter.base[i].adresult - Counter.base[i].new_fixed) >= Counter.base[i].precision)   {
                Counter.base[i].new_fixed = Counter.base[i].adresult;
                res = 1;
            }
        } else if(Counter.base[i].adresult < Counter.base[i].new_fixed) {
            if((Counter.base[i].new_fixed - Counter.base[i].adresult) >= Counter.base[i].precision)   {
                Counter.base[i].new_fixed = Counter.base[i].adresult;
                res = 1;
            }
        }
    }
    return res;
}
void ADXL345_detect_step(void) {
    //------------------------- ��̬�����о� ----------------------------------
    static uint32_t time = 0 ;
    int16_t i;
    for(i = 0; i < 3; i++) { //�������ٶȲ��òμ���
        if((Counter.base[i].vpp >= Counter.base[(i + 1) % 3].vpp) && (Counter.base[i].vpp >= Counter.base[(i + 2) % 3].vpp)) { //�½���
            if((Counter.base[i].old_fixed >= Counter.base[i].dc) && (Counter.base[i].new_fixed < Counter.base[i].dc) && ((Counter.base[i].max - Counter.base[i].min) > information.threshold)) {
                //���ż��ٶ�
                //if(Counter.base[i].vpp < information.threshold) break; //���ٶ�С��0.2gֱ���˵�
                //MYLOG(WARNING, "Counter.base[%d].dc =%d %d %d %d", i, Counter.base[i].dc, Counter.base[i].vpp, Counter.base[i].new_fixed, Counter.base[i].old_fixed);
                //ʱ�䴰��0.2��2s����Ч
                uint32_t value = HAL_GetTick() ;
                value = DelayGetInterval(&value, &time);
                time = HAL_GetTick();
                if((value < 200) || (value > 3000)) {
                    MYLOG(WARNING, "time window =%d ", value);
                    break;
                }
                Counter.steps++;
            }
        }
    }
}

void ADXL345_Step2(int16_t *pbuf) {
    ADXL345_filter(pbuf);
    ADXL345_peak_update();
    if(ADXL345_slid_update()) {
        ADXL345_detect_step();
    }
}

void ADXL345_Step(int16_t *pbuf) {
    static uint32_t time = 0 ;
    int16_t i;
    Counter.flag = 1;
    //MYLOG(WARNING, "<x y z %5.2f><%04d %04d %04d>",sqrt((pbuf[0]*4)*(pbuf[0]*4)+(pbuf[1]*4)*(pbuf[1]*4)+(pbuf[2]*4)*(pbuf[2]*4)),pbuf[0]*4,pbuf[1]*4,pbuf[2]*4);
    //------------------------------------------�����˲�----------------------//
    for(i = 0; i < 3; i++) {
        Counter.base[i].data[2] = Counter.base[i].data[1];
        Counter.base[i].data[1] = Counter.base[i].data[0];
        Counter.base[i].data[0] = pbuf[i];
        Counter.base[i].adresult = (Counter.base[i].data[0] + Counter.base[i].data[1] + Counter.base[i].data[2]) / 3;
        if(Counter.base[i].adresult > Counter.base[i].max) {Counter.base[i].max = Counter.base[i].adresult;}  //���
        if(Counter.base[i].adresult < Counter.base[i].min) {Counter.base[i].min = Counter.base[i].adresult;}  //��С
    }
    //----------------------------------���㶯̬���޺Ͷ�̬����-----------------------//
    //ÿ�����ٶȱ仯Լ 55 mg
    //if(Counter.sampling++ >= Counter.samplingtimes) {
    if(Counter.sampling++ >= 50) {
        Counter.sampling = 0;
        for(i = 0; i < 3; i++) {
            Counter.base[i].vpp =  Counter.base[i].max - Counter.base[i].min;         //�����ٶ�
            Counter.base[i].dc  = (Counter.base[i].max + Counter.base[i].min) / 2;    //dcΪ��ֵ
            //Counter.base[i].dc  = Counter.base[i].min + (Counter.base[i].vpp >> 1); //dcΪ��ֵ
            Counter.base[i].bad_flag = 0;
            if(Counter.base[i].vpp >= 160 * 4) {
                Counter.base[i].precision = Counter.base[i].vpp / 32; //8
            } else if((Counter.base[i].vpp >= 50 * 4) && (Counter.base[i].vpp < 160 * 4)) {
                Counter.base[i].precision = 4;
            } else if((Counter.base[i].vpp >= 15 * 4) && (Counter.base[i].vpp < 50 * 4)) {
                Counter.base[i].precision = 3;
            } else {
                Counter.base[i].precision = 2;
                Counter.base[i].bad_flag = 1;//��Ч
            }
            //MYLOG(WARNING, "bad_flag[%d] =%d max =%03d min =%03d <<vpp = %03d dc = %03d precision = %d>>",i,Counter.base[i].bad_flag, Counter.base[i].max,Counter.base[i].min,Counter.base[i].vpp,Counter.base[i].dc,Counter.base[i].precision);
            Counter.base[i].precision = 30;  //�������������ֵ����������̫����
            Counter.base[i].max = -1000;        //ÿsamplingtimes���ڸ���  16G <4096= 0X0FFF>
            Counter.base[i].min =  1000;     //ÿsamplingtimes���ڸ���
        }
        Counter.flag = 1;
    }
    if(Counter.flag == 0) return; //������־
    //--------------------------������λ�Ĵ���--------------------------------------
    for(i = 0; i < 3; i++) {
        Counter.base[i].old_fixed = Counter.base[i].new_fixed;
//        if(Counter.base[i].adresult >= Counter.base[i].new_fixed) {      //����  ����
//            if((Counter.base[i].adresult - Counter.base[i].new_fixed) >= Counter.base[i].precision)   {Counter.base[i].new_fixed = Counter.base[i].adresult;}
//            //MYLOG(WARNING, "Counter.base[%d].new_fixed =%d %d Up",i,Counter.base[i].new_fixed,Counter.base[i].adresult);
//        } else if(Counter.base[i].adresult < Counter.base[i].new_fixed) {//����
//            if((Counter.base[i].new_fixed - Counter.base[i].adresult) >= ((Counter.base[i].vpp) / 2) + Counter.base[i].precision)   {Counter.base[i].new_fixed = Counter.base[i].adresult;}
//            //MYLOG(WARNING, "Counter.base[%d].new_fixed =%d %d Down",i,Counter.base[i].new_fixed,Counter.base[i].adresult);
//        }
        if(Counter.base[i].adresult >= Counter.base[i].new_fixed) {
            if((Counter.base[i].adresult - Counter.base[i].new_fixed) >= Counter.base[i].precision)   {Counter.base[i].new_fixed = Counter.base[i].adresult;}
        } else if(Counter.base[i].adresult < Counter.base[i].new_fixed) {
            if((Counter.base[i].new_fixed - Counter.base[i].adresult) >= Counter.base[i].precision)   {Counter.base[i].new_fixed = Counter.base[i].adresult;}
        }
    }
    //------------------------- ��̬�����о� ----------------------------------
    for(i = 0; i < 3; i++) { //�������ٶȲ��òμ���
        if((Counter.base[i].vpp >= Counter.base[(i + 1) % 3].vpp) && (Counter.base[i].vpp >= Counter.base[(i + 2) % 3].vpp)) { //�½���
            if((Counter.base[i].old_fixed >= Counter.base[i].dc) && (Counter.base[i].new_fixed < Counter.base[i].dc) && (Counter.base[i].bad_flag == 0)) {
                //���ż��ٶ�
                if(Counter.base[i].vpp < 100) break; //���ٶ�С��0.4gֱ���˵�
                //MYLOG(WARNING, "Counter.base[%d].dc =%d %d %d %d", i, Counter.base[i].dc, Counter.base[i].vpp, Counter.base[i].new_fixed, Counter.base[i].old_fixed);
                //ʱ�䴰��0.2��2s����Ч
                uint32_t value = HAL_GetTick() ;
                value = DelayGetInterval(&value, &time);
                time = HAL_GetTick();
                if((value < 200) || (value > 5000)) {
                    MYLOG(WARNING, "time window =%d ", value);
                    break;
                }
                Counter.steps++;
            }
        }
    }
}

uint32_t Step_Count(int16_t *pbuf) ;
void ADXL345_Step_Run(void) {
    static uint32_t stepflag = 0;
    static uint32_t steptime = 0;
//    static uint32_t step1 = 0, step2 = 0;
    int16_t buf[3], i;
    //if(DelayTimeOut(&Counter.time, 1000*10 / Counter.samplingtimes)) { //0.02s����һ��
    if(DelayTimeOut(&Counter.time, 200)) { //0.02s����һ��  50hz   0.2s   ��Լֻ��10����
        Counter.time = HAL_GetTick();
        for(i = 0; i < 10; i++) {
            ADXL345_RD_XYZ(&buf[0], &buf[1], &buf[2]); //������fifoʵ��
            //ת��Ϊ��ʵ��mg
            buf[0] = (buf[0] * 1000) / 255;
            buf[1] = (buf[1] * 1000) / 255;
            buf[2] = (buf[2] * 1000) / 255;
            if(information.algorithm==1) ADXL345_Step2(buf);
            if(information.algorithm==2) Pedometer(buf[0], buf[1], buf[2]);
        }
        if(stepflag == 1) {
            if(Counter.steps != globalvariable.steps) {
                Change_Run_State(1);
                globalvariable.steps = Counter.steps;
                MYLOG(WARNING, "step_counter_run = %d", Counter.steps); //�������
                steptime = HAL_GetTick() ;
            }
        } else {
            if(Counter.steps >= 3) { //��Լ3�������߲����������Ǹ���
                MYLOG(WARNING, "step_counter_start = %d", Counter.steps); //�������
                Change_Run_State(1);
                stepflag = 1;
                globalvariable.steps += 3;
                Counter.steps = globalvariable.steps;
                steptime = HAL_GetTick() ;
            }
        }
        if(DelayTimeOut(&steptime, 1000 * 5)) { //����3sû�и��²����͵�ֹͣ�˶��ˡ���Ҫ���¼���
            stepflag = 0;
            ADXL345_clean_falg();
            steptime = HAL_GetTick() ;
        }
        ADXL345_Get_State(0);
    }
}


#define P_P_DIFF    256*256*0.2   /* ����-���ȵĲ�ֵ,��3D��ֵ */
#define RISING_EDGE  1 /*������״̬ */
#define FALLING_EDGE 0 /*�½���״̬*/
#define FAST_WALK_TIME_LIMIT_MS 200     /* ms */
#define SLOW_WALK_TIME_LIMIT_MS 10000 /* 10s ��û����һ�� */
#define STEP_OK 3   /* 7������ */
#define FALSE 0
#define TRUE 1

uint32_t lastPos = 0;   /* ������*/
uint32_t newMax = 0, newMin = 0; /* ����-���� */

uint8_t walkSta = FALSE; /* ���һ�η�ֵ״̬ */
uint8_t walkOkSta = FALSE; /* ����10s������7�� ��Ч����״̬ */
uint8_t pSta = RISING_EDGE; /* 3D���ݲ���״̬ */

uint32_t lastTime = 0;      /* ��һ�� walkSta ��ʱ�� */
uint32_t stepOK = 0; /*��ʼ�Ʋ����� �˳����� */
uint32_t stepCount = 0; /* �Ʋ�ֵ */
uint32_t Step_Count(int16_t *pbuf) {
    uint32_t nowPos = 0;
    uint32_t ppDiff = 0;
    uint32_t timeDiff = 0;
    /*��ȡ3D IMU */
    //nowPos = (unsigned int)powf(sqrtf(axis0) + sqrtf(axis1) + sqrtf(axis2), 0.5);
    nowPos = pbuf[0] * pbuf[0] + pbuf[1] * pbuf[1] + pbuf[2] * pbuf[2];
    /* �õ�����Ͳ��� */
    if((pSta == RISING_EDGE) && (nowPos <= lastPos)) {
        pSta = FALLING_EDGE;
        newMax = lastPos;
        walkSta = TRUE;
    } else if((pSta == FALLING_EDGE) && (nowPos > lastPos)) {
        pSta = FALLING_EDGE;
        newMin = lastPos;
        walkSta = TRUE;
    } else {
        walkSta = FALSE;
    }
    /* ����3D step״̬���� */
    lastPos = nowPos;
    /* �в���򲨹� */
    if(walkSta == TRUE) {
        walkSta = FALSE;
        ppDiff = newMax - newMin; /* �����벨�ȵĲ�ֵ */
        //MYLOG(WARNING, "ppDiff = %x %x %x", ppDiff,newMax,newMin); //�������
        if(ppDiff > P_P_DIFF) {
            timeDiff = HAL_GetTick() - lastTime; /* ��ȡ����Ͳ��ȵ�ʱ��� */
            if(timeDiff < FAST_WALK_TIME_LIMIT_MS) { /* ���岨��ʱ���С��200ms��ֱ��ȥ�� */
                return stepCount;
            } else if(timeDiff > SLOW_WALK_TIME_LIMIT_MS) { /* ���岨��ʱ������10s����Ϊ��ֹ   */
                walkOkSta = FALSE;
                stepOK = 0;
                return stepCount;
            }
            stepOK++;
            if(stepOK >= STEP_OK) { /* ��7��֮�����״̬ */
                walkOkSta = TRUE;
            }
            lastTime = HAL_GetTick(); /* ����ʱ�� */
        }
    }
    if(walkOkSta == TRUE) { /* ����10s����7��*/
        stepCount += stepOK;
        stepOK = 0;
    }
    return stepCount;
}


//uint8_t  ADXL345_FLAG = 0;        //20MS���жϱ�־
//uint32_t ADXL345_FLAG_time = 0;   //����5s�ſ�ʼ����


//uint8_t  ADXL345_bad_flag[3];                //��Ч��־
//uint32_t ADXL345_array0[3] = {1, 1, 1};      //����X�˲�����
//uint32_t ADXL345_array1[3] = {1, 1, 1};      //����X�˲�����
//uint32_t ADXL345_array2[3] = {0, 0, 0};      //����X�˲�����

//uint32_t ADXL345_adresult[3];                //����ƽ��ֵ
//uint32_t ADXL345_max[3] = {0, 0, 0};         //���ֵ
//uint32_t ADXL345_min[3] = {1000, 1000, 1000};//��С
//uint32_t ADXL345_dc[3] =  {500, 500, 500};    //��ֵ
//uint32_t ADXL345_vpp[3] = {30, 30, 30};      //�����ٶ�
//uint32_t ADXL345_precision[3] = {5, 5, 5};   //��̬����
//uint32_t ADXL345_old_fixed[3];               //������λ����ֵ
//uint32_t ADXL345_new_fixed[3];               //������λ����ֵ
//uint32_t ADXL345_STEPS = 0;                  //����

//void ADXL345_StepXX(void) {
//    static uint8_t sampling_counter = 0; //50��*0.2s = 10s
//    uint8_t buf[6], jtemp;
//    ADXL345_FLAG = 0;
//    ADXL345_RD_XYZ_Buf(buf);          //�����������ݣ��洢��BUF��
//    //------------------------------------------�����˲�----------------------//
//    for(jtemp = 0; jtemp <= 2; jtemp++) { //jtemp 0,1,2�ֱ����x��y��z
//        ADXL345_array2[jtemp] = ADXL345_array1[jtemp];
//        ADXL345_array1[jtemp] = ADXL345_array0[jtemp];
//        ADXL345_array0[jtemp] = buf[2 * jtemp] + (buf[2 * jtemp + 1] << 8);
//        ADXL345_adresult[jtemp] = ADXL345_array0[jtemp] + ADXL345_array1[jtemp] + ADXL345_array2[jtemp];
//        ADXL345_adresult[jtemp] = ADXL345_adresult[jtemp] / 3;
//        if(ADXL345_adresult[jtemp] > ADXL345_max[jtemp]) {ADXL345_max[jtemp] = ADXL345_adresult[jtemp];}
//        if(ADXL345_adresult[jtemp] < ADXL345_min[jtemp]) {ADXL345_min[jtemp] = ADXL345_adresult[jtemp];}
//    }
//    sampling_counter = sampling_counter + 1;
//    //----------------------------------���㶯̬���޺Ͷ�̬����-----------------------//
//    if(sampling_counter >= 20) {
//        sampling_counter = 0;
//        for(jtemp = 0; jtemp <= 2; jtemp++) {
//            ADXL345_vpp[jtemp] = ADXL345_max[jtemp] - ADXL345_min[jtemp];
//            ADXL345_dc[jtemp] = ADXL345_min[jtemp] + (ADXL345_vpp[jtemp] >> 1); //dcΪ��ֵ
//            ADXL345_max[jtemp] = 0;
//            ADXL345_min[jtemp] = 1023;
//            ADXL345_bad_flag[jtemp] = 0;
//            if(ADXL345_vpp[jtemp] >= 160) {
//                ADXL345_precision[jtemp] = ADXL345_vpp[jtemp] / 32; //8
//            } else if((ADXL345_vpp[jtemp] >= 50) && (ADXL345_vpp[jtemp] < 160)) {
//                ADXL345_precision[jtemp] = 4;
//            } else if((ADXL345_vpp[jtemp] >= 15) && (ADXL345_vpp[jtemp] < 50)) {
//                ADXL345_precision[jtemp] = 3;
//            } else {
//                ADXL345_precision[jtemp] = 2;
//                ADXL345_bad_flag[jtemp] = 1;
//            }
//            ADXL345_precision[jtemp] = 50;   //����һ�£�����̫����
//        }
//    }
//    //--------------------------������λ�Ĵ���--------------------------------------
//    for(jtemp = 0; jtemp <= 2; jtemp++) {
//        ADXL345_old_fixed[jtemp] = ADXL345_new_fixed[jtemp];
//        if(ADXL345_adresult[jtemp] >= ADXL345_new_fixed[jtemp]) {
//            if((ADXL345_adresult[jtemp] - ADXL345_new_fixed[jtemp]) >= ADXL345_precision[jtemp])   {ADXL345_new_fixed[jtemp] = ADXL345_adresult[jtemp];}
//        } else if(ADXL345_adresult[jtemp] < ADXL345_new_fixed[jtemp]) {
//            if((ADXL345_new_fixed[jtemp] - ADXL345_adresult[jtemp]) >= ADXL345_precision[jtemp])   {ADXL345_new_fixed[jtemp] = ADXL345_adresult[jtemp];}
//        }
//    }
//    //------------------------- ��̬�����о� ----------------------------------
//    if((ADXL345_vpp[0] >= ADXL345_vpp[1]) && (ADXL345_vpp[0] >= ADXL345_vpp[2])) { //x�����Ծ
//        if((ADXL345_old_fixed[0] >= ADXL345_dc[0]) && (ADXL345_new_fixed[0] < ADXL345_dc[0]) && (ADXL345_bad_flag[0] == 0)) {
//            ADXL345_STEPS = ADXL345_STEPS + 1;
//        }
//    } else if((ADXL345_vpp[1] >= ADXL345_vpp[0]) && (ADXL345_vpp[1] >= ADXL345_vpp[2])) { //y�����Ծ
//        if((ADXL345_old_fixed[1] >= ADXL345_dc[1]) && (ADXL345_new_fixed[1] < ADXL345_dc[1]) && (ADXL345_bad_flag[1] == 0)) {
//            ADXL345_STEPS = ADXL345_STEPS + 1;
//        }
//    } else if((ADXL345_vpp[2] >= ADXL345_vpp[1]) && (ADXL345_vpp[2] >= ADXL345_vpp[0])) { //z�����Ծ
//        if((ADXL345_old_fixed[2] >= ADXL345_dc[2]) && (ADXL345_new_fixed[2] < ADXL345_dc[2]) && (ADXL345_bad_flag[2] == 0)) {
//            ADXL345_STEPS = ADXL345_STEPS + 1;
//        }
//    }
//}

//void ADXL345_Step_RunXX(void) {
//    static uint32_t pSTEPS = 0;                  //����
//    HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_SET);
//    if(DelayTimeOut(&ADXL345_FLAG_time, 50)) {   //0.02s����һ��
//        ADXL345_FLAG_time = HAL_GetTick();
//        ADXL345_StepXX();
//        //MYLOG(WARNING, "step_counter_run = %d", STEPS); //�������
//        if(pSTEPS != ADXL345_STEPS) {
//            pSTEPS = ADXL345_STEPS;
//            MYLOG(WARNING, "step_counter_run = %d", ADXL345_STEPS); //�������
//            HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_RESET);
//        }
//        ADXL345_Get_State(0);
//    }
//}

void ADXL345_Test(void) {
    short x, y, z;
    short xang, yang, zang;
    //uint8_t key;
    ADXL345_Read_Average(&x, &y, &z, 10);
    printf("Average = %d %d %d\r\n", x, y, z);
    xang = ADXL345_Get_Angle(x, y, z, 1);
    yang = ADXL345_Get_Angle(x, y, z, 2);
    zang = ADXL345_Get_Angle(x, y, z, 0);
    printf("Angle   = %d %d %d\r\n", xang, yang, zang);
    //ADXL345_AUTO_Adjust((char*)&x, (char*)&y, (char*)&z);
}
void ADXL345_See(void) {
//    uint8_t Data;
//    //ADXL345_Test();
//    HAL_I2C_Mem_Read(adxl345, ADXL_READ, 0x30, I2C_MEMADD_SIZE_8BIT, &Data, 1, 0xff);
//    printf("ADXL345_See 0x30 = %x\r\n",Data);
    //if(Data&0x10)  printf("ADXL345_See 0x30 = %x\r\n",Data);
}
