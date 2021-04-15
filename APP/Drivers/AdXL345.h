#ifndef __ADX345_H
#define __ADX345_H

#include "main.h"

/* ADX345��I2C��ַ */
//#define ADXL_WRITE         0x3A           //ADX345��I2C��ַ0x3A ALT ADDRESS �ߵ�ƽ
//#define ADXL_READ          ADXL_WRITE+1   //ADX345��I2C��ַ0x3B ALT ADDRESS �ߵ�ƽ

#define ADXL_WRITE         0xA6           //ADX345��I2C��ַ0x3A ALT ADDRESS �͵�ƽ
#define ADXL_READ          ADXL_WRITE+1   //ADX345��I2C��ַ0x3B ALT ADDRESS �͵�ƽ
/* ADXL345��ָ��� */
#define DEVICE_ID          0x00   //����ID
#define DATA_FORMAT        0x31   //���ݸ�ʽ����
#define BW_RATE            0x2C   //�������ʼ�����ģʽ����
#define POWER_CTL          0x2D   //ʡ�����Կ���
#define INT_ENABLE         0x2E   //�ж�ʹ�ܿ���
#define OFSX               0x1E   //�û���ֵ
#define OFSY               0x1F   //X��ƫ��
#define OFSZ               0x20   //Y��ƫ��
#define INT_SOURC          0X30   //�ж�Դ

#define FIFO_CTL           0X38   //FIFO����

#define THRESH_TAP         0x1D   //�û���ֵ ��������Ϊ62.5mg/LSB(��0xFF =16 g)
#define DUR                0x21   //�û�����ʱ��
#define TAP_AXES           0x2A   //����/˫�������
#define ACT_TAP_STATUS     0x2B   //����/˫��Դ

#define THRESH_FF          0x28   //����������ֵ
#define TIME_FF            0x29   //��������ʱ��
#define THRESH_ACT         0x24   //���ֵ
#define THRESH_INACT       0x25   //��ֹ��ֵ
#define TIME_INACT         0x26   //��ֹʱ��

#define ACT_INACT_CTL      0x27   //��ʹ�ܿ��ƻ�;�ֹ���
#define INT_MAP            0x2F   //�ж�ӳ�����

#define DATA_X0            0X32
#define DATA_X1            0X33  //��DATAX0���x���������(�����Ʋ���),DATAX1Ϊ��λ,4mg/Bit
#define DATA_Y0            0X34
#define DATA_Y1            0X35  //��DATAY0���Y���������(�����Ʋ���),DATAY1Ϊ��λ,4mg/Bit
#define DATA_Z0            0X36
#define DATA_Z1            0X37  //��DATAZ0���Z���������(�����Ʋ���),DATAZ1Ϊ��λ,4mg/Bit


void ADXL345_Counter_steps(void) ;
void ADXL345_clean_falg(void);
void Change_Run_State(uint8_t state);
uint8_t ADXL345_Init(void);
void ADXL345_RD_XYZ(short *x, short *y, short *z);
void ADXL345_Read_Average(short *x, short *y, short *z, uint8_t times);
void ADXL345_AUTO_Adjust(char *xval, char *yval, char *zval);
short ADXL345_Get_Angle(float x, float y, float z, uint8_t dir);
void ADXL345_Get_State(uint8_t value);
void ADXL345_Step_Run(void);
void ADXL345_Step2(int16_t *pbuf);
    
void ADXL345_Auto_Adjust(void);
//void ADXL345_Step_RunXX(void);
void ADXL345_Test(void);
void ADXL345_See(void) ;
#endif
