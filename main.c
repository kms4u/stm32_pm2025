#include <stdint.h>
#include <stm32f10x.h>

volatile uint32_t msTicks = 0; // глобальный счётчик миллисекунд

// --- Прерывание SysTick каждые 1 мс
void SysTick_Handler(void) {
    msTicks++;
}

// --- Задержка в миллисекундах
void delay_ms(uint32_t ms) {
    uint32_t start = msTicks;
    while ((msTicks - start) < ms);
}

int __attribute((noreturn)) main(void) {
    // --- Настройка тактирования
    SystemInit();

    // --- Запуск SysTick: прерывание каждые 1 мс (частота шины / 1000)
    SysTick_Config(SystemCoreClock / 1000);

    // --- Включаем тактирование портов
    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN; // LED (PC13)
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN; // Buttons (PA0, PA1)

    // --- Настраиваем PC13 как выход (push-pull, 10 МГц)
    GPIOC->CRH &= ~GPIO_CRH_CNF13;
    GPIOC->CRH |= GPIO_CRH_MODE13_0;

    // --- Настраиваем PA0, PA1 как вход с подтяжкой вверх
    GPIOA->CRL &= ~((GPIO_CRL_MODE0 | GPIO_CRL_CNF0) |
                    (GPIO_CRL_MODE1 | GPIO_CRL_CNF1));
    GPIOA->CRL |= (GPIO_CRL_CNF0_1 | GPIO_CRL_CNF1_1); // input PU/PD
    GPIOA->ODR |= (1U << 0) | (1U << 1); // включаем Pull-Up

    // --- Переменные для частоты
    float freq_factor = 1.0f; // множитель (1x = 1 Гц)
    const float MAX_FREQ = 64.0f;
    const float MIN_FREQ = 1.0f / 64.0f;

    // Период мигания (1 Гц = 1000 мс вкл + 1000 мс выкл = 2000 мс цикл)
    uint32_t base_period_ms = 1000; // половина цикла (LED ON или OFF)
    uint32_t current_period_ms = base_period_ms;

    while (1) {
        // --- Мигание светодиода
        GPIOC->ODR ^= (1U << 13);
        delay_ms(current_period_ms);

        // --- Проверка кнопки A (PA0) — увеличить частоту
        if (!(GPIOA->IDR & (1U << 0))) {
            if (freq_factor < MAX_FREQ) {
                freq_factor *= 2.0f;
                current_period_ms = base_period_ms / freq_factor;
            }
            delay_ms(300); // антидребезг
        }

        // --- Проверка кнопки C (PA1) — уменьшить частоту
        if (!(GPIOA->IDR & (1U << 1))) {
            if (freq_factor > MIN_FREQ) {
                freq_factor /= 2.0f;
                current_period_ms = base_period_ms / freq_factor;
            }
            delay_ms(300); // антидребезг
        }
    }
}
