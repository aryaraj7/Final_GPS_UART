#include "hardreset.h"

// ================== Reset Timings ==================
static unsigned long stateTimer = 0;
const unsigned long powerOnHold = 1000;
const unsigned long resetHold = 50;
const unsigned long waitAfterReset = 4000; // wait 4 sec before AT commands

RTC_DATA_ATTR bool hasReset = false;

void handleHardReset() {
    switch (currentState) {
        case POWER_ON:
            digitalWrite(neoway.powerPin, HIGH);
            stateTimer = millis();
            Serial.println("Power ON sequence started");
            currentState = POWER_ON_WAIT;
            break;

        case POWER_ON_WAIT:
            if (millis() - stateTimer >= powerOnHold) {
                digitalWrite(neoway.powerPin, LOW);
                Serial.println("Power ON sequence complete");
                if (!hasReset) {
                    currentState = RESETTING;
                } else {
                    currentState = READY;
                }
            }
            break;

        case RESETTING:
            digitalWrite(neoway.resetPin, LOW);
            stateTimer = millis();
            currentState = RESET_HOLD_WAIT;
            break;

        case RESET_HOLD_WAIT:
            if (millis() - stateTimer >= resetHold) {
                digitalWrite(neoway.resetPin, HIGH);
                Serial.println("Reset sequence executing");
                stateTimer = millis();
                currentState = WAIT_AFTER_RESET;
            }
            break;

        case WAIT_AFTER_RESET:
            if (millis() - stateTimer >= waitAfterReset) {
                Serial.println("Neoway ON");
                hasReset = true;
                currentState = SEND_AT_SEQUENCE;
            }
            break;

        default:
            break;
    }
}
