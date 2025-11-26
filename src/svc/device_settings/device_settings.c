#include <string.h>
#include <FreeRTOS.h>
#include <semphr.h>
#include <stdio.h>

#include "device_settings.h"
#include "itf/flash_mem/flash_mem.h"

// Offset deve ser alinhado e dentro da área segura
#define DEVICE_SETTINGS_FLASH_OFFSET   0x18000   
#define TIMEOUT_MS                     1000      

// ======================
// PRESETS
// ======================
static const DEVICE_SETTINGS_t presets[] = {
    { MODE_TIMER, DEVICE_SETTINGS_MODE_CONFIG_1,  5000, 10000 },
    { MODE_TIMER, DEVICE_SETTINGS_MODE_CONFIG_2, 10000, 15000 },
    { MODE_TIMER, DEVICE_SETTINGS_MODE_CONFIG_3, 15000, 20000 },
    { MODE_TIMER, DEVICE_SETTINGS_MODE_CONFIG_4, 20000, 25000 },
};


// ======================
// SELF STRUCT
// ======================
static struct
{
    bool is_init;

    SemaphoreHandle_t semaphore;

    union 
    {
        DEVICE_SETTINGS_t device_settings;
        uint8_t container[FLASH_MEM_PAGE_SIZE];
    };

    DEVICE_SETTINGS_t device_settings_default;

} _self = {
    .is_init = false,

    .device_settings_default =
    {
        .mode_operation   = MODE_TIMER,
        .configured       = DEVICE_SETTINGS_MODE_CONFIG_1,
        // Config 1 usa índice 0
        .duration_open    = presets[0].duration_open,
        .duration_closed  = presets[0].duration_closed,
    },
};


// ======================
// INIT
// ======================
RETURN_STATUS_t DEVICE_SETTINGS_Init(void)
{
    _self.semaphore = xSemaphoreCreateMutex();
    if (_self.semaphore == NULL)
    {
        printf("[DEVICE_SETTINGS] Erro ao criar semáforo\n");
        return RETURN_STATUS_INVALID_PARAM;
    }

    printf("[DEVICE_SETTINGS] Iniciando módulo\n");

    if (FLASH_is_empty(DEVICE_SETTINGS_FLASH_OFFSET, FLASH_MEM_PAGE_SIZE) == FLASH_EMPTY)
    {
        printf("[DEVICE_SETTINGS] Flash vazia, salvando configuração padrão\n");

        _self.device_settings = _self.device_settings_default;

        if (FLASH_save_data(DEVICE_SETTINGS_FLASH_OFFSET,
                            (uint8_t *)&_self.device_settings,
                            FLASH_MEM_PAGE_SIZE) != FLASH_OPERATION_SUCCESS)
        {
            printf("[DEVICE_SETTINGS] ERRO: falhou ao salvar configuração inicial\n");
            return RETURN_STATUS_NOT_PERMITTED;
        }

        printf("[DEVICE_SETTINGS] Configuração padrão salva com sucesso!\n");
    }
    else
    {
        printf("[DEVICE_SETTINGS] Carregando da flash...\n");

        if (FLASH_load_data(DEVICE_SETTINGS_FLASH_OFFSET,
                            (uint8_t *)&_self.device_settings,
                            FLASH_MEM_PAGE_SIZE) != FLASH_OPERATION_SUCCESS)
        {
            printf("[DEVICE_SETTINGS] ERRO: falhou ao carregar configuração\n");
            return RETURN_STATUS_NOT_PERMITTED;
        }

        printf("[DEVICE_SETTINGS] Configuração carregada com sucesso!\n");
    }

    _self.is_init = true;
    return RETURN_STATUS_OK;
}


// ======================
// GET CONFIG
// ======================
RETURN_STATUS_t DEVICE_SETTINGS_Get_Config(DEVICE_SETTINGS_t *out)
{
    if (!_self.is_init)
    {
        return RETURN_STATUS_NOT_INITIALIZED;
    }

    if (out == NULL)
    {
        return RETURN_STATUS_INVALID_PARAM;
    }

    if (xSemaphoreTake(_self.semaphore, pdMS_TO_TICKS(TIMEOUT_MS)) == pdFALSE)
    {
        return RETURN_STATUS_TIMEOUT;
    }

    *out = _self.device_settings;

    xSemaphoreGive(_self.semaphore);

    return RETURN_STATUS_OK;
}


// ======================
// SET CONFIG (manual)
// ======================
RETURN_STATUS_t DEVICE_SETTINGS_Set_Config(const DEVICE_SETTINGS_t *in)
{
    if (!_self.is_init)
    {
        return RETURN_STATUS_NOT_INITIALIZED;
    }

    if (in == NULL)
    {
        return RETURN_STATUS_INVALID_PARAM;
    }

    if (xSemaphoreTake(_self.semaphore, pdMS_TO_TICKS(TIMEOUT_MS)) == pdFALSE)
    {
        return RETURN_STATUS_TIMEOUT;
    }

    _self.device_settings = *in;

    // Erase
    if (FLASH_erase_data(DEVICE_SETTINGS_FLASH_OFFSET, FLASH_MEM_SECTOR_SIZE) != FLASH_OPERATION_SUCCESS)
    {
        xSemaphoreGive(_self.semaphore);
        return RETURN_STATUS_NOT_PERMITTED;
    }

    // Save
    if (FLASH_save_data(DEVICE_SETTINGS_FLASH_OFFSET,
                        (uint8_t *)&_self.device_settings,
                        FLASH_MEM_PAGE_SIZE) != FLASH_OPERATION_SUCCESS)
    {
        xSemaphoreGive(_self.semaphore);
        return RETURN_STATUS_NOT_PERMITTED;
    }

    xSemaphoreGive(_self.semaphore);

    return RETURN_STATUS_OK;
}


// ======================
// SET PRESET (salvando na Flash)
// ======================
RETURN_STATUS_t DEVICE_SETTINGS_Set_Preset(DEVICE_SETTINGS_Mode_Config_t config)
{
    if (!_self.is_init)
    {
        return RETURN_STATUS_NOT_INITIALIZED;
    }

    if (config < DEVICE_SETTINGS_MODE_CONFIG_1 ||
        config > DEVICE_SETTINGS_MODE_CONFIG_4)
    {
        printf("[DEVICE_SETTINGS] ERRO: preset inválido (%d)\n", config);
        return RETURN_STATUS_INVALID_PARAM;
    }

    if (xSemaphoreTake(_self.semaphore, pdMS_TO_TICKS(TIMEOUT_MS)) == pdFALSE)
    {
        return RETURN_STATUS_TIMEOUT;
    }

    // aplica preset
    _self.device_settings.mode_operation  = presets[config].mode_operation;
    _self.device_settings.configured      = presets[config].configured;
    _self.device_settings.duration_open   = presets[config].duration_open;
    _self.device_settings.duration_closed = presets[config].duration_closed;

    printf("[DEVICE_SETTINGS] Aplicando preset %d\n", config);

    // ---- Salva na flash ----
    if (FLASH_erase_data(DEVICE_SETTINGS_FLASH_OFFSET, FLASH_MEM_SECTOR_SIZE) != FLASH_OPERATION_SUCCESS)
    {
        printf("[DEVICE_SETTINGS] ERRO: Falha ao apagar flash\n");
        xSemaphoreGive(_self.semaphore);
        return RETURN_STATUS_NOT_PERMITTED;
    }

    if (FLASH_save_data(DEVICE_SETTINGS_FLASH_OFFSET,
                        (uint8_t *)&_self.device_settings,
                        FLASH_MEM_PAGE_SIZE) != FLASH_OPERATION_SUCCESS)
    {
        printf("[DEVICE_SETTINGS] ERRO: Falha ao salvar preset\n");
        xSemaphoreGive(_self.semaphore);
        return RETURN_STATUS_NOT_PERMITTED;
    }

    printf("[DEVICE_SETTINGS] Preset %d salvo na flash com sucesso!\n", config);

    xSemaphoreGive(_self.semaphore);

    return RETURN_STATUS_OK;
}
