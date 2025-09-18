#pragma once
#include <Arduino.h>
#include "esp_system.h"

// ================== Config Struct ==================
struct ModuleConfig {
    int txPin;
    int rxPin;
    int resetPin;
    int powerPin;
    unsigned long baudRate;
    unsigned long responseTimeout;
    unsigned long commandGap;
};

// ================== State Machine ==================
enum SystemState {
    INIT,
    POWER_ON,
    POWER_ON_WAIT,
    RESETTING,
    RESET_HOLD_WAIT,
    WAIT_AFTER_RESET,
    SEND_AT_SEQUENCE,
    READY
};

// Externs
extern ModuleConfig neoway;
extern SystemState currentState;

// Function
void handleHardReset();
