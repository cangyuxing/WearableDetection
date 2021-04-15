#ifndef __TASK_UPLOAD_H__
#define __TASK_UPLOAD_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

extern Upload_calculation UploadRx; //接收数据
extern QueueHandle_t upload_Queue;

void upload_Queue_send(void);
void Upload_calculation_init(void);
void Task_upload_Create(void);

#ifdef __cplusplus
}
#endif
#endif
