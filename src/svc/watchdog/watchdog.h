#pragma once

#include "FreeRTOS.h"
#include "task.h"
#include "hardware/watchdog.h"
#include <stdio.h>
#include "return_status.h"
#include "watchdog_cfg.h"

/**
 * @brief Inicializa o serviço de watchdog
 */
RETURN_STATUS_t WATCHDOG_Init(void);

/**
 * @brief Marca uma tarefa como ativa (alimentação do watchdog de software)
 * 
 * @param task_index Índice da tarefa que deseja notificar
 */
void WATCHDOG_Notify(WdogTaskIndex_t task_index);
