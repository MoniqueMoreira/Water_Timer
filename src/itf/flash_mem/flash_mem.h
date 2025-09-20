/*
 *
 * Informações sobre escrita da flash:
 *
 * - A flash só pode ser escrita em páginas de 256 bytes.
 * - A flash só pode ser apagada em setores de 4096 bytes.
 * - Para escrita de dados, é necessário apagar a flash antes.
 * - A flash deve ser executarda em um contexto seguro,
 * sempre em um único núcleo do processado. Para isso, deve-se usar
 * a função flash_safe_execute().
 *
 */

//==========================================================================

#pragma once

#include "pico/stdlib.h"
#include "pico/flash.h"
#include "return_status.h"
#include "itf/logger/logger.h"
#include "hardware/flash.h"

#include <string.h>


typedef enum
{
    FLASH_OPERATION_SUCCESS = 0, /**< Sucesso */
    FLASH_OPERATION_ERROR,       /**< Erro genérico */
    FLASH_OPERATION_NOT_INITIALIZED, /**< Flash não inicializada */
    FLASH_OPERATION_INVALID_PARAM,   /**< Parâmetro inválido */
    FLASH_OPERATION_SIZE         /**< Quantidade de erros */
} FLASH_Return_Status_t;

typedef enum
{
    FLASH_NOT_EMPTY = 0, /**< Flash não está vazia */
    FLASH_EMPTY,         /**< Flash está vazia */
    FLASH_EMPTY_SIZE     /**< Quantidade de estados */
} FLASH_Empty_Status_t;

/**
 * @brief Carrega os dados de configuração da flash.
 *
 * @param data Ponteiro para os dados a serem carregados.
 * @param size Tamanho dos dados a serem carregados.
 *
 * @return FLASH_Return_Status_t Status da operação.
 */
FLASH_Return_Status_t FLASH_load_data(uint32_t offset, uint8_t *data, uint32_t size);

/**
 * @brief Salva os dados de configuração na flash. Limite: 256 bytes.
 *
 * @param data Ponteiro para os dados a serem salvos.
 * @param size Tamanho dos dados a serem salvos.
 *
 * @return FLASH_Return_Status_t Status da operação.
 */
FLASH_Return_Status_t FLASH_save_data(uint32_t offset, uint8_t *data, uint32_t size);

/**
 * @brief Apaga o primeiro setor de dados de configuração na flash (4096 bytes).
 *
 */
FLASH_Return_Status_t FLASH_erase_data(uint32_t offset, uint32_t size);

/**
 * @brief Verifica se a flash está vazia.
 *
 * @return FLASH_EmptyStatus_t Status da operação.
 */
FLASH_Empty_Status_t FLASH_is_empty(uint32_t offset, uint32_t  size);

/**
 * @brief Inicializa a interface de flash.
 */
RETURN_STATUS_t FLASH_Init(void);