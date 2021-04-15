/************************************************************************************
*                                           NRF24L01�⺯��
*************************************************************************************/
/*------------------------- ���� -----------------------------------------
2015-10-13
------------------------------------------------------------------*/
#include "NRF24L01.h"
//uint8_t TX_ADDRESS[NRF24L01_ADR_LEN] = {192, 168, 1, 1, 1}; //���͵�ַ ����ͨ����ַ
//uint8_t RX_ADDR_P0[NRF24L01_ADR_LEN] = {192, 168, 1, 1, 1}; //P0 ���յ�ַ
//uint8_t RX_ADDR_P1[NRF24L01_ADR_LEN] = {192, 168, 1, 1, 2}; //P1 ���յ�ַ

uint8_t TX_RX_ADDR[6][NRF24L01_ADR_LEN] = {
    {190, 168, 1, 1, 0},//Ƶ��0��ַ ����
    {191, 168, 1, 1, 0},//Ƶ��1��ַ ��֫��
    {192, 168, 1, 1, 0},//Ƶ��2��ַ ��֫��
    {193, 168, 1, 1, 0},//Ƶ��3��ַ ��֫��
    {194, 168, 1, 1, 0},//Ƶ��4��ַ ��֫��
    {195, 168, 1, 1, 0},//Ƶ��5��ַ Ԥ��
};

NRF24_HandleTypeDef hnrf;
uint8_t nrfRXbuff[NRF_RXFIFO_MAX];
uint8_t nrfTXbuff[NRF_TXFIFO_MAX];
/**********************************************************************************/
/*                                          ��ʼ��                                */
/**********************************************************************************/

/**
* @brief  ��ʼ������
* @param  None
* @retval None
*/
void NRF24L_Init(void) {
    ldo3_enable();
    HAL_Delay(50);
    hnrf.Hspi = &hspi1;
    hnrf.RxAddress[0] = TX_RX_ADDR[0];//����  ��ʹ��ͨ��0ʱ�����͵�ַ���ͽ��յ�ַ����һ��������ֵ����0X20�������� ��ʹ��ͨ��0ʱ�������͵�ַ�ͽ��յ�ַ��һ���ġ����������ˡ�
    hnrf.RxAddress[1] = TX_RX_ADDR[1];//��֫��
    hnrf.RxAddress[2] = TX_RX_ADDR[2];//��֫��
    hnrf.RxAddress[3] = TX_RX_ADDR[3];//��֫��
    hnrf.RxAddress[4] = TX_RX_ADDR[4];//��֫��
    hnrf.RxAddress[5] = TX_RX_ADDR[5];//Ԥ���豸
    if(information.EquipmentType >= HOPEN_ECG) {
        hnrf.TxAddress = TX_RX_ADDR[0];
    } else {
        hnrf.TxAddress = TX_RX_ADDR[information.EquipmentType];
    }
    if(NRF24L01_Mode == 1) {
        hnrf.TxAddress = TX_RX_ADDR[0];//�����ȹ̶�
    }
    hnrf.Channel   = 40;  // ͨ��
    hnrf.Flag      =   NRF24L01_AUTO_RX | NRF24L01_AUTO_ACK; //  | NRF24L01_AUTO_SlEEP
    //�����Զ��ط����ʱ��:500us + 86us;����Զ��ط�����:10��
    //NRF24L01_Mode =1 ���ط�������ʱ��û��ϵ��һ�ι̶�15ms����  �ȴ��Է��豸ACK    100/4=25<15  �����ٶȿ��Ը���  ��Ƶ�ʷ���15*N���ȶ�
    //NRF24L01_Mode =0 �ط�1��1ms����
    hnrf.ARTCount  = 3; // �ط�����     15 1 9ms  69ms    2ms
    //hnrf.ARTdelay  = 2; // ��ʱ (N+1) * 250us = 1000us
    hnrf.ARTdelay  = 2 * information.EquipmentType; // ��ʱ (N+1) * 250us = 1000us          2
    hnrf.DataRates = HAL_NRF_DR_1M;   // ����
    hnrf.RFPA      = HAL_NRF_PA_0DBM; // ����
    if((information.EquipmentType > HOPEN_DEF) && (information.EquipmentType < HOPEN_ECG)) {
        hnrf.AutoAckPe = HAL_NRF_ENAA_P0 | (1 << information.EquipmentType); //0���߱�ͨ��
    } else {
        hnrf.AutoAckPe = HAL_NRF_ENAA_P0 | HAL_NRF_ENAA_P1 | HAL_NRF_ENAA_P2 | HAL_NRF_ENAA_P3 | HAL_NRF_ENAA_P4 | HAL_NRF_ENAA_P5; // �Զ�Ӧ��ͨ��
    }
    // �ж�ģʽ��Ҫ����Fifo
    // ���ջ���
    hnrf.pRxBuffPtr = nrfRXbuff;
    hnrf.RxSize = NRF_RXFIFO_MAX;
    hnrf.RxHead = 0;
    hnrf.RxTail = 0;
    // ���ͻ���
    hnrf.pTxBuffPtr = nrfTXbuff;
    hnrf.TxSize = NRF_TXFIFO_MAX;
    hnrf.TxHead = 0;
    hnrf.TxTail = 0;
    hnrf.State = HAL_NRF_STATE_READY;
    hnrf.Lock = HAL_UNLOCKED;
    NRF24_MspInit();
    if(NRF24L01_Check() == 0) {
        globalvariable.EquipmentFlag = ClrBit(globalvariable.EquipmentFlag, nrf24l_flag);
        MYLOG(ERROR, "NRF24L01 Fail! = %x", globalvariable.EquipmentFlag);
        return;
    }
    globalvariable.EquipmentFlag = SetBit(globalvariable.EquipmentFlag, nrf24l_flag);
    MYLOG(INFO, "NRF24L01 OK! = %x", globalvariable.EquipmentFlag);
}

/**
* @brief  NRF24L01оƬ��ʼ������
* @param
* @retval None
*/
void NRF24_MspInit(void) {
    #if BK2423
    _BSP_NRF24_CE_LOW;
    NRF24L01_Write_Reg(ACTIVATE_CMD, 0x73);
    _BSP_NRF24_CE_HIGH();
    #endif
    _BSP_NRF24_CE_LOW;
    // 1 > ʹ��ͨ�����Զ�Ӧ�� �Զ�Ӧ��ͨ��P0 P1
    if(hnrf.Flag & NRF24L01_AUTO_ACK) {
        NRF24L01_Write_Reg(NRF24L01_WRITE_REG | NRF24L01_EN_AA, 0x3f);//0x3f  hnrf.AutoAckPe
    }
    // 2 > ʹ��ͨ���Ľ��յ�ַ
    NRF24L01_Write_Reg(NRF24L01_WRITE_REG | NRF24L01_EN_RXADDR, 0x3f);//0x3f
    // 3 > ���õ�ַ����Ϊ NRF24L01_ADR_LEN
    NRF24L01_Write_Reg(NRF24L01_WRITE_REG | NRF24L01_SETUP_AW, HAL_NRF_ADW_5);
    // 4 > �����Զ��ط����ʱ��;����Զ��ط�����
    NRF24L01_Write_Reg(NRF24L01_WRITE_REG | NRF24L01_SETUP_RETR, ((hnrf.ARTdelay << 4) | hnrf.ARTCount)) ;
    // 5 > ����RFƵ��RFCH
    NRF24L01_Write_Reg(NRF24L01_WRITE_REG | NRF24L01_RF_CH, hnrf.Channel);
    // 6 > ����TX���书�ʲ���,����,���������濪��
    NRF24L01_Write_Reg(NRF24L01_WRITE_REG | NRF24L01_RF_SETUP, (hnrf.DataRates | hnrf.RFPA | 0x01));
    // 7 > ����TX�Զ�Ӧ���ַ ����P0 TX_ADDRESS
    //NRF24L01_Write_Buf(NRF24L01_WRITE_REG | NRF24L01_RX_ADDR_P0, hnrf.RxAddress[0], NRF24L01_ADR_LEN);
    NRF24L01_Write_Buf(NRF24L01_WRITE_REG | NRF24L01_RX_ADDR_P0, hnrf.TxAddress, NRF24L01_ADR_LEN); //0ͨ���Ľ��յ�ַ����ͷ��͵�ַ��ͬ
    // 8 > ���ñ���RX�ڵ��ַ ����P1 RX_ADDRESS
    NRF24L01_Write_Buf(NRF24L01_WRITE_REG | NRF24L01_RX_ADDR_P1, hnrf.RxAddress[1], NRF24L01_ADR_LEN);
    NRF24L01_Write_Buf(NRF24L01_WRITE_REG | NRF24L01_RX_ADDR_P2, hnrf.RxAddress[2], 1);
    NRF24L01_Write_Buf(NRF24L01_WRITE_REG | NRF24L01_RX_ADDR_P3, hnrf.RxAddress[3], 1);
    NRF24L01_Write_Buf(NRF24L01_WRITE_REG | NRF24L01_RX_ADDR_P4, hnrf.RxAddress[4], 1);
    NRF24L01_Write_Buf(NRF24L01_WRITE_REG | NRF24L01_RX_ADDR_P5, hnrf.RxAddress[5], 1);
    // 9 > ������Ч���ݳ��� 32
    NRF24L01_Write_Reg(NRF24L01_WRITE_REG | NRF24L01_RX_PW_P0, NRF24L01_RX_PLOAD_LEN);
    NRF24L01_Write_Reg(NRF24L01_WRITE_REG | NRF24L01_RX_PW_P1, NRF24L01_RX_PLOAD_LEN);
    NRF24L01_Write_Reg(NRF24L01_WRITE_REG | NRF24L01_RX_PW_P2, NRF24L01_RX_PLOAD_LEN);
    NRF24L01_Write_Reg(NRF24L01_WRITE_REG | NRF24L01_RX_PW_P3, NRF24L01_RX_PLOAD_LEN);
    NRF24L01_Write_Reg(NRF24L01_WRITE_REG | NRF24L01_RX_PW_P4, NRF24L01_RX_PLOAD_LEN);
    NRF24L01_Write_Reg(NRF24L01_WRITE_REG | NRF24L01_RX_PW_P5, NRF24L01_RX_PLOAD_LEN);
    // 10 > ���RX FIFO�Ĵ���
    NRF24L01_Write_Reg(NRF24L01_WRITE_REG | NRF24L01_FLUSE_RX, NRF24L01_NOP);
    // 11> TX��ַ
    NRF24L01_Write_Buf(NRF24L01_WRITE_REG | NRF24L01_TX_ADDR, hnrf.TxAddress, NRF24L01_ADR_LEN);
    // 12> ���TX FIFO�Ĵ���
    NRF24L01_Write_Reg(NRF24L01_WRITE_REG | NRF24L01_FLUSE_TX, NRF24L01_NOP);
    // 13> ���״̬
    NRF24L01_Write_Reg(NRF24L01_WRITE_REG | NRF24L01_STATUS, 0x70);
    //---------- ��оƬ����  ----------
    //Ĭ�Ͻ������ģʽ
    //ѡ��ͨ��0��̬���ݰ�����
    //  NRF24L01_Write_Reg(NRF24L01_WRITE_REG | NRF24L01_DYNPD,0X01);
    //  //ʹ�ܶ�̬���ݰ�����,AUTO_ACKģʽ
    //  NRF24L01_Write_Reg(NRF24L01_WRITE_REG | NRF24L01_FEATURE,0x05);
    //
    //  if (hnrf.Flag & NRF24L01_AUTO_SlEEP){
    //      NRF24L01_SleepMode();
    //  }else if(hnrf.Flag & NRF24L01_AUTO_RX){
    //      NRF24L01_RxMode();  // ����ģʽ
    //  }
    // �����ж�
}

/**
* @brief  NRF24L01 SPIͨ�ż��
* @param
* @retval 0:���� 1:����
*/
uint8_t NRF24L01_Check(void) {
    uint8_t i = 0;
    uint8_t Buff[5] = {0};
    NRF24L01_Read_Buf(NRF24L01_TX_ADDR, Buff, NRF24L01_ADR_LEN);
    for(i = 0; i < NRF24L01_ADR_LEN; i++) {
        //printf("%d=%d \r\n",Buff[i],TX_ADDRESS[i]);
        if(Buff[i] != hnrf.TxAddress[i]) {
            hnrf.State = HAL_NRF_STATE_RESET;
            return 0;
        }
    }
    return 1;
}
/**
* @brief  ��ȡFifo״̬�Ĵ�����˳�㷵��״̬
* @param
* @retval 16bit = ���ֽڣ�Fifo״̬�Ĵ��� �� ���ֽڣ�״̬�Ĵ���
*/
uint16_t NRF24L01_Status(void) {
    uint8_t txdata[2] = {0}, rxdata[2] = {0};
    txdata[0] = NRF24L01_FIFO_STATUS;
    txdata[1] = 0;
    _BSP_NRF24_CSN_LOW;          //ʹ��SPI����
    #if (HAL_SPI != 0)
    HAL_SPI_TransmitReceive(hnrf.Hspi, txdata, rxdata, 2, 300); //��ȡ�Ĵ�������
    #else
    BSP_SPI_Buff(hnrf.Hspi, txdata, rxdata, 2);
    #endif
    _BSP_NRF24_CSN_HIGH;          //��ֹSPI����
    return(*((uint16_t*)rxdata)); //����
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
uint8_t NRF24L01_Write_Reg(uint8_t reg, uint8_t value) {
    uint8_t txdata[2], rxdata[2];
    txdata[0] = reg;
    txdata[1] = value;
    _BSP_NRF24_CSN_LOW;                 //ʹ��SPI����
    #if (HAL_SPI != 0)
    HAL_SPI_TransmitReceive(hnrf.Hspi, txdata, rxdata, 2, 300); //��ȡ�Ĵ�������
    #else
    BSP_SPI_Buff(hnrf.Hspi, txdata, rxdata, 2);
    #endif
    _BSP_NRF24_CSN_HIGH;                 //��ֹSPI����
    return(rxdata[0]);                  //����״ֵ̬
}

/**
* @brief  ��NRF24L01�Ĵ���
* @param  reg: �Ĵ�����ַ
* @retval ���ؼĴ���ֵ
*/
uint8_t NRF24L01_Read_Reg(uint8_t reg) {
    uint8_t txdata[2] = {0}, rxdata[2] = {0};
    txdata[0] = reg;
    txdata[1] = 0;
    _BSP_NRF24_CSN_LOW;          //ʹ��SPI����
    #if (HAL_SPI != 0)
    HAL_SPI_TransmitReceive(hnrf.Hspi, txdata, rxdata, 2, 300); //��ȡ�Ĵ�������
    #else
    BSP_SPI_Buff(hnrf.Hspi, txdata, rxdata, 2);
    #endif
    _BSP_NRF24_CSN_HIGH;          //��ֹSPI����
    return(rxdata[1]);           //����
}

/**
* @brief  ��ȡ����������
* @param  reg: �Ĵ�����ַ
* @param  pBuf: �������ݻ���ָ��
* @param  len: ��ȡ����
* @retval None
*/
void NRF24L01_Read_Buf(uint8_t reg, uint8_t *pBuf, uint8_t len) {
    len++;  // ����һ��״̬λ��
    uint8_t txdata[34] = {0}, rxdata[34] = {0};
    txdata[0] = reg;
    _BSP_NRF24_CSN_LOW;          //ʹ��SPI����
    #if (HAL_SPI != 0)
    HAL_SPI_TransmitReceive(hnrf.Hspi, txdata, rxdata, len, 600); //��ȡ�Ĵ�������
    #else
    BSP_SPI_Buff(hnrf.Hspi, txdata, rxdata, len);
    #endif
    _BSP_NRF24_CSN_HIGH;          //��ֹSPI����
    memcpy(pBuf, rxdata + 1, len - 1);
}

/**
* @brief  �򻺳���д������
* @param  pBuf: д�����ݻ���ָ��
* @param  len: д�볤��
* @retval ���ض�����״ֵ̬
*/
uint8_t NRF24L01_Write_Buf(uint8_t reg, uint8_t *pBuf, uint8_t len) {
    uint8_t txdata[34] = {0}, rxdata[34] = {0};
    txdata[0] = reg;
    memcpy(txdata + 1, pBuf, len);
    len++;
    _BSP_NRF24_CSN_LOW;          //ʹ��SPI����
    #if (HAL_SPI != 0)
    HAL_SPI_TransmitReceive(hnrf.Hspi, txdata, rxdata, len, 600);
    #else
    BSP_SPI_Buff(hnrf.Hspi, txdata, rxdata, len);
    #endif
    _BSP_NRF24_CSN_HIGH;       //�ر�SPI����
    return(rxdata[0]);         //���ض�����״ֵ̬
}

/**********************************************************************************/
/*                                          ����ģʽ                               */
/**********************************************************************************/

/**
* @brief  NRF24L01���뷢��ģʽ
* @param
* @retval None
*/
void NRF24L01_TxMode1(void) {
    _BSP_NRF24_CE_LOW;
    NRF24L01_Write_Reg(NRF24L01_WRITE_REG | NRF24L01_CONFIG, NRF24L01_CONFIG_TX);
    NRF24L01_Write_Reg(NRF24L01_WRITE_REG | NRF24L01_FLUSE_TX, NRF24L01_NOP);  //���TX FIFO�Ĵ���
    //_BSP_NRF24_CE_HIGH();
    hnrf.State = HAL_NRF_STATE_TX;
}

void NRF24L01_TxMode2(void) {
    uint8_t txdata, rxdata ;
    _BSP_NRF24_CE_LOW;
    NRF24L01_Write_Reg(NRF24L01_WRITE_REG | NRF24L01_CONFIG, NRF24L01_CONFIG_TX2);//0e // IRQ�շ�����жϿ���,16λCRC,������0x0e
    NRF24L01_Write_Reg(NRF24L01_WRITE_REG | NRF24L01_FLUSE_TX, NRF24L01_NOP);   //��λbit4MAX_RT bit5 Ϊ1����TX_DS bit6 RX_DS
    NRF24L01_Write_Reg(NRF24L01_WRITE_REG | NRF24L01_FLUSE_RX, NRF24L01_NOP);   //���ڷɿأ�����ģʽ2e���Ǻ��õ�
    _BSP_NRF24_CSN_LOW;
    txdata = 0x50;
    HAL_SPI_TransmitReceive(&hspi1, &txdata, &rxdata, 1, 300); //��ȡ�Ĵ�������
    txdata = 0x73;
    HAL_SPI_TransmitReceive(&hspi1, &txdata, &rxdata, 1, 300); //��ȡ�Ĵ�������
    _BSP_NRF24_CSN_HIGH;
    //NRF24L01_Write_Reg(NRF24L01_WRITE_REG | NRF24L01_DYNPD, 0x01);                           //��������ȫͨ��  1 << information.EquipmentType
//    NRF24L01_Write_Reg(NRF24L01_WRITE_REG | NRF24L01_DYNPD, 1 << information.EquipmentType);   //��������ȫͨ��  1 << information.EquipmentType
    NRF24L01_Write_Reg(NRF24L01_WRITE_REG | NRF24L01_DYNPD, 0x3f);   //��������ȫͨ��  1 << information.EquipmentType
    NRF24L01_Write_Reg(NRF24L01_WRITE_REG | NRF24L01_FEATURE, 0x07); //06
    //_BSP_NRF24_CE_HIGH;
    hnrf.State = HAL_NRF_STATE_TX;
}

/**
* @brief  NRF24L01�������ģʽ
* @param
* @retval None
*/
void NRF24L01_RxMode1(void) {
    _BSP_NRF24_CE_LOW;
    NRF24L01_Write_Reg(NRF24L01_WRITE_REG | NRF24L01_CONFIG, NRF24L01_CONFIG_RX); //���û�������ģʽ�Ĳ���;PWR_UP,EN_CRC,16BIT_CRC,����ģʽ
    NRF24L01_Write_Reg(NRF24L01_WRITE_REG | NRF24L01_FLUSE_RX, NRF24L01_NOP);     //���RX FIFO�Ĵ���
    _BSP_NRF24_CE_HIGH;
    hnrf.State = HAL_NRF_STATE_RX;
}

void NRF24L01_RxMode2(void) {
    uint8_t txdata, rxdata ;
    _BSP_NRF24_CE_LOW;
    NRF24L01_Write_Reg(NRF24L01_WRITE_REG | NRF24L01_FLUSE_TX, NRF24L01_NOP);   //��λbit4MAX_RT bit5 Ϊ1����TX_DS bit6 RX_DS
    NRF24L01_Write_Reg(NRF24L01_WRITE_REG | NRF24L01_FLUSE_RX, NRF24L01_NOP);   //���ڷɿأ�����ģʽ2e���Ǻ��õ�
    NRF24L01_Write_Reg(NRF24L01_WRITE_REG | NRF24L01_CONFIG, NRF24L01_CONFIG_RX2); //0e // IRQ�շ�����жϿ���,16λCRC,������0x0e
    //��λΪ3��ʾ���η����ж�
    _BSP_NRF24_CSN_LOW;
    txdata = 0x50;
    HAL_SPI_TransmitReceive(&hspi1, &txdata, &rxdata, 1, 300); //��ȡ�Ĵ�������
    txdata = 0x73;
    HAL_SPI_TransmitReceive(&hspi1, &txdata, &rxdata, 1, 300); //��ȡ�Ĵ�������
    _BSP_NRF24_CSN_HIGH;
    //NRF24L01_Write_Reg(NRF24L01_WRITE_REG | NRF24L01_DYNPD, 0x01);   //��������ͨ��
//    NRF24L01_Write_Reg(NRF24L01_WRITE_REG | NRF24L01_DYNPD, hnrf.AutoAckPe);   //��������ͨ��
    NRF24L01_Write_Reg(NRF24L01_WRITE_REG | NRF24L01_DYNPD, 0x3f);    //��������ͨ��
    NRF24L01_Write_Reg(NRF24L01_WRITE_REG | NRF24L01_FEATURE, 0x07); //06
    _BSP_NRF24_CE_HIGH;
    hnrf.State = HAL_NRF_STATE_RX;
}
/**
* @brief  NRF24L01�������ģʽ
* @param
* @retval None
*/
void NRF24L01_SleepMode(void) {
    _BSP_NRF24_CE_LOW;
    NRF24L01_Write_Reg(NRF24L01_WRITE_REG | NRF24L01_CONFIG, NRF24L01_CONFIG_SLEEP); //���û�������ģʽ�Ĳ���;PWR_UP,EN_CRC,16BIT_CRC,����ģʽ
    NRF24L01_Write_Reg(NRF24L01_WRITE_REG | NRF24L01_FLUSE_RX, NRF24L01_NOP);        //���RX FIFO�Ĵ���
    NRF24L01_Write_Reg(NRF24L01_WRITE_REG | NRF24L01_FLUSE_TX, NRF24L01_NOP);        //���TX FIFO�Ĵ���
    hnrf.State = HAL_NRF_STATE_SEELP;
}

/**********************************************************************************/
/*                                      ��̬�޸Ĳ���                               */
/**********************************************************************************/

/**
* @brief  ���� RF_CH
* @param
* @retval None
*/
void NRF24L01_SetChange(uint8_t ch) {
    _BSP_NRF24_CE_LOW;
    hnrf.Channel = (ch & 0x7F);
    // 5 > ����RFƵ��RFCH
    NRF24L01_Write_Reg(NRF24L01_WRITE_REG | NRF24L01_RF_CH, hnrf.Channel);
}

/**
* @brief  ��ȡ RF_CH
* @param
* @retval ���� RF_CH
*/
uint8_t NRF24L01_GetChange(void) {
    _BSP_NRF24_CE_LOW;
    // 5 > ����RFƵ��RFCH
    hnrf.Channel = NRF24L01_Read_Reg(NRF24L01_RF_CH);
    return hnrf.Channel;
}

/**
* @brief  ����TX��ַ
* @param  ��TX��ַ����ָ�룬5���ֽ�
* @retval None
*/
void NRF24L01_SetTxAddress(uint8_t *addr) {
    _BSP_NRF24_CE_LOW;
    memcpy(hnrf.TxAddress, addr,  NRF24L01_ADR_LEN);
    NRF24L01_Write_Buf(NRF24L01_WRITE_REG | NRF24L01_TX_ADDR, hnrf.TxAddress, NRF24L01_ADR_LEN);
    NRF24L01_Write_Buf(NRF24L01_WRITE_REG | NRF24L01_RX_ADDR_P0, hnrf.TxAddress, NRF24L01_ADR_LEN);
}

//
/**
* @brief  ��ȡTX��ַ
* @param  �ش�TX��ַ����ָ��
* @retval None
*/
void NRF24L01_GetTxAddress(uint8_t *addr) {
    _BSP_NRF24_CE_LOW;
    NRF24L01_Read_Buf(NRF24L01_TX_ADDR, hnrf.TxAddress, NRF24L01_ADR_LEN);
    memcpy(addr, hnrf.TxAddress,  NRF24L01_ADR_LEN);
}

/**
* @brief  ����RX��ַ
* @param  ��RX��ַ����ָ�룬5���ֽ�
* @retval None
*/
void NRF24L01_SetRxAddress(uint8_t *addr, uint8_t channel) {
    _BSP_NRF24_CE_LOW;
//    if(channel == 0)  memcpy(RX_ADDRESS, addr,  NRF24L01_ADR_LEN);
//    if(channel == 1)  memcpy(RX_ADDRESS, addr,  NRF24L01_ADR_LEN);
    if(channel < 6) {
        memcpy(hnrf.RxAddress[channel], addr,  NRF24L01_ADR_LEN);//�ѵ�ַ���ý���Ӧ��λ��
        NRF24L01_Write_Buf(NRF24L01_WRITE_REG | (NRF24L01_RX_ADDR_P0 + channel), hnrf.RxAddress[channel], NRF24L01_ADR_LEN);
    }
}

/**
* @brief  ��ȡRX��ַ
* @param  �ش�RX��ַ����ָ��
* @retval None
*/
void NRF24L01_GetRxAddress(uint8_t *addr, uint8_t channel) {
    _BSP_NRF24_CE_LOW;
    if(channel < 6) {
        NRF24L01_Read_Buf((NRF24L01_RX_ADDR_P0 + channel), addr, NRF24L01_ADR_LEN);
        memcpy(hnrf.RxAddress[channel], addr, NRF24L01_ADR_LEN);
    }
    //if(channel == 1) memcpy(RX_ADDRESS, addr, NRF24L01_ADR_LEN);
}

/**********************************************************************************/
/*                                      �жϴ�����                               */
/**********************************************************************************/

/**
* @brief  NRF24L01�жϴ���
* @param
* @retval None
*/
void NRF24L01_Handler(void) {
    uint8_t state = NRF24L01_Read_Reg(NRF24L01_STATUS);     //��NRF24L01״̬�Ĵ���
    NRF24L01_Write_Reg(NRF24L01_WRITE_REG | NRF24L01_STATUS, state); //����ж�
    //���ͳɹ��ж�
    if(state & NRF24L01_STATUS_TX_DS) {
        if(hnrf.TxHead != hnrf.TxTail) { // �ж����ݻ������Ƿ�������δ������
            NRF24L01_SendFifo();
        } else {
            hnrf.TxHead = hnrf.TxTail = 0;
            if(hnrf.Flag & NRF24L01_AUTO_SlEEP) {
                NRF24L01_SleepMode();
            } else if(hnrf.Flag & NRF24L01_AUTO_RX)      {
                NRF24L01_RxMode1();  //���³�ʼ������ģʽ
            }
        }
    }
    //�ﵽ����ط������ж�
    if(state & NRF24L01_STATUS_MAX_RT) {
        NRF24L01_Write_Reg(NRF24L01_WRITE_REG | NRF24L01_FLUSE_TX, NRF24L01_NOP); //���TX FIFO�Ĵ���
        //NRF24L01_FLAG &= ~NRF24L01_BUSY;  //BUSY��־��0
        hnrf.TxTail = 0;
        hnrf.TxHead = 0;
        NRF24L01_Write_Reg(NRF24L01_WRITE_REG | NRF24L01_RF_CH, hnrf.Channel);  // ������Լ�����
        if(hnrf.Flag & NRF24L01_AUTO_SlEEP) {
            NRF24L01_SleepMode();
        } else if(hnrf.Flag & NRF24L01_AUTO_RX)      {
            NRF24L01_RxMode1();  //���³�ʼ������ģʽ
        }
    }
    //���յ������ж�
    if(state & NRF24L01_STATUS_RX_DR) {
        hnrf.State = HAL_NRF_STATE_BUSY_RX;
        NRF24L01_RecvFifo();
    }
}

/**
* @brief  �������ݵ�����
* @param
* @retval None
*/
void    NRF24L01_RecvFifo(void) {
    uint8_t recvLen, i;
    uint8_t data[NRF24L01_RX_PLOAD_LEN] = {0};
    recvLen = NRF24L01_Read_Reg(R_RX_PL_WID_CMD);
    NRF24L01_Read_Buf(NRF24L01_RD_RX_PLOAD, data, recvLen); //��ȡ����
    NRF24L01_Write_Reg(NRF24L01_FLUSE_RX, 0xff); //���RX FIFO�Ĵ���
    for(i = 0; i < recvLen; i++) {
        hnrf.pRxBuffPtr[hnrf.RxHead++] = data[i];
        if(hnrf.RxHead >= hnrf.RxSize) hnrf.RxHead = 0;
    }
    // �����ݷŵ�Buff
    hnrf.State = HAL_NRF_STATE_RX;
}

/**
* @brief  ��ȡ��������
* @param  rxData �������
* @retval �������ݳ���
*/
uint16_t NRF24L01_GetReceive(uint8_t *rxData) {
    uint16_t len = 0;
    while(hnrf.RxHead != hnrf.RxTail) {
        *rxData++ = hnrf.pRxBuffPtr[hnrf.RxTail++];
        len++;
        if(hnrf.RxTail >= hnrf.RxSize) hnrf.RxTail = 0;
    }
    return len;
}

/**
* @brief  �ȴ����ͻ�������
* @param
* @retval None
*/
void NRF24L01_SendFifo(void) {
    uint8_t i, data[NRF24L01_TX_PLOAD_LEN] = {0};
    _BSP_NRF24_CE_LOW;
    hnrf.State = HAL_NRF_STATE_BUSY_TX;
    NRF24L01_Write_Reg(NRF24L01_WRITE_REG | NRF24L01_RF_CH, hnrf.Channel);  // ������Լ�����
    for(i = 0; i < NRF24L01_TX_PLOAD_LEN; i++) {
        data[i] = hnrf.pTxBuffPtr[hnrf.TxTail++];
        if(hnrf.TxTail >= hnrf.TxSize) hnrf.TxTail = 0;
        if(hnrf.TxTail == hnrf.TxHead) break;
    }
    NRF24L01_Write_Buf(NRF24L01_WR_TX_PLOAD, data, NRF24L01_TX_PLOAD_LEN);
    _BSP_NRF24_CE_HIGH;
}

/**
* @brief  �����жϷ��͹���
* @param
* @retval None
*/
void NRF24L01_SendData(uint8_t *txData, uint16_t len) {
    _BSP_NRF24_CE_LOW;  // ����
    memcpy(hnrf.pTxBuffPtr, txData, len);   //�������
    hnrf.TxHead = len;  // ��Ҫ��������
    hnrf.TxTail = 0;    // δ��������
    NRF24L01_TxMode1();  // ����ģʽ
    NRF24L01_SendFifo();//
}

/**********************************************************************************/
/*                                      ��ѯ������                               */
/**********************************************************************************/
/**
* @brief  ����һ��32���ֽڵ����ݰ���
* @param  TxData  ���͵����ݻ��棬������ NRF24L01_TX_PLOAD_LEN ����
* @retval NRF24L01_STATUS_TX_DS  ���ͳɹ�
NRF24L01_STATUS_MAX_RT �ط�ʧ��
0xff    ����ʧ��
*/
//uint8_t NRF24L01_TxPacket1(uint8_t *TxData, uint8_t len) {
//    //uint8_t state;
//    uint8_t resend = 0;
//    uint8_t reStatus = 0xff;
//    uint32_t tickstart = HAL_GetTick();
//    _BSP_NRF24_CE_LOW;
//    hnrf.State = HAL_NRF_STATE_BUSY_TX;
//    // д������
//    NRF24L01_Write_Buf(NRF24L01_WR_TX_PLOAD, TxData, len);
//SEND:
//    NRF24L01_Write_Reg(NRF24L01_WRITE_REG | NRF24L01_STATUS, 0xf0); //����ж�
//    // ��������
//    _BSP_NRF24_CE_HIGH;
//    // �ȴ��ж�
//    while(_BSP_NRF24_IRQ_READ != GPIO_PIN_RESET){   //������Ҫ�ȴ�60ms����
//      osDelay(1);
//    }
//    //HAL_Delay(50);
//    //state = NRF24L01_Read_Reg(NRF24L01_STATUS);     //��NRF24L01״̬�Ĵ���
//    NRF24L01_Read_Reg(NRF24L01_STATUS);     //��NRF24L01״̬�Ĵ���
//    uint8_t fifostate = NRF24L01_Read_Reg(NRF24L01_FIFO_STATUS);        //��NRF24L01״̬�Ĵ���
//    _BSP_NRF24_CE_LOW;
//    //printf("fifostate = %x resend = %d\r\n", (fifostate&0x30),resend);
//    switch(fifostate & 0x30) {
//        case 0x10:    // TX FIFO empty
//            reStatus =  NRF24L01_STATUS_TX_DS;  // ���ͳɹ�
//            break;
//        case 0x00:    // Data in TX FIFO, but not full
////            if(resend < 10) {
////                resend ++;
////                goto SEND;
////            }
//            //HAL_Delay(1);
//            NRF24L01_Write_Reg(NRF24L01_WRITE_REG | NRF24L01_FLUSE_TX, NRF24L01_NOP); //���TX FIFO�Ĵ���
//            reStatus =  NRF24L01_STATUS_MAX_RT;//�ﵽ����ط������ж�
//            break;
//        case 0x20:    //        RX FIFO full
//            if(resend < 10) {
//                resend ++;
//                goto SEND;
//            }
//            NRF24L01_Write_Reg(NRF24L01_WRITE_REG | NRF24L01_FLUSE_TX, NRF24L01_NOP); //���TX FIFO�Ĵ���
//            reStatus =  NRF24L01_STATUS_MAX_RT;//�ﵽ����ط������ж�
//            break;
//    }
//    /*
//    if(state & NRF24L01_STATUS_TX_DS)
//    {
//        reStatus =  NRF24L01_STATUS_TX_DS;  // ���ͳɹ�
//    }else   if(state & NRF24L01_STATUS_MAX_RT)  {
//        if (resend<10){
//            resend ++;
//                NRF24L01_Write_Reg(NRF24L01_WRITE_REG | NRF24L01_STATUS,state);//����ж�
//            goto SEND;
//        }
//        NRF24L01_Write_Reg(NRF24L01_WRITE_REG | NRF24L01_FLUSE_TX,NRF24L01_NOP);  //���TX FIFO�Ĵ���
//        reStatus =  NRF24L01_STATUS_MAX_RT;//�ﵽ����ط������ж�
//    }else{

//        reStatus = state;
//        state = NRF24L01_Read_Reg(NRF24L01_STATUS);
//        printnrfreg();
//    }
//    */
//    //printf("RESEND:%d \t delay::%d\t",resend,( HAL_GetTick()-tickstart));
//    NRF24L01_Write_Reg(NRF24L01_WRITE_REG | NRF24L01_STATUS, 0xf0); //����ж�
//    //printf("RESEND:%d  \tdelay:%d  \t", resend, (HAL_GetTick() - tickstart));
//    return reStatus;    //�������
//}
uint8_t NRF24L01_TxPacket1(uint8_t *TxData, uint8_t len) {
    uint8_t count = 0, sta;
//    while(NRF24L01_Read_Reg(NRF24L01_CD)){ //����ŵ��Ƿ�ռ�ã�ռ�þ���ʱ���  ���ȶ�
//        if(count++>3) break;         //����ԭ����ʧ��
//        osDelay(1);
//    }
    if(NRF24L01_Read_Reg(NRF24L01_CD)) {
        osDelay(1);//���ղ�ʹ��ʱ��
        //osDelay(information.EquipmentType);//���ղ�ʹ��ʱ��
    }
    _BSP_NRF24_CE_LOW;
    //NRF24L01_Write_Buf(NRF24L01_WR_TX_PLOAD, TxData, len);
    NRF24L01_Write_Buf(NRF24L01_WR_TX_PLOAD, TxData, 32);
    _BSP_NRF24_CE_HIGH;                              //��������
//    count = 0;
    while(_BSP_NRF24_IRQ_READ != GPIO_PIN_RESET);
//    while(_BSP_NRF24_IRQ_READ != GPIO_PIN_RESET){    //�ȴ��������
//        if(count++>3) break;                         //����ԭ����ʧ��
//        osDelay(1);
//    }
    sta = NRF24L01_Read_Reg(NRF24L01_STATUS);        //��ȡ״̬�Ĵ�����ֵ
    NRF24L01_Write_Reg(NRF24L01_WRITE_REG | NRF24L01_STATUS, sta); //���TX_DS��MAX_RT�жϱ�־
    if(sta & NRF24L01_STATUS_MAX_RT) {               //�ﵽ����ط�����
        NRF24L01_Write_Reg(NRF24L01_WRITE_REG | NRF24L01_FLUSE_TX, NRF24L01_NOP); //���TX FIFO�Ĵ���
        return NRF24L01_STATUS_MAX_RT;
    }
    if(sta & NRF24L01_STATUS_TX_DS) {                //�������
        return NRF24L01_STATUS_TX_DS;
    }
    return 0xff;//����ԭ����ʧ��
}
uint8_t NRF24L01_TxPacket2(uint8_t *txbuf, uint8_t len) {
    _BSP_NRF24_CE_LOW;
    NRF24L01_Write_Buf(NRF24L01_WR_TX_PLOAD, txbuf, len);     //д���ݵ�TX BUF  32���ֽ�
    _BSP_NRF24_CE_HIGH; //��������
    //HAL_Delay(1);
    return NRF24L01_STATUS_TX_DS;//����ԭ����ʧ��
}
void NRF24L01_TxPacket1_AP(uint8_t *txbuf, uint8_t len) {
}
uint8_t NRF24L01_TxPacket2_AP(uint8_t *txbuf, uint8_t len) {
    _BSP_NRF24_CE_LOW;
    NRF24L01_Write_Buf(W_ACK_PAYLOAD_CMD, txbuf, len);     //д���ݵ�TX BUF  32���ֽ�
    _BSP_NRF24_CE_HIGH;
    return NRF24L01_STATUS_TX_DS;//����ԭ����ʧ��
}
/**
* @brief  �������� ��ִ�иú���ǰ�����ȵ��� NRF24L01_RxMode();
* @param    RxData  ���յ������ݻ���
* @retval   ���ؽ��յ������ݳ���
*/
uint8_t NRF24L01_RxPacket1(uint8_t *RxData) {
    uint8_t recvLen = 0, state;
    if(_BSP_NRF24_IRQ_READ != GPIO_PIN_RESET) return recvLen;//û�յ�����ֱ���˳�����ֹһֱ���ٶ��Ĵ���
    state = NRF24L01_Read_Reg(NRF24L01_STATUS);     //��NRF24L01״̬�Ĵ���
    NRF24L01_Write_Reg(NRF24L01_WRITE_REG | NRF24L01_STATUS, state); //����ж�
    if(state & NRF24L01_STATUS_RX_DR) { // �����ݽ���
        RxData[0] = (state & 0x0E) >> 1;// ��ȡ���յ�ͨ��
        MYLOG(DEBUG, "RxChannel = %d", RxData[0]); // ��ȡ���յ�ͨ��
        recvLen = NRF24L01_Read_Reg(R_RX_PL_WID_CMD);
        if(recvLen > 32) {
            NRF24L01_RxMode1();  //���³�ʼ������ģʽ
        } else {
            NRF24L01_Read_Buf(NRF24L01_RD_RX_PLOAD, RxData + 1, recvLen); //��ȡ����
            NRF24L01_Write_Reg(NRF24L01_FLUSE_RX, 0xff);              //���RX FIFO�Ĵ���
            recvLen++;
        }
        hnrf.State = HAL_NRF_STATE_RX;
    }
    return recvLen;
}

uint8_t NRF24L01_RxPacket2(uint8_t *RxData) {
    uint8_t recvLen = 0, state;
    if(_BSP_NRF24_IRQ_READ != GPIO_PIN_RESET) return recvLen;//û�յ�����ֱ���˳�����ֹһֱ���ٶ��Ĵ���
    //while(_BSP_NRF24_IRQ_READ != GPIO_PIN_RESET);    //�ȴ��������
    state = NRF24L01_Read_Reg(NRF24L01_STATUS);     //��NRF24L01״̬�Ĵ���
    if(state & NRF24L01_STATUS_RX_DR) { //���յ�����
        RxData[0] = (state & 0x0E) >> 1;// ��ȡ���յ�ͨ��
        MYLOG(DEBUG, "RxChannel = %d", RxData[0]); // ��ȡ���յ�ͨ��
        recvLen = NRF24L01_Read_Reg(R_RX_PL_WID_CMD);
        if(recvLen < 33) {
            NRF24L01_Read_Buf(NRF24L01_RD_RX_PLOAD, RxData + 1, recvLen); //��ȡ����
            recvLen++;
            MYLOG(DEBUG, "NRF24L01_Plus_RX_OK");
//           MYLOGHEX(INFO,RxData, recvLen);
        } else  NRF24L01_Write_Reg(NRF24L01_WRITE_REG | NRF24L01_FLUSE_RX, NRF24L01_NOP); //��ջ�����
    }
    if(state & NRF24L01_STATUS_TX_DS) {
        MYLOG(DEBUG, "NRF24L01_Plus_TX_OK");
    }
    if(state & NRF24L01_STATUS_MAX_RT) {
        if(state & 0x01)  //TX FIFO FULL
            NRF24L01_Write_Reg(NRF24L01_WRITE_REG | NRF24L01_FLUSE_TX, NRF24L01_NOP);
    }
    NRF24L01_Write_Reg(NRF24L01_WRITE_REG | NRF24L01_STATUS, state); //����ж�
    return recvLen;
}
/**********************************************************************************/
/*                                          ���Ժ���                               */
/**********************************************************************************/
void printnrfreg(void) {
    uint8_t reg[16] = {0};
    uint8_t i;
    NRF24L01_Read_Buf(0, reg, 10);
    for(i = 0; i < 10; i++) {
        reg[0] = NRF24L01_Read_Reg(i);
        printf("REG 0x%2x : 0x%02x\r\n", i, reg[0]);
    }
    reg[0] = NRF24L01_Read_Reg(NRF24L01_FIFO_STATUS);
    printf("REG 0x%2x : 0x%02x\r\n", NRF24L01_FIFO_STATUS, reg[0]);
    reg[0] = NRF24L01_Read_Reg(NRF24L01_DYNPD);
    printf("REG 0x%2x : 0x%02x\r\n", NRF24L01_DYNPD, reg[0]);
    reg[0] = NRF24L01_Read_Reg(NRF24L01_FEATURE);
    printf("REG 0x%2x : 0x%02x\r\n", NRF24L01_FEATURE, reg[0]);
    printf("Fifo&Status: 0x%4x\r\n", NRF24L01_Status());
    if(HAL_GPIO_ReadPin(NRF24L01_CE_GPIO_Port, NRF24L01_CE_Pin) == GPIO_PIN_SET) {
        printf("CE HIGH\r\n");
    } else {
        printf("CE LOW\r\n");
    }
    switch(hnrf.State) {
        case HAL_NRF_STATE_RESET  :
            printf("RESET\r\n");
            break;
        case HAL_NRF_STATE_READY:
            printf("READY\r\n");
            break;
        case HAL_NRF_STATE_SEELP:
            printf("SEELP\r\n");
            break;
        case HAL_NRF_STATE_MAXRT:
            printf("MAXRT\r\n");
            break;
        case HAL_NRF_STATE_ERROR:
            printf("ERROR\r\n");
            break;
        case HAL_NRF_STATE_TX:
            printf("TX\r\n");
            break;
        case HAL_NRF_STATE_RX :
            printf("RX\r\n");
            break;
        case HAL_NRF_STATE_BUSY  :
            printf("BUSY\r\n");
            break;
        case HAL_NRF_STATE_BUSY_TX :
            printf("BUSY TX\r\n");
            break;
        case HAL_NRF_STATE_BUSY_RX:
            printf("BUSY RX\r\n");
            break;
    }
}
