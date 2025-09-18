#pragma once
#include <Arduino.h>

// ================== Module Configuration ==================
struct ModuleConfig {
    int txPin;
    int rxPin;
    int resetPin;
    int powerPin;
    unsigned long baudRate;
    unsigned long responseTimeout;
    unsigned long commandGap;
};

extern ModuleConfig neoway;

// ================== Global Flags ==================
extern bool hasReset;
extern bool serialAvailable;
extern bool serial2Available;

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

extern SystemState currentState;

// ================== Timing Helpers ==================
extern unsigned long stateTimer;
const unsigned long powerOnHold = 1000;
const unsigned long resetHold = 50;
const unsigned long waitAfterReset = 3000;

