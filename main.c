#include <stdint.h>
#include <stm32f10x.h>

volatile uint32_t msTicks = 0; // глобальный счётчик миллисекунд

void SysTick_Handler(void) {
    msTicks++;
}

void delay_ms(uint32_t ms) {
    uint32_t start = msTicks;
    while ((msTicks - start) < ms);
}

// --- Функция ожидания отпускания кнопки
void wait_button_release(uint32_t pin) {
    while (!(GPIOA->IDR & (1U << pin))) {
        // ждём когда кнопка отпустится
    }
    delay_ms(20); // антидребезг после отпускания
}

int __attribute((noreturn)) main(void) {
    SystemInit();
    SysTick_Config(SystemCoreClock / 1000);

    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;

    // PC13 как выход
    GPIOC->CRH &= ~GPIO_CRH_CNF13;
    GPIOC->CRH |= GPIO_CRH_MODE13_0;

    // PA0 и PA1 как входы с подтяжкой вверх
    GPIOA->CRL &= ~((GPIO_CRL_MODE0 | GPIO_CRL_CNF0) |
                    (GPIO_CRL_MODE1 | GPIO_CRL_CNF1));
    GPIOA->CRL |= (GPIO_CRL_CNF0_1 | GPIO_CRL_CNF1_1);
    GPIOA->ODR |= (1U << 0) | (1U << 1);

    float freq_factor = 1.0f;
    const float MAX_FREQ = 64.0f;
    const float MIN_FREQ = 1.0f / 64.0f;

    uint32_t base_period_ms = 1000;
    uint32_t current_period_ms = base_period_ms;

    while (1) {
        // мигание
        GPIOC->ODR ^= (1U << 13);
        delay_ms(current_period_ms);

        // кнопка PA0 — увеличить частоту
        if (!(GPIOA->IDR & (1U << 0))) {
            wait_button_release(0);
            if (freq_factor < MAX_FREQ) {
                freq_factor *= 2.0f;
                current_period_ms = base_period_ms / freq_factor;
            }
        }

        // кнопка PA1 — уменьшить частоту
        if (!(GPIOA->IDR & (1U << 1))) {
            wait_button_release(1);
            if (freq_factor > MIN_FREQ) {
                freq_factor /= 2.0f;
                current_period_ms = base_period_ms / freq_factor;
            }
        }
    }
}
