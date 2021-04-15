#include "user_spi.h"

void User_SPI_Init(SPI_TypeDef *pspi,SPI_HandleTypeDef *hspi) {
    hspi->Instance = pspi;
    hspi->Init.Mode = SPI_MODE_MASTER;
    hspi->Init.Direction = SPI_DIRECTION_2LINES;
    hspi->Init.DataSize = SPI_DATASIZE_8BIT;
    hspi->Init.CLKPolarity = SPI_POLARITY_LOW;
    hspi->Init.CLKPhase = SPI_PHASE_1EDGE;
    hspi->Init.NSS = SPI_NSS_SOFT;
    hspi->Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
    hspi->Init.FirstBit = SPI_FIRSTBIT_MSB;
    hspi->Init.TIMode = SPI_TIMODE_DISABLED;
    hspi->Init.CRCCalculation = SPI_CRCCALCULATION_DISABLED;
    hspi->Init.CRCPolynomial = 10;
    HAL_SPI_Init(hspi);
}
