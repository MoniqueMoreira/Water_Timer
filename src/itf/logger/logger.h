#ifndef LOGGER_H
#define LOGGER_H

#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include <stdint.h>
#include <stdbool.h>

#include "return_status.h"

// Cores ANSI
#define COLOR_RESET   "\033[0m"
#define COLOR_INFO    "\033[34m" // azul
#define COLOR_SUCCESS "\033[32m" // verde
#define COLOR_ERROR   "\033[31m" // vermelho

#define LOGGER_QUEUE_LEN 32
#define LOGGER_MSG_LEN   128

// Tipos de saída
typedef enum {
    LOGGER_BACKEND_UART,
    LOGGER_BACKEND_DEBUG
} LOGGER_backend_t;

// Níveis de log
typedef enum {
    LOGGER_INFO,
    LOGGER_SUCCESS,
    LOGGER_ERROR
} LOGGER_level_t;

// Configuração
typedef struct {
    LOGGER_backend_t backend;
    void *uart;   // ponteiro para handle da UART (se backend = UART)
} LOGGER_Config_t;

/**
 * @brief Inicializa o sistema de logging
 */
RETURN_STATUS_t LOGGER_Init(const LOGGER_Config_t *config);

/**
 * @brief Loga uma mensagem informativa
 * @param module Nome do módulo (ex: "SENSOR", "MAIN", etc)
 * @param fmt Formato da mensagem (como printf)
 * @return true se a mensagem foi enfileirada com sucesso, false caso contrário
 */
bool LOGGER_Info(const char *module, const char *fmt, ...);
/**
 * @brief Loga uma mensagem de sucesso
 * @param module Nome do módulo (ex: "SENSOR", "MAIN", etc)
 * @param fmt Formato da mensagem (como printf)
 * @return true se a mensagem foi enfileirada com sucesso, false caso contrário
 */
bool LOGGER_Success(const char *module, const char *fmt, ...);
/**
 * @brief Loga uma mensagem de erro
 * @param module Nome do módulo (ex: "SENSOR", "MAIN", etc)
 * @param fmt Formato da mensagem (como printf)
 * @return true se a mensagem foi enfileirada com sucesso, false caso contrário
 */
bool LOGGER_Error(const char *module, const char *fmt, ...);

#endif // LOGGER_H
