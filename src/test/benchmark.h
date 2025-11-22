#ifndef BENCHMARK_H
#define BENCHMARK_H

#include <stdint.h>

#define BENCHMARK_MAX_SAMPLES 2000

void BENCHMARK_Init(void);
void BENCHMARK_Start(void);
void BENCHMARK_Stop(void);
void BENCHMARK_Report(const char *label);
void BENCHMARK_Reset(void);

extern uint32_t benchmark_samples[];
extern uint32_t benchmark_sample_count;

#endif //BENCHMARK_H