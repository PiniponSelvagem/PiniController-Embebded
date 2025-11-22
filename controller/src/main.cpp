#include <pinicore.hpp>
#include <Arduino.h>
#include <ArduinoHttpClient.h>

#define TAG_MAIN    "main"

#define FIRMWARE_VERSION    1



/**
 * Certificate used on requests to: ota.trigger.systems
 * Certificate information:
 *      Identity:       ISRG Root X1
 *      Verified by:    ISRG Root X1
 *      Expires:        2035-06-04
 */
#define OTA_SSL_CERTIFICATE \
"-----BEGIN CERTIFICATE-----\n" \
"-----END CERTIFICATE-----\n"



MobileTSComm mobile;
WiFiComm wifi;

INetwork* network = (INetwork*)&wifi;

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

    /*
    mobile.init(23, 4, 5, 27, 26);
    mobile.config("", "");
    mobile.connect();
    */

    wifi.init();
    wifi.config("Vodafone-50F5D2", "2EGk3XTe9H");
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

    OTATS ota(client, 0, "r_e_0CDC7E69AEE4");
    ota.addCertificate(OTA_SSL_CERTIFICATE);
    ota.checkUpdate();
    
    ota.update();

    LOG_I(TAG_MAIN, "Setup completed");
}

void loop() {
    network->maintain();
}
