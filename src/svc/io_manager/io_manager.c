#include "io_manager.h"
#include "io_benchmark.h"
#include "svc/debounce/debounce.h"
#include "svc/watchdog/watchdog.h"
#include "svc/watchdog/watchdog_cfg.h"
#include "itf/logger/logger.h"
#include "test/benchmark.h"
#include "test/benchmark_cfg.h"

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

const uint8_t button_pins[] = { BTN1_PIN, BTN2_PIN };
const uint8_t led_pins[]    = { LED1_PIN, LED2_PIN, LED3_PIN, LED_RELAY_PIN };
const uint8_t relay_pins[]  = { WATER_PUMP };
const uint8_t adc_pins[]    = { HUMIDITY_PIN };

// ==== Prototypes ====
static void __IO_MANAGER_Input_Task__(void *pvParameters);
static void __IO_MANAGER_Output_Task__(void *pvParameters);
RETURN_STATUS_t IO_MANAGER_Input_Run(void);
RETURN_STATUS_t IO_MANAGER_Output_Run(void);

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
    for (int i=0; i<RELAY_COUNT; i++){
        gpio_init(relay_pins[i]);
        gpio_set_dir(relay_pins[i], true);
        gpio_put(relay_pins[i], 0);
    }
    
    // ADCs
    adc_init();
    for (int i = 0; i < ADC_COUNT; i++) {
        adc_gpio_init(adc_pins[i]);
    }
    
    // Debounce
    DEBOUNCE_Init();

    // Mutex
    self.io_mutex = xSemaphoreCreateMutex();

#ifdef IO_BENCHMARK_ENABLED
    IO_BENCHMARK_Init();
    BENCHMARK_Reset();

    // Executa benchmark de Input
    for(int i=0; i<BENCHMARK_MAX_SAMPLES; i++){
        BENCHMARK_Start();
        IO_MANAGER_Input_Run();
        BENCHMARK_Stop();
    }
    BENCHMARK_Report("IO_INPUT");

    // Executa benchmark de Output
    BENCHMARK_Reset();
    for(int i=0; i<BENCHMARK_MAX_SAMPLES; i++){
        BENCHMARK_Start();
        IO_MANAGER_Output_Run();
        BENCHMARK_Stop();
    }
    BENCHMARK_Report("IO_INPUT");

#else
    // Tasks FreeRTOS
    xTaskCreate(__IO_MANAGER_Input_Task__, "Input", 256, NULL, 2, &self.xTask_Input);
    xTaskCreate(__IO_MANAGER_Output_Task__, "Output", 256, NULL, 2, &self.xTask_Output);
#endif

    return RETURN_STATUS_OK;
}

// ==== Funções Run com retorno de status ====
RETURN_STATUS_t IO_MANAGER_Input_Run(void) {
    RETURN_STATUS_t err = RETURN_STATUS_TIMEOUT;
    if(xSemaphoreTake(self.io_mutex, pdMS_TO_TICKS(100)) == pdTRUE) 
    {
        for(uint8_t i=0; i<BTN_COUNT; i++){
            uint8_t raw = gpio_get(button_pins[i]);
            self.inputs.button[i] = DEBOUNCE_Update(i, raw);
        }

        for (uint8_t i = 0; i < ADC_COUNT; i++) { 
            adc_select_input(i); 
            uint16_t value = adc_read();
            float adc_percentage = (value / 4095.0f) * 100.0f;
            self.inputs.adc[i] = adc_percentage;
        }

        xSemaphoreGive(self.io_mutex);
        err = RETURN_STATUS_OK;
    }
    WATCHDOG_Notify(WDOG_TASK_IO_INPUT);
    return err;
}

RETURN_STATUS_t IO_MANAGER_Output_Run(void) {
    RETURN_STATUS_t err = RETURN_STATUS_TIMEOUT;
    if(xSemaphoreTake(self.io_mutex, pdMS_TO_TICKS(100)) == pdTRUE) 
    {
        for(int i=0; i<LED_COUNT; i++){
            gpio_put(led_pins[i], self.outputs.led[i]);
        }

        for(int i=0; i<RELAY_COUNT; i++){
            gpio_put(relay_pins[i], self.outputs.relay[i]);
        }

        xSemaphoreGive(self.io_mutex);

        err = RETURN_STATUS_OK;
    }
    WATCHDOG_Notify(WDOG_TASK_IO_OUTPUT);
    return err;
}

// ==== Input Task ====
static void __IO_MANAGER_Input_Task__(void *pvParameters) {
    (void) pvParameters;
    TickType_t xLastWakeTime = xTaskGetTickCount();

    while(1){
        IO_MANAGER_Input_Run();
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(50));
    }
}

// ==== Output Task ====
static void __IO_MANAGER_Output_Task__(void *pvParameters) {
    (void) pvParameters;
    TickType_t xLastWakeTime = xTaskGetTickCount();
    while(1){
        IO_MANAGER_Output_Run();
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(50));
    }
}

// ==== APIs com ponteiro para dados e retorno de status ====
RETURN_STATUS_t IO_MANAGER_Get_Inputs(IO_InputState_t *pCopy){
    if(pCopy == NULL) 
    {
        return RETURN_STATUS_INVALID_PARAM;  // opcional: verifica ponteiro válido
    }

    if(xSemaphoreTake(self.io_mutex, pdMS_TO_TICKS(100)) == pdTRUE) 
    {
        *pCopy = self.inputs;
        xSemaphoreGive(self.io_mutex);
        return RETURN_STATUS_OK;
    }
    return RETURN_STATUS_TIMEOUT;
}

RETURN_STATUS_t IO_MANAGER_Set_Outputs(const IO_OutputState_t *pNewState){
    if(pNewState == NULL) 
    {
        return RETURN_STATUS_INVALID_PARAM;  // opcional: verifica ponteiro válido
    }

    if(xSemaphoreTake(self.io_mutex, pdMS_TO_TICKS(100)) == pdTRUE) 
    {
        self.outputs = *pNewState;
        xSemaphoreGive(self.io_mutex);
        return RETURN_STATUS_OK;
    }
    return RETURN_STATUS_TIMEOUT;
}

RETURN_STATUS_t IO_MANAGER_Get_Outputs(IO_OutputState_t *pCopy){
    if(pCopy == NULL) return RETURN_STATUS_INVALID_PARAM;

    if(xSemaphoreTake(self.io_mutex, pdMS_TO_TICKS(100)) == pdTRUE) 
    {
        *pCopy = self.outputs;
        xSemaphoreGive(self.io_mutex);
        return RETURN_STATUS_OK;
    }
    return RETURN_STATUS_TIMEOUT;
}
