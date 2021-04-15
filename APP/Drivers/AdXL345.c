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
* Description    : 初始化ADX345，并核对ADX的ID
* Input          : None
* Output         : None
* Return         : 0：成功。0xFF：失败
****************************************************************************/
void ADXL345_Auto_Adjust(void) {
    int8_t xval, yval, zval;
    short tx, ty, tz;
    uint8_t i;
    short offx = 0, offy = 0, offz = 0;
    adxl345_write(ADXL_WRITE, POWER_CTL, 0x00);     //先进入休眠模式
    HAL_Delay(100);
    adxl345_write(ADXL_WRITE, INT_ENABLE, 0x00);    //不使用中断
    adxl345_write(ADXL_WRITE, DATA_FORMAT, 0x2B);   //低电平触发,13位全分辨率,输出数据右对齐,16g量程  0中断至高电平有效（上升） 1中中断至低电平有效（下降）
    adxl345_write(ADXL_WRITE, BW_RATE, 0x09);       //正常功率:低功耗操作，100Hz输出  ，频率 1hz   50hz  1秒  50个数据  200ms读一次
    adxl345_write(ADXL_WRITE, POWER_CTL, 0x28);     //测量模式 ，自动睡眠    08
    for(i = 0; i < 10; i++) {
        ADXL345_Read_Average(&tx, &ty, &tz, 3);
        offx += tx; offy += ty; offz += tz;
    }
    offx /= 10; offy /= 10; offz /= 10;
    xval = -offx / 4;
    yval = -offy / 4;
    zval = -(offz - 256) / 4;
    adxl345_write(ADXL_WRITE, OFSX, xval);      //不使用中断
    adxl345_write(ADXL_WRITE, OFSY, yval);      //不使用中断
    adxl345_write(ADXL_WRITE, OFSZ, zval);      //不使用中断
    HAL_Delay(20);
    adxl345_write(ADXL_WRITE, INT_MAP, 0x5c);       //中断引脚选择，此处我们将activity和Inactivity映射到INT2
    adxl345_write(ADXL_WRITE, INT_ENABLE, 0x5c);    //打开中断 此处我们打开activity和Inactivity中断
    adxl345_read(ADXL_READ, INT_SOURC);             //其实没有关机一直工作
    MYLOG(WARNING, "ADXL345_Auto_Adjust %d %d %d>", xval, yval, zval);
}

uint8_t ADXL345_Init(void) {
    uint8_t id, Data;
    id = adxl345_read(ADXL_READ, DEVICE_ID);            //读取设备ID
    if(id != 0XE5) {
        HAL_Delay(20);
        id = adxl345_read(ADXL_READ, DEVICE_ID);        //读取设备ID
    }
    if(id == 0XE5) {                                    //读器件ID，ADXL345的器件ID为0XE5
        adxl345_write(ADXL_WRITE, POWER_CTL, 0x00);     //先进入休眠模式
        HAL_Delay(100);
        adxl345_write(ADXL_WRITE, INT_ENABLE, 0x00);    //先关中断
        adxl345_write(ADXL_WRITE, DATA_FORMAT, 0x2B);   //低电平触发,13位全分辨率,输出数据右对齐,16g量程  0中断至高电平有效（上升） 1中中断至低电平有效（下降）
        adxl345_write(ADXL_WRITE, BW_RATE, 0x09);       //正常功率:低功耗操作，100Hz输出  ，频率 1hz   50hz  1秒  50个数据  200ms读一次
        adxl345_write(ADXL_WRITE, POWER_CTL, 0x28);     //测量模式 ，自动睡眠    08
        adxl345_write(ADXL_WRITE, FIFO_CTL, 0x8a);      //FIFO ，10个fifo
        adxl345_write(ADXL_WRITE, THRESH_TAP, 0x30);    //设置THRESH_TAP(敲击门限) 62.5mg/LSB   0xff=16g  此时试取2g
        adxl345_write(ADXL_WRITE, DUR, 0x0F);           //设置DUR（敲击持续时间）  625us/LSB  此时试取10ms
        adxl345_write(ADXL_WRITE, TAP_AXES, 0x07);      //设置各轴方向是否参加敲击检测  xyz全参与检测
        adxl345_write(ADXL_WRITE, THRESH_FF, 0x09);     //自由落体阀值  0x05至0x09 300mg-600mg 所有轴参与判断加速度((62.5mg/LSB)) 《暂时没有验证出来》
        adxl345_write(ADXL_WRITE, TIME_FF, 0x14);       //自由落体时间  0x14至0x46 100ms-350ms (5ms/LSB)  直流耦合
        adxl345_write(ADXL_WRITE, THRESH_ACT, 0x1a);    //睡眠激活activity阈值，  当大于这个值的时候唤醒，其中10代表1g    Activity活动阀值              (62.5mg/LSB)*0xxx
        adxl345_write(ADXL_WRITE, THRESH_INACT, 0x06);  //睡眠开始inactivity阈值，当小于这个值的时候睡眠，其中02代表0.2g  THRESH_INACT保存检测静止阀值; (62.5mg/LSB)*0xxx
        adxl345_write(ADXL_WRITE, TIME_INACT, 0x05);   //当小于inactivity值时间超过这个值的时候进入睡眠，其中02代表2秒   TIME_INACT检测静止时间阀值; 2s(1s/LSB)*0xxx
        adxl345_write(ADXL_WRITE, ACT_INACT_CTL, 0xee); //交流耦合，所有轴参与  由于有干扰没法用交流
        adxl345_write(ADXL_WRITE, INT_MAP, 0x5c);       //中断引脚选择，此处我们将activity和Inactivity映射到INT2
        adxl345_write(ADXL_WRITE, INT_ENABLE, 0x5c);    //打开中断 此处我们打开activity和Inactivity中断
        Data = adxl345_read(ADXL_READ, INT_SOURC);      //其实没有关机一直工作
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
* Description    : 读取X,Y,Z的AD值
* Input          : xValue：X轴的保存地址
*                * yValue：Y轴的保存地址
*                * zValue：Z轴的保存地址
* Output         : None
* Return         : 0：读取成功。0xFF：读取失败
****************************************************************************/
/*读取ADXL345三个轴的数据*/
#include "math.h"
void ADXL345_RD_XYZ(short *x, short *y, short *z) {
    static short xf = 0, yf = 0, zf = 256;
    short xt = 0, yt = 0, zt = 0;
    const  short acceleration = 200; //突变加速度 256=1G
    float  scale = 3.9;              //扩大10倍
    uint8_t buf[6];
    buf[0] = adxl345_read(ADXL_READ, DATA_X0);
    buf[1] = adxl345_read(ADXL_READ, DATA_X1);
    buf[2] = adxl345_read(ADXL_READ, DATA_Y0);
    buf[3] = adxl345_read(ADXL_READ, DATA_Y1);
    buf[4] = adxl345_read(ADXL_READ, DATA_Z0);
    buf[5] = adxl345_read(ADXL_READ, DATA_Z1);
    xt = (short)(((uint16_t)buf[1] << 8) + buf[0]); //DATA_X1为高位有效字节
    yt = (short)(((uint16_t)buf[3] << 8) + buf[2]); //DATA_Y1为高位有效字节
    zt = (short)(((uint16_t)buf[5] << 8) + buf[4]); //DATA_Z1为高位有效字节
    //MYLOG(ERROR, "<x y z %04.f><%04.f %04.f %04.f>", sqrt((xt * scale) * (xt * scale) + (yt * scale) * (yt * scale) + (zt * scale) * (zt * scale)), xt * scale, yt * scale, zt * scale);
    //消除白噪声
    if(MYabs(xf, xt) < acceleration)  xf = (xf + 2 * xt) / 3;
    if(MYabs(yf, yt) < acceleration)  yf = (yf + 2 * yt) / 3;
    if(MYabs(zf, zt) < acceleration)  zf = (zf + 2 * zt) / 3;
    //卡尔曼滤波 波形平滑度变好
    //xf = kalmanFilter(&KFP_adxl345_x,xf);
    //yf = kalmanFilter(&KFP_adxl345_y,yf);
    //zf = kalmanFilter(&KFP_adxl345_z,zf);
    *x = xf;//转化为真是的mg
    *y = yf;//转化为真是的mg
    *z = zf;//转化为真是的mg
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
/*读取ADXL345的数据并做滤波处理，读times次再取平均值*/
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

/*ADXL345自动校准函数*/
void ADXL345_AUTO_Adjust(char *xval, char *yval, char *zval) {
    short tx, ty, tz;
    uint8_t i;
    short offx = 0, offy = 0, offz = 0;
    adxl345_write(ADXL_WRITE, POWER_CTL, 0x00); //先进入休眠模式
    HAL_Delay(100);
    adxl345_write(ADXL_WRITE, DATA_FORMAT, 0x08); //低电平中断输出，13位全分辨率，输出数据右对齐，16g量程
    adxl345_write(ADXL_WRITE, BW_RATE, 0x09); //数据输出速度为100Hz
    adxl345_write(ADXL_WRITE, POWER_CTL, 0x28); //链接使能，测量模式
    adxl345_write(ADXL_WRITE, INT_ENABLE, 0x00); //不使用中断
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

/*计算ADXL345角度，x/y/表示各方向上的加速度分量，dir表示要获得的角度*/
short ADXL345_Get_Angle(float x, float y, float z, uint8_t dir) {
    float temp;
    float res = 0;  //弧度值
    switch(dir) {
        case 0:     //0表示与Z轴的角度
            temp = sqrt((x * x + y * y)) / z;
            res = atan(temp);
            break;
        case 1:     //1表示与X轴的角度
            temp = x / sqrt((y * y + z * z));
            res = atan(temp);
            break;
        case 2:     //2表示与Y轴的角度
            temp = y / sqrt((x * x + z * z));
            res = atan(temp);
            break;
    }
    return res * 180 / 3.14; //返回角度值
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
        //Change_Run_State(2);  不执行任何操作
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
*Function:  实现Pedometer的基本算法.
*Input:     void
*Output:    void
*------------------------------------------------------------------------------------------------------------------------*/
typedef struct {
    uint8_t bad_flag;   //无效标志
    int16_t data[3];    //ADXL345 原始数据
    int16_t adresult;   //三次平均值
    int16_t max;        //最大值
    int16_t min;        //最小
    int16_t dc;         //阀值
    int16_t vpp;        //最大加速度
    int16_t precision;  //动态精度
    int16_t old_fixed;  //线序移位器旧值
    int16_t new_fixed;  //线序移位器新值
} ADXL345_Base;

typedef struct {
    uint8_t  flag;               //开始测量标志
    uint32_t time;               //定时处理时间
    uint32_t steps;              //计步步数
    const uint8_t samplingtimes; //采样计数
    uint8_t  sampling;           //采样计数
    ADXL345_Base base[3];        //X,Y,Z基本数据
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
    //------------------------------------------采样滤波----------------------//
    int16_t i;
    for(i = 0; i < 3; i++) {
        Counter.base[i].data[2] = Counter.base[i].data[1];
        Counter.base[i].data[1] = Counter.base[i].data[0];
        Counter.base[i].data[0] = pbuf[i];
        Counter.base[i].adresult = (Counter.base[i].data[0] + Counter.base[i].data[1] + Counter.base[i].data[2]) / 3;
        if(Counter.base[i].adresult > Counter.base[i].max) {Counter.base[i].max = Counter.base[i].adresult;}  //最大
        if(Counter.base[i].adresult < Counter.base[i].min) {Counter.base[i].min = Counter.base[i].adresult;}  //最小
    }
}
void ADXL345_peak_update(void) {
    //----------------------------------计算动态门限和动态精度-----------------------//
    //每步加速度变化约 55 mg
    int16_t i;
    if(Counter.sampling++ >= 50) {
        Counter.sampling = 0;
        for(i = 0; i < 3; i++) {
            Counter.base[i].vpp =  Counter.base[i].max - Counter.base[i].min;         //最大加速度
            Counter.base[i].dc  = (Counter.base[i].max + Counter.base[i].min) / 2;    //dc为阈值
            Counter.base[i].precision = information.accuracy;     //还是修正到这个值，其它干扰太大了
            Counter.base[i].max = -1000;        //每samplingtimes周期更新  16G <4096= 0X0FFF>
            Counter.base[i].min =  1000;        //每samplingtimes周期更新
        }
    }
}
int16_t ADXL345_slid_update(void) {
    //--------------------------线性移位寄存器--------------------------------------
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
    //------------------------- 动态门限判决 ----------------------------------
    static uint32_t time = 0 ;
    int16_t i;
    for(i = 0; i < 3; i++) { //重力加速度不让参加了
        if((Counter.base[i].vpp >= Counter.base[(i + 1) % 3].vpp) && (Counter.base[i].vpp >= Counter.base[(i + 2) % 3].vpp)) { //下降沿
            if((Counter.base[i].old_fixed >= Counter.base[i].dc) && (Counter.base[i].new_fixed < Counter.base[i].dc) && ((Counter.base[i].max - Counter.base[i].min) > information.threshold)) {
                //干扰加速度
                //if(Counter.base[i].vpp < information.threshold) break; //加速度小于0.2g直接滤掉
                //MYLOG(WARNING, "Counter.base[%d].dc =%d %d %d %d", i, Counter.base[i].dc, Counter.base[i].vpp, Counter.base[i].new_fixed, Counter.base[i].old_fixed);
                //时间窗口0.2到2s内有效
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
    //------------------------------------------采样滤波----------------------//
    for(i = 0; i < 3; i++) {
        Counter.base[i].data[2] = Counter.base[i].data[1];
        Counter.base[i].data[1] = Counter.base[i].data[0];
        Counter.base[i].data[0] = pbuf[i];
        Counter.base[i].adresult = (Counter.base[i].data[0] + Counter.base[i].data[1] + Counter.base[i].data[2]) / 3;
        if(Counter.base[i].adresult > Counter.base[i].max) {Counter.base[i].max = Counter.base[i].adresult;}  //最大
        if(Counter.base[i].adresult < Counter.base[i].min) {Counter.base[i].min = Counter.base[i].adresult;}  //最小
    }
    //----------------------------------计算动态门限和动态精度-----------------------//
    //每步加速度变化约 55 mg
    //if(Counter.sampling++ >= Counter.samplingtimes) {
    if(Counter.sampling++ >= 50) {
        Counter.sampling = 0;
        for(i = 0; i < 3; i++) {
            Counter.base[i].vpp =  Counter.base[i].max - Counter.base[i].min;         //最大加速度
            Counter.base[i].dc  = (Counter.base[i].max + Counter.base[i].min) / 2;    //dc为阈值
            //Counter.base[i].dc  = Counter.base[i].min + (Counter.base[i].vpp >> 1); //dc为阈值
            Counter.base[i].bad_flag = 0;
            if(Counter.base[i].vpp >= 160 * 4) {
                Counter.base[i].precision = Counter.base[i].vpp / 32; //8
            } else if((Counter.base[i].vpp >= 50 * 4) && (Counter.base[i].vpp < 160 * 4)) {
                Counter.base[i].precision = 4;
            } else if((Counter.base[i].vpp >= 15 * 4) && (Counter.base[i].vpp < 50 * 4)) {
                Counter.base[i].precision = 3;
            } else {
                Counter.base[i].precision = 2;
                Counter.base[i].bad_flag = 1;//无效
            }
            //MYLOG(WARNING, "bad_flag[%d] =%d max =%03d min =%03d <<vpp = %03d dc = %03d precision = %d>>",i,Counter.base[i].bad_flag, Counter.base[i].max,Counter.base[i].min,Counter.base[i].vpp,Counter.base[i].dc,Counter.base[i].precision);
            Counter.base[i].precision = 30;  //还是修正到这个值，其它干扰太大了
            Counter.base[i].max = -1000;        //每samplingtimes周期更新  16G <4096= 0X0FFF>
            Counter.base[i].min =  1000;     //每samplingtimes周期更新
        }
        Counter.flag = 1;
    }
    if(Counter.flag == 0) return; //测量标志
    //--------------------------线性移位寄存器--------------------------------------
    for(i = 0; i < 3; i++) {
        Counter.base[i].old_fixed = Counter.base[i].new_fixed;
//        if(Counter.base[i].adresult >= Counter.base[i].new_fixed) {      //上升  波峰
//            if((Counter.base[i].adresult - Counter.base[i].new_fixed) >= Counter.base[i].precision)   {Counter.base[i].new_fixed = Counter.base[i].adresult;}
//            //MYLOG(WARNING, "Counter.base[%d].new_fixed =%d %d Up",i,Counter.base[i].new_fixed,Counter.base[i].adresult);
//        } else if(Counter.base[i].adresult < Counter.base[i].new_fixed) {//下升
//            if((Counter.base[i].new_fixed - Counter.base[i].adresult) >= ((Counter.base[i].vpp) / 2) + Counter.base[i].precision)   {Counter.base[i].new_fixed = Counter.base[i].adresult;}
//            //MYLOG(WARNING, "Counter.base[%d].new_fixed =%d %d Down",i,Counter.base[i].new_fixed,Counter.base[i].adresult);
//        }
        if(Counter.base[i].adresult >= Counter.base[i].new_fixed) {
            if((Counter.base[i].adresult - Counter.base[i].new_fixed) >= Counter.base[i].precision)   {Counter.base[i].new_fixed = Counter.base[i].adresult;}
        } else if(Counter.base[i].adresult < Counter.base[i].new_fixed) {
            if((Counter.base[i].new_fixed - Counter.base[i].adresult) >= Counter.base[i].precision)   {Counter.base[i].new_fixed = Counter.base[i].adresult;}
        }
    }
    //------------------------- 动态门限判决 ----------------------------------
    for(i = 0; i < 3; i++) { //重力加速度不让参加了
        if((Counter.base[i].vpp >= Counter.base[(i + 1) % 3].vpp) && (Counter.base[i].vpp >= Counter.base[(i + 2) % 3].vpp)) { //下降沿
            if((Counter.base[i].old_fixed >= Counter.base[i].dc) && (Counter.base[i].new_fixed < Counter.base[i].dc) && (Counter.base[i].bad_flag == 0)) {
                //干扰加速度
                if(Counter.base[i].vpp < 100) break; //加速度小于0.4g直接滤掉
                //MYLOG(WARNING, "Counter.base[%d].dc =%d %d %d %d", i, Counter.base[i].dc, Counter.base[i].vpp, Counter.base[i].new_fixed, Counter.base[i].old_fixed);
                //时间窗口0.2到2s内有效
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
    //if(DelayTimeOut(&Counter.time, 1000*10 / Counter.samplingtimes)) { //0.02s处理一次
    if(DelayTimeOut(&Counter.time, 200)) { //0.02s处理一次  50hz   0.2s   大约只有10数据
        Counter.time = HAL_GetTick();
        for(i = 0; i < 10; i++) {
            ADXL345_RD_XYZ(&buf[0], &buf[1], &buf[2]); //可以用fifo实现
            //转化为真实的mg
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
                MYLOG(WARNING, "step_counter_run = %d", Counter.steps); //输出步数
                steptime = HAL_GetTick() ;
            }
        } else {
            if(Counter.steps >= 3) { //大约3步才算走步，其它都是干扰
                MYLOG(WARNING, "step_counter_start = %d", Counter.steps); //输出步数
                Change_Run_State(1);
                stepflag = 1;
                globalvariable.steps += 3;
                Counter.steps = globalvariable.steps;
                steptime = HAL_GetTick() ;
            }
        }
        if(DelayTimeOut(&steptime, 1000 * 5)) { //超过3s没有更新步伐就当停止运动了。需要重新计数
            stepflag = 0;
            ADXL345_clean_falg();
            steptime = HAL_GetTick() ;
        }
        ADXL345_Get_State(0);
    }
}


#define P_P_DIFF    256*256*0.2   /* 波峰-波谷的差值,即3D阈值 */
#define RISING_EDGE  1 /*上升沿状态 */
#define FALLING_EDGE 0 /*下降沿状态*/
#define FAST_WALK_TIME_LIMIT_MS 200     /* ms */
#define SLOW_WALK_TIME_LIMIT_MS 10000 /* 10s 内没有走一步 */
#define STEP_OK 3   /* 7步法则 */
#define FALSE 0
#define TRUE 1

uint32_t lastPos = 0;   /* 旧数据*/
uint32_t newMax = 0, newMin = 0; /* 波峰-波谷 */

uint8_t walkSta = FALSE; /* 获得一次峰值状态 */
uint8_t walkOkSta = FALSE; /* 连续10s内走了7步 有效行走状态 */
uint8_t pSta = RISING_EDGE; /* 3D数据波形状态 */

uint32_t lastTime = 0;      /* 上一次 walkSta 的时间 */
uint32_t stepOK = 0; /*初始计步门限 滤除干扰 */
uint32_t stepCount = 0; /* 计步值 */
uint32_t Step_Count(int16_t *pbuf) {
    uint32_t nowPos = 0;
    uint32_t ppDiff = 0;
    uint32_t timeDiff = 0;
    /*获取3D IMU */
    //nowPos = (unsigned int)powf(sqrtf(axis0) + sqrtf(axis1) + sqrtf(axis2), 0.5);
    nowPos = pbuf[0] * pbuf[0] + pbuf[1] * pbuf[1] + pbuf[2] * pbuf[2];
    /* 得到波峰和波谷 */
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
    /* 更新3D step状态数据 */
    lastPos = nowPos;
    /* 有波峰或波谷 */
    if(walkSta == TRUE) {
        walkSta = FALSE;
        ppDiff = newMax - newMin; /* 波峰与波谷的差值 */
        //MYLOG(WARNING, "ppDiff = %x %x %x", ppDiff,newMax,newMin); //输出步数
        if(ppDiff > P_P_DIFF) {
            timeDiff = HAL_GetTick() - lastTime; /* 获取波峰和波谷的时间差 */
            if(timeDiff < FAST_WALK_TIME_LIMIT_MS) { /* 波峰波谷时间差小于200ms的直接去掉 */
                return stepCount;
            } else if(timeDiff > SLOW_WALK_TIME_LIMIT_MS) { /* 波峰波谷时间差大于10s的视为静止   */
                walkOkSta = FALSE;
                stepOK = 0;
                return stepCount;
            }
            stepOK++;
            if(stepOK >= STEP_OK) { /* 走7步之后更新状态 */
                walkOkSta = TRUE;
            }
            lastTime = HAL_GetTick(); /* 更新时间 */
        }
    }
    if(walkOkSta == TRUE) { /* 满足10s内走7步*/
        stepCount += stepOK;
        stepOK = 0;
    }
    return stepCount;
}


//uint8_t  ADXL345_FLAG = 0;        //20MS的中断标志
//uint32_t ADXL345_FLAG_time = 0;   //开机5s才开始计数


//uint8_t  ADXL345_bad_flag[3];                //无效标志
//uint32_t ADXL345_array0[3] = {1, 1, 1};      //三次X滤波数组
//uint32_t ADXL345_array1[3] = {1, 1, 1};      //三次X滤波数组
//uint32_t ADXL345_array2[3] = {0, 0, 0};      //三次X滤波数组

//uint32_t ADXL345_adresult[3];                //三次平均值
//uint32_t ADXL345_max[3] = {0, 0, 0};         //最大值
//uint32_t ADXL345_min[3] = {1000, 1000, 1000};//最小
//uint32_t ADXL345_dc[3] =  {500, 500, 500};    //阀值
//uint32_t ADXL345_vpp[3] = {30, 30, 30};      //最大加速度
//uint32_t ADXL345_precision[3] = {5, 5, 5};   //动态精度
//uint32_t ADXL345_old_fixed[3];               //线序移位器旧值
//uint32_t ADXL345_new_fixed[3];               //线序移位器新值
//uint32_t ADXL345_STEPS = 0;                  //步数

//void ADXL345_StepXX(void) {
//    static uint8_t sampling_counter = 0; //50次*0.2s = 10s
//    uint8_t buf[6], jtemp;
//    ADXL345_FLAG = 0;
//    ADXL345_RD_XYZ_Buf(buf);          //连续读出数据，存储在BUF中
//    //------------------------------------------采样滤波----------------------//
//    for(jtemp = 0; jtemp <= 2; jtemp++) { //jtemp 0,1,2分别代表x，y，z
//        ADXL345_array2[jtemp] = ADXL345_array1[jtemp];
//        ADXL345_array1[jtemp] = ADXL345_array0[jtemp];
//        ADXL345_array0[jtemp] = buf[2 * jtemp] + (buf[2 * jtemp + 1] << 8);
//        ADXL345_adresult[jtemp] = ADXL345_array0[jtemp] + ADXL345_array1[jtemp] + ADXL345_array2[jtemp];
//        ADXL345_adresult[jtemp] = ADXL345_adresult[jtemp] / 3;
//        if(ADXL345_adresult[jtemp] > ADXL345_max[jtemp]) {ADXL345_max[jtemp] = ADXL345_adresult[jtemp];}
//        if(ADXL345_adresult[jtemp] < ADXL345_min[jtemp]) {ADXL345_min[jtemp] = ADXL345_adresult[jtemp];}
//    }
//    sampling_counter = sampling_counter + 1;
//    //----------------------------------计算动态门限和动态精度-----------------------//
//    if(sampling_counter >= 20) {
//        sampling_counter = 0;
//        for(jtemp = 0; jtemp <= 2; jtemp++) {
//            ADXL345_vpp[jtemp] = ADXL345_max[jtemp] - ADXL345_min[jtemp];
//            ADXL345_dc[jtemp] = ADXL345_min[jtemp] + (ADXL345_vpp[jtemp] >> 1); //dc为阈值
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
//            ADXL345_precision[jtemp] = 50;   //修正一下，否则太灵敏
//        }
//    }
//    //--------------------------线性移位寄存器--------------------------------------
//    for(jtemp = 0; jtemp <= 2; jtemp++) {
//        ADXL345_old_fixed[jtemp] = ADXL345_new_fixed[jtemp];
//        if(ADXL345_adresult[jtemp] >= ADXL345_new_fixed[jtemp]) {
//            if((ADXL345_adresult[jtemp] - ADXL345_new_fixed[jtemp]) >= ADXL345_precision[jtemp])   {ADXL345_new_fixed[jtemp] = ADXL345_adresult[jtemp];}
//        } else if(ADXL345_adresult[jtemp] < ADXL345_new_fixed[jtemp]) {
//            if((ADXL345_new_fixed[jtemp] - ADXL345_adresult[jtemp]) >= ADXL345_precision[jtemp])   {ADXL345_new_fixed[jtemp] = ADXL345_adresult[jtemp];}
//        }
//    }
//    //------------------------- 动态门限判决 ----------------------------------
//    if((ADXL345_vpp[0] >= ADXL345_vpp[1]) && (ADXL345_vpp[0] >= ADXL345_vpp[2])) { //x轴最活跃
//        if((ADXL345_old_fixed[0] >= ADXL345_dc[0]) && (ADXL345_new_fixed[0] < ADXL345_dc[0]) && (ADXL345_bad_flag[0] == 0)) {
//            ADXL345_STEPS = ADXL345_STEPS + 1;
//        }
//    } else if((ADXL345_vpp[1] >= ADXL345_vpp[0]) && (ADXL345_vpp[1] >= ADXL345_vpp[2])) { //y轴最活跃
//        if((ADXL345_old_fixed[1] >= ADXL345_dc[1]) && (ADXL345_new_fixed[1] < ADXL345_dc[1]) && (ADXL345_bad_flag[1] == 0)) {
//            ADXL345_STEPS = ADXL345_STEPS + 1;
//        }
//    } else if((ADXL345_vpp[2] >= ADXL345_vpp[1]) && (ADXL345_vpp[2] >= ADXL345_vpp[0])) { //z轴最活跃
//        if((ADXL345_old_fixed[2] >= ADXL345_dc[2]) && (ADXL345_new_fixed[2] < ADXL345_dc[2]) && (ADXL345_bad_flag[2] == 0)) {
//            ADXL345_STEPS = ADXL345_STEPS + 1;
//        }
//    }
//}

//void ADXL345_Step_RunXX(void) {
//    static uint32_t pSTEPS = 0;                  //步数
//    HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_SET);
//    if(DelayTimeOut(&ADXL345_FLAG_time, 50)) {   //0.02s处理一次
//        ADXL345_FLAG_time = HAL_GetTick();
//        ADXL345_StepXX();
//        //MYLOG(WARNING, "step_counter_run = %d", STEPS); //输出步数
//        if(pSTEPS != ADXL345_STEPS) {
//            pSTEPS = ADXL345_STEPS;
//            MYLOG(WARNING, "step_counter_run = %d", ADXL345_STEPS); //输出步数
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
