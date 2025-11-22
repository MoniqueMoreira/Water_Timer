#ifndef IO_MANAGER_CFG_H
#define IO_MANAGER_CFG_H

#define BTN_COUNT 2
#define LED_COUNT 4
#define ADC_COUNT 1
#define RELAY_COUNT 1  

//BOTÕES
#define BTN1_PIN       2
#define BTN2_PIN       3

// ADC
#define HUMIDITY_PIN   26   // ADC

// LEDs
#define LED1_PIN       11
#define LED2_PIN       12
#define LED3_PIN       13
#define LED_RELAY_PIN  25

//RELE
#define  WATER_PUMP      28

extern const uint8_t button_pins[];
extern const uint8_t led_pins[];
extern const uint8_t relay_pins[];
extern const uint8_t adc_pins[];

#endif //IO_MANAGER_CFG_H