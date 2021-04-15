#include "user_usart.h"

__IO uint8_t  Uart1_HAL_Buffer[USART_HAL_LEN];//HAL库使用的串口接收缓冲
__IO uint8_t  Uart2_HAL_Buffer[USART_HAL_LEN];//HAL库使用的串口接收缓冲

BSP_UART_NewlineString UART1_NewlineString;
BSP_UART_NewlineString UART2_NewlineString;

AT_DEBUG_STR AtDebug[] = { //所有的字符串多申请一个字节防止打印出错
    {(void *)"AT+EquipmentName",       0, information.EquipmentName,       sizeof(information.EquipmentName)},
    {(void *)"AT+SoftwareVersion",     0, information.SoftwareVersion,     sizeof(information.SoftwareVersion)},
    {(void *)"AT+SoftwareReleaseDate", 0, information.SoftwareReleaseDate, sizeof(information.SoftwareReleaseDate)},
    {(void *)"AT+EquipmentID",         2, information.EquipmentID,         sizeof(information.EquipmentID)},  //需重启
    {(void *)"AT+Password",            2, information.Password,            sizeof(information.Password)},     //需重启
};

AT_DEBUG_STR AtDebugVar[] = { //变量处理
    {(void *)"AT+EquipmentType",       2, &information.EquipmentType,      sizeof(information.EquipmentType)},//需重启
    {(void *)"AT+Channel",             2, &information.Channel,            sizeof(information.Channel)},      //需重启
    {(void *)"AT+UpId",                2, &information.UpId,               sizeof(information.UpId)},         //需重启
    {(void *)"AT+Debugbak",            2, &information.Debugbak,           sizeof(information.Debugbak)},     //需重启

    {(void *)"AT+ThreShold",           2, &information.threshold,          sizeof(information.threshold)},   //需重启
    {(void *)"AT+Accuracy",            2, &information.accuracy,           sizeof(information.accuracy)},     //需重启
    {(void *)"AT+Algorithm",           2, &information.algorithm,          sizeof(information.algorithm)},   //需重启

    {(void *)"AT+Debug",               1, &globalvariable.Debug,           sizeof(globalvariable.Debug)},
    {(void *)"AT+EquipmentFlag",       0, &globalvariable.EquipmentFlag,   sizeof(globalvariable.EquipmentFlag)},
    {(void *)"AT+ADXL345Adj",          1, &globalvariable.ADXL345_adj,     sizeof(globalvariable.ADXL345_adj)},
};

uint8_t AT_AnalysisVar(uint8_t* src, uint16_t srclen, uint8_t offset) {
    AT_DEBUG_STR *atdebug = &AtDebugVar[offset];
    uint16_t cmdlen = IsStartWithSrc3(src, atdebug->cmd, srclen);
    if(cmdlen != 0xffff) {
        cmdlen = strlen((void *)atdebug->cmd);
        //printf("cmdlen = %d\r\n", cmdlen);
        if((*(src + cmdlen)) == '=') {
            int32_t temp = atoi((void *)(src + cmdlen + 1));
            //printf("temp = %d\r\n", temp);
            if((atdebug->rdwr == 1) || (atdebug->rdwr == 2)) {
                if(atdebug->len == 1) {
                    if(temp > 0xff) temp = 0xff;
                    *((uint8_t *)atdebug->parameter) = (uint8_t)temp;
                    printf("\r\n%s=%d,Set OK\r\n", atdebug->cmd, *((uint8_t *)atdebug->parameter));
                }
                if(atdebug->len == 2) {
                    if(temp > 0xffff) temp = 0xffff;
                    *((uint16_t *)atdebug->parameter) = (uint16_t)temp;
                    printf("\r\n%s=%d,Set OK\r\n", atdebug->cmd, *((uint16_t *)atdebug->parameter));
                }
                if(atdebug->len == 4) {
                    *((uint32_t *)atdebug->parameter) = (uint32_t)temp;
                    printf("\r\n%s=%d,Set OK\r\n", atdebug->cmd, *((uint32_t *)atdebug->parameter));
                }
                if(atdebug->rdwr == 2) {
                    Parameter_Save();  //配置并保存参数
                    printf("\r\nAT+Rest APP Rest \r\n");
                    globalvariable.RestFlag = 1;
                }
            } else {
                printf("\r\n%s, Not Suppor Write\r\n", atdebug->cmd);
            }
        } else if((*(src + cmdlen)) == '?') {
            if(atdebug->len == 1) {
                printf("\r\n%s=%d,Get OK\r\n", atdebug->cmd, *((uint8_t *)atdebug->parameter));
            }
            if(atdebug->len == 2) {
                printf("\r\n%s=%d,Get OK\r\n", atdebug->cmd, *((uint16_t *)atdebug->parameter));
            }
            if(atdebug->len == 4) {
                printf("\r\n%s=%d,Get OK\r\n", atdebug->cmd, *((uint32_t *)atdebug->parameter));
            }
        } else {
            printf("\r\n%s,Format Error\r\n", atdebug->cmd);
        }
        return 1;
    }
    return 0;
}

uint8_t AT_Analysis(uint8_t* src, uint16_t srclen, uint8_t offset) {
    AT_DEBUG_STR *atdebug = &AtDebug[offset];
    uint16_t cmdlen = 0;//命令在字符串中的相对位置
    uint16_t len = 0;
    cmdlen = IsStartWithSrc3(src, atdebug->cmd, srclen);
    if(cmdlen != 0xffff) {
//      SPI_Flash_Read((void*)(&gt_parameter), GT_Cfg_Run_ADDR , sizeof(GT_PARAMETER));
        cmdlen = strlen((void *)atdebug->cmd);
        if((srclen - cmdlen - 3) >= (atdebug->len - 1))
            len = atdebug->len - 1;           //正常参数处理
        else
            len = srclen - cmdlen - 3;       //参数超长特殊处理
        if((*(src + cmdlen)) == '=') {
            if((atdebug->rdwr == 1) || (atdebug->rdwr == 2)) {
                memset(atdebug->parameter, 0, atdebug->len);
                memcpy(atdebug->parameter, src + cmdlen + 1, len);
                if(atdebug->rdwr == 2) {
                    Parameter_Save();  //配置并保存参数
                    printf("\r\nAT+Rest APP Rest \r\n");
                    globalvariable.RestFlag = 1;
                }
                printf("\r\n%s=%s,Set OK\r\n", atdebug->cmd, (uint8_t *)atdebug->parameter);
            } else {
                printf("\r\n%s, Not Suppor Write\r\n", atdebug->cmd);
            }
        } else if((*(src + cmdlen)) == '?') {
            printf("\r\n%s=%s,Get OK\r\n", atdebug->cmd, (uint8_t *)atdebug->parameter);
        } else {
            printf("\r\n%s,Format Error\r\n", atdebug->cmd);
        }
        return 1;
    }
    return 0;
}

void AtDebugDo(uint8_t *pbuf, uint8_t pbuflen) {//pbuf  回车换行都带进来了 pbuflen-2
    uint16_t cmdlen, i;
    if(globalvariable.CfgFlag == 1) {
        cmdlen = sizeof(AtDebug) / sizeof(AT_DEBUG_STR);
        for(i = 0; i < cmdlen; i++) {
            if(AT_Analysis(pbuf, pbuflen, i))  return;
        }
        cmdlen = sizeof(AtDebugVar) / sizeof(AT_DEBUG_STR);
        for(i = 0; i < cmdlen; i++) {
            if(AT_AnalysisVar(pbuf, pbuflen, i)) return;
        }
        //停止配置
        //特殊命令
        cmdlen = IsStartWithSrc2(pbuf, (void*)"AT+CfgStop", pbuflen);
        if(cmdlen != 0xffff) {
            printf("\r\nAT+CfgStop APP Running\r\n");
            globalvariable.HKJ_15C = 1;
            globalvariable.CfgFlag = 0;
            return;
        }
        //特殊命令
        cmdlen = IsStartWithSrc2(pbuf, (void*)"AT+Rest", pbuflen);
        if(cmdlen != 0xffff) {
            printf("\r\nAT+Rest APP Rest \r\n");
            globalvariable.RestFlag = 1;
            return;
        }
        cmdlen = IsStartWithSrc2(pbuf, (void*)"AT+Sleep", pbuflen);
        if(cmdlen != 0xffff) {
            printf("\r\nAT+Sleep APP Sleep \r\n");
            globalvariable.SleepFlag = 1;
            return;
        }
    } else {
        //开始配置
        uint8_t ppbuf[32];
        sprintf((void *)ppbuf, "AT+CfgStart=%s", information.Password);
        cmdlen = IsStartWithSrc2(pbuf, ppbuf, pbuflen);
        if(cmdlen != 0xffff) {
            ldo4_disable();
            globalvariable.CfgFlag = 1;
            printf("\r\nAT+CfgStart=%s Password correct start parameter configuration\r\n", information.Password);
            return;
        } else {
            cmdlen = IsStartWithSrc2(pbuf, (void*)"AT+CfgStart=HOPEN", pbuflen);
            if(cmdlen != 0xffff) {
                ldo4_disable();
                globalvariable.CfgFlag = 1;
                printf("\r\nAT+CfgStart=%s Password correct start parameter configuration\r\n", information.Password);
            } else {
                printf("\r\nAT+CfgStart  Password Error\r\n");
            }
            return;
        }
    }
}

void MX_Struct_Test(void) {
    uint16_t temp1 = 0x1234;
    uint8_t  *temp2 = (uint8_t*)&temp1;
    uint8_t A = 0xFF;
    uint8_t B = 0xF9;
    //xt = (short)(((uint16_t)buf[1] << 8) + buf[0]);
    int16_t C = (int16_t)(((uint16_t)A << 8) + B);
    MYLOG(DEBUG, "C=%x %d", C, C);
    MYLOG(DEBUG, "temp1=%x", temp1);
    MYLOG(DEBUG, "temp2[0]=%x", temp2[0]); //低
    MYLOG(DEBUG, "temp2[1]=%x", temp2[1]); //高
    MYLOG(DEBUG, "Upload_DATA=%d", sizeof(Upload_DATA));
    MYLOG(DEBUG, "HKJ_15C_DATA=%d", sizeof(HKJ_15C_DATA));
    MYLOG(DEBUG, "BSP_UART_HKJ_15C=%d", sizeof(BSP_UART_HKJ_15C));
    MYLOG(DEBUG, "JY_901_DATA=%d", sizeof(JY_901_DATA));
    MYLOG(DEBUG, "BSP_UART_JY_901=%d", sizeof(BSP_UART_JY_901));
//    uint8_t value=0x50;
//    value = SetBit(value,0x01);
//    printf("value=%x\r\n",value);
//    value = ClrBit(value,0x01);
//    printf("value=%x\r\n",value);
}

void User_USART_Init(USART_TypeDef *puart, UART_HandleTypeDef *huart, uint32_t pbaudrate) {
    huart->Instance = puart;
    huart->Init.BaudRate = pbaudrate;
    huart->Init.WordLength = UART_WORDLENGTH_8B;
    huart->Init.StopBits = UART_STOPBITS_1;
    huart->Init.Parity = UART_PARITY_NONE;
    huart->Init.Mode = UART_MODE_TX_RX;
    huart->Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart->Init.OverSampling = UART_OVERSAMPLING_16;
    HAL_UART_Init(huart);
    if(huart->Instance == USART1) {
        HAL_UART_Receive_IT(huart, (void *)Uart1_HAL_Buffer, USART_HAL_LEN);//该函数会开启接收中断：标志位UART_IT_RXNE，并且设置接收缓冲以及接收缓冲接收最大数据量
        //__HAL_UART_CLEAR_FLAG(huart, UART_FLAG_TC);
    }
    if(huart->Instance == USART2) {
        HAL_UART_Receive_IT(huart, (void *)Uart2_HAL_Buffer, USART_HAL_LEN);//该函数会开启接收中断：标志位UART_IT_RXNE，并且设置接收缓冲以及接收缓冲接收最大数据量
        //__HAL_UART_CLEAR_FLAG(huart, UART_FLAG_TC);
    }
}

//应用代码
void BSP_NewlineString_printf(BSP_UART_NewlineString * receivedata, uint8_t state) {
    MYLOG(DEBUG, "BSP_NewlineString:<%d><%s>", (receivedata->DataSta & 0X3FFF), receivedata->Data);
    AtDebugDo(receivedata->Data, (receivedata->DataSta & 0X3FFF));
    if(state)receivedata->DataSta = 0;//设置为1清除接收标准
}

void BSP_Receive_NewlineStringXXX(uint8_t pData, BSP_UART_NewlineString * receivedata) {
    if((receivedata->DataSta & 0x8000) == 0) {         //接收未完成
        if(receivedata->DataSta & 0x4000) {            //接收到了0x0d
            if(pData != 0x0a)receivedata->DataSta = 0; //接收错误,重新开始
            else {
                receivedata->DataSta |= 0x8000;        //接收完成了
                receivedata->Data[receivedata->DataSta & 0X3FFF] = 0 ;//添加字符串结束符
                if((receivedata->Data[0] == 'A') && (receivedata->Data[1] == 'T')) { //严格点，防止干扰出错
                    BSP_NewlineString_printf(receivedata, 1);
                }
                receivedata->DataSta = 0;
            }
        } else {                                       //还没收到0X0D
            if(pData == 0x0d)receivedata->DataSta |= 0x4000;
            else {
                receivedata->Data[receivedata->DataSta & 0X3FFF] = pData ;
                receivedata->DataSta++;
                if(receivedata->DataSta > (BSP_UART_NewlineString_MAX - 1))receivedata->DataSta = 0; //接收数据错误,重新开始接收
            }
        }
    }
}
void BSP_Receive_NewlineString(uint8_t pData, BSP_UART_NewlineString * receivedata) {
    if((receivedata->DataSta & 0x8000) == 0) {         //接收未完成
        if((receivedata->DataSta & 0X3FFF) > (BSP_UART_NewlineString_MAX - 2))receivedata->DataSta = 0; //接收数据错误,重新开始接收 58  59 60
        receivedata->Data[receivedata->DataSta & 0X3FFF] = pData ;
        receivedata->DataSta++;
        if((receivedata->DataSta & 0X3FFF) == 1) {
            if(receivedata->Data[0] != 'A') {
                receivedata->DataSta = 0; //接收数据错误,重新开始接收
            }
        } else if((receivedata->DataSta & 0X3FFF) == 2) {
            if((receivedata->Data[1]) != 'T') {
                receivedata->DataSta = 0; //接收数据错误,重新开始接收
            }
        } else if(pData == 0x0a) {
            receivedata->DataSta |= 0x8000;        //接收完成了
            receivedata->Data[receivedata->DataSta & 0X3FFF] = 0 ;//添加字符串结束符
            if((receivedata->Data[0] == 'A') &&
               (receivedata->Data[1] == 'T') &&
               (receivedata->Data[(receivedata->DataSta - 2) & 0X3FFF] == '\r') &&
               (receivedata->Data[(receivedata->DataSta - 1) & 0X3FFF] == '\n')) { //严格点，防止干扰出错
                receivedata->Data[receivedata->DataSta & 0X3FFF] = 0 ;             //加一个字符串结束符
                BSP_NewlineString_printf(receivedata, 1);
            }
            receivedata->DataSta = 0;
        }
    }
}

void BSP_UART_IRQHandler(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size) {
    uint32_t timeout = 0;
//    if(__HAL_UART_GET_FLAG(huart, UART_FLAG_ORE) == SET) {
//        HAL_UART_Receive_IT(huart, (uint8_t *)pData, Size);
//        __HAL_UART_CLEAR_FLAG(huart, UART_FLAG_ORE);
//    }
    while(HAL_UART_GetState(huart) != HAL_UART_STATE_READY) { //等待就绪
        timeout++;////超时处理
        if(timeout > HAL_MAX_DELAY) break;//HAL_MAX_DELAY
    }
    timeout = 0;
    while(HAL_UART_Receive_IT(huart, (uint8_t *)pData, Size) != HAL_OK) { //一次处理完成之后，重新开启中断并设置RxXferCount为1
        timeout++; //超时处理
        if(timeout > HAL_MAX_DELAY) break;//HAL_MAX_DELAY
    }
}
void BSP_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if(huart->Instance == USART1) { //如果是串口1
        //BSP_Receive_NewlineString(Uart1_HAL_Buffer[0], &UART1_NewlineString);  //开启就卡死好奇怪
        if((information.EquipmentType >= HOPEN_UL) && (information.EquipmentType <= HOPEN_DX)) {
            BSP_Receive_JY_901(Uart1_HAL_Buffer[0], &UART1_JY_901);
            if((information.EquipmentType == HOPEN_DL) || (information.EquipmentType == HOPEN_DR) || (information.EquipmentType == HOPEN_DX)) {
                //BSP_Receive_HKJ_15C(Uart1_HAL_Buffer[0], &UART1_HKJ_15C);
            }
        }
    }
    if(huart->Instance == USART2) {
        BSP_Receive_NewlineString(Uart2_HAL_Buffer[0], &UART2_NewlineString);
        if((information.EquipmentType >= HOPEN_UL) && (information.EquipmentType <= HOPEN_DX)) {
            //BSP_Receive_JY_901(Uart2_HAL_Buffer[0], &UART2_JY_901);
            if((information.EquipmentType == HOPEN_DL) || (information.EquipmentType == HOPEN_DR) || (information.EquipmentType == HOPEN_DX)) {
                BSP_Receive_HKJ_15C(Uart2_HAL_Buffer[0], &UART2_HKJ_15C);
            }
        }
    }
}
//串口回调函数
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    BSP_UART_RxCpltCallback(huart);
}
//串口1中断服务程序 __weak
__weak void USART1_IRQHandler(void) {
    #if SYSTEM_SUPPORT_OS       //使用OS
    OSIntEnter();
    #endif
    HAL_UART_IRQHandler(&huart1);   //调用HAL库中断处理公用函数
    BSP_UART_IRQHandler(&huart1, (void *)Uart1_HAL_Buffer, USART_HAL_LEN);
    #if SYSTEM_SUPPORT_OS       //使用OS
    OSIntExit();
    #endif
}
__weak void USART2_IRQHandler(void) {
    #if SYSTEM_SUPPORT_OS       //使用OS
    OSIntEnter();
    #endif
    HAL_UART_IRQHandler(&huart2);   //调用HAL库中断处理公用函数
    BSP_UART_IRQHandler(&huart2, (void *)Uart2_HAL_Buffer, USART_HAL_LEN);
    #if SYSTEM_SUPPORT_OS       //使用OS
    OSIntExit();
    #endif
}

//支持printf
int fputc(int ch, FILE *f) {
    static int FlagStatus = 0;
    if(FlagStatus == 1) return ch; //防止print卡死
    FlagStatus = 1;
    //while(__HAL_UART_GET_FLAG(&huart2, UART_FLAG_TC) == RESET);
    while((huart2.Instance->SR & 0X40) == 0); //循环发送,直到发送完毕
    //HAL_UART_Transmit(&huart2, (void *)&ch, 1, 0xffff);
    //xSemaphoreTake(xSemaphore, portMAX_DELAY);
    //HAL_UART_Transmit(&huart2, (void *)&ch, 1, 200);
    huart2.Instance->DR = (uint8_t)ch;
    FlagStatus = 0;
    //xSemaphoreGive(xSemaphore);
    //HAL_UART_Transmit(&huart2, (void *)&ch, 1, 10);
    return ch;
}
//支持scantf
int fgetc(FILE *f) {
    uint8_t ch;
    //HAL_UART_Receive(&huart2, &ch, 1, 0xffff);
    //xSemaphoreTake(xSemaphore, portMAX_DELAY);
    HAL_UART_Receive(&huart2, &ch, 1, 200);
    //xSemaphoreGive(xSemaphore);
    return ch;
}

////===================================================  printf uart1
//#ifdef __GNUC__
///* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
//set to 'Yes') calls __io_putchar() */
//#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
//#else
//#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
//#endif
//PUTCHAR_PROTOTYPE {
//    UART_HandleTypeDef *mydebug = &huart2;
//    while(__HAL_UART_GET_FLAG(mydebug, UART_FLAG_TC) == RESET);
//    #ifdef __STM32F0xx_HAL_H

//    huart1.Instance->TDR = (uint8_t)ch;
//    #endif

//    #ifdef __STM32F1xx_HAL_H
//    huart1.Instance->DR = (uint8_t)ch;
//    #endif
//    #ifdef __STM32L1xx_HAL_H
//    //while((mydebug->Instance->SR & 0X40) == 0); //循环发送,直到发送完毕
//    mydebug->Instance->DR = (uint8_t)ch;
//    #endif
//    return ch;
//}

//#include "stdio.h"
//int fputc(int ch, FILE *f){
//    ITM_SendChar(ch);
//    return ch;
//}

//#include "stdio.h"
//#define ITM_Port8(n)    (*((volatile unsigned char *)(0xE0000000+4*n)))
//#define ITM_Port16(n)   (*((volatile unsigned short*)(0xE0000000+4*n)))
//#define ITM_Port32(n)   (*((volatile unsigned long *)(0xE0000000+4*n)))

//#define DEMCR           (*((volatile unsigned long *)(0xE000EDFC)))
//#define TRCENA          0x01000000

//int fputc(int ch, FILE *f) {
//    if(DEMCR & TRCENA) {
//        while(ITM_Port32(0) == 0);
//        ITM_Port8(0) = ch;
//    }
//    return(ch);
//}
