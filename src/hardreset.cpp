#include "hardreset.h"
#include <esp32-hal-gpio.h>
#include <HardwareSerial.h>
#include "config.h"

RTC_DATA_ATTR bool hasReset = false;
unsigned long stateTimer = 0;

void initModule() {
    pinMode(neoway.resetPin, OUTPUT);
    pinMode(neoway.powerPin, OUTPUT);
    digitalWrite(neoway.resetPin, HIGH);
    digitalWrite(neoway.powerPin, LOW);
    Serial2.begin(neoway.baudRate, SERIAL_8N1, neoway.rxPin, neoway.txPin);
    Serial.println("Neoway N58 Initialized");
}

void handlePowerOn() {
    digitalWrite(neoway.powerPin, HIGH);
    stateTimer = millis();
    Serial.println("Power ON sequence started");
    currentState = POWER_ON_WAIT;
}

void handlePowerOnWait() {
    if (millis() - stateTimer >= powerOnHold) {
        digitalWrite(neoway.powerPin, LOW);
        Serial.println("Power ON complete");
        currentState = (hasReset == false) ? RESETTING : READY;
    }
}

void handleResetting() {
    digitalWrite(neoway.resetPin, LOW);
    stateTimer = millis();
    currentState = RESET_HOLD_WAIT;
}

void handleResetHoldWait() {
    if (millis() - stateTimer >= resetHold) {
        digitalWrite(neoway.resetPin, HIGH);
        Serial.println("Reset triggered");
        stateTimer = millis();
        currentState = WAIT_AFTER_RESET;
    }
}

void handleWaitAfterReset() {
    if (millis() - stateTimer >= waitAfterReset) {
        Serial.println("Neoway Ready");
        hasReset = true;
        currentState = SEND_AT_SEQUENCE;
    }
}
