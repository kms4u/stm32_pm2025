#include "spi1.h"
#include <stm32f10x.h>

void SPI1_Init(void) {
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;

    // PA5 (SCK), PA7 (MOSI)
    GPIOA->CRL &= ~(
        GPIO_CRL_MODE5 | GPIO_CRL_CNF5 |
        GPIO_CRL_MODE7 | GPIO_CRL_CNF7
    );
    GPIOA->CRL |= (
        (GPIO_CRL_MODE5_1 | GPIO_CRL_MODE5_0) | GPIO_CRL_CNF5_1 |
        (GPIO_CRL_MODE7_1 | GPIO_CRL_MODE7_0) | GPIO_CRL_CNF7_1
    );

    // PA6 (MISO)
    GPIOA->CRL &= ~(GPIO_CRL_MODE6 | GPIO_CRL_CNF6);
    GPIOA->CRL |= GPIO_CRL_CNF6_0;

    // PA4 (CS)
    GPIOA->CRL &= ~(GPIO_CRL_MODE4 | GPIO_CRL_CNF4);
    GPIOA->CRL |= GPIO_CRL_MODE4_1 | GPIO_CRL_MODE4_0;

    GPIOA->BSRR = GPIO_BSRR_BS4;

    // SPI setup
    SPI1->CR1 = 0;
    SPI1->CR1 |= SPI_CR1_MSTR;
    SPI1->CR1 |= SPI_CR1_BR_1;
    SPI1->CR1 |= SPI_CR1_SSI | SPI_CR1_SSM;
    SPI1->CR1 |= SPI_CR1_SPE;
}

void SPI1_Write(const uint8_t data) {
    while (!(SPI1->SR & SPI_SR_TXE));
    SPI1->DR = data;
    while (SPI1->SR & SPI_SR_BSY);
}

uint8_t SPI1_Read(void) {
    while (!(SPI1->SR & SPI_SR_TXE));
    SPI1->DR = 0xFF;
    while (!(SPI1->SR & SPI_SR_RXNE));
    return SPI1->DR;
}
