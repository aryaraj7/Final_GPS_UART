#include <Arduino.h>
#include "config.h"
#include "hardreset.h"
#include "gps.h"

// Global config
ModuleConfig neoway = {
    .txPin = 17,
    .rxPin = 16,
    .resetPin = 32,
    .powerPin = 33,
    .baudRate = 115200,
    .responseTimeout = 500,
    .commandGap = 3000
};

SystemState currentState = INIT;

void setup() {
    Serial.begin(115200);
    initModule();
    currentState = POWER_ON;
}

void loop() {
    serialAvailable = Serial.available() > 0;
    serial2Available = Serial2.available() > 0;

    switch (currentState) {
        case POWER_ON:          handlePowerOn(); break;
        case POWER_ON_WAIT:     handlePowerOnWait(); break;
        case RESETTING:         handleResetting(); break;
        case RESET_HOLD_WAIT:   handleResetHoldWait(); break;
        case WAIT_AFTER_RESET:  handleWaitAfterReset(); break;
        case SEND_AT_SEQUENCE:  handleSendATSequence(); break;
        case READY:             handleReadyState(); break;
        default: break;
    }
}
