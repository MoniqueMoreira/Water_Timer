#include "itf/logger/logger.h"
#include "svc/timer/timer.h"
#include "pico/stdlib.h"
#include <stdio.h>

#include "hardware/uart.h"
#include "FreeRTOS.h"

#define LED_PIN 25
#define TIMEOUT_MS 10000
#define TIMEOUT_SHORT_MS 2000

TaskHandle_t xTask_Init = NULL;

static void __Init_Task__(void *params);

void setup_task(void *pvParameters) {
    (void) pvParameters;
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_put(LED_PIN, 0);
    
    TIMER_Start("led_timer", 1000); // 1 segundo

    while(1) {
        if(TIMER_Check("led_timer") == TIMER_EXPIRED) {
            printf("LED Timer Expirou!\n");
            // decide quando reiniciar
            TIMER_Reset("led_timer");
            gpio_put(LED_PIN, !gpio_get(LED_PIN)); // toggle LED
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

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

    xTaskCreate(setup_task, "SetupTask", 256, NULL, 1, NULL);
    vTaskDelete(xTask_Init);

}