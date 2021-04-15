#include "axp173.h"
//#include "math.h"
//#include "stdlib.h"
//#include "i2c.h"
I2C_HandleTypeDef *axp173 = &hi2c2;

unsigned char axp173_read(unsigned char id, unsigned char addr) {
    uint8_t readValue = 0xFF;
    #if AnalogHardware
    HAL_I2C_Mem_Read(axp173, id, addr, I2C_MEMADD_SIZE_8BIT, &readValue, 1, 0xff);
    #else
    readValue = IIC2_ReadReg(id-1, addr);
    #endif
    return readValue;
}

void axp173_write(unsigned char id, unsigned char addr, unsigned char dat) {
    #if AnalogHardware
    HAL_I2C_Mem_Write(axp173, id, addr, I2C_MEMADD_SIZE_8BIT, &dat, 1, 0xFF);
    #else
    IIC2_WriteReg(id, addr,dat);
    #endif
}

void ldo2_enable(void) {
    unsigned char temp;
    temp = axp173_read(I2C1_SLAVE_ADDRESS7 + 1, POWER_OUTPUT_CONTROL);
    temp |= 0x04;
    axp173_write(I2C1_SLAVE_ADDRESS7, POWER_OUTPUT_CONTROL, temp);
}

void ldo2_disable(void) {
    unsigned char temp;
    temp = axp173_read(I2C1_SLAVE_ADDRESS7 + 1, POWER_OUTPUT_CONTROL);
    temp &= ~0x04;
    axp173_write(I2C1_SLAVE_ADDRESS7, POWER_OUTPUT_CONTROL, temp);
}

void ldo3_enable(void) {
    unsigned char temp;
    temp = axp173_read(I2C1_SLAVE_ADDRESS7 + 1, POWER_OUTPUT_CONTROL);
    temp |= 0x08;
    axp173_write(I2C1_SLAVE_ADDRESS7, POWER_OUTPUT_CONTROL, temp);
}

void ldo3_disable(void) {
    unsigned char temp;
    temp = axp173_read(I2C1_SLAVE_ADDRESS7 + 1, POWER_OUTPUT_CONTROL);
    temp &= ~0x08;
    axp173_write(I2C1_SLAVE_ADDRESS7, POWER_OUTPUT_CONTROL, temp);
}

void ldo4_enable(void) {
    unsigned char temp;
    temp = axp173_read(I2C1_SLAVE_ADDRESS7 + 1, POWER_OUTPUT_CONTROL);
    temp |= 0x02;
    axp173_write(I2C1_SLAVE_ADDRESS7, POWER_OUTPUT_CONTROL, temp);
}

void ldo4_disable(void) {
    unsigned char temp;
    temp = axp173_read(I2C1_SLAVE_ADDRESS7 + 1, POWER_OUTPUT_CONTROL);
    temp &= ~0x02;
    axp173_write(I2C1_SLAVE_ADDRESS7, POWER_OUTPUT_CONTROL, temp);
}

void axp173_poweroff(void) {
    unsigned char temp;
    temp = axp173_read(I2C1_SLAVE_ADDRESS7 + 1, TURN_OFF);
    temp |= 0x80;
    axp173_write(I2C1_SLAVE_ADDRESS7, TURN_OFF, temp);
}

uint8_t axp173_init(void) {
    //LDO1----VRTC: STM32_VBAT ʼ����
    //LDO2----VCC_JY901: JY901
    //LDO3----VCC_2401:  NRF24L01
    //LDO4----VCC_EMG:   EMG ����
    //DCDC1----VCC3V3:   STM32 LED AXP173 ADXL345
    //DCDC2---��ʱ����
    uint8_t temp;
//    temp = axp173_read(I2C1_SLAVE_ADDRESS7 + 1, INPUT_POWER_STATUS);
//    MYLOG(INFO,"INPUT_POWER_STATUS =%x", temp);
    HAL_Delay(200);
    axp173_write(I2C1_SLAVE_ADDRESS7, POWER_OUTPUT_CONTROL, 0x0d);         //DC-DC1 ���ؿ��� LDO2 LDO3 LDO4 �ر�
    HAL_Delay(20);
    temp = axp173_read(I2C1_SLAVE_ADDRESS7 + 1, POWER_OUTPUT_CONTROL);
    if(temp != 0x0d) {
        HAL_Delay(20);
        temp = axp173_read(I2C1_SLAVE_ADDRESS7 + 1, POWER_OUTPUT_CONTROL);
    }
    if(temp == 0x0d) {
        //0.7-3.5V�� 25mV/step   V = 25*0x5c +0.7 = 3000mv
        axp173_write(I2C1_SLAVE_ADDRESS7, LDO4_OUTPUT,   0x68);           //����LDO4���3V    ����3V 5C  3.0V����Ϊ0x5c 3.3V����Ϊ0x68
        HAL_Delay(5);
        axp173_write(I2C1_SLAVE_ADDRESS7, DCDC1_OUTPUT,  0x68);           //����DCDC1���3V   ����3V 5C  3.0V����Ϊ0x5c 3.3V����Ϊ0x68
        HAL_Delay(5);
        //1.8-3.3V�� 100mV/step  V = 100*0x0c+1.8 = 3000mv
        axp173_write(I2C1_SLAVE_ADDRESS7, LDO2_3_OUTPUT, 0xff);           //LDO2 LDO3���3V              3.0V����Ϊ0xCC 3.3V����Ϊ0xff
        HAL_Delay(5);
        axp173_write(I2C1_SLAVE_ADDRESS7, PEK_STATE, 0xFC);               //2S���� ����4S�ػ�  ������ʱ������Ϊ2.5S
        HAL_Delay(5);
        //axp173_write(I2C1_SLAVE_ADDRESS7, ADP_CHAR_ENABLE, 0x85);       //ʹ�ܳ�繦�ܣ� Ŀ�����ѹ4.1V ������550mA
        //axp173_write(I2C1_SLAVE_ADDRESS7, ADP_CHAR_ENABLE, 0xc8);       //ʹ�ܳ�繦�ܣ� Ŀ�����ѹ4.2V ������780mA
        axp173_write(I2C1_SLAVE_ADDRESS7, ADP_CHAR_ENABLE, 0xc8);         //ʹ�ܳ�繦�ܣ� Ŀ�����ѹ4.2V ������550mA
        HAL_Delay(5);
        axp173_write(I2C1_SLAVE_ADDRESS7, ADP_CHAR_2, 0x41);              //Ԥ��糬ʱʱ��40���ӣ� ����ģʽ�³�ʱʱ��Ϊ8Сʱ
        HAL_Delay(5);
        axp173_write(I2C1_SLAVE_ADDRESS7, VOLT_ADC_ENABLE, 0xc3);         //ʹ�ܵ�ص�ѹ������ADC
        //ldo2_enable();//��JY901
        //ldo3_enable();//��NRF24L01
        //ldo4_enable();//�򿪼���
        globalvariable.EquipmentFlag = SetBit(globalvariable.EquipmentFlag, axp173_flag);
        MYLOG(INFO, "AXP173_Init OK! = %x", globalvariable.EquipmentFlag);
        return 0;
    }
    globalvariable.EquipmentFlag = ClrBit(globalvariable.EquipmentFlag, axp173_flag);
    MYLOG(ERROR, "AXP173_Init Fail! = %x", temp);
    return 0;
}

uint8_t is_charging(void) {
    uint8_t tmp;
    tmp = axp173_read(I2C1_SLAVE_ADDRESS7 + 1, INPUT_POWER_STATUS);
    if(tmp & 0x04) {
        return 1;  //������ڱ����
    }
    return 0;
}

//��ȡ��ص�ѹ������mvֵ
uint16_t getVoltage(void) {
    uint8_t H, L;
    uint16_t bat_volt = 0;
    H = axp173_read(I2C1_SLAVE_ADDRESS7 + 1, BAT_ADDRESS_Voltage);
    L = axp173_read(I2C1_SLAVE_ADDRESS7 + 1, BAT_ADDRESS_Voltage + 1);
    bat_volt = (uint16_t)((H << 4) + L) * 1.1; //Battery Voltage  0mV  1.1mV  4.5045V
    return bat_volt;//��λmv 10��
}
//��ȡ��ص�ѹ������mvֵ
uint16_t getCurrent(void) {
    uint8_t H, L;
    uint16_t bat_volt = 0;
    H = axp173_read(I2C1_SLAVE_ADDRESS7 + 1, BAT_ADDRESS_Current);
    L = axp173_read(I2C1_SLAVE_ADDRESS7 + 1, BAT_ADDRESS_Current + 1);
    bat_volt = (uint16_t)((H << 5) + L) * 0.5; //Bat discharge current  0mA  0.5mA  4.095A
    return bat_volt;//��λmv 10��
}
