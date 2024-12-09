#include "bme68xLibrary.h"

//
// Install BME68x Sensor library by Bosch Sensortec
//

#ifndef ADD_I2C
#define ADD_I2C 0x77
#endif

Bme68x bme;

/**
 * @brief Initializes the sensor and hardware settings
 */
void setup(void) {
  Wire.begin();  //I2C mode
  Serial.begin(115200);

  while (!Serial)
    delay(10);

  setup_bmp680_i2c();
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
void loop(void) {
  read_bmp680_i2c();
}