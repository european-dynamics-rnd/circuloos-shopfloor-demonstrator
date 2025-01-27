#include "Seeed_MCP9600.h"
#include <DFRobotHighTemperatureSensor.h>

#include <ESP32-TWAI-CAN.hpp>
#define CAN_TX 5  // Connects to CTX
#define CAN_RX 35  // Connects to CRX


CanFrame rxFrame;  // Create frame to read

MCP9600 sensor(0x66);
float mcp9600_temp = 0;
int int_mcp9600_temp = 0;

// Pt1000
const float voltageRef = 3.300;                                     //Set reference voltage,you need test your IOREF voltage.
int HighTemperaturePin = 15;                                        // pin 15-> adc3                           //Setting pin
DFRobotHighTemperature PT100 = DFRobotHighTemperature(voltageRef);  //Define an PT100 object
int pt100_temp = 0;
float_pt100_temp=0;

err_t mcp9600_sensor_basic_config() {
  err_t ret = NO_ERROR;
  CHECK_RESULT(ret, sensor.set_filt_coefficients(FILT_MID));
  CHECK_RESULT(ret, sensor.set_cold_junc_resolution(COLD_JUNC_RESOLUTION_0_25));
  CHECK_RESULT(ret, sensor.set_ADC_meas_resolution(ADC_14BIT_RESOLUTION));
  CHECK_RESULT(ret, sensor.set_burst_mode_samp(BURST_32_SAMPLE));
  CHECK_RESULT(ret, sensor.set_sensor_mode(NORMAL_OPERATION));
  return ret;
}


err_t mcp9600_get_temperature(float* value) {
  err_t ret = NO_ERROR;
  float hot_junc = 0;
  float junc_delta = 0;
  float cold_junc = 0;
  CHECK_RESULT(ret, sensor.read_hot_junc(&hot_junc));
  CHECK_RESULT(ret, sensor.read_junc_temp_delta(&junc_delta));

  CHECK_RESULT(ret, sensor.read_cold_junc(&cold_junc));

  // Serial.print("hot junc=");
  // Serial.println(hot_junc);
  // Serial.print("junc_delta=");
  // Serial.println(junc_delta);
  // Serial.print("cold_junc=");
  // Serial.println(cold_junc);

  *value = hot_junc;

  return ret;
}

void setup_mcp9600() {

  Serial.println("Serial start!!");
  if (sensor.init(THER_TYPE_K)) {
    Serial.println("sensor init failed!!");
  }
  mcp9600_sensor_basic_config();
}

void setup_CAN() {
  // Set the pins
  ESP32Can.setPins(CAN_TX, CAN_RX);

  // Start the CAN bus at 500 kbps
  if (ESP32Can.begin(ESP32Can.convertSpeed(500))) {
    Serial.println("CAN bus started!");
  } else {
    Serial.println("CAN bus failed!");
  }
}

void setup() {
  Serial.begin(115200);
  delay(10);
  setup_mcp9600();
  setup_CAN() ;
}


void send_measurement_can(int temp1, int temp2) {
  // Max temp  -327.68 to 327.67
  CanFrame tempFrame = { 0 };
  tempFrame.identifier = 0xED1;
  // Split number1 into 2 bytes
  tempFrame.extd = 0;
  tempFrame.data_length_code = 8;
  tempFrame.data[0] = (temp1 >> 8) & 0xFF;  // High byte
  tempFrame.data[1] = temp1 & 0xFF;         // Low byte
  tempFrame.data[2] = (temp2 >> 8) & 0xFF;  // High byte
  tempFrame.data[3] = temp2 & 0xFF;        // Low byte
  tempFrame.data[4] = 0xAA;                   //// Best to use 0xAA (0b10101010) instead of 0
  tempFrame.data[5] = 0xAA;                   //  CAN works better this way as it needs
  tempFrame.data[6] = 0xAA;                   //to avoid bit-stuffing
  tempFrame.data[7] = 0xAA;
  // Accepts both pointers and references
  ESP32Can.writeFrame(tempFrame);  // timeout defaults to 1 ms

}


void loop() {

  mcp9600_get_temperature(&mcp9600_temp);
  int_mcp9600_temp = round(mcp9600_temp * 100);

  Serial.print("mcp9600_temp: ");
  Serial.print(mcp9600_temp);
  Serial.println(" C");
  delay(10);
  pt100_temp = PT100.readTemperature(HighTemperaturePin);  //Get temperature
  float_pt100_temp=pt100_temp/10;
  Serial.print("PT100:  ");
  Serial.print(float_pt100_temp);
  Serial.println(" C");

  send_measurement_can(int_mcp9600_temp,pt100_temp);
  delay(1000*10 );
}