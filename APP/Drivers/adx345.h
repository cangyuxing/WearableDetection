#ifndef __ADX345_H__
#define __ADX345_H__

#include "main.h"

/* ADX345的I2C地址 */
#define ADX345_ADDR        0xA6   //ADX345的I2C地址
void Init_ADXL345(void) ;
void step_counter_run(void);
void Get_ADXL345_State(uint8_t value);


/* ADXL345的指令表 */
#define ADX_DEVID          0x00   //器件ID
#define ADX_DATA_FORMAT    0x31   //数据格式控制
#define THRESH_ACT         0x24   //活动阈值
#define THRESH_INACT       0x25   //静止阈值
#define TIME_INACT         0x26   //静止时间
#define ACT_INACT_CTL      0x27   //轴使能控制活动和静止检测
#define THRESH_FF          0x28   //自由落体阈值
#define TIME_FF            0x29   //自由落体时间
#define INT_SOURCE         0x30   //中断源

#define ADX_BW_RATE        0x2C   //数据速率及功率模式控制
#define ADX_POWER_CTL      0x2D   //省电特性控制
#define ADX_INT_ENABLE     0x2E   //中断使能控制

#define INT_MAP            0x2F   //中断映射控制
#define FIFO_CTL           0x38   //FIFO控制
#define FIFO_STATUS        0x39   //FIFO状态
#define ADX_OFSX           0x1E   //敲击阈值
#define ADX_OFSY           0x1F   //X轴偏移
#define ADX_OFSZ           0x20   //Y轴偏移

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

///* 声明全局函数 */
//int ADX345_Init(void);
//void ADX345_Adjust(void);
////void ADX_GetXYZData(int16_t *xValue, int16_t *yValue, int16_t *zValue);
//void ADX_GetXYZData(int16_t *xValue, int16_t *yValue, int16_t *zValue, SENSOR_DATA_TypeDef *axis_converted_avg);
//int16_t ADX_GetAngle(float xValue, float yValue, float zValue, uint8_t dir);
//void ADX_Test(void);
#endif
