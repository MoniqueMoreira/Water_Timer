#ifndef DEVICE_SETTINGS_H
#define DEVICE_SETTINGS_H

#include <stdint.h>
#include <stdbool.h>
#include "return_status.h"

typedef enum
{
    MODE_TIMER = 0,
    MODE_MOISTURE,

} MODE_OPERATION_t;

typedef enum{
    DEVICE_SETTINGS_MODE_CONFIG_1 = 0,
    DEVICE_SETTINGS_MODE_CONFIG_2,
    DEVICE_SETTINGS_MODE_CONFIG_3,
    DEVICE_SETTINGS_MODE_CONFIG_4,    
}DEVICE_SETTINGS_Mode_Config_t;

typedef struct __attribute__((packed))
{
    MODE_OPERATION_t mode_operation;   // Timer ou Moisture
    DEVICE_SETTINGS_Mode_Config_t  configured;               // Flag de configuração válida
    uint32_t duration_open;            // ms válvula aberta
    uint32_t duration_closed;          // ms válvula fechada

} DEVICE_SETTINGS_t;

RETURN_STATUS_t DEVICE_SETTINGS_Init(void);
RETURN_STATUS_t DEVICE_SETTINGS_Get_Config(DEVICE_SETTINGS_t *out);
RETURN_STATUS_t DEVICE_SETTINGS_Set_Config(const DEVICE_SETTINGS_t *in);
RETURN_STATUS_t DEVICE_SETTINGS_Set_Preset(DEVICE_SETTINGS_Mode_Config_t config);

#endif //DEVICE_SETTIGNS_H