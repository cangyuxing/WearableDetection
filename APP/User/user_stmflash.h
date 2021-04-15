#ifndef __STMFLASH_H___
#define __STMFLASH_H___

#include "main.h"

//内部FALSH读写
void STMFLASH_Erase(uint32_t WriteAddr, uint32_t NbPages);
void STMFLASH_Read(uint32_t ReadAddr, uint8_t *pBuffer, uint32_t NumToRead);
void STMFLASH_Write(uint32_t WriteAddr, uint8_t *pBuffer, uint32_t NumToWrite);
//内部EEPROM读写
void STMEEPROM_Erase(uint32_t EraseAddr, uint32_t NumToErase);
void STMEEPROM_Read(uint32_t ReadAddr, uint8_t *pBuffer, uint32_t NumToRead);
void STMEEPROM_Write(uint32_t WriteAddr, uint8_t *pBuffer, uint32_t NumToWrite);
//内部FALSH EEPROM 读写测试
void FLASH_EEPROM_Test(void);
#endif
