/**
 * @file wdog_svc.c
 * @brief Implementação do serviço de watchdog cooperativo
 */

#include "pico/stdlib.h"
#include <stdio.h>

#include "watchdog.h"
#include "watchdog_cfg.h"

TaskHandle_t xWatchdog = NULL;

void WATCHDOG_Notify(WdogTaskIndex_t task_index)
{
    if (task_index < WDOG_TASK_SIZE && xWatchdog != NULL)
    {
        xTaskNotifyGiveIndexed(xWatchdog, task_index);
    }
}

static void __WATCHDOG_Task__(void *param)
{
    (void)param;
    TickType_t last_wake = xTaskGetTickCount();

    while (true)
    {
        bool all_ok = true;
        int failed_task = -1;

        // Verifica se todas as tarefas notificaram desde o último ciclo
        for (int i = 0; i < WDOG_TASK_SIZE; i++)
        {
            if (ulTaskNotifyTakeIndexed(i, pdTRUE, pdMS_TO_TICKS(200)) == 0)
            {
                all_ok = false;
                failed_task = i;
                break;
            }
        }

        if (all_ok)
        {
            watchdog_update(); // alimenta o watchdog de hardware
        }
        else
        {
            printf("[WDG] Falha na tarefa %d. Reiniciando...\n", failed_task);
            watchdog_hw->scratch[0] = failed_task; // armazena a falha
            sleep_ms(100);
            watchdog_reboot(0, 0, 0); // reinicia o RP2040
        }

        vTaskDelayUntil(&last_wake, pdMS_TO_TICKS(WDOG_CHECK_PERIOD_MS));
    }
}


RETURN_STATUS_t WATCHDOG_Init(void)
{
    // Se o último reset foi causado pelo watchdog, exibe qual tarefa falhou
    if (watchdog_caused_reboot())
    {
        uint32_t failed_task = watchdog_hw->scratch[0];
        printf("[WDG] Sistema reiniciado pelo watchdog! Falha na tarefa %lu\n",
               failed_task);
    }

    watchdog_enable(WDOG_TIMEOUT_MS, true);

    xTaskCreate(__WATCHDOG_Task__, "WATCHDOG", 512, NULL, tskIDLE_PRIORITY + 2, &xWatchdog);

    return RETURN_STATUS_OK;
}
