#include <stm32f10x.h>
#include "stm32/spi1.h"
#include "stm32/ssd1306.h"
#include "stm32/delay.h"

int main(void) {
    SPI1_Init();
    SSD1306_Init();
    SSD1306_Clear();
    SSD1306_DrawChessboard();

    while (1) {
        // main loop
    }
}
