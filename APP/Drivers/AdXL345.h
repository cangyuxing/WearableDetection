#ifndef __ADX345_H
#define __ADX345_H

#include "main.h"

/* ADX345的I2C地址 */
//#define ADXL_WRITE         0x3A           //ADX345的I2C地址0x3A ALT ADDRESS 高电平
//#define ADXL_READ          ADXL_WRITE+1   //ADX345的I2C地址0x3B ALT ADDRESS 高电平

#define ADXL_WRITE         0xA6           //ADX345的I2C地址0x3A ALT ADDRESS 低电平
#define ADXL_READ          ADXL_WRITE+1   //ADX345的I2C地址0x3B ALT ADDRESS 低电平
/* ADXL345的指令表 */
#define DEVICE_ID          0x00   //器件ID
#define DATA_FORMAT        0x31   //数据格式控制
#define BW_RATE            0x2C   //数据速率及功率模式控制
#define POWER_CTL          0x2D   //省电特性控制
#define INT_ENABLE         0x2E   //中断使能控制
#define OFSX               0x1E   //敲击阈值
#define OFSY               0x1F   //X轴偏移
#define OFSZ               0x20   //Y轴偏移
#define INT_SOURC          0X30   //中断源

#define FIFO_CTL           0X38   //FIFO控制

#define THRESH_TAP         0x1D   //敲击阈值 比例因子为62.5mg/LSB(即0xFF =16 g)
#define DUR                0x21   //敲击持续时间
#define TAP_AXES           0x2A   //单击/双击轴控制
#define ACT_TAP_STATUS     0x2B   //单击/双击源

#define THRESH_FF          0x28   //自由落体阈值
#define TIME_FF            0x29   //自由落体时间
#define THRESH_ACT         0x24   //活动阈值
#define THRESH_INACT       0x25   //静止阈值
#define TIME_INACT         0x26   //静止时间

#define ACT_INACT_CTL      0x27   //轴使能控制活动和静止检测
#define INT_MAP            0x2F   //中断映射控制

#define DATA_X0            0X32
#define DATA_X1            0X33  //与DATAX0组成x轴输出数据(二进制补码),DATAX1为高位,4mg/Bit
#define DATA_Y0            0X34
#define DATA_Y1            0X35  //与DATAY0组成Y轴输出数据(二进制补码),DATAY1为高位,4mg/Bit
#define DATA_Z0            0X36
#define DATA_Z1            0X37  //与DATAZ0组成Z轴输出数据(二进制补码),DATAZ1为高位,4mg/Bit


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
