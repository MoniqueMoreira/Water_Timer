#include "pico/stdlib.h"
#include "hardware/adc.h"
#include <stdio.h>

#include "FreeRTOS.h"
#include "task.h"

#include "io_benchmark.h"
#include "io_manager_cfg.h"

#include "test/benchmark.h"
#include "test/benchmark_cfg.h"

#ifdef IO_BENCHMARK_ENABLED


// Prototipos
void IO_BENCHMARK_ReadButtons(void);
void IO_BENCHMARK_ReadADC(void);
void IO_BENCHMARK_WriteLEDs(void);
void IO_BENCHMARK_WriteRelay(void);
void IO_BENCHMARK_Init(void);

// === Funções de teste =====

void IO_BENCHMARK_ReadButtons(void)
{
    for (int i = 0; i < BTN_COUNT; i++) {
        volatile uint8_t raw = gpio_get(button_pins[i]);
        (void)raw;
    }
}

void IO_BENCHMARK_ReadADC(void)
{
    for (int i = 0; i < ADC_COUNT; i++) {
        adc_select_input(i);
        volatile uint16_t v = adc_read();
        (void)v;
    }
}

void IO_BENCHMARK_WriteLEDs(void)
{
    for (int i = 0; i < LED_COUNT; i++) {
        gpio_put(led_pins[i], i & 1);
    }
}

void IO_BENCHMARK_WriteRelay(void)
{
    for (int i = 0; i < RELAY_COUNT; i++) {
        gpio_put(relay_pins[i], 1);
        gpio_put(relay_pins[i], 0);
    }
}

// === Função principal do benchmark ===
void IO_BENCHMARK_Init(void)
{
    printf("\n=== BENCHMARK IO TASKS ===\n");

    // ------------------------------
    //  TESTE: READ BUTTONS
    // ------------------------------
    BENCHMARK_Reset();
    for (int i = 0; i < BENCHMARK_MAX_SAMPLES; i++) {
        BENCHMARK_Start();
        IO_BENCHMARK_ReadButtons();
        BENCHMARK_Stop();
    }
    BENCHMARK_Report("Read Buttons");

    // ------------------------------
    //  TESTE: READ ADC
    // ------------------------------
    BENCHMARK_Reset();
    for (int i = 0; i < BENCHMARK_MAX_SAMPLES; i++) {
        BENCHMARK_Start();
        IO_BENCHMARK_ReadADC();
        BENCHMARK_Stop();
    }
    BENCHMARK_Report("Read ADC");

    // ------------------------------
    //  TESTE: WRITE LEDS
    // ------------------------------
    BENCHMARK_Reset();
    for (int i = 0; i < BENCHMARK_MAX_SAMPLES; i++) {
        BENCHMARK_Start();
        IO_BENCHMARK_WriteLEDs();
        BENCHMARK_Stop();
    }
    BENCHMARK_Report("Write LEDs");

    // ------------------------------
    //  TESTE: WRITE RELAY
    // ------------------------------
    BENCHMARK_Reset();
    for (int i = 0; i < BENCHMARK_MAX_SAMPLES; i++) {
        BENCHMARK_Start();
        IO_BENCHMARK_WriteRelay();
        BENCHMARK_Stop();
    }
    BENCHMARK_Report("Write Relay");

    printf("Benchmark finalizado. Task encerrada.\n");
}

#endif // IO_BENCHMARK_ENABLED