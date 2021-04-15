#ifndef __USER_FILTER_H__
#define __USER_FILTER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
//1. 结构体类型定义
typedef struct {
    float LastP;//上次估算协方差 初始化值为0.02
    float Now_P;//当前估算协方差 初始化值为0
    float out;  //卡尔曼滤波器输出 初始化值为0
    float Kg;   //卡尔曼增益 初始化值为0
    float Q;    //过程噪声协方差 初始化值为0.001
    float R;    //观测噪声协方差 初始化值为0.543
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
