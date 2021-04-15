#ifndef __DATA_H__
#define __DATA_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32l1xx_hal.h"
#include "stdint.h"
//**************************************************************************************
//JY_901 �������ݽṹ
#pragma pack(push,1) //�������״̬  1�ֽ� __packed
typedef struct {
    uint8_t *cmd;       //����
    uint8_t rdwr;       //��д��־ 0�ɶ� 1��д������ 2��д������
    void *parameter; //����
    uint8_t len;        //����
}AT_DEBUG_STR;          //�ַ�������
//typedef struct {
//    uint8_t *cmd;       //����
//    uint8_t rdwr;       //��д��־ 0�ɶ� 1��д
//    void *parameter;    //����
//    uint8_t len;        //����
//}AT_DEBUG_VAR;//��������

typedef struct {
    //------------------------------
    //�з���
    //ƽ���Ƕ�
    int16_t Rx_mid;
    int16_t Ry_mid;
    int16_t Rz_mid;
    //���ٶ�
    int16_t Ax_max;
    int16_t Ay_max;
    int16_t Az_max;
    //���ٶ�
    int16_t Ax_min;
    int16_t Ay_min;
    int16_t Az_min;
    //------------------------------
    //�޷���
    //�ų�
    uint16_t JD_max;
    uint16_t JD_min;
    //�Ʋ���
    uint16_t StepbyStep;
    //��ص�ѹ
    uint8_t vbat;
} Upload; //25�ֽڹ̶�

typedef struct {
    __IO uint32_t A_max;  //�����ٶ� = x^2 + y^2 + z^2
    __IO uint32_t A_min;  //��С���ٶ� = x^2 + y^2 + z^2
    __IO uint16_t J_max;  //�������ֵ
    __IO uint16_t J_min;  //������Сֵ
    __IO int32_t  Flag;   //��־λ
    __IO int32_t  R_count;//�Ƕȼ���
    __IO int32_t  Rx_add; //X�ǶȺ�
    __IO int32_t  Ry_add; //Y�ǶȺ�
    __IO int32_t  Rz_add; //Z�ǶȺ�
    __IO Upload   upload; //ԭʼ����
    __IO uint16_t StepbyStep;//�Ʋ�����
    __IO uint8_t  vbat;       //�������
} Upload_calculation;         //26�ֽڹ̶�

typedef struct {
    //ŷ����
    int16_t Rx;
    int16_t Ry;
    int16_t Rz;
    //���ٶ�
    int16_t Wx;
    int16_t Wy;
    int16_t Wz;
    //���ٶ�
    int16_t Ax;
    int16_t Ay;
    int16_t Az;
    //�ų�
    int16_t Hx;
    int16_t Hy;
    int16_t Hz;
    //�Ʋ���
    uint16_t StepbyStep;
} JY_901_Upload; //9�� ���11�ֽ�

typedef struct {
    uint8_t  head;
    uint8_t  cmd;
    int16_t  data[4];
    uint8_t  sum;
} JY_901_DATA; //9�� ���11�ֽ�

typedef struct {
    uint16_t DataSta;         //���ձ���
    union {
        uint8_t     Data[11]; //��������
        JY_901_DATA JY_901;
    } Data;
} BSP_UART_JY_901;
//#pragma pack(pop) //�ָ�����״̬

//**************************************************************************************
//HKJ_15C �������ݽṹ
//#pragma pack(push,1) //�������״̬  1�ֽ� __packed
typedef struct {
    //����
    //uint16_t JD[13];
    uint16_t JD1;
    uint16_t JD2;
    uint16_t JD3;
    uint16_t JD4;
    uint16_t JD5;
    uint16_t JD6;
    uint16_t JD7;
    uint16_t JD8;
    uint16_t JD9;
    uint16_t JD10;
    uint16_t JD11;
    uint16_t JD12;
    uint16_t JD13;
} HKJ_15C_Upload; //9�� ���11�ֽ�

typedef struct {
    uint8_t  head;//0xff �̶�Ϊ����֡��ʼλ
    uint8_t  dev; //����0xc6 ���磺HKJ-15C
    uint8_t  len; //�������ȡ�У�顢����������ֽ���
    uint8_t  sum; //���ȡ����������ͺ�ȡ���ֽ�
    //0xA0  ����
    //0xA1  ֹͣ
    //0xA2  ���豸��
    //0xA3  ����������
    //0xA4  ��������
    //0xAA  ����
    //0xA8  �ػ�
    uint8_t  cmd; //�������
    uint16_t data[25];
} HKJ_15C_DATA; //���� ���55�ֽ�

typedef struct {
    uint16_t DataSta;         //���ձ���
    union {
        uint8_t     Data[55]; //��������
        HKJ_15C_DATA HKJ_15C;
    } Data;
} BSP_UART_HKJ_15C;
//#pragma pack(pop) //�ָ�����״̬

//**************************************************************************************
//���� �������ݽṹ
//#pragma pack(push,1) //�������״̬  1�ֽ� __packed
typedef struct {
#define  BSP_UART_NewlineString_MAX   64
    uint16_t DataSta;                          //���ձ���
    uint8_t  Data[BSP_UART_NewlineString_MAX]; //��������
} BSP_UART_NewlineString;
//**************************************************************************************
//24L01 �ϴ�����Э��
typedef struct {
    uint8_t  head;//��ͷ0xaa
    uint8_t  dev; //��֫0x0a ��֫0x0b
    uint8_t  len; //����
    uint8_t  cmd; //����
    union {
        uint16_t       Data[13];
        JY_901_Upload  JY_901;  //���ٶ�����
        HKJ_15C_Upload HKJ_15C; //��������
    } Data;
    uint8_t  sum;
    uint8_t  tail;//β��0x55
} Upload_DATA;    //9�� ���32�ֽ�

typedef struct {
    uint8_t  head;         //��ͷ0xaa
    uint8_t  serialnumber; //���к�
    uint16_t id;           //ͨѶ��
    uint8_t  cmd;          //����
    union {
        uint8_t       Data[25];
        Upload        upload;  //�����ϴ�Э��
    } Data;
    uint8_t  sum;
    uint8_t  tail;     //β��0x55
} Upload_DATA_V1_0;    //9�� ���32�ֽ�

enum equipments {
    HOPEN_DEF = 0,  //Ĭ�ϣ�����ģʽ��Ҫ��Ӧ��������ת��Ϊ�������豸
    HOPEN_UL,       //��֫�� 1
    HOPEN_UR,       //��֫�� 2
    HOPEN_DL,       //��֫�� 3
    HOPEN_DR,       //��֫�� 4
    HOPEN_DX,       //Ԥ��   5
    HOPEN_ECG       // �ĵ�   6
};

enum Equipmentflag {
    axp173_flag = 0x01,  //01  1��Ч 0��Ч
    adx345_flag = 0x02,  //02  1��Ч 0��Ч
    nrf24l_flag = 0x04,  //04  1��Ч 0��Ч
    jiy901_flag = 0x08,  //08  1��Ч 0��Ч
    hkj15c_flag = 0x10,  //10  1��Ч 0��Ч
};

//�豸��Ϣ
typedef struct {
    //ֻ������
    uint8_t EquipmentName[15];            //R  �豸���ƣ�HOPEN_Wearable
    uint8_t SoftwareVersion[5];           //R  ����汾: V1.0
    uint8_t SoftwareReleaseDate[9];       //R  ����������� 20201215
    uint8_t EquipmentID[11];              //RW �豸ID��HOPEN_1234
    uint8_t Password[7];                  //RW �������룺admin
    //��д���� �ַ���
    uint8_t  EquipmentType;               //RW �豸���� 0������ģʽ����Ҫ������ز�����1:��֫L   2:��֫R   3:��֫L  4����֫R  5��Ԥ��  6:�ĵ�ģ��   ������Ч
    uint8_t  Channel;                     //R  ����Ƶ��255
    uint16_t UpId;                        //RW �ϴ�ͨѶID
    uint8_t  Debugbak;                    //RW  ����debug�ȼ�
    
    uint16_t threshold;                   //RW  ��̬��ֵ
    uint8_t  accuracy;                    //RW  ��̬����
    
    uint8_t  algorithm;                   //RW  �㷨1 2 3 4
    uint8_t  crc;                         //crc
}DeviceInformation;
typedef struct {
    uint8_t  Debug;                       //RW �ȼ�
    uint8_t  CfgFlag;                     //T ���Կ���
    uint8_t  RestFlag;                    //T ��λ
    uint8_t  SleepFlag;                   //O ��λ��
    uint8_t  EquipmentFlag;               //R ģ���־λ
    
    uint8_t  RunFlag;                     //R ����״̬
    uint32_t RunTime;                     //R ״̬����ʱ��
    uint32_t steps;                       //R ����
    uint32_t HKJ_15C;                     //��ʼ����־
    uint32_t ADXL345_adj;                 //ADXL345У׼
}GlobalVariable;
    
#pragma pack(pop) //�ָ�����״̬

#ifdef __cplusplus
}
#endif
#endif
