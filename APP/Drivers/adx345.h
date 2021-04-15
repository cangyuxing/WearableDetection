#ifndef __ADX345_H__
#define __ADX345_H__

#include "main.h"

/* ADX345��I2C��ַ */
#define ADX345_ADDR        0xA6   //ADX345��I2C��ַ
void Init_ADXL345(void) ;
void step_counter_run(void);
void Get_ADXL345_State(uint8_t value);


/* ADXL345��ָ��� */
#define ADX_DEVID          0x00   //����ID
#define ADX_DATA_FORMAT    0x31   //���ݸ�ʽ����
#define THRESH_ACT         0x24   //���ֵ
#define THRESH_INACT       0x25   //��ֹ��ֵ
#define TIME_INACT         0x26   //��ֹʱ��
#define ACT_INACT_CTL      0x27   //��ʹ�ܿ��ƻ�;�ֹ���
#define THRESH_FF          0x28   //����������ֵ
#define TIME_FF            0x29   //��������ʱ��
#define INT_SOURCE         0x30   //�ж�Դ

#define ADX_BW_RATE        0x2C   //�������ʼ�����ģʽ����
#define ADX_POWER_CTL      0x2D   //ʡ�����Կ���
#define ADX_INT_ENABLE     0x2E   //�ж�ʹ�ܿ���

#define INT_MAP            0x2F   //�ж�ӳ�����
#define FIFO_CTL           0x38   //FIFO����
#define FIFO_STATUS        0x39   //FIFO״̬
#define ADX_OFSX           0x1E   //�û���ֵ
#define ADX_OFSY           0x1F   //X��ƫ��
#define ADX_OFSZ           0x20   //Y��ƫ��

//typedef struct {
//    int16_t X;
//    int16_t Y;
//    int16_t Z;
//} SENSOR_DATA_TypeDef;

//SENSOR_DATA_TypeDef ADXL345_ADD(SENSOR_DATA_TypeDef data0, SENSOR_DATA_TypeDef data1);
//SENSOR_DATA_TypeDef ADXL345_MINUS(SENSOR_DATA_TypeDef data0, SENSOR_DATA_TypeDef data1);
//SENSOR_DATA_TypeDef get_thresh(SENSOR_DATA_TypeDef data0, SENSOR_DATA_TypeDef data1);
//void ADX_GetXYZDataNEW(int16_t *xValue1, int16_t *yValue1, int16_t *zValue1, SENSOR_DATA_TypeDef *buf);
//void ADXL345_STEPCOUNT(SENSOR_DATA_TypeDef NEW_SAMPLE);

///* ����ȫ�ֺ��� */
//int ADX345_Init(void);
//void ADX345_Adjust(void);
////void ADX_GetXYZData(int16_t *xValue, int16_t *yValue, int16_t *zValue);
//void ADX_GetXYZData(int16_t *xValue, int16_t *yValue, int16_t *zValue, SENSOR_DATA_TypeDef *axis_converted_avg);
//int16_t ADX_GetAngle(float xValue, float yValue, float zValue, uint8_t dir);
//void ADX_Test(void);
#endif
