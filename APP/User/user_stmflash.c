#include "user_stmflash.h"

void STMFLASH_Erase(uint32_t WriteAddr, uint32_t NbPages) {//相对地址FLASH_BASE 必须擦除
    FLASH_EraseInitTypeDef My_Flash;                       //声明 FLASH_EraseInitTypeDef 结构体为 My_Flash
    HAL_FLASH_Unlock();                                    //解锁Flash
    My_Flash.TypeErase = FLASH_TYPEERASE_PAGES;            //标明Flash执行页面只做擦除操作
    My_Flash.PageAddress = FLASH_BASE + WriteAddr;         //声明要擦除的地址 0到64K相对地址
    My_Flash.NbPages = NbPages;                            //说明要擦除的页数，此参数必须是Min_Data = 1和Max_Data =(最大页数-初始页的值)之间的值 FLASH_PAGE_SIZE
    uint32_t PageError = 0;                                //设置PageError,如果出现错误这个变量会被设置为出错的FLASH地址
    HAL_FLASHEx_Erase(&My_Flash, &PageError);              //调用擦除函数擦除
    HAL_FLASH_Lock();                                      //锁住Flash
}

void STMFLASH_Read(uint32_t ReadAddr, uint8_t *pBuffer, uint32_t NumToRead) {
    uint32_t i;
    for(i = 0; i < NumToRead; i++) {
        pBuffer[i] = *(__IO uint8_t*)(FLASH_BASE + ReadAddr + i);
    }
}

void STMFLASH_Write(uint32_t WriteAddr, uint8_t *pBuffer, uint32_t NumToWrite) {
    uint16_t i;
    uint32_t *pbuf = (uint32_t *)pBuffer;
    HAL_FLASH_Unlock();
    for(i = 0; i < (NumToWrite + 3) / 4; i++) {
        HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, (FLASH_BASE + WriteAddr + 4 * i), pbuf[i]);
    }
    HAL_FLASH_Lock();
}

void STMEEPROM_Erase(uint32_t EraseAddr, uint32_t NumToErase) { //4k地址擦除  相对地址 其实可以不用擦除
    uint32_t i;
    //HAL_FLASHEx_DATAEEPROM_Unlock(); //这里很奇怪，解锁擦除卡死
    for(i = 0; i < NumToErase; i++) {
        HAL_FLASHEx_DATAEEPROM_Erase(FLASH_TYPEERASEDATA_BYTE, FLASH_EEPROM_BASE + EraseAddr + i);
    }
    HAL_FLASHEx_DATAEEPROM_Lock();
}

void STMEEPROM_Read(uint32_t ReadAddr, uint8_t *pBuffer, uint32_t NumToRead) { //4k地址擦除
    uint32_t i;
    for(i = 0; i < NumToRead; i++) {
        pBuffer[i] = *(__IO uint8_t*)(FLASH_EEPROM_BASE + ReadAddr + i);
    }
}

void STMEEPROM_Write(uint32_t WriteAddr, uint8_t *pBuffer, uint32_t NumToWrite) { //4k地址擦除
    uint32_t i;
    HAL_FLASHEx_DATAEEPROM_Unlock();
    for(i = 0; i < NumToWrite; i++) {
        HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_FASTBYTE,  FLASH_EEPROM_BASE + WriteAddr + i, pBuffer[i]);
    }
    HAL_FLASHEx_DATAEEPROM_Lock();
}

void FLASH_EEPROM_Test(void) {
//    uint8_t crc;
//    DeviceInformation pinformation;
//    STMEEPROM_Erase(0,52);
//    STMEEPROM_Write(0, (void *)&information, sizeof(DeviceInformation));
//    crc = CalU8CRC((void *)&information, sizeof(DeviceInformation));
//    MYLOG(INFO, "information crc= %x size =%d", crc,sizeof(DeviceInformation));
//    STMEEPROM_Read(0, (void *)&pinformation, sizeof(DeviceInformation));
//    crc = CalU8CRC((void *)&pinformation, sizeof(DeviceInformation));
//    MYLOG(INFO, "information crc= %x size =%d", crc,sizeof(DeviceInformation));
//    STMEEPROM_Erase(0,sizeof(DeviceInformation));
    
    
    
//    crc = CalU8CRC((void *)&pinformation, sizeof(DeviceInformation));
//    MYLOG(INFO, "information crc= %x", crc);
//    if(crc != pinformation.crc) {
//        MYLOG(INFO, "Write informationdef");
//        //information = informationdef;
//        STMEEPROM_Erase(0,sizeof(DeviceInformation));
//        STMEEPROM_Write(0, (void *)&information, sizeof(DeviceInformation));
//    }
    
    
    //内部EEPROM  测试OK
    uint8_t buf1[10] = {"FFFFFFFFF"};
    uint8_t buf2[10] = {"555555555"};
    uint8_t buf3[10] = {0};
    STMEEPROM_Read(0, buf3, 10);
    MYLOG(INFO,"STMEEPROM_Read :%s", buf3);
    STMEEPROM_Erase(0, 52);
    STMEEPROM_Write(0, buf1, 10);
    STMEEPROM_Read(0, buf3, 10);
    MYLOG(INFO,"STMEEPROM_Read :%s", buf3);
    //STMEEPROM_Erase(0, 10);
    STMEEPROM_Write(0, buf2, 10);
    STMEEPROM_Read(0, buf3, 10);
    MYLOG(INFO,"STMEEPROM_Read :%s", buf3);
    
    
    
    //内部flash  测试OK
    STMFLASH_Read(1024 * 64, buf3, 10);
    MYLOG(DEBUG,"STMFLASH_Read :%s", buf3);
    STMFLASH_Erase(1024 * 64, 4);
    STMFLASH_Write(1024 * 64, buf1, 10);
    STMFLASH_Read(1024 * 64, buf3, 10);
    MYLOG(DEBUG,"STMFLASH_Read :%s", buf3);
    STMFLASH_Erase(1024 * 64, 4);
    STMFLASH_Write(1024 * 64, buf2, 10);
    STMFLASH_Read(1024 * 64, buf3, 10);
    MYLOG(DEBUG,"STMFLASH_Read :%s", buf3);
}
