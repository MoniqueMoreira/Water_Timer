#ifndef IO_MANAGER_H
#define IO_MANAGER_H

#include <stdint.h>
#include <stdbool.h>

#include "return_status.h"
#include "io_manager_cfg.h"


// ==== Estruturas para armazenar estados ====
typedef struct {
    bool button [BTN_COUNT];
    uint16_t adc[ADC_COUNT];
} IO_InputState_t;

typedef struct {
    bool led [LED_COUNT];
    bool relay[RELEY_COUNT];
} IO_OutputState_t;

/**
 * @brief Inicializa o módulo de I/O 
 */
RETURN_STATUS_t IO_MANAGER_Init(void);

/**
 * @brief Obtém o estado atual das entradas (botões, ADCs)
 * @return Estrutura com o estado das entradas
 */
IO_InputState_t IO_MANAGER_Get_Inputs(void);

/**
 * @brief Obtém o estado atual das saídas (LEDs, relé)
 * @return Estrutura com o estado das saídas
 */
IO_OutputState_t IO_MANAGER_Get_Outputs(void);

/**
 * @brief Define o estado das saídas (LEDs, relé)
 * @param newState Estrutura com o novo estado das saídas
 */
void IO_MANAGER_Set_Outputs(IO_OutputState_t newState);

#endif
