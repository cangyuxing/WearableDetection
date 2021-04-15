#ifndef __NRF24L01_PLUS_H_
#define __NRF24L01_PLUS_H_

#include "main.h"

#define NRF24L01_Plus_Role 1    //0：接收  1：发送

//NRF24L01寄存器操作命令
#define NRF24L01_Plus_NRF_READ_REG    0x00  //读配置寄存器,低5位为寄存器地址
#define NRF24L01_Plus_NRF_WRITE_REG   0x20  //写配置寄存器,低5位为寄存器地址
#define NRF24L01_Plus_RD_RX_PLOAD     0x61  //读RX有效数据,1~32字节
#define NRF24L01_Plus_WR_TX_PLOAD     0xA0  //写TX有效数据,1~32字节
#define NRF24L01_Plus_ACK_PAYLOAD_CMD 0xa8  //适用于接收方，通过PIPE PPP将数据通过ACK形式发送，最多允许三帧数据存在于FIFO
#define NRF24L01_Plus_FLUSH_TX        0xE1  //清除TX FIFO寄存器.发射模式下用
#define NRF24L01_Plus_FLUSH_RX        0xE2  //清除RX FIFO寄存器.接收模式下用
#define NRF24L01_Plus_REUSE_TX_PL     0xE3  //重新使用上一包数据,CE为高,数据包被不断发送.
#define NRF24L01_Plus_NOP             0xFF  //空操作,可以用来读状态寄存器  
//SPI(NRF24L01)寄存器地址
#define NRF24L01_Plus_CONFIG          0x00  //配置寄存器地址;bit0:1接收模式,0发射模式;bit1:电选择;bit2:CRC模式;bit3:CRC使能;
//bit4:中断MAX_RT(达到最大重发次数中断)使能;bit5:中断TX_DS使能;bit6:中断RX_DR使能
#define NRF24L01_Plus_EN_AA           0x01  //使能自动应答功能  bit0~5,对应通道0~5
#define NRF24L01_Plus_EN_RXADDR       0x02  //接收地址允许,bit0~5,对应通道0~5
#define NRF24L01_Plus_SETUP_AW        0x03  //设置地址宽度(所有数据通道):bit1,0:00,3字节;01,4字节;02,5字节;
#define NRF24L01_Plus_SETUP_RETR      0x04  //建立自动重发;bit3:0,自动重发计数器;bit7:4,自动重发延时 250*x+86us
#define NRF24L01_Plus_RF_CH           0x05  //RF通道,bit6:0,工作通道频率;
#define NRF24L01_Plus_RF_SETUP        0x06  //RF寄存器;bit3:传输速率(0:1Mbps,1:2Mbps);bit2:1,发射功率;bit0:低噪声放大器增益
#define NRF24L01_Plus_STATUS          0x07  //状态寄存器;bit0:TX FIFO满标志;bit3:1,接收数据通道号(最大:6);bit4,达到最多次重发
//bit5:数据发送完成中断;bit6:接收数据中断;
#define NRF24L01_Plus_MAX_TX          0x10  //达到最大发送次数中断
#define NRF24L01_Plus_TX_OK           0x20  //TX发送完成中断
#define NRF24L01_Plus_RX_OK           0x40  //接收到数据中断

#define NRF24L01_Plus_R_RX_PL_WID     0x60

#define NRF24L01_Plus_OBSERVE_TX      0x08  //发送检测寄存器,bit7:4,数据包丢失计数器;bit3:0,重发计数器
#define NRF24L01_Plus_CD              0x09  //载波检测寄存器,bit0,载波检测;
#define NRF24L01_Plus_RX_ADDR_P0      0x0A  //数据通道0接收地址,最大长度5个字节,低字节在前
#define NRF24L01_Plus_RX_ADDR_P1      0x0B  //数据通道1接收地址,最大长度5个字节,低字节在前
#define NRF24L01_Plus_RX_ADDR_P2      0x0C  //数据通道2接收地址,最低字节可设置,高字节,必须同RX_ADDR_P1[39:8]相等;
#define NRF24L01_Plus_RX_ADDR_P3      0x0D  //数据通道3接收地址,最低字节可设置,高字节,必须同RX_ADDR_P1[39:8]相等;
#define NRF24L01_Plus_RX_ADDR_P4      0x0E  //数据通道4接收地址,最低字节可设置,高字节,必须同RX_ADDR_P1[39:8]相等;
#define NRF24L01_Plus_RX_ADDR_P5      0x0F  //数据通道5接收地址,最低字节可设置,高字节,必须同RX_ADDR_P1[39:8]相等;
#define NRF24L01_Plus_TX_ADDR         0x10  //发送地址(低字节在前),ShockBurstTM模式下,RX_ADDR_P0与此地址相等
#define NRF24L01_Plus_RX_PW_P0        0x11  //接收数据通道0有效数据宽度(1~32字节),设置为0则非法
#define NRF24L01_Plus_RX_PW_P1        0x12  //接收数据通道1有效数据宽度(1~32字节),设置为0则非法
#define NRF24L01_Plus_RX_PW_P2        0x13  //接收数据通道2有效数据宽度(1~32字节),设置为0则非法
#define NRF24L01_Plus_RX_PW_P3        0x14  //接收数据通道3有效数据宽度(1~32字节),设置为0则非法
#define NRF24L01_Plus_RX_PW_P4        0x15  //接收数据通道4有效数据宽度(1~32字节),设置为0则非法
#define NRF24L01_Plus_RX_PW_P5        0x16  //接收数据通道5有效数据宽度(1~32字节),设置为0则非法
#define NRF24L01_Plus_NRF_FIFO_STATUS 0x17  //FIFO状态寄存器;bit0,RX FIFO寄存器空标志;bit1,RX FIFO满标志;bit2,3,保留

#define NRF24L01_Plus_DYNPD           0x1C   // 启用动态数据长度
#define NRF24L01_Plus_FEATURE         0x1D   // 特征寄存器
//bit4,TX FIFO空标志;bit5,TX FIFO满标志;bit6,1,循环发送上一数据包.0,不循环;

//配置24L01的IO口（除标准SPI接口引脚）
#define NRF24L01_Plus_CE_LOW   HAL_GPIO_WritePin(NRF24L01_CE_GPIO_Port, NRF24L01_CE_Pin, GPIO_PIN_RESET) //24L01使能信号
#define NRF24L01_Plus_CE_HIGH  HAL_GPIO_WritePin(NRF24L01_CE_GPIO_Port, NRF24L01_CE_Pin, GPIO_PIN_SET)
#define NRF24L01_Plus_CSN_LOW  HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_RESET)         //SPI片选信号
#define NRF24L01_Plus_CSN_HIGH HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_SET)
#define NRF24L01_Plus_IRQ_READ HAL_GPIO_ReadPin(NRF24L01_IRQ_GPIO_Port, NRF24L01_IRQ_Pin)                //IRQ主机数据输入
//24L01发送接收数据宽度定义
#define NRF24L01_Plus_TX_ADR_WIDTH    5       //5字节的地址宽度
#define NRF24L01_Plus_RX_ADR_WIDTH    5       //5字节的地址宽度
#define NRF24L01_Plus_TX_PLOAD_WIDTH  32      //32字节的用户数据宽度
#define NRF24L01_Plus_RX_PLOAD_WIDTH  32      //32字节的用户数据宽度

#define NRF24L01_Plus_MODEL_RX1       1
#define NRF24L01_Plus_MODEL_TX1       2
#define NRF24L01_Plus_MODEL_RX2       3
#define NRF24L01_Plus_MODEL_TX2       4

extern  uint8_t NRF24L01_Plus_Rx_Len;
extern  uint8_t NRF24L01_Plus_Rx_flag;
extern  uint8_t NRF24L01_Plus_Rx_BUF[33];
extern  uint8_t NRF24L01_Plus_Tx_BUF[33];
extern  uint8_t NRF24L01_Plus_Tx_Len;
extern  uint8_t NRF24L01_Plus_Tx_flag;

void    NRF24L01_Plus_Init(void);//初始化
uint8_t NRF24L01_Plus_Write_Buf(uint8_t reg, uint8_t *pBuf, uint8_t u8s);//写数据区
uint8_t NRF24L01_Plus_Read_Buf(uint8_t reg, uint8_t *pBuf, uint8_t u8s);//读数据区
uint8_t NRF24L01_Plus_Read_Reg(uint8_t reg);           //读寄存器
uint8_t NRF24L01_Plus_Write_Reg(uint8_t reg, uint8_t value);//写寄存器
uint8_t NRF24L01_Plus_Check(void);//检查24L01是否存在

void    NRF24L01_Plus_Mode(uint8_t model);
void    NRF24L01_Plus_Check_Event(void);
uint8_t NRF24L01_Plus_RxPacket(uint8_t *RxData);
void    NRF24L01_Plus_TxPacket_AP(uint8_t *txbuf, uint8_t len);
uint8_t NRF24L01_Plus_TxPacket(uint8_t *txbuf, uint8_t len); //发送一个包的数据
#endif
