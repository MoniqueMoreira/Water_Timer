//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  Automatic generated C-code by the application:
//     HFSM Editor v0.5r10
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Methodology: Hierarchical Switch-case state. [v1.2r3]
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Exported function: void setup_run(void);
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Initialization proposed by the user (Start).
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  #include <string.h>
  
  #include <itf/logger/logger.h>
  #include <svc/timer/timer.h>
  #include <svc/device_settings/device_settings.h>
  #include <svc/io_manager/io_manager.h>
  #include <svc/watchdog/watchdog.h>
  #include <svc/watchdog/watchdog_cfg.h>
  #include <test/benchmark.h>
  #include <test/benchmark_cfg.h>
  
  #include "setup.h"
  
  
  #define INCREMENT_CIRCULAR(var) ((var < 3) ? (var + 1) : 0)
  #define DECREMENT_CIRCULAR(var) ((var > 0) ? (var - 1) : 3)
  
  
  static struct {
      DEVICE_SETTINGS_t settings;
      SETUP_Button_t commad;
      uint8_t config; 
      IO_OutputState_t outputs;
      float humidity;
  } self = {0};
  
  
  static void __SETUP_Task(void * param);
  static SETUP_Button_t __SETUP_Check_Inputs__(void);
  static void __SETUP_Update_LEDs(uint8_t mode);
  static void __SETUP_Set_Outputs(unsigned char ind_setup_task);


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Initialization proposed by the user (End).
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Entry flags (Start).
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Entry flags (End).
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Automatic initialization (Start).
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 // States index.
  #define  i_IDLE           0
  #define  i_MODE_TIMER     1
  #define  i_PUMP_OPEN      2
  #define  i_MODE_MOISTURE  3
  #define  i_CONFIG         4

 // Index variable.
static unsigned char ind_setup_run = i_IDLE;

// Macros & functions.
// Force the FSM to the initial state.
#define setup_run_Init() { ind_setup_run = i_IDLE; }

// This is not a history FSM.
#define _setup_run_Init()  {setup_run_Init(); /* Children: */ }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Automatic initialization (End).
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Local child functions  (Start).
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Local child functions  (End).
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// FSM dispatcher function to be called by the O.S.
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void setup_run(void)
{
    // User actions to be executed before analyse the state.
    self.commad = __SETUP_Check_Inputs__();

    // Next state analyser.
    switch(ind_setup_run){
        case i_IDLE:
            // State actions (Moore).
            DEVICE_SETTINGS_Get_Config(&self.settings);
            // Next state selection.
            if(self.settings.mode_operation == MODE_TIMER){	// [1].
                // Transition actions (Meally).
                LOGGER_Info("SETUP_TASK", "MODE_TIMER");
                TIMER_Start("PUMP", self.settings.duration_closed);
                // Pointing to the next state.
                ind_setup_run = i_MODE_TIMER;
            }else if(self.settings.mode_operation == MODE_MOISTURE){	// [2].
                // Transition actions (Meally).
                LOGGER_Info("SETUP_TASK", "MODO_MOISTURE");
                // Pointing to the next state.
                ind_setup_run = i_MODE_MOISTURE;
            }
        break;
        case i_MODE_TIMER:
            // Next state selection.
            if(self.commad == SETUP_BUTTON2_LONG){	// [1].
                // Transition actions (Meally).
                LOGGER_Info("SETUP_TASK", "MODO_MOISTURE");
                self.settings.mode_operation = MODE_MOISTURE;
                DEVICE_SETTINGS_Set_Config(&self.settings);
                // Pointing to the next state.
                ind_setup_run = i_MODE_MOISTURE;
            }else if(self.commad == SETUP_BUTTON1_LONG){	// [2].
                // Transition actions (Meally).
                LOGGER_Info("SETUP_TASK", "MODO CONFIG");
                self.config = 0;
                // Pointing to the next state.
                ind_setup_run = i_CONFIG;
            }else if(TIMER_Check("PUMP") == TIMER_EXPIRED){	// [3].
                // Transition actions (Meally).
                LOGGER_Info("SETUP_TASK", "OPEN PUMP");
                TIMER_Stop("PUMP");
                TIMER_Start("PUMP", self.settings.duration_open);
                // Pointing to the next state.
                ind_setup_run = i_PUMP_OPEN;
            }
        break;
        case i_PUMP_OPEN:
            // Next state selection.
            if((TIMER_Check("PUMP") == TIMER_EXPIRED &&
               self.settings.mode_operation == MODE_TIMER)){	// [1].
                // Transition actions (Meally).
                LOGGER_Info("SETUP_TASK", "CLOSET PUMP");
                TIMER_Stop("PUMP");
                TIMER_Start("PUMP", self.settings.duration_closed);
                // Pointing to the next state.
                ind_setup_run = i_MODE_TIMER;
            }else if(((self.humidity >= 80.0 || 
                     TIMER_Check("PUMP") == TIMER_EXPIRED) && 
                     self.settings.mode_operation == MODE_MOISTURE)){	// [2].
                // Transition actions (Meally).
                LOGGER_Info("SETUP_TASK", "CLOSET PUMP");
                LOGGER_Info("SETUP_TASK", "HUMIDITY: %f", self.humidity);
                TIMER_Stop("PUMP");
                // Pointing to the next state.
                ind_setup_run = i_MODE_MOISTURE;
            }
        break;
        case i_MODE_MOISTURE:
            // Next state selection.
            if(self.commad == SETUP_BUTTON2_LONG){	// [1].
                // Transition actions (Meally).
                LOGGER_Info("SETUP_TASK", "MODE_TIMER");
                self.settings.mode_operation = MODE_TIMER;
                DEVICE_SETTINGS_Set_Config(&self.settings);
                TIMER_Start("PUMP", self.settings.duration_closed);
                // Pointing to the next state.
                ind_setup_run = i_MODE_TIMER;
            }else if(self.humidity <= 40.0){	// [2].
                // Transition actions (Meally).
                LOGGER_Info("SETUP_TASK", "OPEN PUMP");
                LOGGER_Info("SETUP_TASK", "HUMIDITY: %f", self.humidity);
                TIMER_Stop("PUMP");
                TIMER_Start("PUMP", 300000);
                // Pointing to the next state.
                ind_setup_run = i_PUMP_OPEN;
            }
        break;
        case i_CONFIG:
            // Next state selection.
            if(self.commad == SETUP_BUTTON1_SHORT){	// [1].
                // Transition actions (Meally).
                self.config = INCREMENT_CIRCULAR(self.config);
            }else if(self.commad == SETUP_BUTTON2_SHORT){	// [2].
                // Transition actions (Meally).
                self.config = DECREMENT_CIRCULAR(self.config);
            }else if(self.commad == SETUP_BUTTON2_LONG){	// [3].
                // Transition actions (Meally).
                self.config = 0;
                // Pointing to the next state.
                ind_setup_run = i_MODE_TIMER;
            }else if(self.commad == SETUP_BUTTON1_LONG){	// [4].
                // Transition actions (Meally).
                DEVICE_SETTINGS_Set_Preset(self.config);
                // Pointing to the next state.
                ind_setup_run = i_IDLE;
            }
        break;
    }

    // User actions to be executed after analyse the state.
    __SETUP_Set_Outputs(ind_setup_run);
    IO_MANAGER_Set_Outputs(&self.outputs);
    WATCHDOG_Notify(WDOG_TASK_SETUP);
}


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Ending code proposed by the user (Start).
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

RETURN_STATUS_t SETUP_Init(void) 
{
    
#ifdef SETUP_BENCHMARK_ENABLED
    BENCHMARK_Reset();
    for(int i=0; i<BENCHMARK_MAX_SAMPLES; i++)
    {
        BENCHMARK_Start();
        setup_run();
        BENCHMARK_Stop();
    }
    BENCHMARK_Report("SETUP_TASK");
#else
    if (xTaskCreate(__SETUP_Task, "Setup", 512, NULL, 2, NULL) != pdPASS) 
    {
        return RETURN_STATUS_ERROR;
    }
#endif //SETUP_BENCHMARK_ENABLED
    return RETURN_STATUS_OK;
}

static void __SETUP_Task(void * param)
{
    (void) param;
    TickType_t xLastWakeTime = xTaskGetTickCount();
    while (1)
    {
        setup_run();
    }
    vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(50));
}

static void __SETUP_Set_Outputs(unsigned char ind_setup_task){
    switch(ind_setup_task){
        case i_IDLE:
            self.outputs.relay[0] = false;
            memset(self.outputs.led, 0, sizeof(self.outputs.led));
        break;
        case i_MODE_TIMER:
            self.outputs.relay[0] = false;
            self.outputs.led[0] = false;
            __SETUP_Update_LEDs(self.settings.configured);
        break;
        case i_PUMP_OPEN:
            self.outputs.relay[0] = true;
            self.outputs.led[0] = true;
        break;
        case i_MODE_MOISTURE:
            self.outputs.relay[0] = false;
            memset(self.outputs.led, 0, sizeof(self.outputs.led));
        break;
        case i_CONFIG:
            self.outputs.relay[0] = false;
            self.outputs.led[0] = false;
            __SETUP_Update_LEDs(self.config);
        break;
        default:
            self.outputs.relay[0] = false;
            memset(self.outputs.led, 0, sizeof(self.outputs.led));
        break;
    }
}

static SETUP_Button_t __SETUP_Check_Inputs__(void)
{
    static bool prev_btn1 = false;
    static bool prev_btn2 = false;

    SETUP_Button_t cmd = SETUP_BUTTON_NOT_PRESSED;
    IO_InputState_t inputs;

    IO_MANAGER_Get_Inputs(&inputs);

    bool btn1 = inputs.button[0];
    bool btn2 = inputs.button[1];
    self.humidity = inputs.adc[0];
  
    if (btn1 != prev_btn1)
    {
        if (btn1 == SETUP_BUTTON_PRESSED)
        {
            TIMER_Start("BTN1", 5000);
        }
        else
        {
            if (TIMER_Check("BTN1") == TIMER_ACTIVE)
            {
                cmd = SETUP_BUTTON1_SHORT;
            }
            TIMER_Stop("BTN1");
        }

        prev_btn1 = btn1;
    }
    else
    {
        if ((btn1 == SETUP_BUTTON_PRESSED) &&
            (TIMER_Check("BTN1") == TIMER_EXPIRED))
        {
            cmd = SETUP_BUTTON1_LONG;
            TIMER_Stop("BTN1");
        }
    }

    if (btn2 != prev_btn2)
    {
        if (btn2 == SETUP_BUTTON_PRESSED)
        {
            TIMER_Start("BTN2", 5000);
        }
        else
        {
            if (TIMER_Check("BTN2") == TIMER_ACTIVE)
            {
                cmd = SETUP_BUTTON2_SHORT;
            }
            TIMER_Stop("BTN2");
        }

        prev_btn2 = btn2;
    }
    else
    {
        if ((btn2 == SETUP_BUTTON_PRESSED) &&
            (TIMER_Check("BTN2") == TIMER_EXPIRED))
        {
            cmd = SETUP_BUTTON2_LONG;
            TIMER_Stop("BTN2");
        }
    }

    return cmd;
}

static void __SETUP_Update_LEDs(uint8_t mode)
{
    self.outputs.led[1] = false;
    self.outputs.led[2] = false;

    switch (mode)
    {
        case DEVICE_SETTINGS_MODE_CONFIG_1:
            self.outputs.led[1] = false;
            self.outputs.led[2] = false;
            break;

        case DEVICE_SETTINGS_MODE_CONFIG_2:
            self.outputs.led[1] = true;
            break;

        case DEVICE_SETTINGS_MODE_CONFIG_3:
            self.outputs.led[2] = true;
            break;

        case DEVICE_SETTINGS_MODE_CONFIG_4:
            self.outputs.led[1] = true;
            self.outputs.led[2] = true;
            break;
    }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Ending code proposed by the user (End).
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  End of the automatic generated C-code.
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

