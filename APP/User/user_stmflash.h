#ifndef __STMFLASH_H___
#define __STMFLASH_H___

#include "main.h"

//�ڲ�FALSH��д
void STMFLASH_Erase(uint32_t WriteAddr, uint32_t NbPages);
void STMFLASH_Read(uint32_t ReadAddr, uint8_t *pBuffer, uint32_t NumToRead);
void STMFLASH_Write(uint32_t WriteAddr, uint8_t *pBuffer, uint32_t NumToWrite);
//�ڲ�EEPROM��д
void STMEEPROM_Erase(uint32_t EraseAddr, uint32_t NumToErase);
void STMEEPROM_Read(uint32_t ReadAddr, uint8_t *pBuffer, uint32_t NumToRead);
void STMEEPROM_Write(uint32_t WriteAddr, uint8_t *pBuffer, uint32_t NumToWrite);
//�ڲ�FALSH EEPROM ��д����
void FLASH_EEPROM_Test(void);
#endif
