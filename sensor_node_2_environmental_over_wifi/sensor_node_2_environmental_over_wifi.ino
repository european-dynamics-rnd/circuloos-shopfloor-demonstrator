#include "bme68xLibrary.h"

//
// Install BME68x Sensor library by Bosch Sensortec
// Install MQTT library for Arduino by Joel Gaehwiler <joel.gaehwiler@gmail.com> https://github.com/256dpi/arduino-mqtt

#ifndef ADD_I2C
#define ADD_I2C 0x77
#endif

Bme68x bme;

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <MQTT.h>

const char ssid[] = "LEDE";
const char pass[] = "kostasgompakis";

WiFiClientSecure net;
MQTTClient mqtt_client;

unsigned long lastMillis = 0;

void setup_wifi_mqtt() {
  WiFi.begin(ssid, pass);
  mqtt_client.begin("public.cloud.shiftr.io", 8883, net);
  mqtt_client.onMessage(messageReceived);
}

void connect_mqtt() {
  Serial.print("checking wifi...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }

  Serial.print("\nconnecting...");
  // do not verify tls certificate
  // check the following example for methods to verify the server:
  // https://github.com/espressif/arduino-esp32/blob/master/libraries/WiFiClientSecure/examples/WiFiClientSecure/WiFiClientSecure.ino
  net.setInsecure();
  while (!mqtt_client.connect("arduino", "public", "public")) {
    Serial.print(".");
    delay(1000);
  }

  Serial.println("\nconnected!");

}



void mqtt_publish() {
  mqtt_client.loop();
  delay(10);  // <- fixes some issues with WiFi stability

  if (!mqtt_client.connected()) {
    connect();
  }

  // publish a message roughly every second.
  if (millis() - lastMillis > 1000) {
    lastMillis = millis();
    mqtt_client.publish("/helloADFADF", "aQFK24PU2985WIJDFLSDHFLSHG;");
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

  Serial.println("TimeStamp(ms), Temperature(deg C), Pressure(Pa), Humidity(%), Gas resistance(ohm), Status");
}

void read_bmp680_i2c() {
  bme68xData data;

  bme.setOpMode(BME68X_FORCED_MODE);
  delay(500 + bme.getMeasDur() / 200);

  if (bme.fetchData()) {
    bme.getData(data);
    Serial.print(String(millis()) + ", ");
    Serial.print(String(data.temperature) + ", ");
    Serial.print(String(data.pressure) + ", ");
    Serial.print(String(data.humidity) + ", ");
    Serial.print(String(data.gas_resistance) + ", ");
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
}
