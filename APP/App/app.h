#ifndef __APP_H__
#define __APP_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "data.h"
#include "main.h"
#include "data.h"

extern DeviceInformation information;
extern GlobalVariable globalvariable;
enum loglevels {
    LOG_OFF = 0,    //OFF
    LOG_ERROR,      //ERROR
    LOG_WARNING,    //WARNING
    LOG_INFO,       //INFO
    LOG_DEBUG       //DEBUG
};

extern SemaphoreHandle_t xSemaphore,xprintf;              //�����ź������
void Parameter_Save(void) ;

#define ValBit(VAR,Place)    (VAR &   Place)
#define SetBit(VAR,Place)    (VAR |=  Place)
#define ClrBit(VAR,Place)    (VAR &=(~Place))


#define AnalogHardware 0 //0 ʹ��ģ��I2C 1 ʹ��Ӳ����I2C

#define MYDEBUG
#ifdef  MYDEBUG
#define MYLOG(level, fmt, ...)                                                                     \
    do {                                                                                           \
      if (LOG_##level <= globalvariable.Debug) {                                                   \
            printf("[ " #level " ][ %s %05d ][ "fmt" ]\r\n", __FUNCTION__,__LINE__,##__VA_ARGS__); \
        }                                                                                          \
    } while(0)
    
#define MYLOGHEX(level, buf, len)                                                                  \
    do {                                                                                           \
      int32_t i;                                                                                   \
      if (LOG_##level <= globalvariable.Debug) {                                                   \
            printf("[ " #level " ][ %s %05d ]", __FUNCTION__,__LINE__);                            \
            printf("[ %02d ][ ",len);                                                              \
            for(i=0;i<len;i++){                                                                    \
                printf("%02x ",buf[i]);                                                            \
            }                                                                                      \
            printf("]\r\n");                                                                       \
        }                                                                                          \
    } while(0)
    
#else  
#define MYLOG(level, fmt, ...)
#define MYLOGHEX(level, buf, len)
//#define MYLOG(format,...) printf("File: "__FILE__", Line: %05d: "format"\r\n", __LINE__, ##__VA_ARGS__)
//unsigned char code DataStr[]=__DATE__;          //����
//unsigned char code TimeStr[]=__TIME__;          //ʱ��
//printf("The file is %s.\n", __FILE__);          //�ļ�����
//printf("The date is %s.\n", __DATE__);          //��������
//printf("The time is %s.\n", __TIME__);          //����ʱ��
//printf("This is line %d.\n", __LINE__);         //�к�
//printf("This function is %s.\n", __FUNCTION__); //��������
//printf("["%s %05d"]",__FUNCTION__,__LINE__);
#endif

void Task_Create(void);
#ifdef __cplusplus
}
#endif
#endif 
