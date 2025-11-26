#ifndef SETUP_H
#define SETUP_H

#include "return_status.h"

typedef enum{
    SETUP_BUTTON_NOT_PRESSED = 0,
    SETUP_BUTTON_PRESSED = 1,
    SETUP_BUTTON1_SHORT,
    SETUP_BUTTON1_LONG,
    SETUP_BUTTON2_SHORT,
    SETUP_BUTTON2_LONG,
} SETUP_Button_t;

RETURN_STATUS_t SETUP_Init(void);

#endif // SETUP_H