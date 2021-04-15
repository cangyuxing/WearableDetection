#include "nrf24l01plus.h"

uint8_t NRF24L01_Plus_TX_ADDRESS[NRF24L01_Plus_TX_ADR_WIDTH] = {190, 168, 1, 1, 0};
uint8_t NRF24L01_Plus_RX_ADDRESS[NRF24L01_Plus_RX_ADR_WIDTH] = {190, 168, 1, 1, 0};

uint8_t NRF24L01_Plus_Rx_Len = 0;
uint8_t NRF24L01_Plus_Rx_BUF[33];
uint8_t NRF24L01_Plus_Tx_BUF[33];
uint8_t NRF24L01_Plus_Rx_flag = 0;
//初始化24L01的IO口

void NRF24L01_Plus_Init(void) {
    ldo3_enable();
    HAL_Delay(50);
    NRF24L01_Plus_CE_LOW;   //使能24L01
    NRF24L01_Plus_CSN_HIGH; //SPI片选取消
    if(NRF24L01_Plus_Check() == 0) {
        MYLOG(ERROR, "NRF24L01 Fail!");
    }
    MYLOG(INFO, "NRF24L01 OK!");
}
//检测24L01是否存在
//返回值:0，成功;1，失败
uint8_t NRF24L01_Plus_Check(void) {
    uint8_t buf[5] = {190, 168, 1, 1, 0};
    uint8_t i;
    NRF24L01_Plus_Write_Buf(NRF24L01_Plus_NRF_WRITE_REG + NRF24L01_Plus_TX_ADDR, buf, 5); //写入5个字节的地址.
    NRF24L01_Plus_Read_Buf(NRF24L01_Plus_TX_ADDR, buf, 5); //读出写入的地址
    for(i = 0; i < 5; i++)if(buf[i] != 0XA5)break;
    if(i != 5)return 0; //检测24L01错误
    return 1;           //检测到24L01
}
/**********************************************************************************/
/*                                    硬件SPI通讯                                 */
/**********************************************************************************/
/**
* @brief  写NRF24L01寄存器
* @param  reg: 寄存器地址 或命令
* @param  value: 写入数据
* @retval 返回状态值
*/
uint8_t NRF24L01_Plus_Write_Reg(uint8_t reg, uint8_t value) {
    uint8_t txdata[2], rxdata[2];
    txdata[0] = reg;
    txdata[1] = value;
    NRF24L01_Plus_CSN_LOW;                 //使能SPI传输
    HAL_SPI_TransmitReceive(&hspi1, txdata, rxdata, 2, 300); //读取寄存器内容
    NRF24L01_Plus_CSN_HIGH;                 //禁止SPI传输
    return(rxdata[0]);                  //返回状态值
}

/**
* @brief  读NRF24L01寄存器
* @param  reg: 寄存器地址
* @retval 返回寄存器值
*/
uint8_t NRF24L01_Plus_Read_Reg(uint8_t reg) {
    uint8_t txdata[2] = {0}, rxdata[2] = {0};
    txdata[0] = reg;
    txdata[1] = 0;
    NRF24L01_Plus_CSN_LOW;           //使能SPI传输
    HAL_SPI_TransmitReceive(&hspi1, txdata, rxdata, 2, 300); //读取寄存器内容
    NRF24L01_Plus_CSN_HIGH;          //禁止SPI传输
    return(rxdata[1]);               //返回
}

/**
* @brief  读取缓冲区数据
* @param  reg: 寄存器地址
* @param  pBuf: 返回数据缓存指针
* @param  len: 读取长度
* @retval None
*/
uint8_t NRF24L01_Plus_Read_Buf(uint8_t reg, uint8_t *pBuf, uint8_t len) {
    len++;  // 增加一个状态位置
    uint8_t txdata[34] = {0}, rxdata[34] = {0};
    txdata[0] = reg;
    NRF24L01_Plus_CSN_LOW;           //使能SPI传输
    HAL_SPI_TransmitReceive(&hspi1, txdata, rxdata, len, 600); //读取寄存器内容
    NRF24L01_Plus_CSN_HIGH;          //禁止SPI传输
    memcpy(pBuf, rxdata + 1, len - 1);
    return(rxdata[0]);               //返回读到的状态值
}

/**
* @brief  向缓冲区写入数据
* @param  pBuf: 写入数据缓存指针
* @param  len: 写入长度
* @retval 返回读到的状态值
*/
uint8_t NRF24L01_Plus_Write_Buf(uint8_t reg, uint8_t *pBuf, uint8_t len) {
    uint8_t txdata[34] = {0}, rxdata[34] = {0};
    txdata[0] = reg;
    memcpy(txdata + 1, pBuf, len);
    len++;
    NRF24L01_Plus_CSN_LOW;          //使能SPI传输
    HAL_SPI_TransmitReceive(&hspi1, txdata, rxdata, len, 600);
    NRF24L01_Plus_CSN_HIGH;       //关闭SPI传输
    return(rxdata[0]);         //返回读到的状态值
}

//启动NRF24L01发送一次数据
//txbuf:待发送数据首地址
//返回值:0，接收完成；其他，错误代码
void NRF24L01_Plus_Mode(uint8_t model) {
    uint8_t txdata, rxdata ;
    NRF24L01_Plus_CE_LOW;
    NRF24L01_Plus_Write_Buf(NRF24L01_Plus_NRF_WRITE_REG + NRF24L01_Plus_TX_ADDR   , (uint8_t*)NRF24L01_Plus_TX_ADDRESS, NRF24L01_Plus_TX_ADR_WIDTH); //写TX节点地址
    NRF24L01_Plus_Write_Buf(NRF24L01_Plus_NRF_WRITE_REG + NRF24L01_Plus_RX_ADDR_P0, (uint8_t*)NRF24L01_Plus_RX_ADDRESS, NRF24L01_Plus_RX_ADR_WIDTH); //设置TX节点地址,主要为了使能ACK
    NRF24L01_Plus_Write_Reg(NRF24L01_Plus_NRF_WRITE_REG + NRF24L01_Plus_EN_AA,      0x01); //使能通道0的自动应答 00表示不应答
    NRF24L01_Plus_Write_Reg(NRF24L01_Plus_NRF_WRITE_REG + NRF24L01_Plus_EN_RXADDR,  0x01); //使能通道0的接收地址 为0，不接收
    NRF24L01_Plus_Write_Reg(NRF24L01_Plus_NRF_WRITE_REG + NRF24L01_Plus_SETUP_RETR, 0x1a); //设置自动重发间隔时间:500us + 86us;最大自动重发次数:10次 不重发
    NRF24L01_Plus_Write_Reg(NRF24L01_Plus_NRF_WRITE_REG + NRF24L01_Plus_RF_CH,      40);   //40设置RF通道为40设置信道工作为2.5GHZ，收发必须一致
    NRF24L01_Plus_Write_Reg(NRF24L01_Plus_NRF_WRITE_REG + NRF24L01_Plus_RF_SETUP,   0x0f); //设置TX发射参数,0db增益,2Mbps,低噪声增益开启
    if(model == NRF24L01_Plus_MODEL_RX1) {        //RX
        NRF24L01_Plus_Write_Reg(NRF24L01_Plus_NRF_WRITE_REG + NRF24L01_Plus_RX_PW_P0, NRF24L01_Plus_RX_PLOAD_WIDTH); //选择通道0的有效数据宽度
        NRF24L01_Plus_Write_Reg(NRF24L01_Plus_NRF_WRITE_REG + NRF24L01_Plus_CONFIG, 0x0f);                           //IRQ收发完成中断开启,16位CRC,主接收
    } else if(model == NRF24L01_Plus_MODEL_TX1) { //TX
        NRF24L01_Plus_Write_Reg(NRF24L01_Plus_NRF_WRITE_REG + NRF24L01_Plus_RX_PW_P0, NRF24L01_Plus_RX_PLOAD_WIDTH); //选择通道0的有效数据宽度
        NRF24L01_Plus_Write_Reg(NRF24L01_Plus_NRF_WRITE_REG + NRF24L01_Plus_CONFIG, 0x0e);                           //IRQ收发完成中断开启,16位CRC,主发送
    } else if(model == NRF24L01_Plus_MODEL_RX2) { //RX2
        NRF24L01_Plus_Write_Reg(NRF24L01_Plus_FLUSH_TX, 0xff);
        NRF24L01_Plus_Write_Reg(NRF24L01_Plus_FLUSH_RX, 0xff);
        NRF24L01_Plus_Write_Reg(NRF24L01_Plus_NRF_WRITE_REG + NRF24L01_Plus_CONFIG, 0x3f);                           //0f // IRQ收发完成中断开启,16位CRC,主接收0x0f
        //高位为3表示屏蔽发送中断
        NRF24L01_Plus_CSN_LOW;
        txdata = 0x50;
        HAL_SPI_TransmitReceive(&hspi1, &txdata, &rxdata, 1, 300); //读取寄存器内容
        txdata = 0x73;
        HAL_SPI_TransmitReceive(&hspi1, &txdata, &rxdata, 1, 300); //读取寄存器内容
        NRF24L01_Plus_CSN_HIGH;
        NRF24L01_Plus_Write_Reg(NRF24L01_Plus_NRF_WRITE_REG + NRF24L01_Plus_DYNPD, 0x01);
        NRF24L01_Plus_Write_Reg(NRF24L01_Plus_NRF_WRITE_REG + NRF24L01_Plus_FEATURE, 0x07);                  //06
    } else {                            //TX2
        NRF24L01_Plus_Write_Reg(NRF24L01_Plus_NRF_WRITE_REG + NRF24L01_Plus_CONFIG, 0x2e);  //0e // IRQ收发完成中断开启,16位CRC,主发送0x0e
        NRF24L01_Plus_Write_Reg(NRF24L01_Plus_FLUSH_TX, 0xff);                              //高位bit4MAX_RT bit5 为1屏蔽TX_DS bit6 RX_DS
        NRF24L01_Plus_Write_Reg(NRF24L01_Plus_FLUSH_RX, 0xff);                              //对于飞控，发送模式2e才是好用的
        NRF24L01_Plus_CSN_LOW;
        txdata = 0x50;
        HAL_SPI_TransmitReceive(&hspi1, &txdata, &rxdata, 1, 300); //读取寄存器内容
        txdata = 0x73;
        HAL_SPI_TransmitReceive(&hspi1, &txdata, &rxdata, 1, 300); //读取寄存器内容
        NRF24L01_Plus_CSN_HIGH;
        NRF24L01_Plus_Write_Reg(NRF24L01_Plus_NRF_WRITE_REG + NRF24L01_Plus_DYNPD, 0x01);
        NRF24L01_Plus_Write_Reg(NRF24L01_Plus_NRF_WRITE_REG + NRF24L01_Plus_FEATURE, 0x07); //06
    }
    NRF24L01_Plus_CE_HIGH;
}

uint8_t NRF24L01_Plus_TxPacket(uint8_t *txbuf, uint8_t len) {
    NRF24L01_Plus_CE_LOW;
    NRF24L01_Plus_Write_Buf(NRF24L01_Plus_WR_TX_PLOAD, txbuf, len); //写数据到TX BUF  32个字节
    NRF24L01_Plus_CE_HIGH; //启动发送
    while(NRF24L01_Plus_IRQ_READ != GPIO_PIN_RESET);   //这里需要等待60ms左右
    //delay_us(10);
    return 0xff;//其他原因发送失败
}
void NRF24L01_Plus_TxPacket_AP(uint8_t *txbuf, uint8_t len) {
    NRF24L01_Plus_CE_LOW;
    NRF24L01_Plus_Write_Buf(NRF24L01_Plus_ACK_PAYLOAD_CMD, txbuf, len); //写数据到TX BUF
    NRF24L01_Plus_CE_HIGH;
}

uint8_t NRF24L01_RxPacketxxx(uint8_t *RxData) {
    uint8_t recvLen = 0;
    uint8_t state = NRF24L01_Read_Reg(NRF24L01_STATUS);     //读NRF24L01状态寄存器
    NRF24L01_Write_Reg(NRF24L01_WRITE_REG | NRF24L01_STATUS, state); //清除中断
    if(state & NRF24L01_STATUS_RX_DR) { // 有数据接收
        RxData[0] = (state & 0x0E) >> 1;// 获取接收的通道
        MYLOG(DEBUG,"RxChannel = %d",RxData[0]); // 获取接收的通道
        recvLen = NRF24L01_Read_Reg(R_RX_PL_WID_CMD);
        if(recvLen > 32) {
            NRF24L01_RxMode();  //重新初始化接收模式
        } else {
            NRF24L01_Read_Buf(NRF24L01_RD_RX_PLOAD, RxData+1, recvLen); //读取数据
            NRF24L01_Write_Reg(NRF24L01_FLUSE_RX, 0xff);              //清除RX FIFO寄存器
            recvLen++;
        }
        hnrf.State = HAL_NRF_STATE_RX;
    }
    return recvLen;
}
uint8_t NRF24L01_Plus_RxPacket(uint8_t *RxData) {
    uint8_t recvLen = 0;
    uint8_t sta;
    sta = NRF24L01_Plus_Read_Reg(NRF24L01_Plus_STATUS);  //读取状态寄存器的值;
    if(sta & NRF24L01_Plus_RX_OK) { //接收到数据
        RxData[0] = (sta & 0x0E) >> 1;// 获取接收的通道
        MYLOG(DEBUG,"RxChannel = %d",RxData[0]); // 获取接收的通道
        recvLen = NRF24L01_Plus_Read_Reg(NRF24L01_Plus_R_RX_PL_WID);
        if(recvLen < 33) {
             NRF24L01_Plus_Read_Buf(NRF24L01_Plus_RD_RX_PLOAD, RxData+1, recvLen); // read receive payload from RX_FIFO buffer
             recvLen++;
             MYLOG(DEBUG, "NRF24L01_Plus_RX_OK");
//           MYLOGHEX(INFO,RxData, recvLen);
        } else NRF24L01_Plus_Write_Reg(NRF24L01_Plus_FLUSH_RX, 0xff); //清空缓冲区
    }
    if(sta & NRF24L01_Plus_TX_OK) {
        MYLOG(DEBUG, "NRF24L01_Plus_TX_OK");
    }
    if(sta & NRF24L01_Plus_MAX_TX) {
        if(sta & 0x01)  //TX FIFO FULL
            NRF24L01_Plus_Write_Reg(NRF24L01_Plus_FLUSH_TX, 0xff);
    }
    NRF24L01_Plus_Write_Reg(NRF24L01_Plus_NRF_WRITE_REG + NRF24L01_Plus_STATUS, sta); //清除TX_DS或MAX_RT中断标志
    return recvLen;
}

void NRF24L01_Plus_Check_Event(void) {
    uint8_t sta;
    sta = NRF24L01_Plus_Read_Reg(NRF24L01_Plus_STATUS);  //读取状态寄存器的值;
    //MYLOG(WARNING, "NRF24L01_Plus_STATUS %x",sta);
    if(sta & NRF24L01_Plus_RX_OK) { //接收到数据
        NRF24L01_Plus_Rx_Len = NRF24L01_Plus_Read_Reg(NRF24L01_Plus_R_RX_PL_WID);
        if(NRF24L01_Plus_Rx_Len < 33) {
            NRF24L01_Plus_Rx_flag = 1;
            NRF24L01_Plus_Read_Buf(NRF24L01_Plus_RD_RX_PLOAD, NRF24L01_Plus_Rx_BUF, NRF24L01_Plus_Rx_Len); // read receive payload from RX_FIFO buffer
             MYLOG(WARNING, "NRF24L01_Plus_RX_OK");
             MYLOGHEX(INFO,NRF24L01_Plus_Rx_BUF, NRF24L01_Plus_Rx_Len);
        } else NRF24L01_Plus_Write_Reg(NRF24L01_Plus_FLUSH_RX, 0xff); //清空缓冲区
    }
    if(sta & NRF24L01_Plus_TX_OK) {
        MYLOG(WARNING, "NRF24L01_Plus_TX_OK");
    }
    if(sta & NRF24L01_Plus_MAX_TX) {
        if(sta & 0x01)  //TX FIFO FULL
            NRF24L01_Plus_Write_Reg(NRF24L01_Plus_FLUSH_TX, 0xff);
    }
    NRF24L01_Plus_Write_Reg(NRF24L01_Plus_NRF_WRITE_REG + NRF24L01_Plus_STATUS, sta); //清除TX_DS或MAX_RT中断标志
}
