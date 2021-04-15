#include "protocoldata.h"

void delay_us(uint32_t nus) {
    //12M�б�baiʾ1us��6M�б�ʾ2us��24M�б�ʾ0.5us(΢�32
//    __nop();__nop();__nop();__nop();__nop();__nop();
//    __nop();__nop();__nop();__nop();__nop();__nop();
    uint32_t i = 0;
    while(nus--) {
        i = 10;  //10
        while(i--) ;
    }
}

void delay_ms(uint32_t nms) {
    uint32_t i = 0;
    while(nms--) {
        i = 12000;
        while(i--) ;
    }
}

uint8_t DelayTimeOut(uint32_t *time, uint32_t delay) {
    uint32_t nowtime = HAL_GetTick();
    if(nowtime > (*time)) {
        if(delay > nowtime - (*time)) {
            return 0;
        } else {
            return 1;
        }
    } else { //ʱ�䷭ת
        if(delay > 0xffffffff - (*time) + nowtime + 1) {
            return 0;
        } else {
            return 1;
        }
    }
}

uint32_t DelayGetInterval(uint32_t *nowtime, uint32_t *oldtime) {
    uint32_t time = 0;
    if(*nowtime > (*oldtime)) {
        time = *nowtime - (*oldtime);
    } else {  //ʱ�䷭ת
        time = 0xffffffff - (*oldtime) + (*nowtime) + 1 ;
    }
    return time;
}

uint32_t MYabs(int32_t a, int32_t b) {
    if(a > b) {
        return a - b;
    } else {
        return b - a;
    }
}

void BSP_QueueSend(QueueHandle_t Handle, uint8_t *pbuf) {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xQueueSendToFrontFromISR(Handle, pbuf,  &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

void Upload_Data_Check(Upload_DATA *data) {
    uint8_t *pbuf = (uint8_t *)data;
    uint16_t i = 0, sum = 0;
    for(i = 1; i < (data->len + 4); i++) {
        sum += pbuf[i];
    }
    data->sum = sum;
}

void Upload_Data_Check_V_0(Upload_DATA_V1_0 *data) {
    uint8_t *pbuf = (uint8_t *)data;
    uint16_t i = 0, sum = 0;
    for(i = 1; i < 30; i++) {
        sum += pbuf[i];
    }
    data->sum = sum;
    MYLOG(DEBUG, "Check = %02x", data->sum);
}
/***********************************************************************************/
//ͨ�ù��ߺ���
uint32_t ModeChange32(uint32_t pdata) {
    union {
        uint32_t s;
        uint8_t  c[4];
    } A, B;
    A.s = pdata;
    B.c[0] = A.c[3];
    B.c[1] = A.c[2];
    B.c[2] = A.c[1];
    B.c[3] = A.c[0];
    return B.s;
}

uint16_t ModeChange16(uint16_t pdata) {
    union {
        uint16_t s;
        uint8_t  c[2];
    } A, B;
    A.s = pdata;
    B.c[0] = A.c[1];
    B.c[1] = A.c[0];
    return B.s;
}

uint16_t MyStrLen(uint8_t *Dst) {
    return strlen((char *)Dst);
}
uint16_t MyMemcmp(uint8_t* str1, uint8_t* str2, uint8_t len) {
    for(uint8_t i = 0; i < len; i++) {
        if(str1[i] != str2[i])  return 0xffff;
    }
    return 0;
}
uint16_t IsStartWithchar(uint8_t * src, uint8_t s) {
    char* p;
    p = strchr((char *)src, s);
    if(p == NULL) {
        return 0xffff;
    }
    return (p - (char *)src);
}
uint16_t IsStartWithSrc(uint8_t* src, uint8_t* s) {                 //Ҳ������strstrʵ��
    char* p;
    p = strstr((char *)src, (char *)s);
    if(p == NULL) {
        return 0xffff;
    }
    return (p - (char *)src);
}
uint16_t IsStartWithSrc2(uint8_t* src, uint8_t* s, uint16_t len) {  //Ҳ������strstrʵ��
    char* p;
    uint8_t len2;
    len2 = strlen((char *)s);
    //printf("<%d>=%s<%d>=%s\r\n",len,src,len2,s);
    if(len < len2 + 2) {
        return 0xffff;
    }
    p = strstr((char *)src, (char *)s);
    if(p == NULL) {
        return 0xffff;
    }
    if((len - len2) < (p - (char *)src)) {
        return 0xffff;
    }
    return (p - (char *)src);
}

uint16_t IsStartWithSrc3(uint8_t* src, uint8_t* s, uint16_t len) {  //Ҳ������strstrʵ��
    uint8_t len1 = len - 2; //ȥ���س�����
    uint8_t len2, i = 0;
    len2 = strlen((char *)s);
    //printf("<%d>=%s<%d>=%s\r\n",len,src,len2,s);
    while(len2 <= len1--) {
        if(src[i++] == s[0]) {
//          if(memcmp(&src[i-1],s,len2)==0){//���ԱȽϻ��ֹ�ڴ��������
            if(MyMemcmp(&src[i - 1], s, len2) == 0) { //���ԱȽϻ��ֹ�ڴ��������
                return i - 1;
            }
        }
    }
    return 0xffff;
}


uint8_t CalU8CRC(uint8_t *pOutBuf, uint16_t OutLen) { //����λ�������  ����CRCУ��ֵ����
    uint16_t i;
    uint8_t result = 0;
    if(!pOutBuf) {
        return 0;
    }
    for(i = 0; i < OutLen; i++) {
        result = result ^ pOutBuf[i];
    }
    //MYLOG(WARNING, "CRC = <%x>", result);
    return result;
}

uint32_t GetCheckSum32(uint8_t *Src, uint32_t Len) {
    uint32_t CheckSum = 0;
    uint32_t i = 0;
    for(i = 0; i < Len; i++) {
        CheckSum += Src[i];
    }
    return CheckSum;
}
