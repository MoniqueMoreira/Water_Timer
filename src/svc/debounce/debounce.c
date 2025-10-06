#include "debounce.h"

// Estrutura interna do módulo
typedef struct {
    DebounceSample_t debounce_sample[MAX_DEBOUNCE_CHANNELS];
} Self;

static Self self;

void DEBOUNCE_Init(void) {
    for (uint8_t i = 0; i < MAX_DEBOUNCE_CHANNELS; i++) {
        self.debounce_sample[i].state = 0;
        self.debounce_sample[i].counter = 0;
        self.debounce_sample[i].fsm_state = DEB_LO;
    }
}

uint8_t DEBOUNCE_Update(uint8_t channel, uint8_t raw) {
    if (channel >= MAX_DEBOUNCE_CHANNELS) return 0;

    DebounceSample_t *d = &self.debounce_sample[channel];

    switch (d->fsm_state) {
        case DEB_LO:
            if (raw) { // detectou subida
                d->counter = 1;
                d->fsm_state = DEB_HI_EVAL;
            }
            break;

        case DEB_HI:
            if (!raw) { // detectou descida
                d->counter = 1;
                d->fsm_state = DEB_LO_EVAL;
            }
            break;

        case DEB_HI_EVAL:
            if (!raw) {
                d->fsm_state = DEB_LO; // voltou ao zero → ruído
                d->counter = 0;
            } else if (d->counter >= DEBOUNCE_THRESHOLD) {
                d->state = 1;
                d->fsm_state = DEB_HI; // confirmado
            }
            else {
                d->counter++;
            }
            break;

        case DEB_LO_EVAL:
            if (raw) {
                d->fsm_state = DEB_HI; // voltou ao 1 → ruído
                d->counter = 0;
            } else if (d->counter >= DEBOUNCE_THRESHOLD) {
                d->state = 0;
                d->fsm_state = DEB_LO; // confirmado
            }
            else {
                d->counter++;
            }
            break;
    }

    return d->state; // sempre retorna estado estável
}
