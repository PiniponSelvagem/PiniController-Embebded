#include <pinicore.hpp>
#include <Arduino.h>
#include <ArduinoHttpClient.h>

#include "secrets.hpp"

#include <PubSubClient.h>


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

MQTT mqtt;



const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 0;      // adjust if needed
const int   daylightOffset_sec = 0; // adjust if needed




void MQTT_callback(const char *topic, const byte *payload, const unsigned int length) {
    LOG_I(TAG_MAIN, "Received: [topic: %s] [payload: %s] [length: %d]", topic, payload, length);
}

void setup() {
    Serial.begin(115200);
    Serial.println();	// Just to start on a new clean line

    LOG_I(TAG_MAIN, "Setup started");
    LOG_I(TAG_MAIN, "Firmware: [%d] | Build: [%s, %s]", FIRMWARE_VERSION, __DATE__, __TIME__);

    LOG_F(TAG_MAIN, "Fatal example");
    LOG_E(TAG_MAIN, "Error example");
    LOG_W(TAG_MAIN, "Warning example");
    LOG_I(TAG_MAIN, "Information example");
    LOG_D(TAG_MAIN, "Debug example");
    LOG_T(TAG_MAIN, "Trace example");

#ifdef USE_WIFI
    wifi.init();
    wifi.config("", "");
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

    Client* client = network->getClient();

    HttpClient http(*client, "example.com");
    int error = http.get("/");
    int status  = http.responseStatusCode();
    int bodyLen = http.contentLength();
    String body = http.responseBody();
    LOG_D(TAG_MAIN, "Received: [status: %d] [body (%d): %s]", status, bodyLen, body.c_str());


    /*
    HttpClient http2(*client, "worldtimeapi.org");
    LOG_T(TAG_MAIN, "timeout %d", http2.getTimeout());
    int error2 = http2.get("/");
    LOG_T(TAG_MAIN, "after get");
    int status2  = http2.responseStatusCode();
    int bodyLen2 = http2.contentLength();
    String body2 = http2.responseBody();
    LOG_D(TAG_MAIN, "Received: [status: %d] [body (%d): %s]", status2, bodyLen2, body2.c_str());
    */






    OTATS ota(client, FIRMWARE_VERSION, "r_e_0CDC7E69AEE4");
    ota.setProgressCallback(
        /* onProgress */ [](uint32_t downloadedBytes, uint32_t totalBytes) {
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


    mqtt.setClient(client, "PINI_666");
    mqtt.setServer("staging.trigger.systems", 9069);
    mqtt.setCredentials(MQTT_TS_USER, MQTT_TS_PASS);

    mqtt.setWill("pinicore/up/lwt", "0", 2, true);

    mqtt.onTopic(
        "pinicore/down/test",
        [](const char* payload, uint32_t length) {
            LOG_I(TAG_MAIN, "TEST topic: [payload: '%s'] [length: %d]", payload, length);
        }
    );

    mqtt.onConnect(
        []() {
            mqtt.publish("pinicore/up/lwt", "1", true);
        }
    );
    mqtt.connect();

    LOG_I(TAG_MAIN, "Setup completed");
}

uint64_t lastUpdate;
void loop() {
    network->maintain();
    mqtt.maintain();
}
