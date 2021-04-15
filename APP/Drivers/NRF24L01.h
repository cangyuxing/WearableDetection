/****************************************
NRF24L01库函数

P0 RX 发送模式下的接收端地址
P1 RX 地址 本机地址
******************************************/

#ifndef __NRF24L01_H_
#define __NRF24L01_H_

#include "main.h"

//#include <stdio.h>
//#include <string.h>
//#include <stdlib.h>

//#include "usart.h"
//#include "spi.h"
//#include "gpio.h"

#define BK2423  0                       // 若使用BK2423使能,否则置0
#define HAL_SPI 1                       // 若使用ST官方SPI驱动，请置1
#define NRF_RXFIFO_MAX  256             // 缓存大小
#define NRF_TXFIFO_MAX  NRF_RXFIFO_MAX  // 缓存大小

//**************************************************************************************
//NRF24L01引脚定义
//24L01操作线
#define _BSP_NRF24_CE_LOW       HAL_GPIO_WritePin(NRF24L01_CE_GPIO_Port, NRF24L01_CE_Pin, GPIO_PIN_RESET)   //24L01使能信号
#define _BSP_NRF24_CE_HIGH      HAL_GPIO_WritePin(NRF24L01_CE_GPIO_Port, NRF24L01_CE_Pin, GPIO_PIN_SET)

#define _BSP_NRF24_CSN_LOW      HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_RESET) //SPI片选信号
#define _BSP_NRF24_CSN_HIGH     HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_SET)

#define _BSP_NRF24_IRQ_READ     HAL_GPIO_ReadPin(NRF24L01_IRQ_GPIO_Port, NRF24L01_IRQ_Pin)       //IRQ主机数据输入

//********************************    NRF24L01   *************************************
#define NRF24L01_ADR_LEN        (5)     //NRF24L01收发地址宽度
#define NRF24L01_TX_PLOAD_LEN   (32)    //uints TX payload
#define NRF24L01_RX_PLOAD_LEN   (32)    //uints RX payload

// 功能 标记
#define NRF24L01_AUTO_SlEEP     (1<<3)  // 自动休眠
#define NRF24L01_AUTO_ACK       (1<<2)  // 自动回复 Enhanced ShockBurst
#define NRF24L01_AUTO_RX        (1<<1)  // 发送完成自动转入接收模式

//**************************************************************************************
//NRF24L01寄存器指令
#define ACTIVATE_CMD             0x50   // 写入0x73激活R_RX_PL_WID,W_ACK_PAYLOAD,W_TX_PAYLOAD_NOACK寄存器,在次写入禁用.只能在省电模式和掉电模式执行
#define W_TX_PAYLOAD_NOACK_CMD   0xb0   // 应用于发送模式，无需应答，需要将EN_DYN_ACK置1有效
#define W_ACK_PAYLOAD_CMD        0xa8   // 适用于接收方，通过PIPE PPP将数据通过ACK形式发送，最多允许三帧数据存在于FIFO
#define R_RX_PL_WID_CMD          0x60   // 读接收到的动态数据包长度（大于32清空RX_FIFO）
#define NRF24L01_READ_REG        0x00   // 读寄存器指令
#define NRF24L01_WRITE_REG       0x20   // 写寄存器指令
#define NRF24L01_RD_RX_PLOAD     0x61   // 读取接收数据指令
#define NRF24L01_WR_TX_PLOAD     0xA0   // 写待发数据指令
#define NRF24L01_FLUSE_TX        0xE1   // 清除TX FIFO中的数据，应用与发射模式下
#define NRF24L01_FLUSE_RX        0xE2   // 清除RX FIFO中的数据，应用与接收模式下
#define NRF24L01_REUSE_TX_PL     0xE3   // 定义重复装载数据指令
#define NRF24L01_NOP             0xFF   // 无操作，用于读取STATUS寄存器

//**************************************************************************************
// (nRF24L01)寄存器地址
#define NRF24L01_CONFIG          0x00   // 配置收发状态，CRC校验模式以及收发状态响应方式
#define NRF24L01_EN_AA           0x01   // 自动应答功能设置
#define NRF24L01_EN_RXADDR       0x02   // 可用信道设置
#define NRF24L01_SETUP_AW        0x03   // 收发地址宽度设置
#define NRF24L01_SETUP_RETR      0x04   // 自动重发功能设置
#define NRF24L01_RF_CH           0x05   // 工作频率设置
#define NRF24L01_RF_SETUP        0x06   // 发射速率、功耗功能设置
#define NRF24L01_STATUS          0x07   // 状态寄存器
#define NRF24L01_OBSERVE_TX      0x08   // 发送监测功能
#define NRF24L01_CD              0x09   // 地址检测 可以检测信道是否被占用
#define NRF24L01_RX_ADDR_P0      0x0A   // 频道0接收数据地址
#define NRF24L01_RX_ADDR_P1      0x0B   // 频道1接收数据地址
#define NRF24L01_RX_ADDR_P2      0x0C   // 频道2接收数据地址
#define NRF24L01_RX_ADDR_P3      0x0D   // 频道3接收数据地址
#define NRF24L01_RX_ADDR_P4      0x0E   // 频道4接收数据地址
#define NRF24L01_RX_ADDR_P5      0x0F   // 频道5接收数据地址
#define NRF24L01_TX_ADDR         0x10   // 发送地址寄存器
#define NRF24L01_RX_PW_P0        0x11   // 接收频道0接收数据长度
#define NRF24L01_RX_PW_P1        0x12   // 接收频道1接收数据长度
#define NRF24L01_RX_PW_P2        0x13   // 接收频道2接收数据长度
#define NRF24L01_RX_PW_P3        0x14   // 接收频道3接收数据长度
#define NRF24L01_RX_PW_P4        0x15   // 接收频道4接收数据长度
#define NRF24L01_RX_PW_P5        0x16   // 接收频道5接收数据长度
#define NRF24L01_FIFO_STATUS     0x17   // FIFO栈入栈出状态寄存器设置
#define NRF24L01_DYNPD           0x1C   // 启用动态数据长度
#define NRF24L01_FEATURE         0x1D   // 特征寄存器

//**************************************************************************************
//#define RX_OK                    0x40  //接收到数据中断
//#define TX_OK                    0x20  //TX发送完成中断
//#define MAX_TX                   0x10  //达到最大发送次数中断
//NRF24L01状态寄存器值
#define NRF24L01_STATUS_RX_DR    0x40   // 接收到数据中断标志位
#define NRF24L01_STATUS_TX_DS    0x20   // 发送完成中断标志位
#define NRF24L01_STATUS_MAX_RT   0x10   // 达到最大重发中断标志位
#define NRF24L01_STATUS_RX_MARK  0x0E   // RX 接收通道有数据 111 空
#define NRF24L01_STATUS_TX_FULL  0x01   // TX FIFO寄存器满标志 1 为 满，0为 不满

//**************************************************************************************
//NRF24L01配置寄存器值
#define NRF24L01_CONFIG_RX_DR    0x40   // 屏蔽接收中断
#define NRF24L01_CONFIG_TX_DS    0x20   // 屏蔽发送中断
#define NRF24L01_CONFIG_MAX_RT   0x10   // 屏蔽重发满中断
#define NRF24L01_CONFIG_EN_CRC   0x08   // CRC 使能
#define NRF24L01_CONFIG_CRCO     0x04   // CRC 模式
#define NRF24L01_CONFIG_PWR_UP   0x02   // 1 上电    0 掉电
#define NRF24L01_CONFIG_PRIM_RX  0x01   // 1 RX模式  0 TX模式

// TX模式 config配置
#define NRF24L01_CONFIG_TX        ( NRF24L01_CONFIG_EN_CRC | NRF24L01_CONFIG_CRCO | NRF24L01_CONFIG_PWR_UP  )
// RX模式 config配置
#define NRF24L01_CONFIG_RX        ( NRF24L01_CONFIG_EN_CRC | NRF24L01_CONFIG_CRCO | NRF24L01_CONFIG_PWR_UP | NRF24L01_CONFIG_PRIM_RX)
// PowerDown模式 config配置
#define NRF24L01_CONFIG_SLEEP     ( NRF24L01_CONFIG_EN_CRC | NRF24L01_CONFIG_CRCO )

// TX模式 config配置
#define NRF24L01_CONFIG_TX2       ( NRF24L01_CONFIG_TX_DS  | NRF24L01_CONFIG_EN_CRC | NRF24L01_CONFIG_CRCO | NRF24L01_CONFIG_PWR_UP  )
// RX模式 config配置
#define NRF24L01_CONFIG_RX2       ( NRF24L01_CONFIG_TX_DS  | NRF24L01_CONFIG_MAX_RT |NRF24L01_CONFIG_EN_CRC | NRF24L01_CONFIG_CRCO | NRF24L01_CONFIG_PWR_UP | NRF24L01_CONFIG_PRIM_RX)
//**************************************************************************************
//结构体 枚举
typedef enum {
    HAL_NRF_STATE_RESET       = 0x00,    /*!<    */
    HAL_NRF_STATE_READY       = 0x01,    /*!<    */
    HAL_NRF_STATE_SEELP       = 0x02,    /*!<    */
    HAL_NRF_STATE_MAXRT       = 0x04,    /*!<    */
    HAL_NRF_STATE_ERROR       = 0x08,    /*!<    */
    HAL_NRF_STATE_TX          = 0x40,    /*!<    */
    HAL_NRF_STATE_RX          = 0x20,    /*!<    */
    HAL_NRF_STATE_BUSY        = 0x80,    /*!<    */
    HAL_NRF_STATE_BUSY_TX     = 0xC0,    /*!<    */
    HAL_NRF_STATE_BUSY_RX     = 0xA0     /*!<    */
} HAL_NRF_StateTypeDef;

// 数据速率     HAL_NRF_DataRates_TypeDef
#define HAL_NRF_DR_1M           0x00    /*!<    */
#define HAL_NRF_DR_2M           0x08    /*!<    */
#define HAL_NRF_DR_250K         0x20    /*!<    */
#define HAL_NRF_DR_MARK         0x28    /*!<    */

// 发射功率     HAL_NRF_PA_TypeDef
#define HAL_NRF_PA_0DBM         0x06    /*!<    */
#define HAL_NRF_PA_6DBM         0x04    /*!<    */
#define HAL_NRF_PA_12DBM        0x02    /*!<    */
#define HAL_NRF_PA_18DBM        0x00    /*!<    */
#define HAL_NRF_PA_MARK         0x06    /*!<    */

// 地址宽度 HAL_NRF_ADW_TypeDef
#define HAL_NRF_ADW_3           0x01    /*!<    */
#define HAL_NRF_ADW_4           0x02    /*!<    */
#define HAL_NRF_ADW_5           0x03    /*!<    */
#define HAL_NRF_ADW_MARK        0x03    /*!<    */

// 接收通道 HAL_NRF_ENAA_TypeDef;
#define HAL_NRF_ENAA_P0         0x01    /*!<    */
#define HAL_NRF_ENAA_P1         0x02    /*!<    */
#define HAL_NRF_ENAA_P2         0x04    /*!<    */
#define HAL_NRF_ENAA_P3         0x08    /*!<    */
#define HAL_NRF_ENAA_P4         0x10    /*!<    */
#define HAL_NRF_ENAA_P5         0x20    /*!<    */
#define HAL_NRF_ENAA_MARK       0x3F

/**
* @brief  NRF24L01 handle Structure definition
*/
typedef struct {
    SPI_HandleTypeDef           *Hspi;          /* SPI 接口*/
    uint8_t                     *RxAddress[6];  /*!<    */
    uint8_t                     *TxAddress;     /*!<    */
    uint8_t                     AutoAckPe;      /*!<    */
    uint8_t                     Channel;        /*!<    */
    uint8_t                     Flag;           /*!<    */
    uint8_t                     ARTCount;       /*!<    */
    uint8_t                     ARTdelay;       /*!<    */
    uint8_t                     DataRates;      /*!<    */
    uint8_t                     RFPA;           /*!<    */
    uint8_t                     *pTxBuffPtr;    /*!<    */
    uint8_t                     *pRxBuffPtr;    /*!< Pointer to NRF24L01 Rx transfer Buffer */
    uint16_t                    TxSize;         /*!< NRF24L01 Tx Transfer size   */
    uint16_t                    TxHead;         /*!< NRF24L01 数据头指针          */
    uint16_t                    TxTail;         /*!< 数据尾指针     */
    uint16_t                    RxSize;         /*!<    */
    uint16_t                    RxHead;         /*!<    */
    uint16_t                    RxTail;         /*!<    */
    HAL_LockTypeDef             Lock;           /*!<    */
    __IO HAL_NRF_StateTypeDef   State;          /*!< NRF24L01 communication state   */
    uint16_t                    FifoStates;     /*!<    */
    uint16_t                    Error;          /*!<    */
} NRF24_HandleTypeDef;

//**************************************************************************************
//变量
extern NRF24_HandleTypeDef hnrf;
//extern    uint8_t NRF24L01_FLAG;              //NRF24L01当前状态标志
extern  uint8_t TX_ADDRESS[NRF24L01_ADR_LEN];   //本地地址
extern  uint8_t RX_ADDRESS[NRF24L01_ADR_LEN];   //接收地址
extern  uint8_t DYNPD_LEN;                      //用于存放动态数据包长度

//**************************************************************************************
//NRF SPI底层功能函数
uint8_t NRF24L01_Write_Reg(uint8_t reg, uint8_t value);              // 写NRF24L01寄存器，返回状态值
uint8_t NRF24L01_Read_Reg(uint8_t reg);                              // 读NRF24L01寄存器，返回寄存器值
void   NRF24L01_Read_Buf(uint8_t reg, uint8_t *pBuf, uint8_t len);   // 读取缓冲区数据
uint8_t NRF24L01_Write_Buf(uint8_t reg, uint8_t *pBuf, uint8_t len); // 向缓冲区写入数据


void NRF24L_Init(void);      // 参数初始化
void NRF24_MspInit(void);       // NRF24L01初始化函数
uint8_t NRF24L01_Check(void);   // NRF24L01 SPI通信检查
void NRF24L01_SleepMode(void);  // 休眠模式


void NRF24L01_TxMode1(void);     // 发送模式
void NRF24L01_RxMode1(void);     // 接收模式
uint8_t NRF24L01_TxPacket1(uint8_t *TxData, uint8_t len); // 发送一个32个字节的数据包
uint8_t NRF24L01_RxPacket1(uint8_t *RxData);
void NRF24L01_TxPacket1_AP(uint8_t *txbuf, uint8_t len);

void NRF24L01_RxMode2(void);
void NRF24L01_TxMode2(void); 
uint8_t NRF24L01_RxPacket2(uint8_t *RxData);
uint8_t NRF24L01_TxPacket2(uint8_t *txbuf, uint8_t len);
uint8_t NRF24L01_TxPacket2_AP(uint8_t *txbuf, uint8_t len);

#define  NRF24L01_Mode     0    // 自动收发模式
#if (NRF24L01_Mode == 1)
#define NRF24L01_RxMode      NRF24L01_RxMode2
#define NRF24L01_TxMode      NRF24L01_TxMode2
#define NRF24L01_RxPacket    NRF24L01_RxPacket2
#define NRF24L01_TxPacket    NRF24L01_TxPacket2
#define NRF24L01_TxPacket_AP NRF24L01_TxPacket2_AP
#else
#define NRF24L01_RxMode      NRF24L01_RxMode1
#define NRF24L01_TxMode      NRF24L01_TxMode1
#define NRF24L01_RxPacket    NRF24L01_RxPacket1
#define NRF24L01_TxPacket    NRF24L01_TxPacket1
#define NRF24L01_TxPacket_AP NRF24L01_TxPacket1_AP
#endif


void NRF24L01_SetChange(uint8_t ch); //频道
uint8_t NRF24L01_GetChange(void);

void NRF24L01_SetTxAddress(uint8_t *addr);  //TX地址
void NRF24L01_GetTxAddress(uint8_t *addr);

void NRF24L01_SetRxAddress(uint8_t *addr, uint8_t channel); //RX地址
void NRF24L01_GetRxAddress(uint8_t *addr, uint8_t channel);

void NRF24L01_Handler(void);    //NRF24L01中断处理


void NRF24L01_RecvFifo(void);   // 读取接收数据,保存到FIFO
void NRF24L01_SendFifo(void);   // 等待发送缓存数据
uint16_t NRF24L01_GetReceive(uint8_t *rxData);

void NRF24L01_Send_Packet(uint8_t * tx_buf, uint8_t len);

void printnrfreg(void);
uint16_t NRF24L01_Status(void);
#endif
