#ifndef __NRF24L01_PLUS_H_
#define __NRF24L01_PLUS_H_

#include "main.h"

#define NRF24L01_Plus_Role 1    //0������  1������

//NRF24L01�Ĵ�����������
#define NRF24L01_Plus_NRF_READ_REG    0x00  //�����üĴ���,��5λΪ�Ĵ�����ַ
#define NRF24L01_Plus_NRF_WRITE_REG   0x20  //д���üĴ���,��5λΪ�Ĵ�����ַ
#define NRF24L01_Plus_RD_RX_PLOAD     0x61  //��RX��Ч����,1~32�ֽ�
#define NRF24L01_Plus_WR_TX_PLOAD     0xA0  //дTX��Ч����,1~32�ֽ�
#define NRF24L01_Plus_ACK_PAYLOAD_CMD 0xa8  //�����ڽ��շ���ͨ��PIPE PPP������ͨ��ACK��ʽ���ͣ����������֡���ݴ�����FIFO
#define NRF24L01_Plus_FLUSH_TX        0xE1  //���TX FIFO�Ĵ���.����ģʽ����
#define NRF24L01_Plus_FLUSH_RX        0xE2  //���RX FIFO�Ĵ���.����ģʽ����
#define NRF24L01_Plus_REUSE_TX_PL     0xE3  //����ʹ����һ������,CEΪ��,���ݰ������Ϸ���.
#define NRF24L01_Plus_NOP             0xFF  //�ղ���,����������״̬�Ĵ���  
//SPI(NRF24L01)�Ĵ�����ַ
#define NRF24L01_Plus_CONFIG          0x00  //���üĴ�����ַ;bit0:1����ģʽ,0����ģʽ;bit1:��ѡ��;bit2:CRCģʽ;bit3:CRCʹ��;
//bit4:�ж�MAX_RT(�ﵽ����ط������ж�)ʹ��;bit5:�ж�TX_DSʹ��;bit6:�ж�RX_DRʹ��
#define NRF24L01_Plus_EN_AA           0x01  //ʹ���Զ�Ӧ����  bit0~5,��Ӧͨ��0~5
#define NRF24L01_Plus_EN_RXADDR       0x02  //���յ�ַ����,bit0~5,��Ӧͨ��0~5
#define NRF24L01_Plus_SETUP_AW        0x03  //���õ�ַ���(��������ͨ��):bit1,0:00,3�ֽ�;01,4�ֽ�;02,5�ֽ�;
#define NRF24L01_Plus_SETUP_RETR      0x04  //�����Զ��ط�;bit3:0,�Զ��ط�������;bit7:4,�Զ��ط���ʱ 250*x+86us
#define NRF24L01_Plus_RF_CH           0x05  //RFͨ��,bit6:0,����ͨ��Ƶ��;
#define NRF24L01_Plus_RF_SETUP        0x06  //RF�Ĵ���;bit3:��������(0:1Mbps,1:2Mbps);bit2:1,���书��;bit0:�������Ŵ�������
#define NRF24L01_Plus_STATUS          0x07  //״̬�Ĵ���;bit0:TX FIFO����־;bit3:1,��������ͨ����(���:6);bit4,�ﵽ�����ط�
//bit5:���ݷ�������ж�;bit6:���������ж�;
#define NRF24L01_Plus_MAX_TX          0x10  //�ﵽ����ʹ����ж�
#define NRF24L01_Plus_TX_OK           0x20  //TX��������ж�
#define NRF24L01_Plus_RX_OK           0x40  //���յ������ж�

#define NRF24L01_Plus_R_RX_PL_WID     0x60

#define NRF24L01_Plus_OBSERVE_TX      0x08  //���ͼ��Ĵ���,bit7:4,���ݰ���ʧ������;bit3:0,�ط�������
#define NRF24L01_Plus_CD              0x09  //�ز����Ĵ���,bit0,�ز����;
#define NRF24L01_Plus_RX_ADDR_P0      0x0A  //����ͨ��0���յ�ַ,��󳤶�5���ֽ�,���ֽ���ǰ
#define NRF24L01_Plus_RX_ADDR_P1      0x0B  //����ͨ��1���յ�ַ,��󳤶�5���ֽ�,���ֽ���ǰ
#define NRF24L01_Plus_RX_ADDR_P2      0x0C  //����ͨ��2���յ�ַ,����ֽڿ�����,���ֽ�,����ͬRX_ADDR_P1[39:8]���;
#define NRF24L01_Plus_RX_ADDR_P3      0x0D  //����ͨ��3���յ�ַ,����ֽڿ�����,���ֽ�,����ͬRX_ADDR_P1[39:8]���;
#define NRF24L01_Plus_RX_ADDR_P4      0x0E  //����ͨ��4���յ�ַ,����ֽڿ�����,���ֽ�,����ͬRX_ADDR_P1[39:8]���;
#define NRF24L01_Plus_RX_ADDR_P5      0x0F  //����ͨ��5���յ�ַ,����ֽڿ�����,���ֽ�,����ͬRX_ADDR_P1[39:8]���;
#define NRF24L01_Plus_TX_ADDR         0x10  //���͵�ַ(���ֽ���ǰ),ShockBurstTMģʽ��,RX_ADDR_P0��˵�ַ���
#define NRF24L01_Plus_RX_PW_P0        0x11  //��������ͨ��0��Ч���ݿ��(1~32�ֽ�),����Ϊ0��Ƿ�
#define NRF24L01_Plus_RX_PW_P1        0x12  //��������ͨ��1��Ч���ݿ��(1~32�ֽ�),����Ϊ0��Ƿ�
#define NRF24L01_Plus_RX_PW_P2        0x13  //��������ͨ��2��Ч���ݿ��(1~32�ֽ�),����Ϊ0��Ƿ�
#define NRF24L01_Plus_RX_PW_P3        0x14  //��������ͨ��3��Ч���ݿ��(1~32�ֽ�),����Ϊ0��Ƿ�
#define NRF24L01_Plus_RX_PW_P4        0x15  //��������ͨ��4��Ч���ݿ��(1~32�ֽ�),����Ϊ0��Ƿ�
#define NRF24L01_Plus_RX_PW_P5        0x16  //��������ͨ��5��Ч���ݿ��(1~32�ֽ�),����Ϊ0��Ƿ�
#define NRF24L01_Plus_NRF_FIFO_STATUS 0x17  //FIFO״̬�Ĵ���;bit0,RX FIFO�Ĵ����ձ�־;bit1,RX FIFO����־;bit2,3,����

#define NRF24L01_Plus_DYNPD           0x1C   // ���ö�̬���ݳ���
#define NRF24L01_Plus_FEATURE         0x1D   // �����Ĵ���
//bit4,TX FIFO�ձ�־;bit5,TX FIFO����־;bit6,1,ѭ��������һ���ݰ�.0,��ѭ��;

//����24L01��IO�ڣ�����׼SPI�ӿ����ţ�
#define NRF24L01_Plus_CE_LOW   HAL_GPIO_WritePin(NRF24L01_CE_GPIO_Port, NRF24L01_CE_Pin, GPIO_PIN_RESET) //24L01ʹ���ź�
#define NRF24L01_Plus_CE_HIGH  HAL_GPIO_WritePin(NRF24L01_CE_GPIO_Port, NRF24L01_CE_Pin, GPIO_PIN_SET)
#define NRF24L01_Plus_CSN_LOW  HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_RESET)         //SPIƬѡ�ź�
#define NRF24L01_Plus_CSN_HIGH HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_SET)
#define NRF24L01_Plus_IRQ_READ HAL_GPIO_ReadPin(NRF24L01_IRQ_GPIO_Port, NRF24L01_IRQ_Pin)                //IRQ������������
//24L01���ͽ������ݿ�ȶ���
#define NRF24L01_Plus_TX_ADR_WIDTH    5       //5�ֽڵĵ�ַ���
#define NRF24L01_Plus_RX_ADR_WIDTH    5       //5�ֽڵĵ�ַ���
#define NRF24L01_Plus_TX_PLOAD_WIDTH  32      //32�ֽڵ��û����ݿ��
#define NRF24L01_Plus_RX_PLOAD_WIDTH  32      //32�ֽڵ��û����ݿ��

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

void    NRF24L01_Plus_Init(void);//��ʼ��
uint8_t NRF24L01_Plus_Write_Buf(uint8_t reg, uint8_t *pBuf, uint8_t u8s);//д������
uint8_t NRF24L01_Plus_Read_Buf(uint8_t reg, uint8_t *pBuf, uint8_t u8s);//��������
uint8_t NRF24L01_Plus_Read_Reg(uint8_t reg);           //���Ĵ���
uint8_t NRF24L01_Plus_Write_Reg(uint8_t reg, uint8_t value);//д�Ĵ���
uint8_t NRF24L01_Plus_Check(void);//���24L01�Ƿ����

void    NRF24L01_Plus_Mode(uint8_t model);
void    NRF24L01_Plus_Check_Event(void);
uint8_t NRF24L01_Plus_RxPacket(uint8_t *RxData);
void    NRF24L01_Plus_TxPacket_AP(uint8_t *txbuf, uint8_t len);
uint8_t NRF24L01_Plus_TxPacket(uint8_t *txbuf, uint8_t len); //����һ����������
#endif
