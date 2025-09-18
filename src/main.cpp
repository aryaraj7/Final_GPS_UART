#include <Arduino.h>
#include "esp_system.h"
#include "hardreset.h"
#include "gps.h"

// ================== Module Configuration ==================
ModuleConfig neoway = {
    .txPin = 17,
    .rxPin = 16,
    .resetPin = 32,
    .powerPin = 33,
    .baudRate = 115200,
    .responseTimeout = 2000, // response timeout
    .commandGap = 3000       // not used (we use 4s gap in gps.cpp)
};

SystemState currentState = INIT;

bool serialAvailable = false;
bool serial2Available = false;

void setup() {
    Serial.begin(115200);
    Serial2.begin(neoway.baudRate, SERIAL_8N1, neoway.rxPin, neoway.txPin);

    pinMode(neoway.resetPin, OUTPUT);
    pinMode(neoway.powerPin, OUTPUT);
    digitalWrite(neoway.resetPin, HIGH);
    digitalWrite(neoway.powerPin, LOW);

    Serial.println("\nNeoway N58 Interface (State Machine)");
    Serial.println("--------------------------------------");

    currentState = POWER_ON;
}

void loop() {
    serialAvailable = Serial.available() > 0;
    serial2Available = Serial2.available() > 0;

    switch (currentState) {
        case POWER_ON:
        case POWER_ON_WAIT:
        case RESETTING:
        case RESET_HOLD_WAIT:
        case WAIT_AFTER_RESET:
            handleHardReset();   // hardreset.cpp handles all reset workflow
            break;

        case SEND_AT_SEQUENCE:
            handleSendATSequence(); // gps.cpp handles AT commands
            break;

        case READY:
            handleReadyState();  // gps.cpp handles interactive mode
            break;

        default:
            break;
    }
}
