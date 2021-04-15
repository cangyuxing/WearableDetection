#include "user_adc.h"

void User_Adc_Init(ADC_TypeDef *padc, ADC_HandleTypeDef *hadc) {
    hadc->Instance = padc;
    hadc->Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
    hadc->Init.Resolution = ADC_RESOLUTION_12B;
    hadc->Init.DataAlign = ADC_DATAALIGN_RIGHT;
    hadc->Init.ScanConvMode = ADC_SCAN_DISABLE;
    hadc->Init.EOCSelection = ADC_EOC_SEQ_CONV;
    hadc->Init.LowPowerAutoWait = ADC_AUTOWAIT_DISABLE;
    hadc->Init.LowPowerAutoPowerOff = ADC_AUTOPOWEROFF_DISABLE;
    hadc->Init.ChannelsBank = ADC_CHANNELS_BANK_A;
    hadc->Init.ContinuousConvMode = DISABLE;
    hadc->Init.NbrOfConversion = 1;
    hadc->Init.DiscontinuousConvMode = DISABLE;
    hadc->Init.ExternalTrigConv = ADC_SOFTWARE_START;
    hadc->Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
    hadc->Init.DMAContinuousRequests = DISABLE;
    if(HAL_ADC_Init(hadc) != HAL_OK) {
        Error_Handler();
    }
    HAL_ADCEx_InjectedStart(hadc); //校准ADC
}

//获得ADC值
//ch: 通道值 0~16，取值范围为：ADC_CHANNEL_0~ADC_CHANNEL_16
//返回值:转换结果
uint16_t Get_Adc(uint32_t ch) {
    ADC_ChannelConfTypeDef   sConfig;
    sConfig.Channel = ch;                             //通道
    sConfig.Rank = ADC_REGULAR_RANK_1;                //第1个序列，序列1
    sConfig.SamplingTime = ADC_SAMPLETIME_4CYCLES;    //采样时间
    HAL_ADC_ConfigChannel(&hadc, &sConfig);          //通道配置
    HAL_ADC_Start(&hadc);                            //开启ADC
    HAL_ADC_PollForConversion(&hadc, 10);            //轮询转换
    return (uint16_t)HAL_ADC_GetValue(&hadc);        //返回最近一次ADC1规则组的转换结果
}

//获取指定通道的转换值，取times次,然后平均
//times:获取次数
//返回值:通道ch的times次转换结果平均值
uint16_t Get_Adc_Average(uint32_t ch, uint8_t times) {
    uint32_t temp_val = 0;
    uint8_t t;
    for(t = 0; t < times; t++) {
        temp_val += Get_Adc(ch);
        //HAL_Delay(5);
        osDelay(5);
    }
    return temp_val / times;
}
