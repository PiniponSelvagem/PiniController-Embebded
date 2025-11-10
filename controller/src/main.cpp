#define PLOG_COLORED
#define PLOG_LEVEL PLOG_LEVEL_TRACE
#include <pinicore.h>

#define TAG_MAIN        "main"

#define FIRMWARE_VERSION    1

void setup() {
    Serial.begin(115200);
    Serial.println();	// Just to start on a new clean line

    LOG_I(TAG_MAIN, "Setup started");
    LOG_I(TAG_MAIN, "Firmware: [%d] | Build: [%s, %s]", FIRMWARE_VERSION, __DATE__, __TIME__);

    LOG_F(TAG_MAIN, "FATAL");
    LOG_E(TAG_MAIN, "ERROR");
    LOG_W(TAG_MAIN, "WARN");
    LOG_D(TAG_MAIN, "DEBUG");
    LOG_T(TAG_MAIN, "TRACE");
    
    LOG_I(TAG_MAIN, "123");
    LOG_I(TAG_MAIN, "456");
    LOG_I(TAG_MAIN, "789");
    LOG_I(TAG_MAIN, "%d", 123);
    LOG_I(TAG_MAIN, "%d", 5345);
    LOG_I(TAG_MAIN, "%d", 75676575);
    LOG_I(TAG_MAIN, "%d", 99999);
    LOG_I(TAG_MAIN, "%d", 55555);
    LOG_I(TAG_MAIN, "%llu", 1231231);

    LOG_I(TAG_MAIN, "Setup completed");
}

void loop() {
    delay(1000);
}
