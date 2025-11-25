#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/cm3/nvic.h>

static void gpio_setup(void) {
    rcc_periph_clock_enable(RCC_GPIOA);
    rcc_periph_clock_enable(RCC_GPIOC);

    // PC13 — LED
    gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_2_MHZ,
                  GPIO_CNF_OUTPUT_PUSHPULL, GPIO13);

    // PA0, PA1 — вход с подтяжкой вверх
    gpio_set_mode(GPIOA, GPIO_MODE_INPUT,
                  GPIO_CNF_INPUT_PULL_UPDOWN, GPIO0 | GPIO1);

    // включаем подтяжку вверх
    gpio_set(GPIOA, GPIO0 | GPIO1);
}

// переменная для предделителя TIM2
static uint32_t tim2_psc = 7999;

static void tim2_setup(void) {
    rcc_periph_clock_enable(RCC_TIM2);

    timer_set_prescaler(TIM2, tim2_psc);
    timer_set_period(TIM2, 500);

    timer_enable_irq(TIM2, TIM_DIER_UIE);
    nvic_enable_irq(NVIC_TIM2_IRQ);

    timer_generate_event(TIM2, TIM_EGR_UG);
    timer_enable_counter(TIM2);
}

void tim2_isr(void) {
    if (timer_get_flag(TIM2, TIM_SR_UIF)) {
        timer_clear_flag(TIM2, TIM_SR_UIF);
        gpio_toggle(GPIOC, GPIO13);
    }
}

static void wait_button_release(uint16_t pin_mask) {
    while (!gpio_get(GPIOA, pin_mask)) {
        // ждать отпускания
    }
    for (volatile int i = 0; i < 100000; i++); // антидребезг
}

int main(void) {
    gpio_setup();
    tim2_setup();

    while (1) {
        // PA0 — увеличить PSC
        if (!gpio_get(GPIOA, GPIO0)) {
            wait_button_release(GPIO0);

            tim2_psc <<= 1;
            timer_set_prescaler(TIM2, tim2_psc);
            timer_generate_event(TIM2, TIM_EGR_UG);
        }

        // PA1 — уменьшить PSC
        if (!gpio_get(GPIOA, GPIO1)) {
            wait_button_release(GPIO1);

            tim2_psc >>= 1;
            if (tim2_psc == 0) tim2_psc = 1;

            timer_set_prescaler(TIM2, tim2_psc);
            timer_generate_event(TIM2, TIM_EGR_UG);
        }
    }
}
