#ifndef SPI1_H
#define SPI1_H

#include <stdint.h>

void SPI1_Init(void);
void SPI1_Write(uint8_t data);
uint8_t SPI1_Read(void);

#endif
