#include "gps.h"

// ================== AT Command List ==================
const char* atCommands[] = {
    "AT",
    "AT$MYGPSPWR=1",
    "AT+CSQ",
    "AT$MYGPSPOS=0,0",
};
const int totalCommands = sizeof(atCommands) / sizeof(atCommands[0]);
int currentCommandIndex = 0;

// ================== Flags & Timers ==================
static bool commandInProgress = false;
static unsigned long commandStartTime = 0;
static const unsigned long interCommandDelay = 4000; // 4 sec wait
static bool waitingForGap = false;

// ================== Serial Helpers ==================
String readResponse() {
    String response;
    while (Serial2.available()) {
        response += (char)Serial2.read();
    }
    return response;
}

void sendATCommand(const String &command) {
    while (Serial2.available()) Serial2.read(); // clear buffer
    Serial.print("\n>> ");
    Serial.println(command);
    Serial2.println(command);

    commandInProgress = true;
    commandStartTime = millis();
}

bool checkATResponse() {
    String response = readResponse();
    if (response.length()) {
        Serial.println("<< " + response);
        return response.indexOf("OK") != -1;
    }
    return false;
}

// ================== Command Processor ==================
void processCommandState() {
    // All commands done
    if (currentCommandIndex >= totalCommands) {
        Serial.println("AT sequence complete");
        currentState = READY;
        return;
    }

    // Wait gap between commands
    if (waitingForGap) {
        if (millis() - commandStartTime >= interCommandDelay) {
            waitingForGap = false;
        } else {
            return;
        }
    }

    // If no command in progress → send one
    if (!commandInProgress) {
        sendATCommand(atCommands[currentCommandIndex]);
        return;
    }

    // If command is in progress → check response
    bool gotOK = checkATResponse();

    if (gotOK) {
        Serial.printf("Command %s OK\n", atCommands[currentCommandIndex]);
        commandInProgress = false;
        currentCommandIndex++;
        waitingForGap = true;
        commandStartTime = millis();
    } else if (millis() - commandStartTime >= neoway.responseTimeout) {
        Serial.printf("Command %s TIMEOUT/FAILED\n", atCommands[currentCommandIndex]);
        commandInProgress = false;
        currentCommandIndex++;
        waitingForGap = true;
        commandStartTime = millis();
    }
}

// ================== GPS Handlers ==================
void handleSendATSequence() {
    processCommandState();
}

void handleReadyState() {
    if (serialAvailable) {
        String command = Serial.readStringUntil('\n');
        command.trim();
        sendATCommand(command);
    }

    if (serial2Available) {
        String response = readResponse();
        if (response.length()) Serial.println(response);
    }
}
