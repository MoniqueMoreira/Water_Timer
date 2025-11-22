#include "benchmark.h"
#include "pico/stdlib.h"
#include <stdio.h>

uint32_t benchmark_samples[BENCHMARK_MAX_SAMPLES];
uint32_t benchmark_sample_count = 0;

static uint32_t t_start;

// Resetar contador de amostras
void BENCHMARK_Reset(void) {
    benchmark_sample_count = 0;
}

// Início da medição
void BENCHMARK_Start(void) {
    t_start = time_us_32();
}

// Fim da medição e armazenamento
void BENCHMARK_Stop(void) {
    uint32_t t_end = time_us_32();
    uint32_t dt = t_end - t_start;

    if (benchmark_sample_count < BENCHMARK_MAX_SAMPLES) {
        benchmark_samples[benchmark_sample_count++] = dt;
    }
}

// Cálculo de média, HWM e impressão completa das amostras
void BENCHMARK_Report(const char *label) {

    uint32_t hwm = 0;
    uint64_t sum = 0;

    for (uint32_t i = 0; i < benchmark_sample_count; i++) {
        uint32_t v = benchmark_samples[i];
        if (v > hwm) hwm = v;
        sum += v;
    }

    float mean = sum / (float) benchmark_sample_count;

    printf("\n=== BENCHMARK: %s ===\n", label);
    printf("Samples: %u\n", benchmark_sample_count);
    printf("HWM: %u us\n", hwm);
    printf("Mean: %.2f us\n", mean);
    printf("=================================\n");

    // Impressão estilo CSV para tabela
    printf("Raw Samples (%s):\n", label);

    for (uint32_t i = 0; i < benchmark_sample_count; i++) {
        printf("%u%s", benchmark_samples[i], (i < benchmark_sample_count - 1) ? ", " : "");
    }
    printf("\n---------------------------------\n\n");
}
