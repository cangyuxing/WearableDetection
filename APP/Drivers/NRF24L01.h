/****************************************
NRF24L01�⺯��

P0 RX ����ģʽ�µĽ��ն˵�ַ
P1 RX ��ַ ������ַ
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

#define BK2423  0                       // ��ʹ��BK2423ʹ��,������0
#define HAL_SPI 1                       // ��ʹ��ST�ٷ�SPI����������1
#define NRF_RXFIFO_MAX  256             // �����С
#define NRF_TXFIFO_MAX  NRF_RXFIFO_MAX  // �����С

//**************************************************************************************
//NRF24L01���Ŷ���
//24L01������
#define _BSP_NRF24_CE_LOW       HAL_GPIO_WritePin(NRF24L01_CE_GPIO_Port, NRF24L01_CE_Pin, GPIO_PIN_RESET)   //24L01ʹ���ź�
#define _BSP_NRF24_CE_HIGH      HAL_GPIO_WritePin(NRF24L01_CE_GPIO_Port, NRF24L01_CE_Pin, GPIO_PIN_SET)

#define _BSP_NRF24_CSN_LOW      HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_RESET) //SPIƬѡ�ź�
#define _BSP_NRF24_CSN_HIGH     HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_SET)

#define _BSP_NRF24_IRQ_READ     HAL_GPIO_ReadPin(NRF24L01_IRQ_GPIO_Port, NRF24L01_IRQ_Pin)       //IRQ������������

//********************************    NRF24L01   *************************************
#define NRF24L01_ADR_LEN        (5)     //NRF24L01�շ���ַ���
#define NRF24L01_TX_PLOAD_LEN   (32)    //uints TX payload
#define NRF24L01_RX_PLOAD_LEN   (32)    //uints RX payload

// ���� ���
#define NRF24L01_AUTO_SlEEP     (1<<3)  // �Զ�����
#define NRF24L01_AUTO_ACK       (1<<2)  // �Զ��ظ� Enhanced ShockBurst
#define NRF24L01_AUTO_RX        (1<<1)  // ��������Զ�ת�����ģʽ

//**************************************************************************************
//NRF24L01�Ĵ���ָ��
#define ACTIVATE_CMD             0x50   // д��0x73����R_RX_PL_WID,W_ACK_PAYLOAD,W_TX_PAYLOAD_NOACK�Ĵ���,�ڴ�д�����.ֻ����ʡ��ģʽ�͵���ģʽִ��
#define W_TX_PAYLOAD_NOACK_CMD   0xb0   // Ӧ���ڷ���ģʽ������Ӧ����Ҫ��EN_DYN_ACK��1��Ч
#define W_ACK_PAYLOAD_CMD        0xa8   // �����ڽ��շ���ͨ��PIPE PPP������ͨ��ACK��ʽ���ͣ����������֡���ݴ�����FIFO
#define R_RX_PL_WID_CMD          0x60   // �����յ��Ķ�̬���ݰ����ȣ�����32���RX_FIFO��
#define NRF24L01_READ_REG        0x00   // ���Ĵ���ָ��
#define NRF24L01_WRITE_REG       0x20   // д�Ĵ���ָ��
#define NRF24L01_RD_RX_PLOAD     0x61   // ��ȡ��������ָ��
#define NRF24L01_WR_TX_PLOAD     0xA0   // д��������ָ��
#define NRF24L01_FLUSE_TX        0xE1   // ���TX FIFO�е����ݣ�Ӧ���뷢��ģʽ��
#define NRF24L01_FLUSE_RX        0xE2   // ���RX FIFO�е����ݣ�Ӧ�������ģʽ��
#define NRF24L01_REUSE_TX_PL     0xE3   // �����ظ�װ������ָ��
#define NRF24L01_NOP             0xFF   // �޲��������ڶ�ȡSTATUS�Ĵ���

//**************************************************************************************
// (nRF24L01)�Ĵ�����ַ
#define NRF24L01_CONFIG          0x00   // �����շ�״̬��CRCУ��ģʽ�Լ��շ�״̬��Ӧ��ʽ
#define NRF24L01_EN_AA           0x01   // �Զ�Ӧ��������
#define NRF24L01_EN_RXADDR       0x02   // �����ŵ�����
#define NRF24L01_SETUP_AW        0x03   // �շ���ַ�������
#define NRF24L01_SETUP_RETR      0x04   // �Զ��ط���������
#define NRF24L01_RF_CH           0x05   // ����Ƶ������
#define NRF24L01_RF_SETUP        0x06   // �������ʡ����Ĺ�������
#define NRF24L01_STATUS          0x07   // ״̬�Ĵ���
#define NRF24L01_OBSERVE_TX      0x08   // ���ͼ�⹦��
#define NRF24L01_CD              0x09   // ��ַ��� ���Լ���ŵ��Ƿ�ռ��
#define NRF24L01_RX_ADDR_P0      0x0A   // Ƶ��0�������ݵ�ַ
#define NRF24L01_RX_ADDR_P1      0x0B   // Ƶ��1�������ݵ�ַ
#define NRF24L01_RX_ADDR_P2      0x0C   // Ƶ��2�������ݵ�ַ
#define NRF24L01_RX_ADDR_P3      0x0D   // Ƶ��3�������ݵ�ַ
#define NRF24L01_RX_ADDR_P4      0x0E   // Ƶ��4�������ݵ�ַ
#define NRF24L01_RX_ADDR_P5      0x0F   // Ƶ��5�������ݵ�ַ
#define NRF24L01_TX_ADDR         0x10   // ���͵�ַ�Ĵ���
#define NRF24L01_RX_PW_P0        0x11   // ����Ƶ��0�������ݳ���
#define NRF24L01_RX_PW_P1        0x12   // ����Ƶ��1�������ݳ���
#define NRF24L01_RX_PW_P2        0x13   // ����Ƶ��2�������ݳ���
#define NRF24L01_RX_PW_P3        0x14   // ����Ƶ��3�������ݳ���
#define NRF24L01_RX_PW_P4        0x15   // ����Ƶ��4�������ݳ���
#define NRF24L01_RX_PW_P5        0x16   // ����Ƶ��5�������ݳ���
#define NRF24L01_FIFO_STATUS     0x17   // FIFOջ��ջ��״̬�Ĵ�������
#define NRF24L01_DYNPD           0x1C   // ���ö�̬���ݳ���
#define NRF24L01_FEATURE         0x1D   // �����Ĵ���

//**************************************************************************************
//#define RX_OK                    0x40  //���յ������ж�
//#define TX_OK                    0x20  //TX��������ж�
//#define MAX_TX                   0x10  //�ﵽ����ʹ����ж�
//NRF24L01״̬�Ĵ���ֵ
#define NRF24L01_STATUS_RX_DR    0x40   // ���յ������жϱ�־λ
#define NRF24L01_STATUS_TX_DS    0x20   // ��������жϱ�־λ
#define NRF24L01_STATUS_MAX_RT   0x10   // �ﵽ����ط��жϱ�־λ
#define NRF24L01_STATUS_RX_MARK  0x0E   // RX ����ͨ�������� 111 ��
#define NRF24L01_STATUS_TX_FULL  0x01   // TX FIFO�Ĵ�������־ 1 Ϊ ����0Ϊ ����

//**************************************************************************************
//NRF24L01���üĴ���ֵ
#define NRF24L01_CONFIG_RX_DR    0x40   // ���ν����ж�
#define NRF24L01_CONFIG_TX_DS    0x20   // ���η����ж�
#define NRF24L01_CONFIG_MAX_RT   0x10   // �����ط����ж�
#define NRF24L01_CONFIG_EN_CRC   0x08   // CRC ʹ��
#define NRF24L01_CONFIG_CRCO     0x04   // CRC ģʽ
#define NRF24L01_CONFIG_PWR_UP   0x02   // 1 �ϵ�    0 ����
#define NRF24L01_CONFIG_PRIM_RX  0x01   // 1 RXģʽ  0 TXģʽ

// TXģʽ config����
#define NRF24L01_CONFIG_TX        ( NRF24L01_CONFIG_EN_CRC | NRF24L01_CONFIG_CRCO | NRF24L01_CONFIG_PWR_UP  )
// RXģʽ config����
#define NRF24L01_CONFIG_RX        ( NRF24L01_CONFIG_EN_CRC | NRF24L01_CONFIG_CRCO | NRF24L01_CONFIG_PWR_UP | NRF24L01_CONFIG_PRIM_RX)
// PowerDownģʽ config����
#define NRF24L01_CONFIG_SLEEP     ( NRF24L01_CONFIG_EN_CRC | NRF24L01_CONFIG_CRCO )

// TXģʽ config����
#define NRF24L01_CONFIG_TX2       ( NRF24L01_CONFIG_TX_DS  | NRF24L01_CONFIG_EN_CRC | NRF24L01_CONFIG_CRCO | NRF24L01_CONFIG_PWR_UP  )
// RXģʽ config����
#define NRF24L01_CONFIG_RX2       ( NRF24L01_CONFIG_TX_DS  | NRF24L01_CONFIG_MAX_RT |NRF24L01_CONFIG_EN_CRC | NRF24L01_CONFIG_CRCO | NRF24L01_CONFIG_PWR_UP | NRF24L01_CONFIG_PRIM_RX)
//**************************************************************************************
//�ṹ�� ö��
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

// ��������     HAL_NRF_DataRates_TypeDef
#define HAL_NRF_DR_1M           0x00    /*!<    */
#define HAL_NRF_DR_2M           0x08    /*!<    */
#define HAL_NRF_DR_250K         0x20    /*!<    */
#define HAL_NRF_DR_MARK         0x28    /*!<    */

// ���书��     HAL_NRF_PA_TypeDef
#define HAL_NRF_PA_0DBM         0x06    /*!<    */
#define HAL_NRF_PA_6DBM         0x04    /*!<    */
#define HAL_NRF_PA_12DBM        0x02    /*!<    */
#define HAL_NRF_PA_18DBM        0x00    /*!<    */
#define HAL_NRF_PA_MARK         0x06    /*!<    */

// ��ַ��� HAL_NRF_ADW_TypeDef
#define HAL_NRF_ADW_3           0x01    /*!<    */
#define HAL_NRF_ADW_4           0x02    /*!<    */
#define HAL_NRF_ADW_5           0x03    /*!<    */
#define HAL_NRF_ADW_MARK        0x03    /*!<    */

// ����ͨ�� HAL_NRF_ENAA_TypeDef;
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
    SPI_HandleTypeDef           *Hspi;          /* SPI �ӿ�*/
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
    uint16_t                    TxHead;         /*!< NRF24L01 ����ͷָ��          */
    uint16_t                    TxTail;         /*!< ����βָ��     */
    uint16_t                    RxSize;         /*!<    */
    uint16_t                    RxHead;         /*!<    */
    uint16_t                    RxTail;         /*!<    */
    HAL_LockTypeDef             Lock;           /*!<    */
    __IO HAL_NRF_StateTypeDef   State;          /*!< NRF24L01 communication state   */
    uint16_t                    FifoStates;     /*!<    */
    uint16_t                    Error;          /*!<    */
} NRF24_HandleTypeDef;

//**************************************************************************************
//����
extern NRF24_HandleTypeDef hnrf;
//extern    uint8_t NRF24L01_FLAG;              //NRF24L01��ǰ״̬��־
extern  uint8_t TX_ADDRESS[NRF24L01_ADR_LEN];   //���ص�ַ
extern  uint8_t RX_ADDRESS[NRF24L01_ADR_LEN];   //���յ�ַ
extern  uint8_t DYNPD_LEN;                      //���ڴ�Ŷ�̬���ݰ�����

//**************************************************************************************
//NRF SPI�ײ㹦�ܺ���
uint8_t NRF24L01_Write_Reg(uint8_t reg, uint8_t value);              // дNRF24L01�Ĵ���������״ֵ̬
uint8_t NRF24L01_Read_Reg(uint8_t reg);                              // ��NRF24L01�Ĵ��������ؼĴ���ֵ
void   NRF24L01_Read_Buf(uint8_t reg, uint8_t *pBuf, uint8_t len);   // ��ȡ����������
uint8_t NRF24L01_Write_Buf(uint8_t reg, uint8_t *pBuf, uint8_t len); // �򻺳���д������


void NRF24L_Init(void);      // ������ʼ��
void NRF24_MspInit(void);       // NRF24L01��ʼ������
uint8_t NRF24L01_Check(void);   // NRF24L01 SPIͨ�ż��
void NRF24L01_SleepMode(void);  // ����ģʽ


void NRF24L01_TxMode1(void);     // ����ģʽ
void NRF24L01_RxMode1(void);     // ����ģʽ
uint8_t NRF24L01_TxPacket1(uint8_t *TxData, uint8_t len); // ����һ��32���ֽڵ����ݰ�
uint8_t NRF24L01_RxPacket1(uint8_t *RxData);
void NRF24L01_TxPacket1_AP(uint8_t *txbuf, uint8_t len);

void NRF24L01_RxMode2(void);
void NRF24L01_TxMode2(void); 
uint8_t NRF24L01_RxPacket2(uint8_t *RxData);
uint8_t NRF24L01_TxPacket2(uint8_t *txbuf, uint8_t len);
uint8_t NRF24L01_TxPacket2_AP(uint8_t *txbuf, uint8_t len);

#define  NRF24L01_Mode     0    // �Զ��շ�ģʽ
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


void NRF24L01_SetChange(uint8_t ch); //Ƶ��
uint8_t NRF24L01_GetChange(void);

void NRF24L01_SetTxAddress(uint8_t *addr);  //TX��ַ
void NRF24L01_GetTxAddress(uint8_t *addr);

void NRF24L01_SetRxAddress(uint8_t *addr, uint8_t channel); //RX��ַ
void NRF24L01_GetRxAddress(uint8_t *addr, uint8_t channel);

void NRF24L01_Handler(void);    //NRF24L01�жϴ���


void NRF24L01_RecvFifo(void);   // ��ȡ��������,���浽FIFO
void NRF24L01_SendFifo(void);   // �ȴ����ͻ�������
uint16_t NRF24L01_GetReceive(uint8_t *rxData);

void NRF24L01_Send_Packet(uint8_t * tx_buf, uint8_t len);

void printnrfreg(void);
uint16_t NRF24L01_Status(void);
#endif
