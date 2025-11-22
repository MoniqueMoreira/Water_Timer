#include "timer_benchmark.h"
#include "test/benchmark.h"
#include "svc/timer/timer.h"
#include "itf/logger/logger.h"

#ifdef TIMER_BENCHMARK_ENABLED

void TIMER_BENCHMARK_Run_Start(void)
{
    BENCHMARK_Reset();

    for (int i = 0; i < BENCHMARK_MAX_SAMPLES; i++) {

        BENCHMARK_Start();
        TIMER_Start("TESTE", 1000);
        BENCHMARK_Stop();

        TIMER_Stop("TESTE");   // garante que não acumula timers ativos
    }

    BENCHMARK_Report("TIMER_START");
}

void TIMER_BENCHMARK_Run_Stop(void)
{
    BENCHMARK_Reset();

    for (int i = 0; i < BENCHMARK_MAX_SAMPLES; i++) {

        TIMER_Start("TESTE", 1000);

        BENCHMARK_Start();
        TIMER_Stop("TESTE");
        BENCHMARK_Stop();
    }

    BENCHMARK_Report("TIMER_STOP");
}

void TIMER_BENCHMARK_Run_Reset(void)
{
    BENCHMARK_Reset();

    for (int i = 0; i < BENCHMARK_MAX_SAMPLES; i++) {

        TIMER_Start("TESTE", 1000);

        BENCHMARK_Start();
        TIMER_Reset("TESTE");
        BENCHMARK_Stop();

        TIMER_Stop("TESTE");  // limpeza
    }

    BENCHMARK_Report("TIMER_RESET");
}

void TIMER_BENCHMARK_Run_Check(void)
{
    BENCHMARK_Reset();

    for (int i = 0; i < BENCHMARK_MAX_SAMPLES; i++) {

        TIMER_Start("TESTE", 1000);

        BENCHMARK_Start();
        TIMER_Check("TESTE");
        BENCHMARK_Stop();

        TIMER_Stop("TESTE");
    }

    BENCHMARK_Report("TIMER_CHECK");  // corrigido
}

void TIMER_BENCHMARK_Run_All(void)
{
    TIMER_BENCHMARK_Run_Start();
    TIMER_BENCHMARK_Run_Stop();
    TIMER_BENCHMARK_Run_Reset();
    TIMER_BENCHMARK_Run_Check();
}

#endif // TIMER_BENCHMARK_ENABLED
