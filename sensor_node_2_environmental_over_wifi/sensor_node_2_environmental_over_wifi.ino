#include "bme68xLibrary.h"

//
// Install BME68x Sensor library by Bosch Sensortec
// Install MQTT library for Arduino by Joel Gaehwiler <joel.gaehwiler@gmail.com> https://github.com/256dpi/arduino-mqtt
// Install ArduinoJson by Benoit Blanchon https://github.com/bblanchon/ArduinoJson
#ifndef ADD_I2C
#define ADD_I2C 0x77
#endif

Bme68x bme;

#include <WiFi.h>
// #include <WiFiClientSecure.h> // enable for mqtts
#include <MQTT.h>
#include <ArduinoJson.h>
#include <ESPmDNS.h>

const char ssid[] = "TP-Link_9DE0";
const char pass[] = "11531081";

const char mqtt_servername[] = "circuloos-shopfloor"; // the name of the rasberyPi (/etc/hostname) using mDNS/avahi 
IPAddress mqtt_server_ip;
const int mqtt_server_port = 1883;// 8883;
const char mqtt_server_username[] = "ramp-iot";
const char mqtt_server_password[] = "PmnMBT@c2Hf62Y4%sAJf";

// WiFiClientSecure net; // enable for mqtts
WiFiClient net;
MQTTClient mqtt_client;

lwmqtt_return_code_t mqtt_client_return_code;  // Arduino_library_folder\Arduino\libraries\MQTT\src\lwmqtt\lwmqtt.h
lwmqtt_err_t mqtt_client_last_error;           // Arduino_library_folder\Arduino\libraries\MQTT\src\lwmqtt\lwmqtt.h


unsigned long lastMillis = 0;

/*! Temperature in degree celsius */
float bme_temperature = 0.0;

/*! Pressure in Pascal */
float bme_pressure = 0.0;

/*! Humidity in % relative humidity x1000 */
float bme_humidity = 0.0;

/*! Gas resistance in Ohms */
float bme_gas_resistance = 0.0;

JsonDocument mqtt_data_json;
String mqtt_data = "";

void setup_wifi_mqtt() {
  WiFi.begin(ssid, pass);

 if(!MDNS.begin("esp32")) {
     Serial.println("Error starting mDNS");
     return;
  }

  while (mqtt_server_ip.toString() == "0.0.0.0") {
    Serial.println("Resolving host...");
    delay(250);
    mqtt_server_ip = MDNS.queryHost(mqtt_servername);
  }
  Serial.println("Host address resolved:");
  Serial.println(mqtt_server_ip.toString());   

// if you are using a normal DNS change the mqtt_server_ip->mqtt_servername
  mqtt_client.begin(mqtt_server_ip, mqtt_server_port, net);
  // mqtt_client.onMessage(messageReceived);
}

void connect_mqtt() {
  Serial.print("checking wifi...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }

  Serial.print("\nconnecting...");
  // do not verify tls certificate
  // net.setInsecure(); // enable for mqtts see https://github.com/espressif/arduino-esp32/blob/master/libraries/WiFiClientSecure/examples/WiFiClientSecure/WiFiClientSecure.ino

  while (!mqtt_client.connect("node_2", mqtt_server_username, mqtt_server_password)) {
    mqtt_client_return_code = mqtt_client.returnCode();
    mqtt_client_last_error = mqtt_client.lastError();
    Serial.print(mqtt_client_return_code);
    Serial.print("_");
    Serial.print(mqtt_client_last_error);
    Serial.print(".");

    delay(1000);
  }

  Serial.println("\nconnected!");
}



void mqtt_publish() {
  mqtt_client.loop();
  delay(10);  // <- fixes some issues with WiFi stability

  if (!mqtt_client.connected()) {
    connect_mqtt();
  }

  // publish a message roughly every second.
  if (millis() - lastMillis > 1000) {
    lastMillis = millis();
    // mqtt_client.publish("/hello_text", mqtt_data);
    delay(10); 
    char output[256];
    serializeJson(mqtt_data_json, output);
    mqtt_client.publish("/json/BME680KEYas32r/node-002/attrs", output); // See Readme_mqtt.md
  }
}



void setup_bmp680_i2c() {
  bme.begin(ADD_I2C, Wire);  //I2C mode

  if (bme.checkStatus()) {
    if (bme.checkStatus() == BME68X_ERROR) {
      Serial.println("Sensor error:" + bme.statusString());
      return;
    } else if (bme.checkStatus() == BME68X_WARNING) {
      Serial.println("Sensor Warning:" + bme.statusString());
    }
  }

  /* Set the default configuration for temperature, pressure and humidity */
  bme.setTPH();

  /* Set the heater configuration to 300 deg C for 100ms for Forced mode */
  bme.setHeaterProf(300, 100);

}

void read_bmp680_i2c() {
  bme68xData data;

  bme.setOpMode(BME68X_FORCED_MODE);
  delay(500 + bme.getMeasDur() / 200);

  if (bme.fetchData()) {
    bme.getData(data);
    bme_temperature = data.temperature;
    bme_pressure = data.pressure;
    bme_humidity = data.humidity;
    bme_gas_resistance = data.gas_resistance;

    mqtt_data = "bme_temperature:" + String(bme_temperature) + ",bme_pressure:" + String(bme_pressure) + ",bme_humidity:" + String(bme_humidity) + ",bme_gas_resistance:" + String(bme_gas_resistance) + ";";
    mqtt_data_json["bme_temp"] = bme_temperature;
    mqtt_data_json["bme_pres"] = bme_pressure;
    mqtt_data_json["bme_rh"] = bme_humidity;
    mqtt_data_json["bme_gas_res"] = bme_gas_resistance;

    Serial.print(String(millis()) + ", bme_temperature: ");
    Serial.print(String(bme_temperature) + ", bme_pressure: ");
    Serial.print(String(bme_pressure) + ", bme_humidity: ");
    Serial.print(String(bme_humidity) + ",bme_gas_resistance: ");
    Serial.print(String(bme_gas_resistance) + ", ");
    Serial.println(data.status, HEX);
  }
}


/**
 * @brief Initializes the sensor and hardware settings
 */
void setup(void) {
  Wire.begin();  //I2C mode
  Serial.begin(115200);

  while (!Serial)
    delay(10);

  setup_bmp680_i2c();

  setup_wifi_mqtt();

  connect_mqtt();
}

void loop(void) {
  read_bmp680_i2c();
  mqtt_publish();
  delay(10 * 60 * 1000); //10min delay
}
