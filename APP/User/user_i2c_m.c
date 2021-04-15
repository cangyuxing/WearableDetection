#include "user_i2c_m.h"

GPIO_InitTypeDef GPIO_Initure_I2C1;

void IIC1_SDA_IN(void) {
    GPIO_Initure_I2C1.Pin =  IIC1_SDA_Pin;   //GPIO_PIN_6和GPIO_PIN_7
    GPIO_Initure_I2C1.Mode = GPIO_MODE_INPUT;
    HAL_GPIO_Init(IIC1_CLK_GPIO_Port, &GPIO_Initure_I2C1); //PB
}
void IIC1_SDA_OUT(void) {
    GPIO_Initure_I2C1.Pin =  IIC1_SDA_Pin;   //GPIO_PIN_6和GPIO_PIN_7
    GPIO_Initure_I2C1.Mode = GPIO_MODE_OUTPUT_PP;
    HAL_GPIO_Init(IIC1_CLK_GPIO_Port, &GPIO_Initure_I2C1); //PB
}
void IIC1_SCL_H(void) {
    HAL_GPIO_WritePin(IIC1_CLK_GPIO_Port, IIC1_CLK_Pin, GPIO_PIN_SET);
}
void IIC1_SCL_L(void) {
    HAL_GPIO_WritePin(IIC1_CLK_GPIO_Port, IIC1_CLK_Pin, GPIO_PIN_RESET);
}
void IIC1_SDA_H(void) {
    HAL_GPIO_WritePin(IIC1_CLK_GPIO_Port, IIC1_SDA_Pin, GPIO_PIN_SET);
}
void IIC1_SDA_L(void) {
    HAL_GPIO_WritePin(IIC1_CLK_GPIO_Port, IIC1_SDA_Pin, GPIO_PIN_RESET);
}
uint8_t IIC1_SDA_Read(void) {
    return HAL_GPIO_ReadPin(IIC1_CLK_GPIO_Port, IIC1_SDA_Pin);
}

//IIC初始化
void IIC1_Init(void) {
    __HAL_RCC_GPIOB_CLK_ENABLE();   //使能GPIOC时钟
    //PC11,12初始化设置
    GPIO_Initure_I2C1.Pin = IIC1_CLK_Pin | IIC1_SDA_Pin;   //GPIO_PIN_6和GPIO_PIN_7
    GPIO_Initure_I2C1.Mode = GPIO_MODE_OUTPUT_PP;          //推挽输出
    GPIO_Initure_I2C1.Pull = GPIO_NOPULL;                  //上拉
    GPIO_Initure_I2C1.Speed = GPIO_SPEED_FREQ_HIGH;        //高速
    HAL_GPIO_Init(IIC1_CLK_GPIO_Port, &GPIO_Initure_I2C1); //PB
    IIC1_SDA_H();
    IIC1_SCL_H();
//    delay_ms(10);
//    IIC1_MYTest();
}

//产生IIC起始信号
void IIC1_Start(void) {
    IIC1_SDA_OUT();//sda线输出
    IIC1_SDA_H();  //拉高数据线
    IIC1_SCL_H();  //拉高时钟线
    delay_us(4);   //延时
    IIC1_SDA_L();  //产生下降沿 START:when CLK is high,DATA change form high to low
    delay_us(4);   //延时
    IIC1_SCL_L();  //钳住I2C总线，准备发送或接收数据
}
//产生IIC停止信号
void IIC1_Stop(void) {
    IIC1_SDA_OUT();//sda线输出
    IIC1_SCL_L();
    IIC1_SDA_L(); //STOP:when CLK is high DATA change form low to high
    delay_us(4);
    IIC1_SCL_H();
    IIC1_SDA_H(); //发送I2C总线结束信号
    delay_us(4);
}
//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
uint8_t IIC1_Wait_Ack(void) {
    uint8_t ucErrTime = 0;
    IIC1_SDA_IN();      //SDA设置为输入
    IIC1_SDA_H(); delay_us(1);
    IIC1_SCL_H(); delay_us(1);
    while(IIC1_SDA_Read()) {
        ucErrTime++;
        if(ucErrTime > 250) {
            IIC1_Stop();
            //MYLOG(ERROR, "time out");
            return 1;
        }
    }
    IIC1_SCL_L(); //时钟输出0
    //MYLOG(ERROR, "time ok");
    return 0;
}
//产生ACK应答
void IIC1_Ack(void) {
    IIC1_SCL_L();
    IIC1_SDA_OUT();
    IIC1_SDA_L();
    delay_us(2);
    IIC1_SCL_H();
    delay_us(2);
    IIC1_SCL_L();
}
//不产生ACK应答
void IIC1_NAck(void) {
    IIC1_SCL_L();
    IIC1_SDA_OUT();
    IIC1_SDA_H();
    delay_us(2);
    IIC1_SCL_H();
    delay_us(2);
    IIC1_SCL_L();
}
//IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答
void IIC1_Send_Byte(uint8_t txd) {
    uint8_t t;
    IIC1_SDA_OUT();
    IIC1_SCL_L(); //拉低时钟开始数据传输
    for(t = 0; t < 8; t++) {
        if((txd & 0x80) >> 7) {
            IIC1_SDA_H();
        } else {
            IIC1_SDA_L();
        }
        //IIC_SDA = (txd & 0x80) >> 7;
        txd <<= 1;
        delay_us(2);   //对TEA5767这三个延时都是必须的
        IIC1_SCL_H();
        delay_us(2);
        IIC1_SCL_L();
        delay_us(2);
    }
}
//读1个字节，ack=1时，发送ACK，ack=0，发送nACK
uint8_t IIC1_Read_Byte(unsigned char ack) {
    unsigned char i, receive = 0;
    IIC1_SDA_IN();//SDA设置为输入
    for(i = 0; i < 8; i++) {
        IIC1_SCL_L();
        delay_us(2);
        IIC1_SCL_H();
        receive <<= 1;
        if(IIC1_SDA_Read()) {
            //MYLOG(ERROR, "Read H");
            receive++;
        }
        delay_us(1);
    }
    if(!ack)
        IIC1_NAck();//发送nACK
    else
        IIC1_Ack(); //发送ACK
    return receive;
}

/**********************************************************************************************/
void IIC1_WriteReg(uint8_t SlaveAddress, uint8_t WriteAddr, uint8_t DataToWrite) {
    IIC1_Start();                   //起始信号
    IIC1_Send_Byte(SlaveAddress);   //发送设备地址+写信号
    IIC1_Wait_Ack();
    IIC1_Send_Byte(WriteAddr);      //内部寄存器地址，请参考中文pdf22页
    IIC1_Wait_Ack();
    IIC1_Send_Byte(DataToWrite);    //内部寄存器数据，请参考中文pdf22页
    IIC1_Wait_Ack();
    IIC1_Stop();                    //产生一个停止条件
    //delay_ms(10);
}

uint8_t IIC1_ReadReg(uint8_t SlaveAddress, uint8_t ReadAddr) {
    uint8_t temp = 0;
    IIC1_Start();                           //起始信号
    IIC1_Send_Byte(SlaveAddress);           //发送设备地址+写信号
    IIC1_Wait_Ack();                        //等待响应
    IIC1_Send_Byte(ReadAddr);               //发送存储单元地址，从0开始
    IIC1_Wait_Ack();                        //等待响应
    IIC1_Start();                           //起始信号
    IIC1_Send_Byte(SlaveAddress + 1);       //发送设备地址+读信号
    IIC1_Wait_Ack();                        //等待响应
    temp = IIC1_Read_Byte(0);               //最后一个数据不需要响应
    IIC1_Stop();                            //停止信号
    return temp;
}
void IIC1_MYTest(void) {
    uint8_t temp = 0;
    while(1) {
        temp = IIC1_ReadReg(ADXL_WRITE, DEVICE_ID);
        MYLOG(ERROR, "IIC1_MYTest 1<%x>", temp);
//        temp = Single_Read_ADXL345(ADXL_WRITE,DEVICE_ID);
//        MYLOG(ERROR, "IIC1_MYTest 2<%x>", temp);
        HAL_Delay(1000);
    }
}
/**********************************************************************************************/
GPIO_InitTypeDef GPIO_Initure_I2C2;

void IIC2_SDA_IN(void) {
    GPIO_Initure_I2C2.Pin =  IIC2_SDA_Pin;   //GPIO_PIN_6和GPIO_PIN_7
    GPIO_Initure_I2C2.Mode = GPIO_MODE_INPUT;
    HAL_GPIO_Init(IIC2_CLK_GPIO_Port, &GPIO_Initure_I2C2); //PB
}
void IIC2_SDA_OUT(void) {
    GPIO_Initure_I2C2.Pin =  IIC2_SDA_Pin;   //GPIO_PIN_6和GPIO_PIN_7
    GPIO_Initure_I2C2.Mode = GPIO_MODE_OUTPUT_PP;
    HAL_GPIO_Init(IIC2_CLK_GPIO_Port, &GPIO_Initure_I2C2); //PB
}
void IIC2_SCL_H(void) {
    HAL_GPIO_WritePin(IIC2_CLK_GPIO_Port, IIC2_CLK_Pin, GPIO_PIN_SET);
}
void IIC2_SCL_L(void) {
    HAL_GPIO_WritePin(IIC2_CLK_GPIO_Port, IIC2_CLK_Pin, GPIO_PIN_RESET);
}
void IIC2_SDA_H(void) {
    HAL_GPIO_WritePin(IIC2_CLK_GPIO_Port, IIC2_SDA_Pin, GPIO_PIN_SET);
}
void IIC2_SDA_L(void) {
    HAL_GPIO_WritePin(IIC2_CLK_GPIO_Port, IIC2_SDA_Pin, GPIO_PIN_RESET);
}
uint8_t IIC2_SDA_Read(void) {
    return HAL_GPIO_ReadPin(IIC2_CLK_GPIO_Port, IIC2_SDA_Pin);
}

//IIC初始化
void IIC2_Init(void) {
    __HAL_RCC_GPIOB_CLK_ENABLE();   //使能GPIOC时钟
    //PC11,12初始化设置
    GPIO_Initure_I2C2.Pin = IIC2_CLK_Pin | IIC2_SDA_Pin;   //GPIO_PIN_6和GPIO_PIN_7
    GPIO_Initure_I2C2.Mode = GPIO_MODE_OUTPUT_PP;          //推挽输出
    GPIO_Initure_I2C2.Pull = GPIO_NOPULL;                  //上拉
    GPIO_Initure_I2C2.Speed = GPIO_SPEED_FREQ_HIGH;        //高速
    HAL_GPIO_Init(IIC1_CLK_GPIO_Port, &GPIO_Initure_I2C2); //PB
    IIC2_SDA_H();
    IIC2_SCL_H();
//    delay_ms(10);
//    IIC2_MYTest();
}

//产生IIC起始信号
void IIC2_Start(void) {
    IIC2_SDA_OUT();//sda线输出
    IIC2_SDA_H();  //拉高数据线
    IIC2_SCL_H();  //拉高时钟线
    delay_us(4);   //延时
    IIC2_SDA_L();  //产生下降沿 START:when CLK is high,DATA change form high to low
    delay_us(4);   //延时
    IIC2_SCL_L();  //钳住I2C总线，准备发送或接收数据
}
//产生IIC停止信号
void IIC2_Stop(void) {
    IIC2_SDA_OUT();//sda线输出
    IIC2_SCL_L();
    IIC2_SDA_L(); //STOP:when CLK is high DATA change form low to high
    delay_us(4);
    IIC2_SCL_H();
    IIC2_SDA_H(); //发送I2C总线结束信号
    delay_us(4);
}
//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
uint8_t IIC2_Wait_Ack(void) {
    uint8_t ucErrTime = 0;
    IIC2_SDA_IN();      //SDA设置为输入
    IIC2_SDA_H(); delay_us(1);
    IIC2_SCL_H(); delay_us(1);
    while(IIC2_SDA_Read()) {
        ucErrTime++;
        if(ucErrTime > 250) {
            IIC2_Stop();
            //MYLOG(ERROR, "time out");
            return 1;
        }
    }
    IIC2_SCL_L(); //时钟输出0
    //MYLOG(ERROR, "time ok");
    return 0;
}
//产生ACK应答
void IIC2_Ack(void) {
    IIC2_SCL_L();
    IIC2_SDA_OUT();
    IIC2_SDA_L();
    delay_us(2);
    IIC2_SCL_H();
    delay_us(2);
    IIC2_SCL_L();
}
//不产生ACK应答
void IIC2_NAck(void) {
    IIC2_SCL_L();
    IIC2_SDA_OUT();
    IIC2_SDA_H();
    delay_us(2);
    IIC2_SCL_H();
    delay_us(2);
    IIC2_SCL_L();
}
//IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答
void IIC2_Send_Byte(uint8_t txd) {
    uint8_t t;
    IIC2_SDA_OUT();
    IIC2_SCL_L(); //拉低时钟开始数据传输
    for(t = 0; t < 8; t++) {
        if((txd & 0x80) >> 7) {
            IIC2_SDA_H();
        } else {
            IIC2_SDA_L();
        }
        //IIC_SDA = (txd & 0x80) >> 7;
        txd <<= 1;
        delay_us(2);   //对TEA5767这三个延时都是必须的
        IIC2_SCL_H();
        delay_us(2);
        IIC2_SCL_L();
        delay_us(2);
    }
}
//读1个字节，ack=1时，发送ACK，ack=0，发送nACK
uint8_t IIC2_Read_Byte(unsigned char ack) {
    unsigned char i, receive = 0;
    IIC2_SDA_IN();//SDA设置为输入
    for(i = 0; i < 8; i++) {
        IIC2_SCL_L();
        delay_us(2);
        IIC2_SCL_H();
        receive <<= 1;
        if(IIC2_SDA_Read()) {
            //MYLOG(ERROR, "Read H");
            receive++;
        }
        delay_us(1);
    }
    if(!ack)
        IIC2_NAck();//发送nACK
    else
        IIC2_Ack(); //发送ACK
    return receive;
}

/**********************************************************************************************/
void IIC2_WriteReg(uint8_t SlaveAddress, uint8_t WriteAddr, uint8_t DataToWrite) {
    IIC2_Start();                   //起始信号
    IIC2_Send_Byte(SlaveAddress);   //发送设备地址+写信号
    IIC2_Wait_Ack();
    IIC2_Send_Byte(WriteAddr);      //内部寄存器地址，请参考中文pdf22页
    IIC2_Wait_Ack();
    IIC2_Send_Byte(DataToWrite);    //内部寄存器数据，请参考中文pdf22页
    IIC2_Wait_Ack();
    IIC2_Stop();                    //产生一个停止条件
    //delay_ms(10);
}

uint8_t IIC2_ReadReg(uint8_t SlaveAddress, uint8_t ReadAddr) {
    uint8_t temp = 0;
    IIC2_Start();                          //起始信号
    IIC2_Send_Byte(SlaveAddress);           //发送设备地址+写信号
    IIC2_Wait_Ack();
    IIC2_Send_Byte(ReadAddr);            //发送存储单元地址，从0开始
    IIC2_Wait_Ack();
    IIC2_Start();                            //起始信号
    IIC2_Send_Byte(SlaveAddress + 1);       //发送设备地址+读信号
    IIC2_Wait_Ack();
    temp = IIC2_Read_Byte(0);
    IIC2_Stop();                           //停止信号
    return temp;
}
void IIC2_MYTest(void) {
    uint8_t temp = 0;
    while(1) {
        //temp = axp173_read(I2C1_SLAVE_ADDRESS7 + 1, POWER_OUTPUT_CONTROL);
        IIC2_WriteReg(I2C1_SLAVE_ADDRESS7, POWER_OUTPUT_CONTROL, 01);
        HAL_Delay(20);
        temp = IIC2_ReadReg(I2C1_SLAVE_ADDRESS7, POWER_OUTPUT_CONTROL);
        MYLOG(ERROR, "IIC2_MYTest 1<%x>", temp);
//        temp = Single_Read_ADXL345(ADXL_WRITE,DEVICE_ID);
//        MYLOG(ERROR, "IIC1_MYTest 2<%x>", temp);
        HAL_Delay(1000);
    }
}








