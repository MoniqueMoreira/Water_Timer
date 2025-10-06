#include "io_manager.h"
#include "svc/debounce/debounce.h"
#include "svc/watchdog/watchdog.h"
#include "svc/watchdog/watchdog_cfg.h"
#include "itf/logger/logger.h"

#include "pico/stdlib.h"
#include "hardware/adc.h"

#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"

typedef struct {
    IO_InputState_t inputs;
    IO_OutputState_t outputs;
    SemaphoreHandle_t io_mutex;
    TaskHandle_t xTask_Input;
    TaskHandle_t xTask_Output;
} Self_t;

static Self_t self = {0};

// ==== Arrays de mapeamento de GPIO ====
static const uint8_t button_pins[] = {
    BTN1_PIN, 
    BTN2_PIN
};

static const uint8_t led_pins[] = {
    LED1_PIN,
    LED2_PIN,
    LED3_PIN,
    LED_RELAY_PIN
};

static uint8_t adc_pins[] = {
    HUMIDITY_PIN,
};

static uint8_t reley_pins[] = {
    WATER_PUMP,
};

// ==== Prototypes das tasks ====
static void __IO_MANAGER_Input_Task__(void *pvParameters);
static void __IO_MANAGER_Output_Task__(void *pvParameters);

// ==== Inicialização ====
RETURN_STATUS_t IO_MANAGER_Init(void) {

    // GPIOs de botões
    for(int i=0; i<BTN_COUNT; i++){
        gpio_init(button_pins[i]);
        gpio_set_dir(button_pins[i], GPIO_IN);
    }

    // GPIOs de LEDs
    for(int i=0; i<LED_COUNT; i++){
        gpio_init(led_pins[i]);
        gpio_set_dir(led_pins[i], true);
        gpio_put(led_pins[i], 0);
    }

    // GPIO do relé
    for (int i=0; i<RELEY_COUNT; i++){
        gpio_init(reley_pins[i]);
        gpio_set_dir(reley_pins[i], true);
        gpio_put(reley_pins[i], 0);
    }
    
    // ADCs
    adc_init();
    for (int i = 0; i < ADC_COUNT; i++) {
        adc_gpio_init(adc_pins[i]); // Inicializa GPIOs ADC sequenciais
    }
    
    // Debounce
    DEBOUNCE_Init();

    // Mutex
    self.io_mutex = xSemaphoreCreateMutex();

    // Tasks
    xTaskCreate(__IO_MANAGER_Input_Task__, "Input", 256, NULL, 2, &self.xTask_Input);
    xTaskCreate(__IO_MANAGER_Output_Task__, "Output", 256, NULL, 2, &self.xTask_Output);

    return RETURN_STATUS_OK;
}

// ==== Task de Input ====
static void __IO_MANAGER_Input_Task__(void *pvParameters) {
    (void) pvParameters;

     TickType_t xLastWakeTime = xTaskGetTickCount();

    while(1){
        xSemaphoreTake(self.io_mutex, portMAX_DELAY);

        // Lê botões com debounce
        for(int i=0; i<BTN_COUNT; i++){
            uint8_t raw = gpio_get(button_pins[i]);
            self.inputs.button[i] = DEBOUNCE_Update(i, raw);
            
        }

        // Lê ADC
        for (uint8_t i = 0; i < ADC_COUNT; i++) { 

            adc_select_input(i); 
            uint16_t value = adc_read();
            float adc_percentage = (value / 4095.0f) * 100.0f;
    
            self.inputs.adc[i] = adc_percentage;
        }

        //LOGGER_Info("IO_MANAGER", "Inputs: BTN1=%d, BTN2=%d, HUMIDITY_ADC=%d", self.inputs.button[0], self.inputs.button[1], self.inputs.adc[0]);

        xSemaphoreGive(self.io_mutex);

        WATCHDOG_Notify(WDOG_TASK_IO_INPUT);
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(25));
    }
}

// ==== Task de Output ====
static void __IO_MANAGER_Output_Task__(void *pvParameters) {
    (void) pvParameters;

    while(1){
        xSemaphoreTake(self.io_mutex, portMAX_DELAY);

        //LOGGER_Info("IO_MANAGER", "SetOutputs called: LED1=%d, LED2=%d, LED3=%d, RELAY_LED=%d, RELAY=%d",self.outputs.led[0], self.outputs.led[1], self.outputs.led[2], self.outputs.led[3], self.outputs.relay[0]);

        // LEDs
        for(int i=0; i<LED_COUNT; i++){
            gpio_put(led_pins[i], self.outputs.led[i]);
        }

        // Relé
        for (int i=0; i<RELEY_COUNT; i++){
            gpio_put(reley_pins[i], self.outputs.relay[i]);
        }

        xSemaphoreGive(self.io_mutex);

        WATCHDOG_Notify(WDOG_TASK_IO_OUTPUT);
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}

// ==== APIs ====
IO_InputState_t IO_MANAGER_Get_Inputs(void){
    IO_InputState_t copy;
    xSemaphoreTake(self.io_mutex, portMAX_DELAY);
    copy = self.inputs;
    xSemaphoreGive(self.io_mutex);
    return copy;
}

void IO_MANAGER_Set_Outputs(IO_OutputState_t newState){
    xSemaphoreTake(self.io_mutex, portMAX_DELAY);
    self.outputs = newState;
    xSemaphoreGive(self.io_mutex);
}

IO_OutputState_t IO__MANAGER_Get_Outputs(void){
    IO_OutputState_t copy;
    xSemaphoreTake(self.io_mutex, portMAX_DELAY);
    copy = self.outputs;
    xSemaphoreGive(self.io_mutex);
    return copy;
}