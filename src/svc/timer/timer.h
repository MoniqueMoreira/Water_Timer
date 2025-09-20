// soft_TIMEr.h
#ifndef SOFT_TIMER_H
#define SOFT_TIMER_H

#include <stdint.h>
#include <stdbool.h>

#include "return_status.h"

#define MAX_TIMERS 5

typedef enum {
    TIMER_INACTIVE,
    TIMER_ACTIVE,
    TIMER_EXPIRED,
    TIMER_UNAVAILABLE, 
    TIMER_NAME_EXISTS,
} TIMER_Return_Status_t;

/**
 * @brief Inicializa o módulo de timers
 * @return RETURN_Status_t - Status da operação
 */
RETURN_STATUS_t TIMER_Init();

/**
 * @brief Inicia um timer com nome e duração
 * @param name Nome do timer
 * @param duration_ms Duração em milissegundos
 * @return RETURN_Status_t - Status do timer
 */
TIMER_Return_Status_t TIMER_Start(const char *name, uint32_t duration_ms);

/**
 * @brief Para um timer pelo nome
 * @param name Nome do timer
 * @return RETURN_Status_t - Status do timer
 */
TIMER_Return_Status_t TIMER_Stop(const char *name);

/**
 * @brief Reseta um timer (zera a contagem)
 * @param name Nome do timer
 * @return RETURN_Status_t - Status do timer
 */
TIMER_Return_Status_t TIMER_Reset(const char *name);

/**
 * @brief Checa o estado do timer
 * @param name Nome do timer
 * @return TIME_Return_Status_t - Estado do timer (INACTIVE, ACTIVE, EXPIRED)
 */
TIMER_Return_Status_t TIMER_Check(const char *name);

#endif // SOFT_TIMER_H
