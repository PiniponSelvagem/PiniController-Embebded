#define PLOG_COLORED
#define PLOG_LEVEL PLOG_LEVEL_TRACE
#include <pinicore.hpp>
#include <Arduino.h>

#define TAG_MAIN    "main"

#define FIRMWARE_VERSION    1

void setup() {
    Serial.begin(115200);
    Serial.println();	// Just to start on a new clean line

    LOG_I(TAG_MAIN, "Setup started");
    LOG_I(TAG_MAIN, "Firmware: [%d] | Build: [%s, %s]", FIRMWARE_VERSION, __DATE__, __TIME__);
    
    LOG_I(TAG_MAIN, "Setup completed");
}

void loop() {
    delay(1000);
}
