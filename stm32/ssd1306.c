#include "ssd1306.h"
#include "spi1.h"
#include "delay.h"
#include <stm32f10x.h>

// === Пины ===
#define SSD1306_DC_PORT   GPIOB
#define SSD1306_DC_PIN    0
#define SSD1306_RST_PORT  GPIOB
#define SSD1306_RST_PIN   1
#define SSD1306_CS_PORT   GPIOA
#define SSD1306_CS_PIN    4

#define DC_COMMAND() (SSD1306_DC_PORT->BRR = (1 << SSD1306_DC_PIN))
#define DC_DATA()    (SSD1306_DC_PORT->BSRR = (1 << SSD1306_DC_PIN))
#define CS_LOW()     (SSD1306_CS_PORT->BRR = (1 << SSD1306_CS_PIN))
#define CS_HIGH()    (SSD1306_CS_PORT->BSRR = (1 << SSD1306_CS_PIN))

static void SSD1306_SendCommand(uint8_t cmd) {
    DC_COMMAND();
    CS_LOW();
    SPI1_Write(cmd);
    CS_HIGH();
}

static void SSD1306_SendData(uint8_t data) {
    DC_DATA();
    CS_LOW();
    SPI1_Write(data);
    CS_HIGH();
}

void SSD1306_Init(void) {
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;

    // PB0 (DC), PB1 (RST)
    GPIOB->CRL &= ~(
        GPIO_CRL_MODE0 | GPIO_CRL_CNF0 |
        GPIO_CRL_MODE1 | GPIO_CRL_CNF1
    );
    GPIOB->CRL |= (
        (GPIO_CRL_MODE0_1 | GPIO_CRL_MODE0_0) |
        (GPIO_CRL_MODE1_1 | GPIO_CRL_MODE1_0)
    );

    // Reset
    SSD1306_RST_PORT->BRR = (1 << SSD1306_RST_PIN);
    delay(10000);
    SSD1306_RST_PORT->BSRR = (1 << SSD1306_RST_PIN);

    // Init commands
    SSD1306_SendCommand(0xAE);
    SSD1306_SendCommand(0x20); SSD1306_SendCommand(0x00);
    SSD1306_SendCommand(0xB0);
    SSD1306_SendCommand(0xC8);
    SSD1306_SendCommand(0x00);
    SSD1306_SendCommand(0x10);
    SSD1306_SendCommand(0x40);
    SSD1306_SendCommand(0x81); SSD1306_SendCommand(0x7F);
    SSD1306_SendCommand(0xA1);
    SSD1306_SendCommand(0xA6);
    SSD1306_SendCommand(0xA8); SSD1306_SendCommand(0x3F);
    SSD1306_SendCommand(0xA4);
    SSD1306_SendCommand(0xD3); SSD1306_SendCommand(0x00);
    SSD1306_SendCommand(0xD5); SSD1306_SendCommand(0x80);
    SSD1306_SendCommand(0xD9); SSD1306_SendCommand(0xF1);
    SSD1306_SendCommand(0xDA); SSD1306_SendCommand(0x12);
    SSD1306_SendCommand(0xDB); SSD1306_SendCommand(0x40);
    SSD1306_SendCommand(0x8D); SSD1306_SendCommand(0x14);
    SSD1306_SendCommand(0xAF);
}

void SSD1306_Clear(void) {
    for (uint8_t page = 0; page < 8; page++) {
        SSD1306_SendCommand(0xB0 + page);
        SSD1306_SendCommand(0x00);
        SSD1306_SendCommand(0x10);
        for (uint8_t col = 0; col < 128; col++) {
            SSD1306_SendData(0x00);
        }
    }
}

void SSD1306_DrawChessboard(void) {
    for (uint8_t page = 0; page < 8; page++) {
        SSD1306_SendCommand(0xB0 + page);
        SSD1306_SendCommand(0x00);
        SSD1306_SendCommand(0x10);
        for (uint8_t col = 0; col < 128; col++) {
            uint8_t blockX = col / 8;
            uint8_t blockY = page;
            uint8_t pattern = ((blockX + blockY) % 2) ? 0xFF : 0x00;
            SSD1306_SendData(pattern);
        }
    }
}
