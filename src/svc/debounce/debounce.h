#ifndef DEBOUNCE_H
#define DEBOUNCE_H

#include <stdint.h>
#include <stdbool.h>

#define MAX_DEBOUNCE_CHANNELS   4   // quantos botões no sistema
#define DEBOUNCE_THRESHOLD     4   // número de chamadas estáveis (ex.: 20ms)

// Estrutura de cada botão
typedef struct {
    uint8_t state;       // estado estável (0 solto, 1 pressionado)
    uint8_t counter;     // contador de estabilidade
    uint8_t fsm_state;   // estado da máquina
} DebounceSample_t;

// Estados da máquina
typedef enum {
    DEB_LO = 0,      // botão estável solto
    DEB_HI,          // botão estável pressionado
    DEB_HI_EVAL,     // avaliando subida
    DEB_LO_EVAL      // avaliando descida
} DebounceFSM_t;

// API
void DEBOUNCE_Init(void);
uint8_t DEBOUNCE_Update(uint8_t channel, uint8_t raw);

#endif
