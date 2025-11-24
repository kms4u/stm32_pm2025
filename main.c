#include <stm32f10x.h>

volatile uint32_t led_state = 0;

// --- Прерывание таймера TIM2 ---
void TIM2_IRQHandler(void) {
    if (TIM2->SR & TIM_SR_UIF) {
        TIM2->SR &= ~TIM_SR_UIF; // сброс флага

        // инверсия светодиода на PC13
        GPIOC->ODR ^= (1U << 13);
    }
}

// ожидание отпускания кнопки
void wait_button_release(uint32_t pin) {
    while (!(GPIOA->IDR & (1U << pin))) {
        // ждём отпускания
    }
    for (volatile int i = 0; i < 100000; i++); // антидребезг ~20–30 мс
}

int main(void) {
    SystemInit();

    // тактирование портов и TIM2
    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

    // PC13 как выход (LED)
    GPIOC->CRH &= ~GPIO_CRH_CNF13;
    GPIOC->CRH |= GPIO_CRH_MODE13_0;

    // PA0, PA1 — вход с подтяжкой вверх
    GPIOA->CRL &= ~((GPIO_CRL_MODE0 | GPIO_CRL_CNF0) |
                    (GPIO_CRL_MODE1 | GPIO_CRL_CNF1));
    GPIOA->CRL |= (GPIO_CRL_CNF0_1 | GPIO_CRL_CNF1_1);
    GPIOA->ODR |= (1U << 0) | (1U << 1);

    // --- Настройка TIM2 ---
    TIM2->PSC = 7999;      // предделитель (1 кГц при 8 МГц)
    TIM2->ARR = 500;       // период таймера = 500 мс мигания

    TIM2->DIER |= TIM_DIER_UIE; // разрешить прерывание по обновлению
    NVIC_EnableIRQ(TIM2_IRQn);

    TIM2->CR1 |= TIM_CR1_CEN;  // запуск таймера

    while (1) {
        // кнопка PA0 — увеличить период (уменьшить частоту)
        if (!(GPIOA->IDR & (1U << 0))) {
            wait_button_release(0);

            // увеличиваем предделитель:
            TIM2->PSC = TIM2->PSC << 1;
        }

        // кнопка PA1 — уменьшить период (увеличить частоту)
        if (!(GPIOA->IDR & (1U << 1))) {
            wait_button_release(1);

            // уменьшаем предделитель:
            TIM2->PSC = TIM2->PSC >> 1;

            // защита от нуля (иначе таймер сломается)
            if (TIM2->PSC == 0)
                TIM2->PSC = 1;
        }
    }
}
