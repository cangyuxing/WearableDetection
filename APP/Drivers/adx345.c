#include "adx345.h"

I2C_HandleTypeDef *myadxl345 = &hi2c1;

static int8_t Single_Write_ADXL345(uint8_t addr, uint8_t dat) {
    HAL_I2C_Mem_Write(myadxl345, ADX345_ADDR + 1, addr, I2C_MEMADD_SIZE_8BIT, &dat, 1, 0xFF);
    return 0;
}
static uint8_t Single_Read_ADXL345(uint8_t addr) {
    uint8_t readValue = 0xFF;
    HAL_I2C_Mem_Read(myadxl345, ADX345_ADDR, addr, I2C_MEMADD_SIZE_8BIT, &readValue, 1, 0xff);
    return readValue;
}
//*********************************************************
//连续读出ADXL345内部加速度数据，地址范围0x32~0x37
//*********************************************************
static int8_t Multiple_read_ADXL345(uint8_t *buf) {
    HAL_I2C_Mem_Read(myadxl345, ADX345_ADDR, 0X32, I2C_MEMADD_SIZE_8BIT, &buf[0], 1, 0xFF);
    HAL_I2C_Mem_Read(myadxl345, ADX345_ADDR, 0X33, I2C_MEMADD_SIZE_8BIT, &buf[1], 1, 0xFF);
    HAL_I2C_Mem_Read(myadxl345, ADX345_ADDR, 0X34, I2C_MEMADD_SIZE_8BIT, &buf[2], 1, 0xFF);
    HAL_I2C_Mem_Read(myadxl345, ADX345_ADDR, 0X35, I2C_MEMADD_SIZE_8BIT, &buf[3], 1, 0xFF);
    HAL_I2C_Mem_Read(myadxl345, ADX345_ADDR, 0X36, I2C_MEMADD_SIZE_8BIT, &buf[4], 1, 0xFF);
    HAL_I2C_Mem_Read(myadxl345, ADX345_ADDR, 0X37, I2C_MEMADD_SIZE_8BIT, &buf[5], 1, 0xFF);
    return 0;
}

void Get_ADXL345_State(uint8_t value){
    uint8_t readValue = Single_Read_ADXL345(INT_SOURCE); 
    if(readValue&0x40) {
        MYLOG(WARNING, "ADXL345 SINGLE_TAP = %d %x", value,readValue);
    }
    if(readValue&0x10) {
        MYLOG(WARNING, "ADXL345 Activity = %d %x", value,readValue);
    }
    if(readValue&0x08) {
        MYLOG(WARNING, "ADXL345 Inactivity = %d %x", value,readValue);
    }
    if(readValue&0x04) {
        MYLOG(WARNING, "ADXL345 FREE_FALL = %d %x", value,readValue);
    }
}

//static int8_t Multiple_read_ADXL345(int16_t *xValue, int16_t *yValue, int16_t *zValue) {
//    uint8_t buf[6];
//    HAL_I2C_Mem_Read(myadxl345, ADX345_ADDR, 0X32, I2C_MEMADD_SIZE_8BIT, &buf[0], 1, 0xFF);
//    HAL_I2C_Mem_Read(myadxl345, ADX345_ADDR, 0X33, I2C_MEMADD_SIZE_8BIT, &buf[1], 1, 0xFF);
//    HAL_I2C_Mem_Read(myadxl345, ADX345_ADDR, 0X34, I2C_MEMADD_SIZE_8BIT, &buf[2], 1, 0xFF);
//    HAL_I2C_Mem_Read(myadxl345, ADX345_ADDR, 0X35, I2C_MEMADD_SIZE_8BIT, &buf[3], 1, 0xFF);
//    HAL_I2C_Mem_Read(myadxl345, ADX345_ADDR, 0X36, I2C_MEMADD_SIZE_8BIT, &buf[4], 1, 0xFF);
//    HAL_I2C_Mem_Read(myadxl345, ADX345_ADDR, 0X37, I2C_MEMADD_SIZE_8BIT, &buf[5], 1, 0xFF);
//    *xValue = (short)(((uint16_t)buf[1] << 8) + buf[0]); //DATA_X1为高位有效字节
//    *yValue = (short)(((uint16_t)buf[3] << 8) + buf[2]); //DATA_Y1为高位有效字节
//    *zValue = (short)(((uint16_t)buf[5] << 8) + buf[4]); //DATA_Z1为高位有效字节
//    return 0;
//}
//*****************************************************************
//初始化ADXL345，根据需要请参考pdf进行修改************************
//void Init_ADXL345(void) {
//    uint8_t readValue = Single_Read_ADXL345(ADX_DEVID);
//    if(readValue == 0xE5) {
//        Single_Write_ADXL345(ADX_DATA_FORMAT, 0x2B); //13位全分辨率,输出数据右对齐,16g量程
//        Single_Write_ADXL345(ADX_BW_RATE, 0x0A);     //数据输出速度为100Hz
//        Single_Write_ADXL345(ADX_POWER_CTL, 0x28);   //链接使能,测量模式
//        Single_Write_ADXL345(ADX_INT_ENABLE, 0x00);  //不使用中断
//        Single_Write_ADXL345(ADX_OFSX, 0x00);        //敲击阀值
//        Single_Write_ADXL345(ADX_OFSY, 0x00);        //X轴偏移
//        Single_Write_ADXL345(ADX_OFSZ, 0x00);        //Y轴偏移
//        MYLOG(WARNING,"ADXL345_Init OK!");
//        return ;
//    }
//   MYLOG(ERROR,"ADXL345_Init Fail! =%x",readValue);
//}

//void Init_ADXL345(void) {
//    uint8_t readValue = Single_Read_ADXL345(ADX_DEVID);
//    if(readValue == 0xE5) {
//        Single_Write_ADXL345(ADX_INT_ENABLE, 0x00);   //关中断
//        Single_Write_ADXL345(ADX_DATA_FORMAT, 0x0B);  //测量范围,正负16g，10位模式
//        Single_Write_ADXL345(ADX_POWER_CTL, 0x28);    //选择电源模式   参考pdf24页 38自动睡眠
//        Single_Write_ADXL345(ADX_BW_RATE, 0x09);      //速率设定为50HZ 参考pdf13页
//        
//        Single_Write_ADXL345(FIFO_CTL, 0x9f);         //FIFO模式设置 32条fifo满产生中断
//        
//        Single_Write_ADXL345(ADX_OFSX, 0x00);        //X偏移量 根据测试传感器的状态写入pdf29页
//        Single_Write_ADXL345(ADX_OFSY, 0x00);        //Y偏移量 根据测试传感器的状态写入pdf29页
//        Single_Write_ADXL345(ADX_OFSZ, 0x00);        //Z偏移量 根据测试传感器的状态写入pdf29页
//        
//        Single_Write_ADXL345(THRESH_TAP, 5);          //睡眠激活activity阈值，当大于这个值的时候唤醒，其中0x10代表1g
//        
//        
//        Single_Write_ADXL345(THRESH_ACT, 2);          //睡眠激活activity阈值，当大于这个值的时候唤醒，其中0x10代表1g
//        Single_Write_ADXL345(THRESH_INACT, 1);        //睡眠开始inactivity阈值，当小于这个值的时候睡眠，其中02代表0.2g
//        Single_Write_ADXL345(TIME_INACT, 2);          //当小于inactivity值时间超过这个值的时候进入睡眠，其中02代表2秒
//        Single_Write_ADXL345(ACT_INACT_CTL, 0xcc);    //直流交流触发配置，XYZ使能触发配置，此处选用X交流触发
//        
//        Single_Write_ADXL345(INT_MAP, 0x03);          //中断映射 0 INT1 1 INT2
//        Single_Write_ADXL345(ADX_INT_ENABLE, 0x7F);   //使能 DATA_READY 中断
//        //Single_Write_ADXL345(ADX_INT_ENABLE, 0x83);   //使能 DATA_READY 中断
//        MYLOG(WARNING, "ADXL345_Init OK!");
//        return ;
//    }
//    MYLOG(ERROR,"ADXL345_Init Fail! =%x",readValue);
//}

void Init_ADXL345(void) {
    uint8_t readValue = Single_Read_ADXL345(ADX_DEVID);
    if(readValue == 0xE5) {
        Single_Write_ADXL345(0x2E,0x00); //该步骤最为重要，因为之前设置过中断的话，会造成中断错误。应先清除后，再使用 
        Single_Write_ADXL345(0x31,0x0B); //中断至低电平有效 测量范围正负16g,13位模式
        Single_Write_ADXL345(0x2C,0x0A); //速率设定为12.5参考PDF13页 
        Single_Write_ADXL345(0x2D,0x28); //选择电源模式 参考PDF24页
        Single_Write_ADXL345(0x2F,0x00); //中断输出脚为INT1 FREE_FALL中断输出为INT1       
        Single_Write_ADXL345(0x24,0x05); //Activity活动阀值  (62.5mg/LSB)*0xxx
        Single_Write_ADXL345(0x25,0x05); //THRESH_INACT保存检测静止阀值; (62.5mg/LSB)*0xxx
        Single_Write_ADXL345(0x26,0x02); //TIME_INACT检测静止时间阀值; 2s(1s/LSB)*0xxx
        Single_Write_ADXL345(0x27,0xcc); //直流耦合，所有轴参与
        Single_Write_ADXL345(0x28,0x06); //自由落体阀值 0x05至0x09 300mg-600mg 所有轴参与判断加速度((62.5mg/LSB))
        Single_Write_ADXL345(0x29,0x30); //自由落体时间 0x14至0x46 100ms-350ms (5ms/LSB)
        Single_Write_ADXL345(0x2E,0x14); //中断使能寄存器  使能Activity中断 使能Free_Fall中断
//        Single_Write_ADXL345(0x31,0x0b);   //测量范围,正负16g，13位模式 高电平有效
//        Single_Write_ADXL345(0x2C,0x09);   //速率设定为12.5 参考pdf13页
//        Single_Write_ADXL345(0x2D,0x28);   //选择电源模式   参考pdf24页
//        
//        Single_Write_ADXL345(0x1E,0x00);   //X 偏移量 根据测试传感器的状态写入pdf29页
//        Single_Write_ADXL345(0x1F,0x00);   //Y 偏移量 根据测试传感器的状态写入pdf29页
//        Single_Write_ADXL345(0x20,0x00);   //Z 偏移量 根据测试传感器的状态写入pdf29页


//        Single_Write_ADXL345(0x1D,0x30);        //设置THRESH_TAP(敲击门限) 62.5mg/LSB   0xff=16g  此时试取3g   
//        Single_Write_ADXL345(0x21,0x11);        //设置DUR（敲击持续时间） 625us/LSB  此时试取10ms
//        
//        Single_Write_ADXL345(0x2A,0x07);        //设置各轴方向是否参加敲击检测  xyz全参与检测
//        
//        Single_Write_ADXL345(0x2E,0x00);        //设置中断前应先禁用中断
//        
//        Single_Write_ADXL345(0x2E,0x40);        //使能 SINGLE_TAP 中断 
//        Single_Write_ADXL345(0x2F,0x40);        //设置中断引脚映射 将单击时间映射到INT2上
        
        
        
//        Single_Write_ADXL345(ADX_INT_ENABLE, 0x00);   //关中断
//        Single_Write_ADXL345(ADX_DATA_FORMAT, 0x0B);  //测量范围,正负16g，10位模式
//        Single_Write_ADXL345(ADX_POWER_CTL, 0x28);    //选择电源模式   参考pdf24页 38自动睡眠
//        Single_Write_ADXL345(ADX_BW_RATE, 0x09);      //速率设定为50HZ 参考pdf13页
//        
//        Single_Write_ADXL345(FIFO_CTL, 0x9f);         //FIFO模式设置 32条fifo满产生中断
//        
//        Single_Write_ADXL345(ADX_OFSX, 0x00);        //X偏移量 根据测试传感器的状态写入pdf29页
//        Single_Write_ADXL345(ADX_OFSY, 0x00);        //Y偏移量 根据测试传感器的状态写入pdf29页
//        Single_Write_ADXL345(ADX_OFSZ, 0x00);        //Z偏移量 根据测试传感器的状态写入pdf29页
//        
//        Single_Write_ADXL345(THRESH_TAP, 5);          //睡眠激活activity阈值，当大于这个值的时候唤醒，其中0x10代表1g
//        
//        
//        Single_Write_ADXL345(THRESH_ACT, 2);          //睡眠激活activity阈值，当大于这个值的时候唤醒，其中0x10代表1g
//        Single_Write_ADXL345(THRESH_INACT, 1);        //睡眠开始inactivity阈值，当小于这个值的时候睡眠，其中02代表0.2g
//        Single_Write_ADXL345(TIME_INACT, 2);          //当小于inactivity值时间超过这个值的时候进入睡眠，其中02代表2秒
//        Single_Write_ADXL345(ACT_INACT_CTL, 0xcc);    //直流交流触发配置，XYZ使能触发配置，此处选用X交流触发
//        
//        Single_Write_ADXL345(INT_MAP, 0x03);          //中断映射 0 INT1 1 INT2
//        Single_Write_ADXL345(ADX_INT_ENABLE, 0x7F);   //使能 DATA_READY 中断
        //Single_Write_ADXL345(ADX_INT_ENABLE, 0x83);   //使能 DATA_READY 中断
        MYLOG(WARNING, "ADXL345_Init OK!");
        return ;
    }
    MYLOG(ERROR,"ADXL345_Init Fail! =%x",readValue);
}

/*------------------------------------------------------------------------------------------------------------------------
*Name:      step_counter()
*Function:  实现Pedometer的基本算法.
*Input:     void
*Output:    void
*------------------------------------------------------------------------------------------------------------------------*/
uint8_t  adxl345_flag = 0;        //20MS的中断标志
uint32_t adxl345_flag_time = 0;   //开机5s才开始计数

uint8_t  bad_flag[3];                //无效标志
uint32_t array0[3] = {1, 1, 1};      //三次X滤波数组
uint32_t array1[3] = {1, 1, 1};      //三次X滤波数组
uint32_t array2[3] = {0, 0, 0};      //三次X滤波数组
uint32_t adresult[3];                //三次平均值
uint32_t max[3] = {0, 0, 0};         //最大值
uint32_t min[3] = {1000, 1000, 1000};//最小
uint32_t dc[3] = {500, 500, 500};    //阀值
uint32_t vpp[3] = {30, 30, 30};      //最大加速度
uint32_t precision[3] = {5, 5, 5};   //动态精度
uint32_t old_fixed[3];               //线序移位器旧值
uint32_t new_fixed[3];               //线序移位器新值
uint32_t STEPS = 0;                  //步数

void step_counter(void) {
    static uint8_t sampling_counter = 0; //50次*0.2s = 10s
    uint8_t buf[6], jtemp;
    adxl345_flag = 0;
    Multiple_read_ADXL345(buf);          //连续读出数据，存储在BUF中
    //------------------------------------------采样滤波----------------------//
    for(jtemp = 0; jtemp <= 2; jtemp++) { //jtemp 0,1,2分别代表x，y，z
        array2[jtemp] = array1[jtemp];
        array1[jtemp] = array0[jtemp];
        array0[jtemp] = buf[2 * jtemp] + (buf[2 * jtemp + 1] << 8);
        adresult[jtemp] = array0[jtemp] + array1[jtemp] + array2[jtemp];
        adresult[jtemp] = adresult[jtemp] / 3;
        if(adresult[jtemp] > max[jtemp]) {max[jtemp] = adresult[jtemp];}
        if(adresult[jtemp] < min[jtemp]) {min[jtemp] = adresult[jtemp];}
    }
    sampling_counter = sampling_counter + 1;
    //----------------------------------计算动态门限和动态精度-----------------------//
    if(sampling_counter >= 50) {
        sampling_counter = 0;
        for(jtemp = 0; jtemp <= 2; jtemp++) {
            vpp[jtemp] = max[jtemp] - min[jtemp];
            dc[jtemp] = min[jtemp] + (vpp[jtemp] >> 1); //dc为阈值
            max[jtemp] = 0;
            min[jtemp] = 1023;
            bad_flag[jtemp] = 0;
            if(vpp[jtemp] >= 160) {
                precision[jtemp] = vpp[jtemp] / 32; //8
            } else if((vpp[jtemp] >= 50) && (vpp[jtemp] < 160)) {
                precision[jtemp] = 4;
            } else if((vpp[jtemp] >= 15) && (vpp[jtemp] < 50)) {
                precision[jtemp] = 3;
            } else {
                precision[jtemp] = 2;
                bad_flag[jtemp] = 1;
            }
        }
    }
    //--------------------------线性移位寄存器--------------------------------------
    for(jtemp = 0; jtemp <= 2; jtemp++) {
        old_fixed[jtemp] = new_fixed[jtemp];
        if(adresult[jtemp] >= new_fixed[jtemp]) {
            if((adresult[jtemp] - new_fixed[jtemp]) >= precision[jtemp])   {new_fixed[jtemp] = adresult[jtemp];}
        } else if(adresult[jtemp] < new_fixed[jtemp]) {
            if((new_fixed[jtemp] - adresult[jtemp]) >= precision[jtemp])   {new_fixed[jtemp] = adresult[jtemp];}
        }
    }
    //------------------------- 动态门限判决 ----------------------------------
    if((vpp[0] >= vpp[1]) && (vpp[0] >= vpp[2])) { //x轴最活跃
        if((old_fixed[0] >= dc[0]) && (new_fixed[0] < dc[0]) && (bad_flag[0] == 0)) {
            STEPS = STEPS + 1;
        }
    } else if((vpp[1] >= vpp[0]) && (vpp[1] >= vpp[2])) { //y轴最活跃
        if((old_fixed[1] >= dc[1]) && (new_fixed[1] < dc[1]) && (bad_flag[1] == 0)) {
            STEPS = STEPS + 1;
        }
    } else if((vpp[2] >= vpp[1]) && (vpp[2] >= vpp[0])) { //z轴最活跃
        if((old_fixed[2] >= dc[2]) && (new_fixed[2] < dc[2]) && (bad_flag[2] == 0)) {
            STEPS = STEPS + 1;
        }
    }
}

void step_counter_run(void) {
    static uint32_t pSTEPS = 0;                  //步数
    HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_SET);
    if(DelayTimeOut(&adxl345_flag_time, 20)) {   //0.02s处理一次
        adxl345_flag_time = HAL_GetTick();
        step_counter();
        //MYLOG(WARNING, "step_counter_run = %d", STEPS); //输出步数
        if(pSTEPS != STEPS) {
            pSTEPS = STEPS;
            MYLOG(WARNING, "step_counter_run = %d", STEPS); //输出步数
            HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_RESET);
        }
    }
}

//uint8_t SAMPLING_COUNTER = 0;
//SENSOR_DATA_TypeDef MAX_DATA = {0, 0, 0};
//SENSOR_DATA_TypeDef MIN_DATA = {1000, 1000, 1000};
//SENSOR_DATA_TypeDef THRESH   = {35, 225, 225};
//SENSOR_DATA_TypeDef DELTA    = {1000, 1000, 1000};

////max-min to select most active axis
//SENSOR_DATA_TypeDef RES = {1000, 1000, 1000};

//SENSOR_DATA_TypeDef g_last_sample = {0, 0, 0};
//SENSOR_DATA_TypeDef g_cur_sample = {0, 0, 0};

//int g_step_count = 0; //记步数

//#define ADX345_DelayMs(x)     {HAL_Delay(x);}  //延时函数
//#define max(a,b) ((a) > (b) ? (a) : (b))
///****************************************************************************
//* Function Name  : ADX345_WriteReg
//* Description    : 设置ADX345寄存器
//* Input          : addr：寄存器地址
//*                * dat：吸入数据
//* Output         : None
//* Return         : None
//****************************************************************************/



///****************************************************************************
//* Function Name  : ADX345_ReadReg
//* Description    : 读取ADX345寄存器
//* Input          : addr：读取地址
//* Output         : None
//* Return         : 读取到的8位数据
//****************************************************************************/



///****************************************************************************
//* Function Name  : ADX345_ReadXYZ
//* Description    : 读取X,Y,Z的AD值
//* Input          : xValue：X轴的保存地址
//*                * yValue：Y轴的保存地址
//*                * zValue：Z轴的保存地址
//* Output         : None
//* Return         : 0：读取成功。0xFF：读取失败
//****************************************************************************/

//static int8_t ADX345_ReadXYZ(int16_t *xValue, int16_t *yValue, int16_t *zValue) {
//    uint8_t buf[6];
//    HAL_I2C_Mem_Read(myadxl345, ADX345_ADDR, 0X32, I2C_MEMADD_SIZE_8BIT, &buf[0], 1, 0xFF);
//    HAL_I2C_Mem_Read(myadxl345, ADX345_ADDR, 0X33, I2C_MEMADD_SIZE_8BIT, &buf[1], 1, 0xFF);
//    HAL_I2C_Mem_Read(myadxl345, ADX345_ADDR, 0X34, I2C_MEMADD_SIZE_8BIT, &buf[2], 1, 0xFF);
//    HAL_I2C_Mem_Read(myadxl345, ADX345_ADDR, 0X35, I2C_MEMADD_SIZE_8BIT, &buf[3], 1, 0xFF);
//    HAL_I2C_Mem_Read(myadxl345, ADX345_ADDR, 0X36, I2C_MEMADD_SIZE_8BIT, &buf[4], 1, 0xFF);
//    HAL_I2C_Mem_Read(myadxl345, ADX345_ADDR, 0X37, I2C_MEMADD_SIZE_8BIT, &buf[5], 1, 0xFF);
//    *xValue = (short)(((uint16_t)buf[1] << 8) + buf[0]); //DATA_X1为高位有效字节
//    *yValue = (short)(((uint16_t)buf[3] << 8) + buf[2]); //DATA_Y1为高位有效字节
//    *zValue = (short)(((uint16_t)buf[5] << 8) + buf[4]); //DATA_Z1为高位有效字节
//    return 0;
//}

///****************************************************************************
//* Function Name  : ADX345_Init
//* Description    : 初始化ADX345，并核对ADX的ID
//* Input          : None
//* Output         : None
//* Return         : 0：成功。0xFF：失败
//****************************************************************************/

//int ADX345_Init(void) {
//    if(ADX345_ReadReg(ADX_DEVID) == 0xE5) {
//        ADX345_WriteReg(ADX_DATA_FORMAT, 0x2B); //13位全分辨率,输出数据右对齐,16g量程
//        ADX345_WriteReg(ADX_BW_RATE, 0x0A);     //数据输出速度为100Hz
//        ADX345_WriteReg(ADX_POWER_CTL, 0x28);   //链接使能,测量模式
//        ADX345_WriteReg(ADX_INT_ENABLE, 0x00);  //不使用中断
//        ADX345_WriteReg(ADX_OFSX, 0x00);        //敲击阀值
//        ADX345_WriteReg(ADX_OFSY, 0x00);        //X轴偏移
//        ADX345_WriteReg(ADX_OFSZ, 0x00);        //Y轴偏移
//        MYLOG(INFO,"ADXL345_Init OK!");
//        return 0;
//    }
//   MYLOG(ERROR,"ADXL345_Init Fail!");
//    return 0xFF; //返回初始化失败
//}

///****************************************************************************
//* Function Name  : ADX345_Adjust
//* Description    : ADX345进行校正。
//* Input          : None
//* Output         : None
//* Return         : None
//****************************************************************************/

//void ADX345_Adjust(void) {
//    int32_t offx = 0, offy = 0, offz = 0;
//    int16_t xValue, yValue, zValue;
//    uint8_t i;
//    ADX345_WriteReg(ADX_POWER_CTL, 0x00);    //先进入休眠模式.
//    ADX345_DelayMs(100);
//    ADX345_Init();
//    ADX345_DelayMs(20);
//    /* 读取十次数值 */
//    for(i = 0; i < 10; i++) {
//        ADX345_ReadXYZ(&xValue, &yValue, &zValue);
//        offx += xValue;
//        offy += yValue;
//        offz += zValue;
//        ADX345_DelayMs(10);   //才样频率在100HZ，10ms采样一次最好
//    }
//    /* 求出平均值 */
//    offx /= 10;
//    offy /= 10;
//    offz /= 10;
//    /* 全分辨率下，每个输出LSB为3.9 mg或偏移寄存器LSB的四分之一，所以除以4 */
//    xValue = -(offx / 4);
//    yValue = -(offy / 4);
//    zValue = -((offz - 256) / 4);
//    /* 设置偏移量 */
//    ADX345_WriteReg(ADX_OFSX, xValue);
//    ADX345_WriteReg(ADX_OFSY, yValue);
//    ADX345_WriteReg(ADX_OFSZ, zValue);
//}

///****************************************************************************
//* Function Name  : ADX_GetXYZData
//* Description    : 读取ADX的XYZ轴的值（进行过数据处理）
//* Input          : xValue：X轴的保存地址
//*                * yValue：Y轴的保存地址
//*                * zValue：Z轴的保存地址
//* Output         : None
//* Return         : None
//****************************************************************************/

//void ADX_GetXYZData(int16_t *xValue, int16_t *yValue, int16_t *zValue, SENSOR_DATA_TypeDef *axis_converted_avg) {
//    uint8_t i;
//    uint8_t res_div = 20;
//    MAX_DATA.X = 0;
//    MAX_DATA.Y = 0;
//    MAX_DATA.Z = 0;
//    MIN_DATA.X = 1000;
//    MIN_DATA.Y = 1000;
//    MIN_DATA.Z = 1000;
//    for(SAMPLING_COUNTER = 0; SAMPLING_COUNTER < 33; SAMPLING_COUNTER++) { //采样50次
//        int32_t xTotal = 0, yTotal = 0, zTotal = 0;
//        /* 读取4次采样值 */
//        for(i = 0; i < 4; i++) {
//            *xValue = 0;
//            *yValue = 0;
//            *zValue = 0;
//            ADX345_ReadXYZ(xValue, yValue, zValue);
//            xTotal += *xValue;
//            yTotal += *yValue;
//            zTotal += *zValue;
//            ADX345_DelayMs(10);  //才样频率在100HZ，10ms采样一次最好
//        }
//        /* 求出平均值 */
//        *xValue = xTotal / 4;
//        *yValue = yTotal / 4;
//        *zValue = zTotal / 4;
//        axis_converted_avg[SAMPLING_COUNTER].X = *xValue;
//        axis_converted_avg[SAMPLING_COUNTER].Y = *yValue;
//        axis_converted_avg[SAMPLING_COUNTER].Z = *zValue;
//        //find maximum and minimum value of 3 axis
//        if(MAX_DATA.X < *xValue) MAX_DATA.X = *xValue;
//        if(MAX_DATA.Y < *yValue) MAX_DATA.Y = *yValue;
//        if(MAX_DATA.Z < *zValue) MAX_DATA.Z = *zValue;
//        if(MIN_DATA.X > *xValue) MIN_DATA.X = *xValue;
//        if(MIN_DATA.Y > *yValue) MIN_DATA.Y = *yValue;
//        if(MIN_DATA.Z > *zValue) MIN_DATA.Z = *zValue;
//    }
//    THRESH = get_thresh(MAX_DATA, MIN_DATA); //动态阈值
//    DELTA = ADXL345_MINUS(MAX_DATA, MIN_DATA); //最大最小值的差值   加速度的变化
//    RES.X = max(MAX_DATA.X - THRESH.X, THRESH.X - MIN_DATA.X) / res_div; //除以10,最后得到动态精度
//    RES.Y = max(MAX_DATA.Y - THRESH.Y, THRESH.Y - MIN_DATA.Y) / res_div;
//    RES.Z = max(MAX_DATA.Z - THRESH.Z, THRESH.Z - MIN_DATA.Z) / res_div;
//}

//void ADX_GetXYZDataNEW(int16_t *xValue1, int16_t *yValue1, int16_t *zValue1, SENSOR_DATA_TypeDef *buf) {
//    uint8_t i;
//    for(SAMPLING_COUNTER = 0; SAMPLING_COUNTER < 33; SAMPLING_COUNTER++) { //采样50次
//        int32_t xTotal = 0, yTotal = 0, zTotal = 0;
//        /* 读取4次采样值 */
//        for(i = 0; i < 4; i++) {
//            *xValue1 = 0;
//            *yValue1 = 0;
//            *zValue1 = 0;
//            ADX345_ReadXYZ(xValue1, yValue1, zValue1);
//            xTotal += *xValue1;
//            yTotal += *yValue1;
//            zTotal += *zValue1;
//            ADX345_DelayMs(10);  //才样频率在100HZ，10ms采样一次最好
//        }
//        /* 求出平均值 */
//        *xValue1 = xTotal / 4;
//        *yValue1 = yTotal / 4;
//        *zValue1 = zTotal / 4;
//        buf[SAMPLING_COUNTER].X = *xValue1;
//        buf[SAMPLING_COUNTER].Y = *yValue1;
//        buf[SAMPLING_COUNTER].Z = *zValue1;
//    }
//}



////计算步数
//void ADXL345_STEPCOUNT(SENSOR_DATA_TypeDef NEW_SAMPLE) {
//    //add time window and step model rule
//    if(abs(DELTA.X) >= abs(DELTA.Y) && abs(DELTA.X) >= abs(DELTA.Z)) { //x是最活跃的轴
//        //找到加速度变化最大的轴，来计算步数
//        if(NEW_SAMPLE.X - THRESH.X > RES.X)
//            //采样值大于阈值与精度和
//        {
//            g_last_sample = g_cur_sample;
//            g_cur_sample = NEW_SAMPLE;
//            g_step_count++;
//        } else {
//            g_last_sample = g_cur_sample;
//        }
//    } else if(abs(DELTA.Y) >= abs(DELTA.X) && abs(DELTA.Y) >= abs(DELTA.Z)) { //Y轴最活跃
//        if(NEW_SAMPLE.Y - THRESH.Y > RES.Y) {
//            g_last_sample = g_cur_sample;
//            g_cur_sample = NEW_SAMPLE;
//            g_step_count++;
//        } else {
//            g_last_sample = g_cur_sample;
//        }
//    } else if(abs(DELTA.Z) >= abs(DELTA.X) && abs(DELTA.Z) >= abs(DELTA.X)) { //Z轴最活跃
//        if(NEW_SAMPLE.Z - THRESH.Z > RES.Z) {
//            g_last_sample = g_cur_sample;
//            g_cur_sample = NEW_SAMPLE;
//            g_step_count++;
//        } else {
//            g_last_sample = g_cur_sample;
//        }
//    }
//}

////求动态阈值
//SENSOR_DATA_TypeDef get_thresh(SENSOR_DATA_TypeDef data0, SENSOR_DATA_TypeDef data1) {
//    SENSOR_DATA_TypeDef result;
//    result.X = (data0.X + data1.X) / 2;
//    result.Y = (data0.Y + data1.Y) / 2;
//    result.Z = (data0.Z + data1.Z) / 2;
//    return result;
//}
////加
//SENSOR_DATA_TypeDef ADXL345_ADD(SENSOR_DATA_TypeDef data0, SENSOR_DATA_TypeDef data1) {
//    SENSOR_DATA_TypeDef result;
//    result.X = data0.X + data1.X;
//    result.Y = data0.Y + data1.Y;
//    result.Z = data0.Z + data1.Z;
//    return result;
//}

////减
//SENSOR_DATA_TypeDef ADXL345_MINUS(SENSOR_DATA_TypeDef data0, SENSOR_DATA_TypeDef data1) {
//    SENSOR_DATA_TypeDef result;
//    result.X = data0.X - data1.X;
//    result.Y = data0.Y - data1.Y;
//    result.Z = data0.Z - data1.Z;
//    return result;
//}

////乘
//SENSOR_DATA_TypeDef ADXL345_TIMES(SENSOR_DATA_TypeDef data0, SENSOR_DATA_TypeDef data1) {
//    SENSOR_DATA_TypeDef result;
//    result.X = data0.X * data1.X;
//    result.Y = data0.Y * data1.Y;
//    result.Z = data0.Z * data1.Z;
//    return result;
//}


////除
//SENSOR_DATA_TypeDef ADXL345_DEVIDE(SENSOR_DATA_TypeDef data0, SENSOR_DATA_TypeDef data1) {
//    SENSOR_DATA_TypeDef result;
//    result.X = data0.X / data1.X;
//    result.Y = data0.Y / data1.Y;
//    result.Z = data0.Z / data1.Z;
//    return result;
//}

//SENSOR_DATA_TypeDef ADXL345_AVERAGE(SENSOR_DATA_TypeDef *data, uint32_t len) {
//    SENSOR_DATA_TypeDef result;
//    uint32_t i;
//    for(i = 0; i < len; i++) {
//        result.X += data[i].X;
//        result.Y += data[i].Y;
//        result.Z += data[i].Z;
//    }
//    result.X /= len;
//    result.Y /= len;
//    result.Z /= len;
//    return result;
//}




///****************************************************************************
//* Function Name  : ADX_GetAngle
//* Description    : 将AD值转换成角度值
//* Input          : xValue：x的值
//*                * yValue：y的值
//*                * zValue：z的值
//*                * dir：0：与Z轴的角度;1：与X轴的角度;2：与Y轴的角度.
//* Output         : None
//* Return         : None
//****************************************************************************/

//int16_t ADX_GetAngle(float xValue, float yValue, float zValue, uint8_t dir) {
//    float temp;
//    float res = 0;
//    switch(dir) {
//        /* 与自然Z轴的角度 */
//        case 0:
//            temp = sqrt((xValue * xValue + yValue * yValue)) / zValue;
//            res = atan(temp);
//            break;
//        /* 与自然X轴的角度 */
//        case 1:
//            temp = xValue / sqrt((yValue * yValue + zValue * zValue));
//            res = atan(temp);
//            break;
//        /* 与自然Y轴的角度 */
//        case 2:
//            temp = yValue / sqrt((xValue * xValue + zValue * zValue));
//            res = atan(temp);
//            break;
//        default:
//            break;
//    }
//    res = res * 1800 / 3.14;
//    return res;
//}
///*---------------------------------------------------------------------------*/



//#define P_P_DIFF     1000               /* 波峰-波谷的差值，即3D阈值 1000*/
//#define RISING_EDGE  1                  /* 上升沿状态 */
//#define FALLING_EDGE 0                  /* 下降沿状态 */
//#define FAST_WALK_TIME_LIMIT_MS 200     /* ms */
//#define SLOW_WALK_TIME_LIMIT_MS 10000   /* 10s 内没有走一步 结束计步 */
//#define STEP_OK 7                       /* 7步法则 */

//unsigned int lastPos = 0;               /* 旧数据 */
//unsigned int newMax = 0, newMin = 0;    /* 波峰-波谷 */

//#define FALSE 0
//#define TRUE 1

//uint8_t walkSta = FALSE;                /* 获得一次峰值状态 */
//uint8_t walkOkSta = FALSE;              /* 连续10s内走了7步 有效行走状态 */
//uint8_t pSta = RISING_EDGE;             /* 3D数据波形状态 */

//long lastTime = 0;                      /* 上一次 walkSta 的时间 */
//unsigned char stepOK = 0;               /* 初始计步门限  滤除干扰 */
//unsigned long stepCount = 0;            /* 步数值 */

///*****************************************************************
//** input: 3 axis or angle
//** output: step count
//** user read:
//    3 axis is filter value.
//******************************************************************/
////extern uint32_t Timer_count;
//unsigned long Step_Count(float axis0, float axis1, float axis2) {
//    unsigned int nowPos = 0;
//    int ppDiff = 0;
//    int timeDiff = 0;
//    /* 获取3D IMU */
//    //nowPos = (unsigned int)powf(sqrtf(axis0) + sqrtf(axis1) + sqrtf(axis2), 0.5);
//    nowPos = (unsigned int)sqrtf(axis0*axis0 + axis1*axis1 + axis2*axis2);//获取加速度
//    /* 得到波峰和波谷 */
//    if((pSta == RISING_EDGE) && (nowPos <= lastPos)) {        //开始转下了
//        pSta = FALLING_EDGE;
//        newMax = lastPos;                                     //波峰
//        walkSta = TRUE;
//    } else if((pSta == FALLING_EDGE) && (nowPos > lastPos)) { //开始转下了
//        pSta = FALLING_EDGE;
//        newMin = lastPos;                                     //波谷
//        walkSta = TRUE;
//    } else {
//        walkSta = FALSE;                                      //中间值，不处理
//    }
//    /* 更新3D step状态数据 */
//    lastPos = nowPos;
//    /* 有波峰或波谷 */
//    if(walkSta == TRUE) {
//        walkSta = FALSE;
//        ppDiff = newMax - newMin; /* 波峰与波谷的差值 */
//        if(ppDiff > P_P_DIFF) {
//            //timeDiff = HAL_GetTick() - lastTime; /* 获取波峰和波谷的时间差 */
//            timeDiff = HAL_GetTick() - lastTime;     /* 获取波峰和波谷的时间差 */
//            if(timeDiff < FAST_WALK_TIME_LIMIT_MS) { /* 波峰波谷时间差小于200ms的直接去掉 */
//                return stepCount;
//            } else if(timeDiff > SLOW_WALK_TIME_LIMIT_MS) { /* 波峰波谷时间差大于10s的视为静止 */
//                walkOkSta = FALSE;
//                stepOK = 0;
//                return stepCount;
//            }
//            stepOK++;
//            if(stepOK >= STEP_OK) {   /* 走7步之后更新状态 */
//                walkOkSta = TRUE;
//            }
//            //lastTime = HAL_GetTick(); /* 更新时间 */
//            lastTime = HAL_GetTick(); /* 更新时间 */
//        }
//    }
//    if(walkOkSta == TRUE) {           /* 满足10s内走7步 */
//        stepCount += stepOK;
//        stepOK = 0;
//    }
//    return stepCount;
//}
//int16_t Xval, Yval, Zval, Xang, Yang, Zang;
//SENSOR_DATA_TypeDef axis_converted_avg2[33];
//unsigned long step = 0;
//void ADX_Test(void) {
//    /* 读取X,Y,Z的加速度值 */
//    ADX_GetXYZData(&Xval, &Yval, &Zval, axis_converted_avg2);
//    printf(" x:%d \t", Xval);
//    printf(" y:%d \t", Yval);
//    printf(" z:%d \n", Zval);
//    /* 将读取到的加速度值转换为角度值 */
//    Xang = ADX_GetAngle(Xval, Yval, Zval, 1);
//    Yang = ADX_GetAngle(Xval, Yval, Zval, 2);
//    Zang = ADX_GetAngle(Xval, Yval, Zval, 0);
//    step = Step_Count(Xang, Yang, Zang);
//    printf("STEP:%lu \n", step);
//}
///*----------------------------------------------------------------------------*/

//int16_t Xval, Yval, Zval;
//int16_t Xval1, Yval1, Zval1;
//SENSOR_DATA_TypeDef axis_converted_avg[33];
//SENSOR_DATA_TypeDef buf[33];
//void ADX_Testxxx(void) {
//    uint32_t i, j;
//    /* 读取X,Y,Z的加速度值 */
//    ADX_GetXYZData(&Xval, &Yval, &Zval, axis_converted_avg);
////            printf(" x:%d \n", Xval);
////            printf(" y:%d \n", Yval);
////            printf(" z:%d \n", Zval);
//    for(i = 0; i < 33; i++) {
//        //printf(" X:%d \t", axis_converted_avg[i].X);         //测试加速度值
//    }
//    ADX_GetXYZDataNEW(&Xval1, &Yval1, &Zval1, buf);
//    for(j = 0; j < 33; j++) {
//        ADXL345_STEPCOUNT(buf[i]);
//    }
//    printf(" step:%d \n", g_step_count);   //打印步数
//}
