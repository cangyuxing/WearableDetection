#ifndef __DATA_H__
#define __DATA_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32l1xx_hal.h"
#include "stdint.h"
//**************************************************************************************
//JY_901 基本数据结构
#pragma pack(push,1) //保存对齐状态  1字节 __packed
typedef struct {
    uint8_t *cmd;       //命令
    uint8_t rdwr;       //读写标志 0可读 1可写不保存 2可写并保存
    void *parameter; //参数
    uint8_t len;        //长度
}AT_DEBUG_STR;          //字符串处理
//typedef struct {
//    uint8_t *cmd;       //命令
//    uint8_t rdwr;       //读写标志 0可读 1可写
//    void *parameter;    //参数
//    uint8_t len;        //长度
//}AT_DEBUG_VAR;//变量处理

typedef struct {
    //------------------------------
    //有符号
    //平均角度
    int16_t Rx_mid;
    int16_t Ry_mid;
    int16_t Rz_mid;
    //加速度
    int16_t Ax_max;
    int16_t Ay_max;
    int16_t Az_max;
    //加速度
    int16_t Ax_min;
    int16_t Ay_min;
    int16_t Az_min;
    //------------------------------
    //无符号
    //磁场
    uint16_t JD_max;
    uint16_t JD_min;
    //计步器
    uint16_t StepbyStep;
    //电池电压
    uint8_t vbat;
} Upload; //25字节固定

typedef struct {
    __IO uint32_t A_max;  //最大加速度 = x^2 + y^2 + z^2
    __IO uint32_t A_min;  //最小加速度 = x^2 + y^2 + z^2
    __IO uint16_t J_max;  //肌肉最大值
    __IO uint16_t J_min;  //肌肉最小值
    __IO int32_t  Flag;   //标志位
    __IO int32_t  R_count;//角度计数
    __IO int32_t  Rx_add; //X角度和
    __IO int32_t  Ry_add; //Y角度和
    __IO int32_t  Rz_add; //Z角度和
    __IO Upload   upload; //原始数据
    __IO uint16_t StepbyStep;//计步数据
    __IO uint8_t  vbat;       //电池数据
} Upload_calculation;         //26字节固定

typedef struct {
    //欧拉角
    int16_t Rx;
    int16_t Ry;
    int16_t Rz;
    //角速度
    int16_t Wx;
    int16_t Wy;
    int16_t Wz;
    //加速度
    int16_t Ax;
    int16_t Ay;
    int16_t Az;
    //磁场
    int16_t Hx;
    int16_t Hy;
    int16_t Hz;
    //计步器
    uint16_t StepbyStep;
} JY_901_Upload; //9轴 最大11字节

typedef struct {
    uint8_t  head;
    uint8_t  cmd;
    int16_t  data[4];
    uint8_t  sum;
} JY_901_DATA; //9轴 最大11字节

typedef struct {
    uint16_t DataSta;         //接收标致
    union {
        uint8_t     Data[11]; //缓存数据
        JY_901_DATA JY_901;
    } Data;
} BSP_UART_JY_901;
//#pragma pack(pop) //恢复对齐状态

//**************************************************************************************
//HKJ_15C 基本数据结构
//#pragma pack(push,1) //保存对齐状态  1字节 __packed
typedef struct {
    //肌肉
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
} HKJ_15C_Upload; //9轴 最大11字节

typedef struct {
    uint8_t  head;//0xff 固定为数据帧起始位
    uint8_t  dev; //肌电0xc6 肌电：HKJ-15C
    uint8_t  len; //包含长度、校验、命令、参数的字节数
    uint8_t  sum; //长度、命令、参数求和后取低字节
    //0xA0  启动
    //0xA1  停止
    //0xA2  读设备号
    //0xA3  读生产日期
    //0xA4  调整幅度
    //0xAA  点名
    //0xA8  关机
    uint8_t  cmd; //命令及返回
    uint16_t data[25];
} HKJ_15C_DATA; //肌电 最大55字节

typedef struct {
    uint16_t DataSta;         //接收标致
    union {
        uint8_t     Data[55]; //缓存数据
        HKJ_15C_DATA HKJ_15C;
    } Data;
} BSP_UART_HKJ_15C;
//#pragma pack(pop) //恢复对齐状态

//**************************************************************************************
//串口 基本数据结构
//#pragma pack(push,1) //保存对齐状态  1字节 __packed
typedef struct {
#define  BSP_UART_NewlineString_MAX   64
    uint16_t DataSta;                          //接收标致
    uint8_t  Data[BSP_UART_NewlineString_MAX]; //缓存数据
} BSP_UART_NewlineString;
//**************************************************************************************
//24L01 上传数据协议
typedef struct {
    uint8_t  head;//包头0xaa
    uint8_t  dev; //上肢0x0a 下肢0x0b
    uint8_t  len; //长度
    uint8_t  cmd; //命令
    union {
        uint16_t       Data[13];
        JY_901_Upload  JY_901;  //加速度数据
        HKJ_15C_Upload HKJ_15C; //肌肉数据
    } Data;
    uint8_t  sum;
    uint8_t  tail;//尾部0x55
} Upload_DATA;    //9轴 最大32字节

typedef struct {
    uint8_t  head;         //包头0xaa
    uint8_t  serialnumber; //序列号
    uint16_t id;           //通讯号
    uint8_t  cmd;          //命令
    union {
        uint8_t       Data[25];
        Upload        upload;  //最新上传协议
    } Data;
    uint8_t  sum;
    uint8_t  tail;     //尾部0x55
} Upload_DATA_V1_0;    //9轴 最大32字节

enum equipments {
    HOPEN_DEF = 0,  //默认：生产模式需要相应参数设置转化为正常的设备
    HOPEN_UL,       //上肢左 1
    HOPEN_UR,       //上肢右 2
    HOPEN_DL,       //下肢左 3
    HOPEN_DR,       //下肢右 4
    HOPEN_DX,       //预留   5
    HOPEN_ECG       // 心电   6
};

enum Equipmentflag {
    axp173_flag = 0x01,  //01  1有效 0无效
    adx345_flag = 0x02,  //02  1有效 0无效
    nrf24l_flag = 0x04,  //04  1有效 0无效
    jiy901_flag = 0x08,  //08  1有效 0无效
    hkj15c_flag = 0x10,  //10  1有效 0无效
};

//设备信息
typedef struct {
    //只读参数
    uint8_t EquipmentName[15];            //R  设备名称：HOPEN_Wearable
    uint8_t SoftwareVersion[5];           //R  软件版本: V1.0
    uint8_t SoftwareReleaseDate[9];       //R  软件发布日期 20201215
    uint8_t EquipmentID[11];              //RW 设备ID：HOPEN_1234
    uint8_t Password[7];                  //RW 调试密码：admin
    //读写参数 字符串
    uint8_t  EquipmentType;               //RW 设备类型 0：生产模式《需要设置相关参数》1:上肢L   2:上肢R   3:下肢L  4：下肢R  5：预留  6:心电模块   其它无效
    uint8_t  Channel;                     //R  发送频率255
    uint16_t UpId;                        //RW 上传通讯ID
    uint8_t  Debugbak;                    //RW  缓存debug等级
    
    uint16_t threshold;                   //RW  动态阀值
    uint8_t  accuracy;                    //RW  动态精度
    
    uint8_t  algorithm;                   //RW  算法1 2 3 4
    uint8_t  crc;                         //crc
}DeviceInformation;
typedef struct {
    uint8_t  Debug;                       //RW 等级
    uint8_t  CfgFlag;                     //T 调试开关
    uint8_t  RestFlag;                    //T 复位
    uint8_t  SleepFlag;                   //O 复位标
    uint8_t  EquipmentFlag;               //R 模板标志位
    
    uint8_t  RunFlag;                     //R 运行状态
    uint32_t RunTime;                     //R 状态更新时间
    uint32_t steps;                       //R 步数
    uint32_t HKJ_15C;                     //初始化标志
    uint32_t ADXL345_adj;                 //ADXL345校准
}GlobalVariable;
    
#pragma pack(pop) //恢复对齐状态

#ifdef __cplusplus
}
#endif
#endif
