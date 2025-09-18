#pragma once
#include <Arduino.h>
#include "hardreset.h"

// Externs
extern bool serialAvailable;
extern bool serial2Available;

// Functions
void handleSendATSequence();
void handleReadyState();
