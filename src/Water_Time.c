#include "app/setup/setup.h"
#include "itf/logger/logger.h"
#include "svc/timer/timer.h"
#include "svc/io_manager/io_manager.h"

#include "pico/stdlib.h"
#include <stdio.h>

#include "hardware/uart.h"
#include "FreeRTOS.h"

TaskHandle_t xTask_Init = NULL;

static void __Init_Task__(void *params);

int main() {
    stdio_init_all();

    sleep_ms(2000); // aguarda conexão serial
    
    xTaskCreate(__Init_Task__, "Init", 1024, NULL, 2, &xTask_Init);

    vTaskStartScheduler();
}

static void __Init_Task__(void *params) {
    (void) params;

    printf("Iniciando sistema...\n");


    if (LOGGER_Init(NULL) != RETURN_STATUS_OK) {
        printf("Erro ao iniciar logger!\n");
        while (1) vTaskDelay(pdMS_TO_TICKS(1000));
    }
    if (TIMER_Init() != RETURN_STATUS_OK) {
        LOGGER_Info("SYSTEM", "Erro ao iniciar timer!");
        while (1) vTaskDelay(pdMS_TO_TICKS(1000));
    }
    if (SETUP_Init() != RETURN_STATUS_OK) {
        LOGGER_Info("SYSTEM", "Erro ao iniciar Setup!");
        while (1) vTaskDelay(pdMS_TO_TICKS(1000));
    }
    if (IO_MANAGER_Init() != RETURN_STATUS_OK) {
        LOGGER_Info("SYSTEM", "Erro ao iniciar IO Manager!");
        while (1) vTaskDelay(pdMS_TO_TICKS(1000));
    }
    if(WATCHDOG_Init() != RETURN_STATUS_OK) {
        LOGGER_Info("SYSTEM", "Erro ao iniciar Watchdog!");
        while (1) vTaskDelay(pdMS_TO_TICKS(1000));
    }
    

    vTaskDelete(xTask_Init);

}