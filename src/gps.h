#pragma once
#include "config.h"

bool sendATCommand(const String &command);
bool sendATCommandWithRetry(const String &command, int maxRetries);
String readResponse();

void handleSendATSequence();
void handleReadyState();
