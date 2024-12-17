#include "Seeed_MCP9600.h"
#include <DFRobotHighTemperatureSensor.h>



MCP9600 sensor(0x66);
float mcp9600_temp = 0;

// Pt1000
const float voltageRef = 3.300;                                     //Set reference voltage,you need test your IOREF voltage.
int HighTemperaturePin = 15;             // pin 15-> adc3                           //Setting pin
DFRobotHighTemperature PT100 = DFRobotHighTemperature(voltageRef);  //Define an PT100 object
int pt100_temp = 0;


err_t sensor_basic_config() {
  err_t ret = NO_ERROR;
  CHECK_RESULT(ret, sensor.set_filt_coefficients(FILT_MID));
  CHECK_RESULT(ret, sensor.set_cold_junc_resolution(COLD_JUNC_RESOLUTION_0_25));
  CHECK_RESULT(ret, sensor.set_ADC_meas_resolution(ADC_14BIT_RESOLUTION));
  CHECK_RESULT(ret, sensor.set_burst_mode_samp(BURST_32_SAMPLE));
  CHECK_RESULT(ret, sensor.set_sensor_mode(NORMAL_OPERATION));
  return ret;
}


err_t get_temperature(float* value) {
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
  sensor_basic_config();
}

void setup() {
  Serial.begin(115200);
  delay(10);
  setup_mcp9600();
}





void loop() {

  get_temperature(&mcp9600_temp);
  Serial.print("mcp9600_temp :");
  Serial.println(mcp9600_temp);
  Serial.println();
  delay(1000);
  pt100_temp = PT100.readTemperature(HighTemperaturePin);  //Get temperature
  Serial.print("PT100:  ");
  Serial.print(pt100_temp);
  Serial.println("  C");
}