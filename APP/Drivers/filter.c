#include "filter.h"
#include "stdlib.h"
/****************************************************************************************************************/
//1. 结构体类型定义
//typedef struct {
//    float LastP;//上次估算协方差 初始化值为0.02
//    float Now_P;//当前估算协方差 初始化值为0
//    float out;  //卡尔曼滤波器输出 初始化值为0
//    float Kg;   //卡尔曼增益 初始化值为0
//    float Q;    //过程噪声协方差 初始化值为0.001
//    float R;    //观测噪声协方差 初始化值为0.543
//} KFP; //Kalman Filter parameter

//2. 以高度为例 定义卡尔曼结构体并初始化参数
//Q：规程噪声，Q增大，动态响应变快，收敛稳定性变坏
//R：测试噪声，R增大，动态响应变慢，收敛稳定性变好
KFP KFP_height    = {0.02, 0, 0, 0, 0.001, 0.543};
KFP KFP_adxl345_x = {0.02, 0, 0, 0, 0.004, 0.3};
KFP KFP_adxl345_y = {0.02, 0, 0, 0, 0.004, 0.3};
KFP KFP_adxl345_z = {0.02, 0, 0, 0, 0.004, 0.3};
/**
 *卡尔曼滤波器
 *@param KFP *kfp 卡尔曼结构体参数
 *   float input 需要滤波的参数的测量值（即传感器的采集值）
 *@return 滤波后的参数（最优值）
 */
short kalmanFilter(KFP *kfp, short input) {
    //>>1、预测协方差方程：k时刻系统估算协方差 = k-1时刻的系统协方差 + 过程噪声协方差
    kfp->Now_P = kfp->LastP + kfp->Q;
    //>>2、卡尔曼增益方程：卡尔曼增益 = k时刻系统估算协方差 / （k时刻系统估算协方差 + 观测噪声协方差）
    kfp->Kg = kfp->Now_P / (kfp->Now_P + kfp->R);
    //>>3、更新最优值方程：k时刻状态变量的最优值 = 状态变量的预测值 + 卡尔曼增益 * （测量值 - 状态变量的预测值）
    kfp->out = kfp->out + kfp->Kg * (input - kfp->out); //因为这一次的预测值就是上一次的输出值
    //>>4、更新协方差方程: 本次的系统协方差付给 kfp->LastP 威下一次运算准备。
    kfp->LastP = (1 - kfp->Kg) * kfp->Now_P;
    return kfp->out;
}
/****************************************************************************************************************/

// 一阶互补算法
float First_order_filter(float Com_angle, float angle_pt, float angle, float dt) {
    float A, K = 0.075;                            // 对加速度计取值的权重
    A = K / (K + dt);
    Com_angle = A * (Com_angle + angle_pt * dt) + (1 - A) * angle;
    return Com_angle;
}
// 二阶互补算法
float Second_order_filter(float Com2_angle, float angle_pt, float angle, float dt) {
    float x1, x2, K = 0.5;
    static float y1;
    x1 = (angle - Com2_angle) * K * K;
    y1 = y1 + x1 * dt;
    x2 = y1 + 2 * K * (angle - Com2_angle) + angle_pt;
    Com2_angle = Com2_angle + x2 * dt;
    return Com2_angle;
}

#define Q_angle 0.01      // 角度数据置信度
#define Q_omega 0.0003    // 角速度数据置信度
#define R_angle 0.01      // 方差噪声
// 卡尔曼滤波
float Kalman_filter(float Klm_angle, float angle_pt, float angle, float dt) {
    static float bias;
    static float P_00, P_01, P_10, P_11;
    float K_0, K_1;
    Klm_angle += (angle_pt - bias) * dt;       // 先验估计
    P_00 += -(P_10 + P_01) * dt + Q_angle * dt;
    P_01 += -P_11 * dt;
    P_10 += -P_11 * dt;
    P_11 += +Q_omega * dt;                     // 先验估计误差协方差
    K_0 = P_00 / (P_00 + R_angle);
    K_1 = P_10 / (P_00 + R_angle);
    bias += K_1 * (angle - Klm_angle);
    Klm_angle += K_0 * (angle - Klm_angle);    // 后验估计
    P_00 -= K_0 * P_00;
    P_01 -= K_0 * P_01;
    P_10 -= K_1 * P_00;
    P_11 -= K_1 * P_01;                        // 后验估计误差协方差
    return Klm_angle;
}
/*****************************************************
*函数名称：kalman_filter
*函数功能：ADC_滤波
*入口参数：ADC_Value
*出口参数：kalman_adc
*****************************************************/
uint16_t kalman_filter(uint16_t ADC_Value) {
    float x_k1_k1, x_k_k1;
    static float ADC_OLD_Value;
    float Z_k;
    static float P_k1_k1;
    static float Q = 0.0001;//Q：规程噪声，Q增大，动态响应变快，收敛稳定性变坏
    static float R = 0.005; //R：测试噪声，R增大，动态响应变慢，收敛稳定性变好
    static float Kg = 0;
    static float P_k_k1 = 1;
    float kalman_adc;
    static float kalman_adc_old = 0;
    Z_k = ADC_Value;
    x_k1_k1 = kalman_adc_old;
    x_k_k1 = x_k1_k1;
    P_k_k1 = P_k1_k1 + Q;
    Kg = P_k_k1 / (P_k_k1 + R);
    kalman_adc = x_k_k1 + Kg * (Z_k - kalman_adc_old);
    P_k1_k1 = (1 - Kg) * P_k_k1;
    P_k_k1 = P_k1_k1;
    ADC_OLD_Value = ADC_Value;
    kalman_adc_old = kalman_adc;
    return kalman_adc;
}
//为上面优化算法：在采样值与优化值相差大于某值时采用一阶滞后滤波算法，小于该值时采用卡尔曼滤波算法；
uint16_t kalman_filter_y(uint16_t ADC_Value) {
    float x_k1_k1, x_k_k1;
    static float ADC_OLD_Value;
    float Z_k;
    static float P_k1_k1;
    static float Q = 0.0001;
    static float R = 5;
    static float Kg = 0;
    static float P_k_k1 = 1;
    float kalman_adc;
    static float kalman_adc_old = 0;
    Z_k = ADC_Value;
    if(abs((int)(kalman_adc_old - ADC_Value)) >= 10) {
        x_k1_k1 = ADC_Value * 0.382 + kalman_adc_old * 0.618;//一阶滞后滤波法 y(n) = q*x(n) + (1-q)*y(n-1)
    } else {
        x_k1_k1 = kalman_adc_old;
    }
    x_k_k1 = x_k1_k1;
    P_k_k1 = P_k1_k1 + Q;
    Kg = P_k_k1 / (P_k_k1 + R);
    kalman_adc = x_k_k1 + Kg * (Z_k - kalman_adc_old);
    P_k1_k1 = (1 - Kg) * P_k_k1;
    P_k_k1 = P_k1_k1;
    ADC_OLD_Value = ADC_Value;
    kalman_adc_old = kalman_adc;
    return kalman_adc;
}


/***************************************************************************************************************************/
//https://blog.csdn.net/Dancer__Sky/article/details/81504778?utm_medium=distribute.pc_relevant.none-task-blog-BlogCommendFromMachineLearnPai2-6.control&depth_1-utm_source=distribute.pc_relevant.none-task-blog-BlogCommendFromMachineLearnPai2-6.control
//>1，均值滤波器---滤波
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#define FILTER_CNT          5
typedef struct {
    short x;
    short y;
    short z;
} axis_info_t ;
typedef struct filter_avg {
    axis_info_t info[FILTER_CNT];
    unsigned char count;
} filter_avg_t;

//读取xyz数据存入均值滤波器，存满进行计算，滤波后样本存入sample,如何读取存满就不多说了。
//static void filter_calculate(filter_avg_t *filter, axis_info_t *sample) {
//    unsigned int i;
//    short x_sum = 0, y_sum = 0, z_sum = 0;
//    for(i = 0; i < FILTER_CNT; i++) {
//        x_sum += filter->info[i].x;
//        y_sum += filter->info[i].y;
//        z_sum += filter->info[i].z;
//    }
//    sample->x = x_sum / FILTER_CNT;
//    sample->y = y_sum / FILTER_CNT;
//    sample->z = z_sum / FILTER_CNT;
//    //HAL_UART_Transmit(&huart2, (uint8_t *)&sample->x, 2, 200);
//    //HAL_UART_Transmit(&huart2, (uint8_t *)&sample->y, 2, 200);
//    //HAL_UART_Transmit(&huart2, (uint8_t *)&sample->z, 2, 200);
//}
static void filter_calculate(filter_avg_t *filter, axis_info_t *sample) {
    unsigned int i;
    short x_min =  1000, y_min =  1000, z_min =  1000;
    short x_max = -1000, y_max = -1000, z_max = -1000;
    short x_sum = 0, y_sum = 0, z_sum = 0;
    for(i = 0; i < FILTER_CNT; i++) {
        x_max = MAX(x_max, filter->info[i].x);
        y_max = MAX(y_max, filter->info[i].y);
        z_max = MAX(z_max, filter->info[i].z);
        x_min = MIN(x_min, filter->info[i].x);
        y_min = MIN(y_min, filter->info[i].y);
        z_min = MIN(z_min, filter->info[i].z);
        x_sum += filter->info[i].x;
        y_sum += filter->info[i].y;
        z_sum += filter->info[i].z;
    }
    x_sum = x_sum - x_max - x_min;
    y_sum = y_sum - y_max - y_min;
    z_sum = z_sum - z_max - z_min;
    sample->x = x_sum / (FILTER_CNT - 2);
    sample->y = y_sum / (FILTER_CNT - 2);
    sample->z = z_sum / (FILTER_CNT - 2);
    //MYLOG(WARNING, "<x y z %5.2f><%04d %04d %04d>", sqrt((sample->x) * (sample->x) + (sample->y) * (sample->y) + (sample->z) * (sample->z)), sample->x, sample->y, sample->z);
    //卡尔曼滤波 波形平滑度变好
//    sample->x = kalmanFilter(&KFP_adxl345_x,sample->x);
//    sample->y = kalmanFilter(&KFP_adxl345_y,sample->y);
//    sample->z = kalmanFilter(&KFP_adxl345_z,sample->z);
    //HAL_UART_Transmit(&huart2, (uint8_t *)&sample->x, 2, 200);
    //HAL_UART_Transmit(&huart2, (uint8_t *)&sample->y, 2, 200);
    //HAL_UART_Transmit(&huart2, (uint8_t *)&sample->z, 2, 200);
}
//>2，动态阈值
#define SAMPLE_SIZE   50

typedef struct {
    axis_info_t newmax;
    axis_info_t newmin;
    axis_info_t oldmax;
    axis_info_t oldmin;
} peak_value_t;

const axis_info_t defmin = { 1000,  1000,  1000};
const axis_info_t defmax = {-1000, -1000, -1000};
static void peak_value_init(peak_value_t *peak) {
    peak->newmax = defmax;
    peak->newmin = defmin;
}
//在动态阈值结构体初始化时，一定要将max的值都赋值为最小值，min赋值为最大值，这样才有利于动态更新。
static void peak_update(peak_value_t *peak, axis_info_t *cur_sample) {
    static unsigned int sample_size = 0;
    sample_size ++;
    if(sample_size > SAMPLE_SIZE) {
        /*采样达到50个，更新一次*/
        sample_size = 1;
        peak->oldmax = peak->newmax;
        peak->oldmin = peak->newmin;
        //初始化
        peak_value_init(peak);
    }
    peak->newmax.x = MAX(peak->newmax.x, cur_sample->x);
    peak->newmax.y = MAX(peak->newmax.y, cur_sample->y);
    peak->newmax.z = MAX(peak->newmax.z, cur_sample->z);
    peak->newmin.x = MIN(peak->newmin.x, cur_sample->x);
    peak->newmin.y = MIN(peak->newmin.y, cur_sample->y);
    peak->newmin.z = MIN(peak->newmin.z, cur_sample->z);
}
//>3，动态精度
#define ABS(a) (0 - (a)) > 0 ? (-(a)) : (a)
#define DYNAMIC_PRECISION               information.accuracy*2/3       /*动态精度 30*/

/*一个线性移位寄存器，用于过滤高频噪声*/
typedef struct slid_reg {
    axis_info_t new_sample;
    axis_info_t old_sample;
} slid_reg_t;

static char slid_update(slid_reg_t *slid, axis_info_t *cur_sample) {
    char res = 0;
    if(MYabs(cur_sample->x, slid->new_sample.x) > DYNAMIC_PRECISION) {
        slid->old_sample.x = slid->new_sample.x;
        slid->new_sample.x = cur_sample->x;
        res = 1;
    } else {
        slid->old_sample.x = slid->new_sample.x;
    }
    if(MYabs(cur_sample->y, slid->new_sample.y) > DYNAMIC_PRECISION) {
        slid->old_sample.y = slid->new_sample.y;
        slid->new_sample.y = cur_sample->y;
        res = 1;
    } else {
        slid->old_sample.y = slid->new_sample.y;
    }
    if(MYabs(cur_sample->z, slid->new_sample.z) > DYNAMIC_PRECISION) {
        slid->old_sample.z = slid->new_sample.z;
        slid->new_sample.z = cur_sample->z;
        res = 1;
    } else {
        slid->old_sample.z = slid->new_sample.z;
    }
    return res;
}

//static char FindPeaksAndTroughs1(char *pSta,short *old_sample, short *cur_sample){
//    char res = 0;
//    //pSta  0上升  1下降
//    if(*cur_sample>*old_sample+DYNAMIC_PRECISION){       //上升
//        if(*pSta!=0) res = 1;                            //得到波谷
//        *pSta = 0;
//    } else if(*cur_sample<*old_sample-DYNAMIC_PRECISION){//下降
//        if(*pSta!=1) res = 1;                            //得到波峰
//        *pSta = 0;
//    }
//    *old_sample = *cur_sample;
//    return res;
//}
//static char slid_update1(slid_reg_t *slid, axis_info_t *cur_sample){
//    char res = 0;
//    static short x = 0,y = 0,z = 256;
//    static char pSta_x = 0,pSta_y = 0,pSta_z = 0;//0上升  1下降
//    if(FindPeaksAndTroughs1(&pSta_x,&x,&cur_sample->x)){
//        slid->old_sample.x = slid->new_sample.x;
//        slid->new_sample.x = cur_sample->x;
//        res = 1;
//    } else {
//        slid->old_sample.x = slid->new_sample.x;
//    }
//    if(FindPeaksAndTroughs1(&pSta_y,&y,&cur_sample->y)){
//        slid->old_sample.y = slid->new_sample.y;
//        slid->new_sample.y = cur_sample->y;
//        res = 1;
//    } else {
//        slid->old_sample.y = slid->new_sample.y;
//    }
//    if(FindPeaksAndTroughs1(&pSta_z,&z,&cur_sample->z)){
//        slid->old_sample.z = slid->new_sample.z;
//        slid->new_sample.z = cur_sample->z;
//        res = 1;
//    } else {
//        slid->old_sample.z = slid->new_sample.z;
//    }
//    return res;
//}

//static char FindPeaksAndTroughs2(char *pSta,short *new_sample,short *old_sample, short *cur_sample){
//    char res = 0;
//    //pSta  0上升  1下降
//    if((*pSta == 0) && (*cur_sample <= *new_sample-DYNAMIC_PRECISION)){
//        *pSta = 1;//得到波峰
//        *old_sample = *new_sample;
//        *new_sample = *cur_sample;
//        res = 1;
//    } else if((*pSta == 1) && (*cur_sample > *new_sample+DYNAMIC_PRECISION)){
//        *pSta = 0;//得到波谷
//        *old_sample = *new_sample;
//        *new_sample = *cur_sample;
//        res = 1;
//    } else {
//        *old_sample = *new_sample;
//    }
//    return res;
//}
//static char slid_update2(slid_reg_t *slid, axis_info_t *cur_sample){
//    char res = 0;
//    static char pSta_x = 0,pSta_y = 0,pSta_z = 0;//0上升  1下降
//    res |= FindPeaksAndTroughs2(&pSta_x,&slid->new_sample.x,&slid->old_sample.x,&cur_sample->x);
//    res |= FindPeaksAndTroughs2(&pSta_y,&slid->new_sample.y,&slid->old_sample.y,&cur_sample->y);
//    res |= FindPeaksAndTroughs2(&pSta_z,&slid->new_sample.z,&slid->old_sample.z,&cur_sample->z);
//    return res;
//}
//>4,步伐判断

#define MOST_ACTIVE_NULL                0        /*未找到最活跃轴*/
#define MOST_ACTIVE_X                   1        /*最活跃轴X*/
#define MOST_ACTIVE_Y                   2        /*最活跃轴Y*/
#define MOST_ACTIVE_Z                   3        /*最活跃轴Z*/

#define ACTIVE_PRECISION                information.threshold*2/3      /*活跃轴最小变化值*/

/*判断当前最活跃轴*/
static char is_most_active(peak_value_t *peak) {
    char res = MOST_ACTIVE_NULL;
    short x_change = MYabs(peak->newmax.x, peak->newmin.x);
    short y_change = MYabs(peak->newmax.y, peak->newmin.y);
    short z_change = MYabs(peak->newmax.z, peak->newmin.z);
    if(x_change > y_change && x_change > z_change && x_change >= ACTIVE_PRECISION) {
        res = MOST_ACTIVE_X;
    } else if(y_change > x_change && y_change > z_change && y_change >= ACTIVE_PRECISION) {
        res = MOST_ACTIVE_Y;
    } else if(z_change > x_change && z_change > y_change && z_change >= ACTIVE_PRECISION) {
        res = MOST_ACTIVE_Z;
    }
    return res;
}
unsigned int step_cntt = 0;
/*判断是否走步*/
static void detect_step(peak_value_t *peak, slid_reg_t *slid, axis_info_t *cur_sample) {
    static uint32_t time = 0 ;
    uint32_t value;
    //static unsigned int step_cnt = 0;
    char res = is_most_active(peak);
    switch(res) {
        case MOST_ACTIVE_NULL: {
            //fix
            break;
        }
        case MOST_ACTIVE_X: {
            short threshold_x = (peak->oldmax.x + peak->oldmin.x) / 2;
            if(slid->old_sample.x > threshold_x && slid->new_sample.x < threshold_x) {
                value = HAL_GetTick() ;
                value = DelayGetInterval(&value, &time);
                time = HAL_GetTick();
                if((value < 200) || (value > 3000)) {
                    MYLOG(WARNING, "time window =%d ", value);
                    break;
                }
                step_cntt ++;
                ADXL345_Counter_steps();
            }
            break;
        }
        case MOST_ACTIVE_Y: {
            short threshold_y = (peak->oldmax.y + peak->oldmin.y) / 2;
            if(slid->old_sample.y > threshold_y && slid->new_sample.y < threshold_y) {
                value = HAL_GetTick() ;
                value = DelayGetInterval(&value, &time);
                time = HAL_GetTick();
                if((value < 200) || (value > 3000)) {
                    MYLOG(WARNING, "time window =%d ", value);
                    break;
                }
                step_cntt ++;
                ADXL345_Counter_steps();
            }
            break;
        }
        case MOST_ACTIVE_Z: {
            short threshold_z = (peak->oldmax.z + peak->oldmin.z) / 2;
            if(slid->old_sample.z > threshold_z && slid->new_sample.z < threshold_z) {
                value = HAL_GetTick() ;
                value = DelayGetInterval(&value, &time);
                time = HAL_GetTick();
                if((value < 200) || (value > 3000)) {
                    MYLOG(WARNING, "time window =%d ", value);
                    break;
                }
                step_cntt ++;
                ADXL345_Counter_steps();
            }
            break;
        }
        default:
            break;
    }
}

filter_avg_t filter;//采集缓存
axis_info_t  sample;//滤波后的数据
peak_value_t peak = {//动态阀值
    .newmin = {1023, 1023, 1023},
    .newmax = {0, 0, 0},
};
slid_reg_t   slid;  //动态精度

void Pedometer(int16_t x, int16_t y, int16_t z) {
    static unsigned int step = 0;
    //>1 需要先采集数据
    axis_info_t *pbuf = &filter.info[(filter.count++) % FILTER_CNT];
    pbuf->x = x;
    pbuf->y = y;
    pbuf->z = z;
//    HAL_UART_Transmit(&huart2, (uint8_t *)&z, 2, 200);
//    ADXL345_RD_XYZ(&pbuf->x, &pbuf->y, &pbuf->z);
//    ADXL345_RD_XYZ(&filter.info[filter.count].x, &filter.info[filter.count].y, &filter.info[filter.count].z);
//    filter.count = (filter.count +1 ) % FILTER_CNT;
    //>2 均值滤波器
    filter_calculate(&filter, &sample);
    //>3 动态阈值
    peak_update(&peak, &sample);
    //>4 动态精度
    if(slid_update(&slid, &sample)) {
        //>5 步伐判断
        detect_step(&peak, &slid, &sample);
        if(step != step_cntt) {
            step = step_cntt;
            //MYLOG(WARNING, "Pedometer = %d", step); //输出步数
        }
    }
}

