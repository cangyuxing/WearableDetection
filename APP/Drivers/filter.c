#include "filter.h"
#include "stdlib.h"
/****************************************************************************************************************/
//1. �ṹ�����Ͷ���
//typedef struct {
//    float LastP;//�ϴι���Э���� ��ʼ��ֵΪ0.02
//    float Now_P;//��ǰ����Э���� ��ʼ��ֵΪ0
//    float out;  //�������˲������ ��ʼ��ֵΪ0
//    float Kg;   //���������� ��ʼ��ֵΪ0
//    float Q;    //��������Э���� ��ʼ��ֵΪ0.001
//    float R;    //�۲�����Э���� ��ʼ��ֵΪ0.543
//} KFP; //Kalman Filter parameter

//2. �Ը߶�Ϊ�� ���忨�����ṹ�岢��ʼ������
//Q�����������Q���󣬶�̬��Ӧ��죬�����ȶ��Ա仵
//R������������R���󣬶�̬��Ӧ�����������ȶ��Ա��
KFP KFP_height    = {0.02, 0, 0, 0, 0.001, 0.543};
KFP KFP_adxl345_x = {0.02, 0, 0, 0, 0.004, 0.3};
KFP KFP_adxl345_y = {0.02, 0, 0, 0, 0.004, 0.3};
KFP KFP_adxl345_z = {0.02, 0, 0, 0, 0.004, 0.3};
/**
 *�������˲���
 *@param KFP *kfp �������ṹ�����
 *   float input ��Ҫ�˲��Ĳ����Ĳ���ֵ�����������Ĳɼ�ֵ��
 *@return �˲���Ĳ���������ֵ��
 */
short kalmanFilter(KFP *kfp, short input) {
    //>>1��Ԥ��Э����̣�kʱ��ϵͳ����Э���� = k-1ʱ�̵�ϵͳЭ���� + ��������Э����
    kfp->Now_P = kfp->LastP + kfp->Q;
    //>>2�����������淽�̣����������� = kʱ��ϵͳ����Э���� / ��kʱ��ϵͳ����Э���� + �۲�����Э���
    kfp->Kg = kfp->Now_P / (kfp->Now_P + kfp->R);
    //>>3����������ֵ���̣�kʱ��״̬����������ֵ = ״̬������Ԥ��ֵ + ���������� * ������ֵ - ״̬������Ԥ��ֵ��
    kfp->out = kfp->out + kfp->Kg * (input - kfp->out); //��Ϊ��һ�ε�Ԥ��ֵ������һ�ε����ֵ
    //>>4������Э�����: ���ε�ϵͳЭ����� kfp->LastP ����һ������׼����
    kfp->LastP = (1 - kfp->Kg) * kfp->Now_P;
    return kfp->out;
}
/****************************************************************************************************************/

// һ�׻����㷨
float First_order_filter(float Com_angle, float angle_pt, float angle, float dt) {
    float A, K = 0.075;                            // �Լ��ٶȼ�ȡֵ��Ȩ��
    A = K / (K + dt);
    Com_angle = A * (Com_angle + angle_pt * dt) + (1 - A) * angle;
    return Com_angle;
}
// ���׻����㷨
float Second_order_filter(float Com2_angle, float angle_pt, float angle, float dt) {
    float x1, x2, K = 0.5;
    static float y1;
    x1 = (angle - Com2_angle) * K * K;
    y1 = y1 + x1 * dt;
    x2 = y1 + 2 * K * (angle - Com2_angle) + angle_pt;
    Com2_angle = Com2_angle + x2 * dt;
    return Com2_angle;
}

#define Q_angle 0.01      // �Ƕ��������Ŷ�
#define Q_omega 0.0003    // ���ٶ��������Ŷ�
#define R_angle 0.01      // ��������
// �������˲�
float Kalman_filter(float Klm_angle, float angle_pt, float angle, float dt) {
    static float bias;
    static float P_00, P_01, P_10, P_11;
    float K_0, K_1;
    Klm_angle += (angle_pt - bias) * dt;       // �������
    P_00 += -(P_10 + P_01) * dt + Q_angle * dt;
    P_01 += -P_11 * dt;
    P_10 += -P_11 * dt;
    P_11 += +Q_omega * dt;                     // ����������Э����
    K_0 = P_00 / (P_00 + R_angle);
    K_1 = P_10 / (P_00 + R_angle);
    bias += K_1 * (angle - Klm_angle);
    Klm_angle += K_0 * (angle - Klm_angle);    // �������
    P_00 -= K_0 * P_00;
    P_01 -= K_0 * P_01;
    P_10 -= K_1 * P_00;
    P_11 -= K_1 * P_01;                        // ����������Э����
    return Klm_angle;
}
/*****************************************************
*�������ƣ�kalman_filter
*�������ܣ�ADC_�˲�
*��ڲ�����ADC_Value
*���ڲ�����kalman_adc
*****************************************************/
uint16_t kalman_filter(uint16_t ADC_Value) {
    float x_k1_k1, x_k_k1;
    static float ADC_OLD_Value;
    float Z_k;
    static float P_k1_k1;
    static float Q = 0.0001;//Q�����������Q���󣬶�̬��Ӧ��죬�����ȶ��Ա仵
    static float R = 0.005; //R������������R���󣬶�̬��Ӧ�����������ȶ��Ա��
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
//Ϊ�����Ż��㷨���ڲ���ֵ���Ż�ֵ������ĳֵʱ����һ���ͺ��˲��㷨��С�ڸ�ֵʱ���ÿ������˲��㷨��
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
        x_k1_k1 = ADC_Value * 0.382 + kalman_adc_old * 0.618;//һ���ͺ��˲��� y(n) = q*x(n) + (1-q)*y(n-1)
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
//>1����ֵ�˲���---�˲�
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

//��ȡxyz���ݴ����ֵ�˲������������м��㣬�˲�����������sample,��ζ�ȡ�����Ͳ���˵�ˡ�
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
    //�������˲� ����ƽ���ȱ��
//    sample->x = kalmanFilter(&KFP_adxl345_x,sample->x);
//    sample->y = kalmanFilter(&KFP_adxl345_y,sample->y);
//    sample->z = kalmanFilter(&KFP_adxl345_z,sample->z);
    //HAL_UART_Transmit(&huart2, (uint8_t *)&sample->x, 2, 200);
    //HAL_UART_Transmit(&huart2, (uint8_t *)&sample->y, 2, 200);
    //HAL_UART_Transmit(&huart2, (uint8_t *)&sample->z, 2, 200);
}
//>2����̬��ֵ
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
//�ڶ�̬��ֵ�ṹ���ʼ��ʱ��һ��Ҫ��max��ֵ����ֵΪ��Сֵ��min��ֵΪ���ֵ�������������ڶ�̬���¡�
static void peak_update(peak_value_t *peak, axis_info_t *cur_sample) {
    static unsigned int sample_size = 0;
    sample_size ++;
    if(sample_size > SAMPLE_SIZE) {
        /*�����ﵽ50��������һ��*/
        sample_size = 1;
        peak->oldmax = peak->newmax;
        peak->oldmin = peak->newmin;
        //��ʼ��
        peak_value_init(peak);
    }
    peak->newmax.x = MAX(peak->newmax.x, cur_sample->x);
    peak->newmax.y = MAX(peak->newmax.y, cur_sample->y);
    peak->newmax.z = MAX(peak->newmax.z, cur_sample->z);
    peak->newmin.x = MIN(peak->newmin.x, cur_sample->x);
    peak->newmin.y = MIN(peak->newmin.y, cur_sample->y);
    peak->newmin.z = MIN(peak->newmin.z, cur_sample->z);
}
//>3����̬����
#define ABS(a) (0 - (a)) > 0 ? (-(a)) : (a)
#define DYNAMIC_PRECISION               information.accuracy*2/3       /*��̬���� 30*/

/*һ��������λ�Ĵ��������ڹ��˸�Ƶ����*/
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
//    //pSta  0����  1�½�
//    if(*cur_sample>*old_sample+DYNAMIC_PRECISION){       //����
//        if(*pSta!=0) res = 1;                            //�õ�����
//        *pSta = 0;
//    } else if(*cur_sample<*old_sample-DYNAMIC_PRECISION){//�½�
//        if(*pSta!=1) res = 1;                            //�õ�����
//        *pSta = 0;
//    }
//    *old_sample = *cur_sample;
//    return res;
//}
//static char slid_update1(slid_reg_t *slid, axis_info_t *cur_sample){
//    char res = 0;
//    static short x = 0,y = 0,z = 256;
//    static char pSta_x = 0,pSta_y = 0,pSta_z = 0;//0����  1�½�
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
//    //pSta  0����  1�½�
//    if((*pSta == 0) && (*cur_sample <= *new_sample-DYNAMIC_PRECISION)){
//        *pSta = 1;//�õ�����
//        *old_sample = *new_sample;
//        *new_sample = *cur_sample;
//        res = 1;
//    } else if((*pSta == 1) && (*cur_sample > *new_sample+DYNAMIC_PRECISION)){
//        *pSta = 0;//�õ�����
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
//    static char pSta_x = 0,pSta_y = 0,pSta_z = 0;//0����  1�½�
//    res |= FindPeaksAndTroughs2(&pSta_x,&slid->new_sample.x,&slid->old_sample.x,&cur_sample->x);
//    res |= FindPeaksAndTroughs2(&pSta_y,&slid->new_sample.y,&slid->old_sample.y,&cur_sample->y);
//    res |= FindPeaksAndTroughs2(&pSta_z,&slid->new_sample.z,&slid->old_sample.z,&cur_sample->z);
//    return res;
//}
//>4,�����ж�

#define MOST_ACTIVE_NULL                0        /*δ�ҵ����Ծ��*/
#define MOST_ACTIVE_X                   1        /*���Ծ��X*/
#define MOST_ACTIVE_Y                   2        /*���Ծ��Y*/
#define MOST_ACTIVE_Z                   3        /*���Ծ��Z*/

#define ACTIVE_PRECISION                information.threshold*2/3      /*��Ծ����С�仯ֵ*/

/*�жϵ�ǰ���Ծ��*/
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
/*�ж��Ƿ��߲�*/
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

filter_avg_t filter;//�ɼ�����
axis_info_t  sample;//�˲��������
peak_value_t peak = {//��̬��ֵ
    .newmin = {1023, 1023, 1023},
    .newmax = {0, 0, 0},
};
slid_reg_t   slid;  //��̬����

void Pedometer(int16_t x, int16_t y, int16_t z) {
    static unsigned int step = 0;
    //>1 ��Ҫ�Ȳɼ�����
    axis_info_t *pbuf = &filter.info[(filter.count++) % FILTER_CNT];
    pbuf->x = x;
    pbuf->y = y;
    pbuf->z = z;
//    HAL_UART_Transmit(&huart2, (uint8_t *)&z, 2, 200);
//    ADXL345_RD_XYZ(&pbuf->x, &pbuf->y, &pbuf->z);
//    ADXL345_RD_XYZ(&filter.info[filter.count].x, &filter.info[filter.count].y, &filter.info[filter.count].z);
//    filter.count = (filter.count +1 ) % FILTER_CNT;
    //>2 ��ֵ�˲���
    filter_calculate(&filter, &sample);
    //>3 ��̬��ֵ
    peak_update(&peak, &sample);
    //>4 ��̬����
    if(slid_update(&slid, &sample)) {
        //>5 �����ж�
        detect_step(&peak, &slid, &sample);
        if(step != step_cntt) {
            step = step_cntt;
            //MYLOG(WARNING, "Pedometer = %d", step); //�������
        }
    }
}

