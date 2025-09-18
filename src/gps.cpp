#include "gps.h"

// ================== AT Commands ==================
const char* atCommands[] = {
    "AT",
    "ATI",
    "AT$MYGMR",
    "AT$MYGPSPWR=1",
    "AT+CSQ",
    "AT$MYGPSPOS=0,0",
    "AT$MYGPSPOS=0,1",
};
const int totalCommands = sizeof(atCommands) / sizeof(atCommands[0]);
int currentCommandIndex = 0;

// Flags & timers
static bool commandInProgress = false;
static unsigned long commandStartTime = 0;

bool serialAvailable = false;
bool serial2Available = false;

// ================== Serial Helpers ==================
String readResponse() {
    String response;
    unsigned long startTime = millis();

    while (millis() - startTime < neoway.responseTimeout) {
        if (Serial2.available()) {
            response += (char)Serial2.read();
            startTime = millis();
        }
        if (response.indexOf("\r\n") != -1) break;
    }
    return response;
}

bool sendATCommand(const String &command) {
    while (Serial2.available()) Serial2.read();

    Serial.print("\n>> ");
    Serial.println(command);
    Serial2.println(command);

    commandInProgress = true;
    commandStartTime = millis();
    return true;
}

bool processATResponse() {
    String response = readResponse();
    if (response.length()) {
        Serial.println("<< " + response);
        return response.indexOf("OK") != -1;
    }
    return false;
}

// ================== GPS Command Handler ==================
void handleSendATSequence() {
    // If no command is active → send next one
    if (!commandInProgress && currentCommandIndex < totalCommands) {
        sendATCommand(atCommands[currentCommandIndex]);
    }

    // If command is active → check for response or timeout
    if (commandInProgress) {
        bool gotResponse = processATResponse();

        if (gotResponse || (millis() - commandStartTime >= neoway.commandGap)) {
            Serial.printf("Command %s %s\n",
                          atCommands[currentCommandIndex],
                          gotResponse ? "OK" : "TIMEOUT/FAILED");

            commandInProgress = false;
            currentCommandIndex++;
        }
    }

    // When all commands done → move to READY
    if (currentCommandIndex >= totalCommands) {
        Serial.println("AT command sequence complete");
        currentState = READY;
    }
}

void handleReadyState() {
    if (serialAvailable) {
        String command = Serial.readStringUntil('\n');
        command.trim();
        sendATCommand(command);  // sends user command
    }

    if (serial2Available) {
        String response = readResponse();
        if (response.length()) Serial.println(response);
    }
}
