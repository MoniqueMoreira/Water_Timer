#include "logger.h"
#include "pico/stdlib.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

static LOGGER_Config_t logger_default = {
    .backend = LOGGER_BACKEND_DEBUG,
    .uart = NULL,
};

typedef struct {
    LOGGER_level_t level;
    uint32_t uptime_ms;
    char task_name[16];
    char module[16];
    char msg[LOGGER_MSG_LEN];
} LOGGER_message_t;

typedef struct {
    bool is_initialized;
    QueueHandle_t logger_queue;
    LOGGER_Config_t logger_cfg;
}Self;

static Self self = {
    .is_initialized = false,
    .logger_queue = NULL,
    .logger_cfg = {0},
};

/**
 * @brief Task dedicada do LOGGER
 * @param params (não usado)
 */
static void __LOGGER_Task__(void *params);

// Task dedicada do LOGGER
static void __LOGGER_Task__(void *params) {
    LOGGER_message_t m;
    const char *color;
    const char *level_str;

    while (1) {
        if (xQueueReceive(self.logger_queue, &m, portMAX_DELAY)) {
            
            switch (m.level) {
                case LOGGER_INFO:    color = COLOR_INFO;    level_str = "INFO";  break;
                case LOGGER_SUCCESS: color = COLOR_SUCCESS; level_str = "OK";    break;
                case LOGGER_ERROR:   color = COLOR_ERROR;   level_str = "ERROR"; break;
                default:             color = COLOR_RESET;   level_str = "UNK";   break;
            }

            uint32_t seconds = m.uptime_ms / 1000;
            uint32_t minutes = seconds / 60;
            uint32_t hours   = minutes / 60;

            char buffer[220];
            snprintf(buffer, sizeof(buffer),
                     "%s[%02lu:%02lu:%02lu][%s][%s][%s] %s%s\r\n",
                     color,
                     hours % 24, minutes % 60, seconds % 60,
                     m.task_name,
                     m.module,
                     level_str,
                     m.msg,
                     COLOR_RESET);

            if (self.logger_cfg.backend == LOGGER_BACKEND_UART) {
                uart_puts((uart_inst_t *)self.logger_cfg.uart, buffer);
            } else {
                printf("%s", buffer);
            }
        }
    }
}

RETURN_STATUS_t LOGGER_Init(const LOGGER_Config_t *config) {

    if (self.is_initialized){
        return RETURN_STATUS_OK; // já inicializado
    };

    if (config == NULL) {
        config = &logger_default; // usa configuração padrão
    }

    self.is_initialized = true;
    self.logger_cfg = *config;
    self.logger_queue = xQueueCreate(LOGGER_QUEUE_LEN, sizeof(LOGGER_message_t));

    xTaskCreate(__LOGGER_Task__, "LoggerTask", 1024, NULL, tskIDLE_PRIORITY + 1, NULL);

    return RETURN_STATUS_OK;
}

// Auxiliar para mandar msg
static bool __LOGGER_Send__(LOGGER_level_t level, const char *module, const char *fmt, va_list args) {
    
    if (!self.is_initialized) return false;

    LOGGER_message_t m;
    m.level = level;
    m.uptime_ms = to_ms_since_boot(get_absolute_time());

    // Nome da task que chamou
    TaskHandle_t current = xTaskGetCurrentTaskHandle();
    const char *task_name = pcTaskGetName(current);
    strncpy(m.task_name, task_name ? task_name : "UNKNOWN", sizeof(m.task_name)-1);
    m.task_name[sizeof(m.task_name)-1] = '\0';

    // Nome do módulo
    strncpy(m.module, module ? module : "GENERIC", sizeof(m.module)-1);
    m.module[sizeof(m.module)-1] = '\0';

    // Mensagem
    vsnprintf(m.msg, sizeof(m.msg), fmt, args);

    if(xQueueSend(self.logger_queue, &m, 0) != pdTRUE) {
        // Fila cheia, descarta
        return false;
    }
    return true;
}

// Wrappers
bool LOGGER_Info(const char *module, const char *fmt, ...) {
    if (!self.is_initialized) return false;

    va_list args; va_start(args, fmt);
    if(__LOGGER_Send__(LOGGER_INFO, module, fmt, args)== false) {
        va_end(args);
        return false;
    }
    va_end(args);

    return true;
}

bool LOGGER_Success(const char *module, const char *fmt, ...) {
    if (!self.is_initialized) return false;

    va_list args; va_start(args, fmt);
    if(__LOGGER_Send__(LOGGER_SUCCESS, module, fmt, args) == false) {
        va_end(args);
        return false;
    }
    va_end(args);

    return true;
}

bool LOGGER_Error(const char *module, const char *fmt, ...) {
    if (!self.is_initialized) return false;

    va_list args; va_start(args, fmt);
    if (__LOGGER_Send__(LOGGER_ERROR, module, fmt, args) == false) {
        va_end(args);
        return false;
    }
    va_end(args);

    return true;
}
