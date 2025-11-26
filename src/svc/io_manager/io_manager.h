#ifndef IO_MANAGER_H
#define IO_MANAGER_H

#include <stdint.h>
#include <stdbool.h>

#include "return_status.h"
#include "io_manager_cfg.h"

// ==== Estruturas para armazenar estados ====
typedef struct {
    bool button[BTN_COUNT];
    float adc[ADC_COUNT];
} IO_InputState_t;

typedef struct {
    bool led[LED_COUNT];
    bool relay[RELAY_COUNT];
} IO_OutputState_t;

// ==== Inicialização ====
/**
 * @brief Inicializa o módulo de I/O
 * @return ERROR_OK se inicializou corretamente, outro valor caso contrário
 */
RETURN_STATUS_t IO_MANAGER_Init(void);

// ==== Funções Run (para task ou benchmark) ====
/**
 * @brief Executa leitura das entradas (botões, ADCs)
 * @return ERROR_OK se sucesso, ERROR_TIMEOUT se não conseguiu adquirir mutex
 */
RETURN_STATUS_t IO_MANAGER_Input_Run(void);

/**
 * @brief Executa atualização das saídas (LEDs, relé)
 * @return ERROR_OK se sucesso, ERROR_TIMEOUT se não conseguiu adquirir mutex
 */
RETURN_STATUS_t IO_MANAGER_Output_Run(void);

// ==== APIs com ponteiro para dados ====
/**
 * @brief Obtém o estado atual das entradas
 * @param pCopy Ponteiro para estrutura que receberá os dados
 * @return ERROR_OK se sucesso, ERROR_TIMEOUT se não conseguiu adquirir mutex, ERROR_PARAM se ponteiro inválido
 */
RETURN_STATUS_t IO_MANAGER_Get_Inputs(IO_InputState_t *pCopy);

/**
 * @brief Define o estado das saídas
 * @param pNewState Ponteiro para estrutura com o novo estado das saídas
 * @return ERROR_OK se sucesso, ERROR_TIMEOUT se não conseguiu adquirir mutex, ERROR_PARAM se ponteiro inválido
 */
RETURN_STATUS_t IO_MANAGER_Set_Outputs(const IO_OutputState_t *pNewState);

/**
 * @brief Obtém o estado atual das saídas
 * @param pCopy Ponteiro para estrutura que receberá os dados
 * @return ERROR_OK se sucesso, ERROR_TIMEOUT se não conseguiu adquirir mutex, ERROR_PARAM se ponteiro inválido
 */
RETURN_STATUS_t IO_MANAGER_Get_Outputs(IO_OutputState_t *pCopy);

#endif // IO_MANAGER_H
