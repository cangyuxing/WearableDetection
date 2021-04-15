#include "nrf24l01plus.h"

uint8_t NRF24L01_Plus_TX_ADDRESS[NRF24L01_Plus_TX_ADR_WIDTH] = {190, 168, 1, 1, 0};
uint8_t NRF24L01_Plus_RX_ADDRESS[NRF24L01_Plus_RX_ADR_WIDTH] = {190, 168, 1, 1, 0};

uint8_t NRF24L01_Plus_Rx_Len = 0;
uint8_t NRF24L01_Plus_Rx_BUF[33];
uint8_t NRF24L01_Plus_Tx_BUF[33];
uint8_t NRF24L01_Plus_Rx_flag = 0;
//��ʼ��24L01��IO��

void NRF24L01_Plus_Init(void) {
    ldo3_enable();
    HAL_Delay(50);
    NRF24L01_Plus_CE_LOW;   //ʹ��24L01
    NRF24L01_Plus_CSN_HIGH; //SPIƬѡȡ��
    if(NRF24L01_Plus_Check() == 0) {
        MYLOG(ERROR, "NRF24L01 Fail!");
    }
    MYLOG(INFO, "NRF24L01 OK!");
}
//���24L01�Ƿ����
//����ֵ:0���ɹ�;1��ʧ��
uint8_t NRF24L01_Plus_Check(void) {
    uint8_t buf[5] = {190, 168, 1, 1, 0};
    uint8_t i;
    NRF24L01_Plus_Write_Buf(NRF24L01_Plus_NRF_WRITE_REG + NRF24L01_Plus_TX_ADDR, buf, 5); //д��5���ֽڵĵ�ַ.
    NRF24L01_Plus_Read_Buf(NRF24L01_Plus_TX_ADDR, buf, 5); //����д��ĵ�ַ
    for(i = 0; i < 5; i++)if(buf[i] != 0XA5)break;
    if(i != 5)return 0; //���24L01����
    return 1;           //��⵽24L01
}
/**********************************************************************************/
/*                                    Ӳ��SPIͨѶ                                 */
/**********************************************************************************/
/**
* @brief  дNRF24L01�Ĵ���
* @param  reg: �Ĵ�����ַ ������
* @param  value: д������
* @retval ����״ֵ̬
*/
uint8_t NRF24L01_Plus_Write_Reg(uint8_t reg, uint8_t value) {
    uint8_t txdata[2], rxdata[2];
    txdata[0] = reg;
    txdata[1] = value;
    NRF24L01_Plus_CSN_LOW;                 //ʹ��SPI����
    HAL_SPI_TransmitReceive(&hspi1, txdata, rxdata, 2, 300); //��ȡ�Ĵ�������
    NRF24L01_Plus_CSN_HIGH;                 //��ֹSPI����
    return(rxdata[0]);                  //����״ֵ̬
}

/**
* @brief  ��NRF24L01�Ĵ���
* @param  reg: �Ĵ�����ַ
* @retval ���ؼĴ���ֵ
*/
uint8_t NRF24L01_Plus_Read_Reg(uint8_t reg) {
    uint8_t txdata[2] = {0}, rxdata[2] = {0};
    txdata[0] = reg;
    txdata[1] = 0;
    NRF24L01_Plus_CSN_LOW;           //ʹ��SPI����
    HAL_SPI_TransmitReceive(&hspi1, txdata, rxdata, 2, 300); //��ȡ�Ĵ�������
    NRF24L01_Plus_CSN_HIGH;          //��ֹSPI����
    return(rxdata[1]);               //����
}

/**
* @brief  ��ȡ����������
* @param  reg: �Ĵ�����ַ
* @param  pBuf: �������ݻ���ָ��
* @param  len: ��ȡ����
* @retval None
*/
uint8_t NRF24L01_Plus_Read_Buf(uint8_t reg, uint8_t *pBuf, uint8_t len) {
    len++;  // ����һ��״̬λ��
    uint8_t txdata[34] = {0}, rxdata[34] = {0};
    txdata[0] = reg;
    NRF24L01_Plus_CSN_LOW;           //ʹ��SPI����
    HAL_SPI_TransmitReceive(&hspi1, txdata, rxdata, len, 600); //��ȡ�Ĵ�������
    NRF24L01_Plus_CSN_HIGH;          //��ֹSPI����
    memcpy(pBuf, rxdata + 1, len - 1);
    return(rxdata[0]);               //���ض�����״ֵ̬
}

/**
* @brief  �򻺳���д������
* @param  pBuf: д�����ݻ���ָ��
* @param  len: д�볤��
* @retval ���ض�����״ֵ̬
*/
uint8_t NRF24L01_Plus_Write_Buf(uint8_t reg, uint8_t *pBuf, uint8_t len) {
    uint8_t txdata[34] = {0}, rxdata[34] = {0};
    txdata[0] = reg;
    memcpy(txdata + 1, pBuf, len);
    len++;
    NRF24L01_Plus_CSN_LOW;          //ʹ��SPI����
    HAL_SPI_TransmitReceive(&hspi1, txdata, rxdata, len, 600);
    NRF24L01_Plus_CSN_HIGH;       //�ر�SPI����
    return(rxdata[0]);         //���ض�����״ֵ̬
}

//����NRF24L01����һ������
//txbuf:�����������׵�ַ
//����ֵ:0��������ɣ��������������
void NRF24L01_Plus_Mode(uint8_t model) {
    uint8_t txdata, rxdata ;
    NRF24L01_Plus_CE_LOW;
    NRF24L01_Plus_Write_Buf(NRF24L01_Plus_NRF_WRITE_REG + NRF24L01_Plus_TX_ADDR   , (uint8_t*)NRF24L01_Plus_TX_ADDRESS, NRF24L01_Plus_TX_ADR_WIDTH); //дTX�ڵ��ַ
    NRF24L01_Plus_Write_Buf(NRF24L01_Plus_NRF_WRITE_REG + NRF24L01_Plus_RX_ADDR_P0, (uint8_t*)NRF24L01_Plus_RX_ADDRESS, NRF24L01_Plus_RX_ADR_WIDTH); //����TX�ڵ��ַ,��ҪΪ��ʹ��ACK
    NRF24L01_Plus_Write_Reg(NRF24L01_Plus_NRF_WRITE_REG + NRF24L01_Plus_EN_AA,      0x01); //ʹ��ͨ��0���Զ�Ӧ�� 00��ʾ��Ӧ��
    NRF24L01_Plus_Write_Reg(NRF24L01_Plus_NRF_WRITE_REG + NRF24L01_Plus_EN_RXADDR,  0x01); //ʹ��ͨ��0�Ľ��յ�ַ Ϊ0��������
    NRF24L01_Plus_Write_Reg(NRF24L01_Plus_NRF_WRITE_REG + NRF24L01_Plus_SETUP_RETR, 0x1a); //�����Զ��ط����ʱ��:500us + 86us;����Զ��ط�����:10�� ���ط�
    NRF24L01_Plus_Write_Reg(NRF24L01_Plus_NRF_WRITE_REG + NRF24L01_Plus_RF_CH,      40);   //40����RFͨ��Ϊ40�����ŵ�����Ϊ2.5GHZ���շ�����һ��
    NRF24L01_Plus_Write_Reg(NRF24L01_Plus_NRF_WRITE_REG + NRF24L01_Plus_RF_SETUP,   0x0f); //����TX�������,0db����,2Mbps,���������濪��
    if(model == NRF24L01_Plus_MODEL_RX1) {        //RX
        NRF24L01_Plus_Write_Reg(NRF24L01_Plus_NRF_WRITE_REG + NRF24L01_Plus_RX_PW_P0, NRF24L01_Plus_RX_PLOAD_WIDTH); //ѡ��ͨ��0����Ч���ݿ��
        NRF24L01_Plus_Write_Reg(NRF24L01_Plus_NRF_WRITE_REG + NRF24L01_Plus_CONFIG, 0x0f);                           //IRQ�շ�����жϿ���,16λCRC,������
    } else if(model == NRF24L01_Plus_MODEL_TX1) { //TX
        NRF24L01_Plus_Write_Reg(NRF24L01_Plus_NRF_WRITE_REG + NRF24L01_Plus_RX_PW_P0, NRF24L01_Plus_RX_PLOAD_WIDTH); //ѡ��ͨ��0����Ч���ݿ��
        NRF24L01_Plus_Write_Reg(NRF24L01_Plus_NRF_WRITE_REG + NRF24L01_Plus_CONFIG, 0x0e);                           //IRQ�շ�����жϿ���,16λCRC,������
    } else if(model == NRF24L01_Plus_MODEL_RX2) { //RX2
        NRF24L01_Plus_Write_Reg(NRF24L01_Plus_FLUSH_TX, 0xff);
        NRF24L01_Plus_Write_Reg(NRF24L01_Plus_FLUSH_RX, 0xff);
        NRF24L01_Plus_Write_Reg(NRF24L01_Plus_NRF_WRITE_REG + NRF24L01_Plus_CONFIG, 0x3f);                           //0f // IRQ�շ�����жϿ���,16λCRC,������0x0f
        //��λΪ3��ʾ���η����ж�
        NRF24L01_Plus_CSN_LOW;
        txdata = 0x50;
        HAL_SPI_TransmitReceive(&hspi1, &txdata, &rxdata, 1, 300); //��ȡ�Ĵ�������
        txdata = 0x73;
        HAL_SPI_TransmitReceive(&hspi1, &txdata, &rxdata, 1, 300); //��ȡ�Ĵ�������
        NRF24L01_Plus_CSN_HIGH;
        NRF24L01_Plus_Write_Reg(NRF24L01_Plus_NRF_WRITE_REG + NRF24L01_Plus_DYNPD, 0x01);
        NRF24L01_Plus_Write_Reg(NRF24L01_Plus_NRF_WRITE_REG + NRF24L01_Plus_FEATURE, 0x07);                  //06
    } else {                            //TX2
        NRF24L01_Plus_Write_Reg(NRF24L01_Plus_NRF_WRITE_REG + NRF24L01_Plus_CONFIG, 0x2e);  //0e // IRQ�շ�����жϿ���,16λCRC,������0x0e
        NRF24L01_Plus_Write_Reg(NRF24L01_Plus_FLUSH_TX, 0xff);                              //��λbit4MAX_RT bit5 Ϊ1����TX_DS bit6 RX_DS
        NRF24L01_Plus_Write_Reg(NRF24L01_Plus_FLUSH_RX, 0xff);                              //���ڷɿأ�����ģʽ2e���Ǻ��õ�
        NRF24L01_Plus_CSN_LOW;
        txdata = 0x50;
        HAL_SPI_TransmitReceive(&hspi1, &txdata, &rxdata, 1, 300); //��ȡ�Ĵ�������
        txdata = 0x73;
        HAL_SPI_TransmitReceive(&hspi1, &txdata, &rxdata, 1, 300); //��ȡ�Ĵ�������
        NRF24L01_Plus_CSN_HIGH;
        NRF24L01_Plus_Write_Reg(NRF24L01_Plus_NRF_WRITE_REG + NRF24L01_Plus_DYNPD, 0x01);
        NRF24L01_Plus_Write_Reg(NRF24L01_Plus_NRF_WRITE_REG + NRF24L01_Plus_FEATURE, 0x07); //06
    }
    NRF24L01_Plus_CE_HIGH;
}

uint8_t NRF24L01_Plus_TxPacket(uint8_t *txbuf, uint8_t len) {
    NRF24L01_Plus_CE_LOW;
    NRF24L01_Plus_Write_Buf(NRF24L01_Plus_WR_TX_PLOAD, txbuf, len); //д���ݵ�TX BUF  32���ֽ�
    NRF24L01_Plus_CE_HIGH; //��������
    while(NRF24L01_Plus_IRQ_READ != GPIO_PIN_RESET);   //������Ҫ�ȴ�60ms����
    //delay_us(10);
    return 0xff;//����ԭ����ʧ��
}
void NRF24L01_Plus_TxPacket_AP(uint8_t *txbuf, uint8_t len) {
    NRF24L01_Plus_CE_LOW;
    NRF24L01_Plus_Write_Buf(NRF24L01_Plus_ACK_PAYLOAD_CMD, txbuf, len); //д���ݵ�TX BUF
    NRF24L01_Plus_CE_HIGH;
}

uint8_t NRF24L01_RxPacketxxx(uint8_t *RxData) {
    uint8_t recvLen = 0;
    uint8_t state = NRF24L01_Read_Reg(NRF24L01_STATUS);     //��NRF24L01״̬�Ĵ���
    NRF24L01_Write_Reg(NRF24L01_WRITE_REG | NRF24L01_STATUS, state); //����ж�
    if(state & NRF24L01_STATUS_RX_DR) { // �����ݽ���
        RxData[0] = (state & 0x0E) >> 1;// ��ȡ���յ�ͨ��
        MYLOG(DEBUG,"RxChannel = %d",RxData[0]); // ��ȡ���յ�ͨ��
        recvLen = NRF24L01_Read_Reg(R_RX_PL_WID_CMD);
        if(recvLen > 32) {
            NRF24L01_RxMode();  //���³�ʼ������ģʽ
        } else {
            NRF24L01_Read_Buf(NRF24L01_RD_RX_PLOAD, RxData+1, recvLen); //��ȡ����
            NRF24L01_Write_Reg(NRF24L01_FLUSE_RX, 0xff);              //���RX FIFO�Ĵ���
            recvLen++;
        }
        hnrf.State = HAL_NRF_STATE_RX;
    }
    return recvLen;
}
uint8_t NRF24L01_Plus_RxPacket(uint8_t *RxData) {
    uint8_t recvLen = 0;
    uint8_t sta;
    sta = NRF24L01_Plus_Read_Reg(NRF24L01_Plus_STATUS);  //��ȡ״̬�Ĵ�����ֵ;
    if(sta & NRF24L01_Plus_RX_OK) { //���յ�����
        RxData[0] = (sta & 0x0E) >> 1;// ��ȡ���յ�ͨ��
        MYLOG(DEBUG,"RxChannel = %d",RxData[0]); // ��ȡ���յ�ͨ��
        recvLen = NRF24L01_Plus_Read_Reg(NRF24L01_Plus_R_RX_PL_WID);
        if(recvLen < 33) {
             NRF24L01_Plus_Read_Buf(NRF24L01_Plus_RD_RX_PLOAD, RxData+1, recvLen); // read receive payload from RX_FIFO buffer
             recvLen++;
             MYLOG(DEBUG, "NRF24L01_Plus_RX_OK");
//           MYLOGHEX(INFO,RxData, recvLen);
        } else NRF24L01_Plus_Write_Reg(NRF24L01_Plus_FLUSH_RX, 0xff); //��ջ�����
    }
    if(sta & NRF24L01_Plus_TX_OK) {
        MYLOG(DEBUG, "NRF24L01_Plus_TX_OK");
    }
    if(sta & NRF24L01_Plus_MAX_TX) {
        if(sta & 0x01)  //TX FIFO FULL
            NRF24L01_Plus_Write_Reg(NRF24L01_Plus_FLUSH_TX, 0xff);
    }
    NRF24L01_Plus_Write_Reg(NRF24L01_Plus_NRF_WRITE_REG + NRF24L01_Plus_STATUS, sta); //���TX_DS��MAX_RT�жϱ�־
    return recvLen;
}

void NRF24L01_Plus_Check_Event(void) {
    uint8_t sta;
    sta = NRF24L01_Plus_Read_Reg(NRF24L01_Plus_STATUS);  //��ȡ״̬�Ĵ�����ֵ;
    //MYLOG(WARNING, "NRF24L01_Plus_STATUS %x",sta);
    if(sta & NRF24L01_Plus_RX_OK) { //���յ�����
        NRF24L01_Plus_Rx_Len = NRF24L01_Plus_Read_Reg(NRF24L01_Plus_R_RX_PL_WID);
        if(NRF24L01_Plus_Rx_Len < 33) {
            NRF24L01_Plus_Rx_flag = 1;
            NRF24L01_Plus_Read_Buf(NRF24L01_Plus_RD_RX_PLOAD, NRF24L01_Plus_Rx_BUF, NRF24L01_Plus_Rx_Len); // read receive payload from RX_FIFO buffer
             MYLOG(WARNING, "NRF24L01_Plus_RX_OK");
             MYLOGHEX(INFO,NRF24L01_Plus_Rx_BUF, NRF24L01_Plus_Rx_Len);
        } else NRF24L01_Plus_Write_Reg(NRF24L01_Plus_FLUSH_RX, 0xff); //��ջ�����
    }
    if(sta & NRF24L01_Plus_TX_OK) {
        MYLOG(WARNING, "NRF24L01_Plus_TX_OK");
    }
    if(sta & NRF24L01_Plus_MAX_TX) {
        if(sta & 0x01)  //TX FIFO FULL
            NRF24L01_Plus_Write_Reg(NRF24L01_Plus_FLUSH_TX, 0xff);
    }
    NRF24L01_Plus_Write_Reg(NRF24L01_Plus_NRF_WRITE_REG + NRF24L01_Plus_STATUS, sta); //���TX_DS��MAX_RT�жϱ�־
}
