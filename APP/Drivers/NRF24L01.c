/************************************************************************************
*                                           NRF24L01库函数
*************************************************************************************/
/*------------------------- 更新 -----------------------------------------
2015-10-13
------------------------------------------------------------------*/
#include "NRF24L01.h"
//uint8_t TX_ADDRESS[NRF24L01_ADR_LEN] = {192, 168, 1, 1, 1}; //发送地址 发送通道地址
//uint8_t RX_ADDR_P0[NRF24L01_ADR_LEN] = {192, 168, 1, 1, 1}; //P0 接收地址
//uint8_t RX_ADDR_P1[NRF24L01_ADR_LEN] = {192, 168, 1, 1, 2}; //P1 接收地址

uint8_t TX_RX_ADDR[6][NRF24L01_ADR_LEN] = {
    {190, 168, 1, 1, 0},//频道0地址 生产
    {191, 168, 1, 1, 0},//频道1地址 上肢左
    {192, 168, 1, 1, 0},//频道2地址 上肢右
    {193, 168, 1, 1, 0},//频道3地址 下肢左
    {194, 168, 1, 1, 0},//频道4地址 下肢右
    {195, 168, 1, 1, 0},//频道5地址 预留
};

NRF24_HandleTypeDef hnrf;
uint8_t nrfRXbuff[NRF_RXFIFO_MAX];
uint8_t nrfTXbuff[NRF_TXFIFO_MAX];
/**********************************************************************************/
/*                                          初始化                                */
/**********************************************************************************/

/**
* @brief  初始化参数
* @param  None
* @retval None
*/
void NRF24L_Init(void) {
    ldo3_enable();
    HAL_Delay(50);
    hnrf.Hspi = &hspi1;
    hnrf.RxAddress[0] = TX_RX_ADDR[0];//生产  当使用通道0时，发送地址，和接收地址必须一样。返回值才是0X20，正常。 当使用通道0时，将发送地址和接收地址改一样的。就能正常了。
    hnrf.RxAddress[1] = TX_RX_ADDR[1];//上肢左
    hnrf.RxAddress[2] = TX_RX_ADDR[2];//上肢右
    hnrf.RxAddress[3] = TX_RX_ADDR[3];//下肢左
    hnrf.RxAddress[4] = TX_RX_ADDR[4];//下肢右
    hnrf.RxAddress[5] = TX_RX_ADDR[5];//预留设备
    if(information.EquipmentType >= HOPEN_ECG) {
        hnrf.TxAddress = TX_RX_ADDR[0];
    } else {
        hnrf.TxAddress = TX_RX_ADDR[information.EquipmentType];
    }
    if(NRF24L01_Mode == 1) {
        hnrf.TxAddress = TX_RX_ADDR[0];//调试先固定
    }
    hnrf.Channel   = 40;  // 通道
    hnrf.Flag      =   NRF24L01_AUTO_RX | NRF24L01_AUTO_ACK; //  | NRF24L01_AUTO_SlEEP
    //设置自动重发间隔时间:500us + 86us;最大自动重发次数:10次
    //NRF24L01_Mode =1 和重发次数和时间没关系，一次固定15ms左右  等待对方设备ACK    100/4=25<15  所以速度可以跟上  分频率发送15*N很稳定
    //NRF24L01_Mode =0 重发1次1ms左右
    hnrf.ARTCount  = 3; // 重发次数     15 1 9ms  69ms    2ms
    //hnrf.ARTdelay  = 2; // 延时 (N+1) * 250us = 1000us
    hnrf.ARTdelay  = 2 * information.EquipmentType; // 延时 (N+1) * 250us = 1000us          2
    hnrf.DataRates = HAL_NRF_DR_1M;   // 速率
    hnrf.RFPA      = HAL_NRF_PA_0DBM; // 功率
    if((information.EquipmentType > HOPEN_DEF) && (information.EquipmentType < HOPEN_ECG)) {
        hnrf.AutoAckPe = HAL_NRF_ENAA_P0 | (1 << information.EquipmentType); //0或者本通道
    } else {
        hnrf.AutoAckPe = HAL_NRF_ENAA_P0 | HAL_NRF_ENAA_P1 | HAL_NRF_ENAA_P2 | HAL_NRF_ENAA_P3 | HAL_NRF_ENAA_P4 | HAL_NRF_ENAA_P5; // 自动应答通道
    }
    // 中断模式需要以下Fifo
    // 接收缓存
    hnrf.pRxBuffPtr = nrfRXbuff;
    hnrf.RxSize = NRF_RXFIFO_MAX;
    hnrf.RxHead = 0;
    hnrf.RxTail = 0;
    // 发送缓存
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
* @brief  NRF24L01芯片初始化函数
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
    // 1 > 使能通道的自动应答 自动应答通道P0 P1
    if(hnrf.Flag & NRF24L01_AUTO_ACK) {
        NRF24L01_Write_Reg(NRF24L01_WRITE_REG | NRF24L01_EN_AA, 0x3f);//0x3f  hnrf.AutoAckPe
    }
    // 2 > 使能通道的接收地址
    NRF24L01_Write_Reg(NRF24L01_WRITE_REG | NRF24L01_EN_RXADDR, 0x3f);//0x3f
    // 3 > 设置地址长度为 NRF24L01_ADR_LEN
    NRF24L01_Write_Reg(NRF24L01_WRITE_REG | NRF24L01_SETUP_AW, HAL_NRF_ADW_5);
    // 4 > 设置自动重发间隔时间;最大自动重发次数
    NRF24L01_Write_Reg(NRF24L01_WRITE_REG | NRF24L01_SETUP_RETR, ((hnrf.ARTdelay << 4) | hnrf.ARTCount)) ;
    // 5 > 设置RF频道RFCH
    NRF24L01_Write_Reg(NRF24L01_WRITE_REG | NRF24L01_RF_CH, hnrf.Channel);
    // 6 > 设置TX发射功率参数,速率,低噪声增益开启
    NRF24L01_Write_Reg(NRF24L01_WRITE_REG | NRF24L01_RF_SETUP, (hnrf.DataRates | hnrf.RFPA | 0x01));
    // 7 > 设置TX自动应答地址 接收P0 TX_ADDRESS
    //NRF24L01_Write_Buf(NRF24L01_WRITE_REG | NRF24L01_RX_ADDR_P0, hnrf.RxAddress[0], NRF24L01_ADR_LEN);
    NRF24L01_Write_Buf(NRF24L01_WRITE_REG | NRF24L01_RX_ADDR_P0, hnrf.TxAddress, NRF24L01_ADR_LEN); //0通道的接收地址必须和发送地址相同
    // 8 > 设置本机RX节点地址 接收P1 RX_ADDRESS
    NRF24L01_Write_Buf(NRF24L01_WRITE_REG | NRF24L01_RX_ADDR_P1, hnrf.RxAddress[1], NRF24L01_ADR_LEN);
    NRF24L01_Write_Buf(NRF24L01_WRITE_REG | NRF24L01_RX_ADDR_P2, hnrf.RxAddress[2], 1);
    NRF24L01_Write_Buf(NRF24L01_WRITE_REG | NRF24L01_RX_ADDR_P3, hnrf.RxAddress[3], 1);
    NRF24L01_Write_Buf(NRF24L01_WRITE_REG | NRF24L01_RX_ADDR_P4, hnrf.RxAddress[4], 1);
    NRF24L01_Write_Buf(NRF24L01_WRITE_REG | NRF24L01_RX_ADDR_P5, hnrf.RxAddress[5], 1);
    // 9 > 设置有效数据长度 32
    NRF24L01_Write_Reg(NRF24L01_WRITE_REG | NRF24L01_RX_PW_P0, NRF24L01_RX_PLOAD_LEN);
    NRF24L01_Write_Reg(NRF24L01_WRITE_REG | NRF24L01_RX_PW_P1, NRF24L01_RX_PLOAD_LEN);
    NRF24L01_Write_Reg(NRF24L01_WRITE_REG | NRF24L01_RX_PW_P2, NRF24L01_RX_PLOAD_LEN);
    NRF24L01_Write_Reg(NRF24L01_WRITE_REG | NRF24L01_RX_PW_P3, NRF24L01_RX_PLOAD_LEN);
    NRF24L01_Write_Reg(NRF24L01_WRITE_REG | NRF24L01_RX_PW_P4, NRF24L01_RX_PLOAD_LEN);
    NRF24L01_Write_Reg(NRF24L01_WRITE_REG | NRF24L01_RX_PW_P5, NRF24L01_RX_PLOAD_LEN);
    // 10 > 清除RX FIFO寄存器
    NRF24L01_Write_Reg(NRF24L01_WRITE_REG | NRF24L01_FLUSE_RX, NRF24L01_NOP);
    // 11> TX地址
    NRF24L01_Write_Buf(NRF24L01_WRITE_REG | NRF24L01_TX_ADDR, hnrf.TxAddress, NRF24L01_ADR_LEN);
    // 12> 清除TX FIFO寄存器
    NRF24L01_Write_Reg(NRF24L01_WRITE_REG | NRF24L01_FLUSE_TX, NRF24L01_NOP);
    // 13> 清除状态
    NRF24L01_Write_Reg(NRF24L01_WRITE_REG | NRF24L01_STATUS, 0x70);
    //---------- 新芯片功能  ----------
    //默认进入接收模式
    //选择通道0动态数据包长度
    //  NRF24L01_Write_Reg(NRF24L01_WRITE_REG | NRF24L01_DYNPD,0X01);
    //  //使能动态数据包长度,AUTO_ACK模式
    //  NRF24L01_Write_Reg(NRF24L01_WRITE_REG | NRF24L01_FEATURE,0x05);
    //
    //  if (hnrf.Flag & NRF24L01_AUTO_SlEEP){
    //      NRF24L01_SleepMode();
    //  }else if(hnrf.Flag & NRF24L01_AUTO_RX){
    //      NRF24L01_RxMode();  // 接收模式
    //  }
    // 配置中断
}

/**
* @brief  NRF24L01 SPI通信检查
* @param
* @retval 0:错误 1:正常
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
* @brief  读取Fifo状态寄存器，顺便返回状态
* @param
* @retval 16bit = 高字节：Fifo状态寄存器 ， 低字节：状态寄存器
*/
uint16_t NRF24L01_Status(void) {
    uint8_t txdata[2] = {0}, rxdata[2] = {0};
    txdata[0] = NRF24L01_FIFO_STATUS;
    txdata[1] = 0;
    _BSP_NRF24_CSN_LOW;          //使能SPI传输
    #if (HAL_SPI != 0)
    HAL_SPI_TransmitReceive(hnrf.Hspi, txdata, rxdata, 2, 300); //读取寄存器内容
    #else
    BSP_SPI_Buff(hnrf.Hspi, txdata, rxdata, 2);
    #endif
    _BSP_NRF24_CSN_HIGH;          //禁止SPI传输
    return(*((uint16_t*)rxdata)); //返回
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
uint8_t NRF24L01_Write_Reg(uint8_t reg, uint8_t value) {
    uint8_t txdata[2], rxdata[2];
    txdata[0] = reg;
    txdata[1] = value;
    _BSP_NRF24_CSN_LOW;                 //使能SPI传输
    #if (HAL_SPI != 0)
    HAL_SPI_TransmitReceive(hnrf.Hspi, txdata, rxdata, 2, 300); //读取寄存器内容
    #else
    BSP_SPI_Buff(hnrf.Hspi, txdata, rxdata, 2);
    #endif
    _BSP_NRF24_CSN_HIGH;                 //禁止SPI传输
    return(rxdata[0]);                  //返回状态值
}

/**
* @brief  读NRF24L01寄存器
* @param  reg: 寄存器地址
* @retval 返回寄存器值
*/
uint8_t NRF24L01_Read_Reg(uint8_t reg) {
    uint8_t txdata[2] = {0}, rxdata[2] = {0};
    txdata[0] = reg;
    txdata[1] = 0;
    _BSP_NRF24_CSN_LOW;          //使能SPI传输
    #if (HAL_SPI != 0)
    HAL_SPI_TransmitReceive(hnrf.Hspi, txdata, rxdata, 2, 300); //读取寄存器内容
    #else
    BSP_SPI_Buff(hnrf.Hspi, txdata, rxdata, 2);
    #endif
    _BSP_NRF24_CSN_HIGH;          //禁止SPI传输
    return(rxdata[1]);           //返回
}

/**
* @brief  读取缓冲区数据
* @param  reg: 寄存器地址
* @param  pBuf: 返回数据缓存指针
* @param  len: 读取长度
* @retval None
*/
void NRF24L01_Read_Buf(uint8_t reg, uint8_t *pBuf, uint8_t len) {
    len++;  // 增加一个状态位置
    uint8_t txdata[34] = {0}, rxdata[34] = {0};
    txdata[0] = reg;
    _BSP_NRF24_CSN_LOW;          //使能SPI传输
    #if (HAL_SPI != 0)
    HAL_SPI_TransmitReceive(hnrf.Hspi, txdata, rxdata, len, 600); //读取寄存器内容
    #else
    BSP_SPI_Buff(hnrf.Hspi, txdata, rxdata, len);
    #endif
    _BSP_NRF24_CSN_HIGH;          //禁止SPI传输
    memcpy(pBuf, rxdata + 1, len - 1);
}

/**
* @brief  向缓冲区写入数据
* @param  pBuf: 写入数据缓存指针
* @param  len: 写入长度
* @retval 返回读到的状态值
*/
uint8_t NRF24L01_Write_Buf(uint8_t reg, uint8_t *pBuf, uint8_t len) {
    uint8_t txdata[34] = {0}, rxdata[34] = {0};
    txdata[0] = reg;
    memcpy(txdata + 1, pBuf, len);
    len++;
    _BSP_NRF24_CSN_LOW;          //使能SPI传输
    #if (HAL_SPI != 0)
    HAL_SPI_TransmitReceive(hnrf.Hspi, txdata, rxdata, len, 600);
    #else
    BSP_SPI_Buff(hnrf.Hspi, txdata, rxdata, len);
    #endif
    _BSP_NRF24_CSN_HIGH;       //关闭SPI传输
    return(rxdata[0]);         //返回读到的状态值
}

/**********************************************************************************/
/*                                          工作模式                               */
/**********************************************************************************/

/**
* @brief  NRF24L01进入发送模式
* @param
* @retval None
*/
void NRF24L01_TxMode1(void) {
    _BSP_NRF24_CE_LOW;
    NRF24L01_Write_Reg(NRF24L01_WRITE_REG | NRF24L01_CONFIG, NRF24L01_CONFIG_TX);
    NRF24L01_Write_Reg(NRF24L01_WRITE_REG | NRF24L01_FLUSE_TX, NRF24L01_NOP);  //清除TX FIFO寄存器
    //_BSP_NRF24_CE_HIGH();
    hnrf.State = HAL_NRF_STATE_TX;
}

void NRF24L01_TxMode2(void) {
    uint8_t txdata, rxdata ;
    _BSP_NRF24_CE_LOW;
    NRF24L01_Write_Reg(NRF24L01_WRITE_REG | NRF24L01_CONFIG, NRF24L01_CONFIG_TX2);//0e // IRQ收发完成中断开启,16位CRC,主发送0x0e
    NRF24L01_Write_Reg(NRF24L01_WRITE_REG | NRF24L01_FLUSE_TX, NRF24L01_NOP);   //高位bit4MAX_RT bit5 为1屏蔽TX_DS bit6 RX_DS
    NRF24L01_Write_Reg(NRF24L01_WRITE_REG | NRF24L01_FLUSE_RX, NRF24L01_NOP);   //对于飞控，发送模式2e才是好用的
    _BSP_NRF24_CSN_LOW;
    txdata = 0x50;
    HAL_SPI_TransmitReceive(&hspi1, &txdata, &rxdata, 1, 300); //读取寄存器内容
    txdata = 0x73;
    HAL_SPI_TransmitReceive(&hspi1, &txdata, &rxdata, 1, 300); //读取寄存器内容
    _BSP_NRF24_CSN_HIGH;
    //NRF24L01_Write_Reg(NRF24L01_WRITE_REG | NRF24L01_DYNPD, 0x01);                           //发送设置全通道  1 << information.EquipmentType
//    NRF24L01_Write_Reg(NRF24L01_WRITE_REG | NRF24L01_DYNPD, 1 << information.EquipmentType);   //发送设置全通道  1 << information.EquipmentType
    NRF24L01_Write_Reg(NRF24L01_WRITE_REG | NRF24L01_DYNPD, 0x3f);   //发送设置全通道  1 << information.EquipmentType
    NRF24L01_Write_Reg(NRF24L01_WRITE_REG | NRF24L01_FEATURE, 0x07); //06
    //_BSP_NRF24_CE_HIGH;
    hnrf.State = HAL_NRF_STATE_TX;
}

/**
* @brief  NRF24L01进入接收模式
* @param
* @retval None
*/
void NRF24L01_RxMode1(void) {
    _BSP_NRF24_CE_LOW;
    NRF24L01_Write_Reg(NRF24L01_WRITE_REG | NRF24L01_CONFIG, NRF24L01_CONFIG_RX); //配置基本工作模式的参数;PWR_UP,EN_CRC,16BIT_CRC,接收模式
    NRF24L01_Write_Reg(NRF24L01_WRITE_REG | NRF24L01_FLUSE_RX, NRF24L01_NOP);     //清除RX FIFO寄存器
    _BSP_NRF24_CE_HIGH;
    hnrf.State = HAL_NRF_STATE_RX;
}

void NRF24L01_RxMode2(void) {
    uint8_t txdata, rxdata ;
    _BSP_NRF24_CE_LOW;
    NRF24L01_Write_Reg(NRF24L01_WRITE_REG | NRF24L01_FLUSE_TX, NRF24L01_NOP);   //高位bit4MAX_RT bit5 为1屏蔽TX_DS bit6 RX_DS
    NRF24L01_Write_Reg(NRF24L01_WRITE_REG | NRF24L01_FLUSE_RX, NRF24L01_NOP);   //对于飞控，发送模式2e才是好用的
    NRF24L01_Write_Reg(NRF24L01_WRITE_REG | NRF24L01_CONFIG, NRF24L01_CONFIG_RX2); //0e // IRQ收发完成中断开启,16位CRC,主发送0x0e
    //高位为3表示屏蔽发送中断
    _BSP_NRF24_CSN_LOW;
    txdata = 0x50;
    HAL_SPI_TransmitReceive(&hspi1, &txdata, &rxdata, 1, 300); //读取寄存器内容
    txdata = 0x73;
    HAL_SPI_TransmitReceive(&hspi1, &txdata, &rxdata, 1, 300); //读取寄存器内容
    _BSP_NRF24_CSN_HIGH;
    //NRF24L01_Write_Reg(NRF24L01_WRITE_REG | NRF24L01_DYNPD, 0x01);   //这里设置通道
//    NRF24L01_Write_Reg(NRF24L01_WRITE_REG | NRF24L01_DYNPD, hnrf.AutoAckPe);   //这里设置通道
    NRF24L01_Write_Reg(NRF24L01_WRITE_REG | NRF24L01_DYNPD, 0x3f);    //这里设置通道
    NRF24L01_Write_Reg(NRF24L01_WRITE_REG | NRF24L01_FEATURE, 0x07); //06
    _BSP_NRF24_CE_HIGH;
    hnrf.State = HAL_NRF_STATE_RX;
}
/**
* @brief  NRF24L01进入掉电模式
* @param
* @retval None
*/
void NRF24L01_SleepMode(void) {
    _BSP_NRF24_CE_LOW;
    NRF24L01_Write_Reg(NRF24L01_WRITE_REG | NRF24L01_CONFIG, NRF24L01_CONFIG_SLEEP); //配置基本工作模式的参数;PWR_UP,EN_CRC,16BIT_CRC,接收模式
    NRF24L01_Write_Reg(NRF24L01_WRITE_REG | NRF24L01_FLUSE_RX, NRF24L01_NOP);        //清除RX FIFO寄存器
    NRF24L01_Write_Reg(NRF24L01_WRITE_REG | NRF24L01_FLUSE_TX, NRF24L01_NOP);        //清除TX FIFO寄存器
    hnrf.State = HAL_NRF_STATE_SEELP;
}

/**********************************************************************************/
/*                                      动态修改参数                               */
/**********************************************************************************/

/**
* @brief  设置 RF_CH
* @param
* @retval None
*/
void NRF24L01_SetChange(uint8_t ch) {
    _BSP_NRF24_CE_LOW;
    hnrf.Channel = (ch & 0x7F);
    // 5 > 设置RF频道RFCH
    NRF24L01_Write_Reg(NRF24L01_WRITE_REG | NRF24L01_RF_CH, hnrf.Channel);
}

/**
* @brief  获取 RF_CH
* @param
* @retval 返回 RF_CH
*/
uint8_t NRF24L01_GetChange(void) {
    _BSP_NRF24_CE_LOW;
    // 5 > 设置RF频道RFCH
    hnrf.Channel = NRF24L01_Read_Reg(NRF24L01_RF_CH);
    return hnrf.Channel;
}

/**
* @brief  设置TX地址
* @param  新TX地址数组指针，5个字节
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
* @brief  获取TX地址
* @param  回传TX地址数组指针
* @retval None
*/
void NRF24L01_GetTxAddress(uint8_t *addr) {
    _BSP_NRF24_CE_LOW;
    NRF24L01_Read_Buf(NRF24L01_TX_ADDR, hnrf.TxAddress, NRF24L01_ADR_LEN);
    memcpy(addr, hnrf.TxAddress,  NRF24L01_ADR_LEN);
}

/**
* @brief  设置RX地址
* @param  新RX地址数组指针，5个字节
* @retval None
*/
void NRF24L01_SetRxAddress(uint8_t *addr, uint8_t channel) {
    _BSP_NRF24_CE_LOW;
//    if(channel == 0)  memcpy(RX_ADDRESS, addr,  NRF24L01_ADR_LEN);
//    if(channel == 1)  memcpy(RX_ADDRESS, addr,  NRF24L01_ADR_LEN);
    if(channel < 6) {
        memcpy(hnrf.RxAddress[channel], addr,  NRF24L01_ADR_LEN);//把地址设置进对应的位置
        NRF24L01_Write_Buf(NRF24L01_WRITE_REG | (NRF24L01_RX_ADDR_P0 + channel), hnrf.RxAddress[channel], NRF24L01_ADR_LEN);
    }
}

/**
* @brief  获取RX地址
* @param  回传RX地址数组指针
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
/*                                      中断处理函数                               */
/**********************************************************************************/

/**
* @brief  NRF24L01中断处理
* @param
* @retval None
*/
void NRF24L01_Handler(void) {
    uint8_t state = NRF24L01_Read_Reg(NRF24L01_STATUS);     //读NRF24L01状态寄存器
    NRF24L01_Write_Reg(NRF24L01_WRITE_REG | NRF24L01_STATUS, state); //清除中断
    //发送成功中断
    if(state & NRF24L01_STATUS_TX_DS) {
        if(hnrf.TxHead != hnrf.TxTail) { // 判断数据缓冲区是否还有数据未发送完
            NRF24L01_SendFifo();
        } else {
            hnrf.TxHead = hnrf.TxTail = 0;
            if(hnrf.Flag & NRF24L01_AUTO_SlEEP) {
                NRF24L01_SleepMode();
            } else if(hnrf.Flag & NRF24L01_AUTO_RX)      {
                NRF24L01_RxMode1();  //重新初始化接收模式
            }
        }
    }
    //达到最大重发次数中断
    if(state & NRF24L01_STATUS_MAX_RT) {
        NRF24L01_Write_Reg(NRF24L01_WRITE_REG | NRF24L01_FLUSE_TX, NRF24L01_NOP); //清除TX FIFO寄存器
        //NRF24L01_FLAG &= ~NRF24L01_BUSY;  //BUSY标志置0
        hnrf.TxTail = 0;
        hnrf.TxHead = 0;
        NRF24L01_Write_Reg(NRF24L01_WRITE_REG | NRF24L01_RF_CH, hnrf.Channel);  // 清除重试计数器
        if(hnrf.Flag & NRF24L01_AUTO_SlEEP) {
            NRF24L01_SleepMode();
        } else if(hnrf.Flag & NRF24L01_AUTO_RX)      {
            NRF24L01_RxMode1();  //重新初始化接收模式
        }
    }
    //接收到数据中断
    if(state & NRF24L01_STATUS_RX_DR) {
        hnrf.State = HAL_NRF_STATE_BUSY_RX;
        NRF24L01_RecvFifo();
    }
}

/**
* @brief  接收数据到缓存
* @param
* @retval None
*/
void    NRF24L01_RecvFifo(void) {
    uint8_t recvLen, i;
    uint8_t data[NRF24L01_RX_PLOAD_LEN] = {0};
    recvLen = NRF24L01_Read_Reg(R_RX_PL_WID_CMD);
    NRF24L01_Read_Buf(NRF24L01_RD_RX_PLOAD, data, recvLen); //读取数据
    NRF24L01_Write_Reg(NRF24L01_FLUSE_RX, 0xff); //清除RX FIFO寄存器
    for(i = 0; i < recvLen; i++) {
        hnrf.pRxBuffPtr[hnrf.RxHead++] = data[i];
        if(hnrf.RxHead >= hnrf.RxSize) hnrf.RxHead = 0;
    }
    // 将数据放到Buff
    hnrf.State = HAL_NRF_STATE_RX;
}

/**
* @brief  获取接收数据
* @param  rxData 输出缓存
* @retval 返回数据长度
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
* @brief  等待发送缓存数据
* @param
* @retval None
*/
void NRF24L01_SendFifo(void) {
    uint8_t i, data[NRF24L01_TX_PLOAD_LEN] = {0};
    _BSP_NRF24_CE_LOW;
    hnrf.State = HAL_NRF_STATE_BUSY_TX;
    NRF24L01_Write_Reg(NRF24L01_WRITE_REG | NRF24L01_RF_CH, hnrf.Channel);  // 清除重试计数器
    for(i = 0; i < NRF24L01_TX_PLOAD_LEN; i++) {
        data[i] = hnrf.pTxBuffPtr[hnrf.TxTail++];
        if(hnrf.TxTail >= hnrf.TxSize) hnrf.TxTail = 0;
        if(hnrf.TxTail == hnrf.TxHead) break;
    }
    NRF24L01_Write_Buf(NRF24L01_WR_TX_PLOAD, data, NRF24L01_TX_PLOAD_LEN);
    _BSP_NRF24_CE_HIGH;
}

/**
* @brief  启动中断发送功能
* @param
* @retval None
*/
void NRF24L01_SendData(uint8_t *txData, uint16_t len) {
    _BSP_NRF24_CE_LOW;  // 待机
    memcpy(hnrf.pTxBuffPtr, txData, len);   //填充数据
    hnrf.TxHead = len;  // 需要发送数量
    hnrf.TxTail = 0;    // 未发送数量
    NRF24L01_TxMode1();  // 发送模式
    NRF24L01_SendFifo();//
}

/**********************************************************************************/
/*                                      轮询处理函数                               */
/**********************************************************************************/
/**
* @brief  发送一个32个字节的数据包；
* @param  TxData  发送的数据缓存，长度由 NRF24L01_TX_PLOAD_LEN 定义
* @retval NRF24L01_STATUS_TX_DS  发送成功
NRF24L01_STATUS_MAX_RT 重发失败
0xff    其他失败
*/
//uint8_t NRF24L01_TxPacket1(uint8_t *TxData, uint8_t len) {
//    //uint8_t state;
//    uint8_t resend = 0;
//    uint8_t reStatus = 0xff;
//    uint32_t tickstart = HAL_GetTick();
//    _BSP_NRF24_CE_LOW;
//    hnrf.State = HAL_NRF_STATE_BUSY_TX;
//    // 写入数据
//    NRF24L01_Write_Buf(NRF24L01_WR_TX_PLOAD, TxData, len);
//SEND:
//    NRF24L01_Write_Reg(NRF24L01_WRITE_REG | NRF24L01_STATUS, 0xf0); //清除中断
//    // 启动发送
//    _BSP_NRF24_CE_HIGH;
//    // 等待中断
//    while(_BSP_NRF24_IRQ_READ != GPIO_PIN_RESET){   //这里需要等待60ms左右
//      osDelay(1);
//    }
//    //HAL_Delay(50);
//    //state = NRF24L01_Read_Reg(NRF24L01_STATUS);     //读NRF24L01状态寄存器
//    NRF24L01_Read_Reg(NRF24L01_STATUS);     //读NRF24L01状态寄存器
//    uint8_t fifostate = NRF24L01_Read_Reg(NRF24L01_FIFO_STATUS);        //读NRF24L01状态寄存器
//    _BSP_NRF24_CE_LOW;
//    //printf("fifostate = %x resend = %d\r\n", (fifostate&0x30),resend);
//    switch(fifostate & 0x30) {
//        case 0x10:    // TX FIFO empty
//            reStatus =  NRF24L01_STATUS_TX_DS;  // 发送成功
//            break;
//        case 0x00:    // Data in TX FIFO, but not full
////            if(resend < 10) {
////                resend ++;
////                goto SEND;
////            }
//            //HAL_Delay(1);
//            NRF24L01_Write_Reg(NRF24L01_WRITE_REG | NRF24L01_FLUSE_TX, NRF24L01_NOP); //清除TX FIFO寄存器
//            reStatus =  NRF24L01_STATUS_MAX_RT;//达到最大重发次数中断
//            break;
//        case 0x20:    //        RX FIFO full
//            if(resend < 10) {
//                resend ++;
//                goto SEND;
//            }
//            NRF24L01_Write_Reg(NRF24L01_WRITE_REG | NRF24L01_FLUSE_TX, NRF24L01_NOP); //清除TX FIFO寄存器
//            reStatus =  NRF24L01_STATUS_MAX_RT;//达到最大重发次数中断
//            break;
//    }
//    /*
//    if(state & NRF24L01_STATUS_TX_DS)
//    {
//        reStatus =  NRF24L01_STATUS_TX_DS;  // 发送成功
//    }else   if(state & NRF24L01_STATUS_MAX_RT)  {
//        if (resend<10){
//            resend ++;
//                NRF24L01_Write_Reg(NRF24L01_WRITE_REG | NRF24L01_STATUS,state);//清除中断
//            goto SEND;
//        }
//        NRF24L01_Write_Reg(NRF24L01_WRITE_REG | NRF24L01_FLUSE_TX,NRF24L01_NOP);  //清除TX FIFO寄存器
//        reStatus =  NRF24L01_STATUS_MAX_RT;//达到最大重发次数中断
//    }else{

//        reStatus = state;
//        state = NRF24L01_Read_Reg(NRF24L01_STATUS);
//        printnrfreg();
//    }
//    */
//    //printf("RESEND:%d \t delay::%d\t",resend,( HAL_GetTick()-tickstart));
//    NRF24L01_Write_Reg(NRF24L01_WRITE_REG | NRF24L01_STATUS, 0xf0); //清除中断
//    //printf("RESEND:%d  \tdelay:%d  \t", resend, (HAL_GetTick() - tickstart));
//    return reStatus;    //其他情况
//}
uint8_t NRF24L01_TxPacket1(uint8_t *TxData, uint8_t len) {
    uint8_t count = 0, sta;
//    while(NRF24L01_Read_Reg(NRF24L01_CD)){ //检测信道是否被占用，占用就延时会儿  不稳定
//        if(count++>3) break;         //其他原因发送失败
//        osDelay(1);
//    }
    if(NRF24L01_Read_Reg(NRF24L01_CD)) {
        osDelay(1);//验收不痛的时间
        //osDelay(information.EquipmentType);//验收不痛的时间
    }
    _BSP_NRF24_CE_LOW;
    //NRF24L01_Write_Buf(NRF24L01_WR_TX_PLOAD, TxData, len);
    NRF24L01_Write_Buf(NRF24L01_WR_TX_PLOAD, TxData, 32);
    _BSP_NRF24_CE_HIGH;                              //启动发送
//    count = 0;
    while(_BSP_NRF24_IRQ_READ != GPIO_PIN_RESET);
//    while(_BSP_NRF24_IRQ_READ != GPIO_PIN_RESET){    //等待发送完成
//        if(count++>3) break;                         //其他原因发送失败
//        osDelay(1);
//    }
    sta = NRF24L01_Read_Reg(NRF24L01_STATUS);        //读取状态寄存器的值
    NRF24L01_Write_Reg(NRF24L01_WRITE_REG | NRF24L01_STATUS, sta); //清除TX_DS或MAX_RT中断标志
    if(sta & NRF24L01_STATUS_MAX_RT) {               //达到最大重发次数
        NRF24L01_Write_Reg(NRF24L01_WRITE_REG | NRF24L01_FLUSE_TX, NRF24L01_NOP); //清除TX FIFO寄存器
        return NRF24L01_STATUS_MAX_RT;
    }
    if(sta & NRF24L01_STATUS_TX_DS) {                //发送完成
        return NRF24L01_STATUS_TX_DS;
    }
    return 0xff;//其他原因发送失败
}
uint8_t NRF24L01_TxPacket2(uint8_t *txbuf, uint8_t len) {
    _BSP_NRF24_CE_LOW;
    NRF24L01_Write_Buf(NRF24L01_WR_TX_PLOAD, txbuf, len);     //写数据到TX BUF  32个字节
    _BSP_NRF24_CE_HIGH; //启动发送
    //HAL_Delay(1);
    return NRF24L01_STATUS_TX_DS;//其他原因发送失败
}
void NRF24L01_TxPacket1_AP(uint8_t *txbuf, uint8_t len) {
}
uint8_t NRF24L01_TxPacket2_AP(uint8_t *txbuf, uint8_t len) {
    _BSP_NRF24_CE_LOW;
    NRF24L01_Write_Buf(W_ACK_PAYLOAD_CMD, txbuf, len);     //写数据到TX BUF  32个字节
    _BSP_NRF24_CE_HIGH;
    return NRF24L01_STATUS_TX_DS;//其他原因发送失败
}
/**
* @brief  接收数据 ；执行该函数前，请先调用 NRF24L01_RxMode();
* @param    RxData  接收到的数据缓存
* @retval   返回接收到的数据长度
*/
uint8_t NRF24L01_RxPacket1(uint8_t *RxData) {
    uint8_t recvLen = 0, state;
    if(_BSP_NRF24_IRQ_READ != GPIO_PIN_RESET) return recvLen;//没收到数据直接退出，防止一直快速读寄存器
    state = NRF24L01_Read_Reg(NRF24L01_STATUS);     //读NRF24L01状态寄存器
    NRF24L01_Write_Reg(NRF24L01_WRITE_REG | NRF24L01_STATUS, state); //清除中断
    if(state & NRF24L01_STATUS_RX_DR) { // 有数据接收
        RxData[0] = (state & 0x0E) >> 1;// 获取接收的通道
        MYLOG(DEBUG, "RxChannel = %d", RxData[0]); // 获取接收的通道
        recvLen = NRF24L01_Read_Reg(R_RX_PL_WID_CMD);
        if(recvLen > 32) {
            NRF24L01_RxMode1();  //重新初始化接收模式
        } else {
            NRF24L01_Read_Buf(NRF24L01_RD_RX_PLOAD, RxData + 1, recvLen); //读取数据
            NRF24L01_Write_Reg(NRF24L01_FLUSE_RX, 0xff);              //清除RX FIFO寄存器
            recvLen++;
        }
        hnrf.State = HAL_NRF_STATE_RX;
    }
    return recvLen;
}

uint8_t NRF24L01_RxPacket2(uint8_t *RxData) {
    uint8_t recvLen = 0, state;
    if(_BSP_NRF24_IRQ_READ != GPIO_PIN_RESET) return recvLen;//没收到数据直接退出，防止一直快速读寄存器
    //while(_BSP_NRF24_IRQ_READ != GPIO_PIN_RESET);    //等待发送完成
    state = NRF24L01_Read_Reg(NRF24L01_STATUS);     //读NRF24L01状态寄存器
    if(state & NRF24L01_STATUS_RX_DR) { //接收到数据
        RxData[0] = (state & 0x0E) >> 1;// 获取接收的通道
        MYLOG(DEBUG, "RxChannel = %d", RxData[0]); // 获取接收的通道
        recvLen = NRF24L01_Read_Reg(R_RX_PL_WID_CMD);
        if(recvLen < 33) {
            NRF24L01_Read_Buf(NRF24L01_RD_RX_PLOAD, RxData + 1, recvLen); //读取数据
            recvLen++;
            MYLOG(DEBUG, "NRF24L01_Plus_RX_OK");
//           MYLOGHEX(INFO,RxData, recvLen);
        } else  NRF24L01_Write_Reg(NRF24L01_WRITE_REG | NRF24L01_FLUSE_RX, NRF24L01_NOP); //清空缓冲区
    }
    if(state & NRF24L01_STATUS_TX_DS) {
        MYLOG(DEBUG, "NRF24L01_Plus_TX_OK");
    }
    if(state & NRF24L01_STATUS_MAX_RT) {
        if(state & 0x01)  //TX FIFO FULL
            NRF24L01_Write_Reg(NRF24L01_WRITE_REG | NRF24L01_FLUSE_TX, NRF24L01_NOP);
    }
    NRF24L01_Write_Reg(NRF24L01_WRITE_REG | NRF24L01_STATUS, state); //清除中断
    return recvLen;
}
/**********************************************************************************/
/*                                          调试函数                               */
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
