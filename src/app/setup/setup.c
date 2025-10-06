#include "setup.h"
#include "itf/logger/logger.h"
#include "svc/timer/timer.h"
#include "svc/io_manager/io_manager.h"
#include "svc/watchdog/watchdog.h"

static void __SETUP_Task__(void *pvParameters) {
    (void) pvParameters;

    // Timers
    TIMER_Start("led_timer", 10000);   // 10 segundos
    TIMER_Start("Relé", 10000);        // desativado inicialmente

    IO_OutputState_t outputs = {0};    // zera saídas
    IO_InputState_t inputs   = {0};

    bool state = false;

    TickType_t xLastWakeTime = xTaskGetTickCount();
    for (;;) {
        inputs = IO_MANAGER_Get_Inputs();

        // Botão 1 -> controla relé com timer
        if (inputs.button[0]) {
            outputs.relay[0] = true; // inverte estado
            LOGGER_Info("SETUP_TASK", "Botão 1 pressionado, estado do relé: %d", outputs.relay[0]);
            TIMER_Start("Relé", 10000); // liga por 10s
        }
        if (TIMER_Check("Relé") == TIMER_EXPIRED) {
            LOGGER_Info("SETUP_TASK", "Timer do relé expirou, desligando relé");
            TIMER_Stop("Relé");
            outputs.relay[0] = false;
        }

        // Botão 2 -> toggle LED1 e LED2
        if (inputs.button[1]) {
            outputs.led[0] = !outputs.led[0];
            outputs.led[1] = !outputs.led[1];
            LOGGER_Info("SETUP_TASK", "Botão 2 pressionado, estado do LED1: %d, LED2: %d",
                        outputs.led[0], outputs.led[1]);
        }

        // Timer LED -> piscar LED3 e LED_RELAY_PIN
        if (TIMER_Check("led_timer") == TIMER_EXPIRED) {
            printf("LED Timer Expirou!\n");
            TIMER_Reset("led_timer");

            state = !state;  // alterna estado geral
            outputs.led[2] = state;
            outputs.led[3] = state;
        }

        // Aplica estados nas saídas
        IO_MANAGER_Set_Outputs(outputs);

        //WATCHDOG_Notify(WDOG_TASK_SETUP);
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(100));
    }
}

RETURN_STATUS_t SETUP_Init(void) {
    // Cria tarefa de setup
    if (xTaskCreate(__SETUP_Task__, "Setup", 512, NULL, 2, NULL) != pdPASS) {
        return RETURN_STATUS_ERROR;
    }
    return RETURN_STATUS_OK;
}
