#pragma once

#define WDOG_TIMEOUT_MS        3000  // tempo total do watchdog de hardware
#define WDOG_CHECK_PERIOD_MS    300  // intervalo de verificação do serviço

typedef enum
{
    WDOG_TASK_IO_INPUT,
    WDOG_TASK_IO_OUTPUT,
    //WDOG_TASK_SETUP,
    //WDOG_TASK_TIMER,

    WDOG_TASK_SIZE,
} WdogTaskIndex_t;