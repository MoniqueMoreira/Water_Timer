#include "flash_mem.h"

typedef struct {
    bool is_initialized;
    const char *module_name;
} Self;

static Self self = {
    .is_initialized = false,
    .module_name = "FLASH_MEM"
};

/**
 * @brief Função auxiliar para chamar flash_range_erase dentro de flash_safe_execute.
 * @param param Ponteiro para os parâmetros (offset e size).
 * @param offset Offset na flash.
 * @param size Tamanho a ser apagado.
 * 
 */
static void __call_flash_range_erase__(void *param);

/**
 * @brief Função auxiliar para chamar flash_range_program dentro de flash_safe_execute.
 * @param param Ponteiro para os parâmetros (offset, data e size).
 * @param offset Offset na flash.
 * @param data Ponteiro para os dados a serem escritos.
 * @param size Tamanho a ser escrito.
 * 
 */
static void __call_flash_range_program__(void *param);


static void __call_flash_range_erase__(void *param)
{
    uintptr_t *params = (uintptr_t *)param;
    uint32_t offset   = (uint32_t)params[0];
    uint32_t size     = (uint32_t)params[1];

    flash_range_erase(offset, size);
}

static void __call_flash_range_program__(void *param)
{
    uintptr_t *params = (uintptr_t *)param;
    uint32_t offset         = (uint32_t)params[0];
    const uint8_t *data     = (const uint8_t *)params[1];
    uint32_t size           = (uint32_t)params[2];

    flash_range_program(offset, data, size);
}


FLASH_Return_Status_t FLASH_load_data(uint32_t offset ,uint8_t *data, uint32_t size)
{
    if(!self.is_initialized) {
        return FLASH_OPERATION_NOT_INITIALIZED;
    }

    if (size == 0)
    {
        LOGGER_Error(self.module_name,"Não é possível ler 0 bytes da flash");
        return FLASH_OPERATION_INVALID_PARAM;
    }

    const volatile uint8_t *flash_ptr = (const volatile uint8_t *)(XIP_BASE + offset);
    memcpy(data, (const void *)flash_ptr, size);

    return FLASH_OPERATION_SUCCESS;
}

FLASH_Return_Status_t FLASH_save_data(uint32_t offset, uint8_t *data, uint32_t size)
{
    if(!self.is_initialized) {
        return FLASH_OPERATION_NOT_INITIALIZED;
    }

    if (data == NULL || size == 0 || (size % FLASH_PAGE_SIZE) != 0)
    {
        LOGGER_Error(self.module_name, "Tamanho inválido para salvar na flash: %d", size);
        return FLASH_OPERATION_INVALID_PARAM;
    }

    uintptr_t params[] = {
        (uintptr_t)offset,
        (uintptr_t)data,
        (uintptr_t)size
    };

    int rc = flash_safe_execute(__call_flash_range_program__, params, UINT32_MAX);

    if (rc != PICO_OK || FLASH_is_empty(offset, size) != FLASH_NOT_EMPTY)
    {
        LOGGER_Error(self.module_name,"Erro ao escrever na flash: %d", rc);
        return FLASH_OPERATION_ERROR;
    }

    LOGGER_Success(self.module_name, "Dados salvos na flash com sucesso: %d bytes", size);
    return FLASH_OPERATION_SUCCESS;
}

FLASH_Return_Status_t FLASH_erase_data(uint32_t offset, uint32_t size)
{
    if(!self.is_initialized) {
        return FLASH_OPERATION_NOT_INITIALIZED;
    }

    if (size == 0 || (size % FLASH_SECTOR_SIZE) != 0)
    {
        LOGGER_Error(self.module_name,"Tamanho inválido para apagar na flash: %d", size);
        return FLASH_OPERATION_INVALID_PARAM;
    }

    uintptr_t params[] = {
        (uintptr_t)offset,
        (uintptr_t)size
    };

    int rc = flash_safe_execute(__call_flash_range_erase__, params, UINT32_MAX);

    if (rc != PICO_OK || FLASH_is_empty(offset, size) != FLASH_EMPTY)
    {
        LOGGER_Error(self.module_name, "Erro ao apagar a flash: %d", rc);
        return FLASH_OPERATION_ERROR;
    }

    LOGGER_Success(self.module_name, "Flash apagada com sucesso: %d bytes", size);
    return FLASH_OPERATION_SUCCESS;
}

FLASH_Empty_Status_t FLASH_is_empty(uint32_t offset, uint32_t size)
{
    const uint8_t *ptr = (const uint8_t *)(XIP_BASE + offset);

    for (uint32_t i = 0; i < size; i++)
    {
        if (ptr[i] != 0xFF)
        {
            return FLASH_NOT_EMPTY;
        }
    }
    return FLASH_EMPTY;
}

RETURN_STATUS_t FLASH_Init(void)
{
    if(self.is_initialized) {
        return RETURN_STATUS_OK; // Já inicializado
    }

    self.is_initialized = true;

    return RETURN_STATUS_OK;
}