#ifndef __USER_FILTER_H__
#define __USER_FILTER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
//1. �ṹ�����Ͷ���
typedef struct {
    float LastP;//�ϴι���Э���� ��ʼ��ֵΪ0.02
    float Now_P;//��ǰ����Э���� ��ʼ��ֵΪ0
    float out;  //�������˲������ ��ʼ��ֵΪ0
    float Kg;   //���������� ��ʼ��ֵΪ0
    float Q;    //��������Э���� ��ʼ��ֵΪ0.001
    float R;    //�۲�����Э���� ��ʼ��ֵΪ0.543
} KFP; //Kalman Filter parameter
extern KFP KFP_height;
extern KFP KFP_adxl345_x;
extern KFP KFP_adxl345_y;
extern KFP KFP_adxl345_z;

short kalmanFilter(KFP *kfp, short input) ;
uint16_t kalman_filter(uint16_t ADC_Value);
uint16_t kalman_filter_y(uint16_t ADC_Value);
//void Pedometer(void);
void Pedometer(int16_t x,int16_t y,int16_t z);
#ifdef __cplusplus
}
#endif
#endif /*__ GPIO_H__ */
