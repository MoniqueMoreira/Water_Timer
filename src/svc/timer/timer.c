#include "timer.h"
#include "timer_benchmark.h"
#include "return_status.h"
#include "itf/logger/logger.h"
#include "test/benchmark.h"
#include "test/benchmark_cfg.h"
#include "svc/watchdog/watchdog.h"
#include "svc/watchdog/watchdog_cfg.h"


#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include <string.h>
#include <stdio.h>

typedef struct {
    TIMER_Return_Status_t status;
    uint32_t duration_ms;
    uint32_t elapsed_ms;
    const char *name;
} TIMER_Config_t;

typedef struct {
    bool is_initialized;
    TIMER_Config_t timers[MAX_TIMERS];
    SemaphoreHandle_t mutex; 
} Self;

static Self self = { .is_initialized = false };

/**
 * @brief Task que atualiza todos os self.timers (chamada pelo FreeRTOS)
 */
static void __TIMER_Task__(void *pvParameters);

static RETURN_STATUS_t __TIMER_Run_Timer(void);

// Inicializa todos os timers
RETURN_STATUS_t TIMER_Init() {
    if (self.is_initialized) {
        return RETURN_STATUS_OK; // já inicializado
    }

    for (int i = 0; i < MAX_TIMERS; i++) {
        self.timers[i].status = TIMER_INACTIVE;
        self.timers[i].elapsed_ms = 0;
        self.timers[i].duration_ms = 0;
        self.timers[i].name = NULL;
    }

    self.mutex = xSemaphoreCreateMutex();
    if (self.mutex == NULL) {
        return RETURN_STATUS_ERROR; // falha ao criar mutex
    }


#ifdef TIMER_BENCHMARK_ENABLED

    TIMER_BENCHMARK_Run_All();
    
    BENCHMARK_Reset();
    for(int i=0; i<BENCHMARK_MAX_SAMPLES; i++){
        BENCHMARK_Start();
        __TIMER_Run_Timer();
        BENCHMARK_Stop();
    }
    BENCHMARK_Report("TIMER_TASK");
#else
    xTaskCreate(__TIMER_Task__, "TIMER_Task", 256, NULL, tskIDLE_PRIORITY + 3, NULL);
#endif // TIMER_BENCHMARK_ENABLED
    self.is_initialized = true;
    return RETURN_STATUS_OK;
}

TIMER_Return_Status_t TIMER_Start(const char *name, uint32_t duration_ms) {
    if (xSemaphoreTake(self.mutex, pdMS_TO_TICKS(100)) == pdTRUE) {
        int free_slot = -1;

        for (int i = 0; i < MAX_TIMERS; i++) {
            if (self.timers[i].status == TIMER_INACTIVE && free_slot == -1) {
                free_slot = i; // primeiro slot livre
            } else if (self.timers[i].status != TIMER_INACTIVE && self.timers[i].name != NULL) {
                if (strcmp(self.timers[i].name, name) == 0) {
                    xSemaphoreGive(self.mutex);
                    return TIMER_NAME_EXISTS; // já existe com esse nome
                }
            }
        }

        if (free_slot != -1) {
            self.timers[free_slot].status = TIMER_ACTIVE;
            self.timers[free_slot].duration_ms = duration_ms;
            self.timers[free_slot].elapsed_ms = 0;
            self.timers[free_slot].name = name;
            xSemaphoreGive(self.mutex);
            return TIMER_ACTIVE;
        }

        xSemaphoreGive(self.mutex);
    }

    return TIMER_UNAVAILABLE;
}

TIMER_Return_Status_t TIMER_Stop(const char *name) {
    if (xSemaphoreTake(self.mutex, pdMS_TO_TICKS(100)) == pdTRUE) {
        for (int i = 0; i < MAX_TIMERS; i++) {
            if (self.timers[i].name && strcmp(self.timers[i].name, name) == 0) {
                self.timers[i].status = TIMER_INACTIVE;
                self.timers[i].elapsed_ms = 0;
                self.timers[i].duration_ms = 0;
                self.timers[i].name = NULL;
                xSemaphoreGive(self.mutex);
                return TIMER_INACTIVE;
            }
        }
        xSemaphoreGive(self.mutex);
    }
    return TIMER_UNAVAILABLE;
}

TIMER_Return_Status_t TIMER_Reset(const char *name) {
    if (xSemaphoreTake(self.mutex, pdMS_TO_TICKS(100)) == pdTRUE) {
        for (int i = 0; i < MAX_TIMERS; i++) {
            if (self.timers[i].name && strcmp(self.timers[i].name, name) == 0) {
                self.timers[i].elapsed_ms = 0;
                self.timers[i].status = TIMER_ACTIVE;
                xSemaphoreGive(self.mutex);
                return TIMER_ACTIVE;
            }
        }
        xSemaphoreGive(self.mutex);
    }
    return TIMER_UNAVAILABLE;
}

TIMER_Return_Status_t TIMER_Check(const char *name) {
    TIMER_Return_Status_t result = TIMER_INACTIVE;

    if (xSemaphoreTake(self.mutex, pdMS_TO_TICKS(100)) == pdTRUE) {
        for (int i = 0; i < MAX_TIMERS; i++) {
            if (self.timers[i].name && strcmp(self.timers[i].name, name) == 0) {
                result = self.timers[i].status;
                break;
            }
        }
        xSemaphoreGive(self.mutex);
    }

    return result;
}

static RETURN_STATUS_t __TIMER_Run_Timer(void)
{
    RETURN_STATUS_t err =  RETURN_STATUS_TIMEOUT;
    if (xSemaphoreTake(self.mutex, pdMS_TO_TICKS(100)) == pdTRUE) {
        for (int i = 0; i < MAX_TIMERS; i++) {
            if (self.timers[i].status == TIMER_ACTIVE) {
                self.timers[i].elapsed_ms++;
                if (self.timers[i].elapsed_ms >= self.timers[i].duration_ms) {
                    self.timers[i].status = TIMER_EXPIRED;
                    //LOGGER_Info("TIMER", "Timer '%s' expirado", self.timers[i].name);
                }
            }
        }
        xSemaphoreGive(self.mutex);
        err = RETURN_STATUS_OK;
    }
    WATCHDOG_Notify(WDOG_TASK_TIMER);
    return err;
}

// Task que atualiza todos os timers
static void __TIMER_Task__(void *pvParameters) {
    while (1) {
        
        __TIMER_Run_Timer();
        vTaskDelay(pdMS_TO_TICKS(1)); // aguarda 1 ms
    }
}
