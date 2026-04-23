#include "secrets.hpp"

#include <pinicore.hpp>
#include <Arduino.h>
#include <ArduinoHttpClient.h>

using namespace pinicore;

#define TAG_MAIN    "main"

#define FIRMWARE_VERSION    666

#define USE_WIFI
#ifdef USE_WIFI
    WiFiComm wifi;
    INetwork* network = (INetwork*)&wifi;
#else
    MobileComm mobile;
    INetwork* network = (INetwork*)&mobile;
#endif

bool connected = false;

#define STORAGE_ID "PINI_TEST_CONTROLLER"
Storage storage;


void setup_() {
    Serial.begin(115200);
    Serial.println();   // Just to start on a new clean line

    LOG_I(TAG_MAIN, "Setup started");
    LOG_I(TAG_MAIN, "Firmware: [%d] | Build: [%s, %s]", FIRMWARE_VERSION, __DATE__, __TIME__);

#ifdef USE_WIFI
    wifi.init();
    wifi.config(WIFI_SSID, WIFI_PASS);
#else
    mobile.init(23, 4, 5, 27, 26);
    mobile.config("", "");
#endif

    network->enable();
    network->connect();
    while (!network->isConnected()) {
        delay(1000);
    }
    LOG_I(TAG_MAIN, "connected = %d", network->isConnected());

    /*
    OTATS ota(client, FIRMWARE_VERSION, "s_");
    ota.setProgressCallback(
        /* onProgress * [](uint32_t downloadedBytes, uint32_t totalBytes) {
            LOG_T(TAG_MAIN, "DL: %6.02f %%", (100.0 * downloadedBytes) / totalBytes);
        }
    );
    ota.setCredentials(OTA_TS_USER, OTA_TS_PASS);
    ota.setCertificate(OTA_TS_SSL_CERTIFICATE);
    ota.checkUpdate();
    
    EOTAUpdateStatus statusOTA = ota.update();
    if (statusOTA == EOTAUpdateStatus::OTA_INSTALLED) {
        ESP.restart();
    }
    */

    LOG_I(TAG_MAIN, "Setup completed");
}

void loop_() {
    network->maintain();
}















#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_VEML7700.h>
#include <ClosedCube_HDC1080.h>
#include <SparkFun_MAX1704x_Fuel_Gauge_Arduino_Library.h>

#define SOIL_PIN 4

#define SDA_PIN 2
#define SCL_PIN 3

#define SOIL_EN 5
#define TEMP_EN 6
#define LIGHT_EN 7

Adafruit_VEML7700 lightSensor;
ClosedCube_HDC1080 tempSensor;
SFE_MAX1704X battery;

void setup() {
    Serial.begin(115200);
    Serial.println();   // Just to start on a new clean line

    LOG_I(TAG_MAIN, "Setup started");

    Wire.begin(SDA_PIN, SCL_PIN);

    // Power sensors
    pinMode(SOIL_EN, OUTPUT);
    pinMode(TEMP_EN, OUTPUT);
    pinMode(LIGHT_EN, OUTPUT);

    digitalWrite(SOIL_EN, HIGH);
    digitalWrite(TEMP_EN, HIGH);
    digitalWrite(LIGHT_EN, HIGH);

    analogReadResolution(12);

    // Light sensor
    if (!lightSensor.begin())
        LOG_I(TAG_MAIN, "VEML7700 not detected");

    // Temperature/Humidity
    tempSensor.begin(0x40);

    // Battery
    if (!battery.begin())
        LOG_I(TAG_MAIN, "MAX17043 not detected");
}

float readSoilPercent() {
    int mv = analogReadMilliVolts(SOIL_PIN);  // calibrated reading
    float voltage = mv / 1000.0;

    const float DRY = 1.98;
    const float WET = 0.89;

    float percent = (DRY - voltage) * 100.0 / (DRY - WET);
    if (percent < 0) percent = 0;
    if (percent > 100) percent = 100;
    return percent;
}

void loop() {
    float soil = readSoilPercent();

    float lux = lightSensor.readLux();

    float temperature = tempSensor.readTemperature();
    float humidity = tempSensor.readHumidity();

    float voltage = battery.getVoltage();
    float percent = battery.getSOC();

    LOG_I(TAG_MAIN,
        "Soil: %3.3f | Light (lux): %3.3f | Temperature (C): %3.3f | Humidity (%): %3.3f | Battery Voltage: %1.3f | Battery Level (%%): %3.2f",
        soil, lux, temperature, humidity, voltage, percent
    );
    delay(1000);
}