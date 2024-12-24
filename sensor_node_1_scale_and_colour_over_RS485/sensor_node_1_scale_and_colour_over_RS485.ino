#include <math.h>
#include <ezButton.h>  // by ArduinoGetStarted.com

ezButton BUTTON_SCALE(15);      //yellow button
ezButton BUTTON_AS7341(2);      //blue buttoon
ezButton BUTTON_SEND_DATA(14);  // green button

int BUTTON_SCALE_state = 0;
int BUTTON_AS7341_state = 0;
int BUTTON_SEND_DATA_state = 0;

#include "DFRobot_HX711_I2C.h"
#include "DFRobot_AS7341.h"

//DFRobot_HX711_I2C MyScale(&Wire,/*addr=*/0x64);
DFRobot_HX711_I2C MyScale;
float weight = 0;

DFRobot_AS7341 as7341;

struct t_as7341 {
  int spectralData[8];
  String as7341_string;
};

t_as7341 as7341_data;
String hex_color;
#define RXD1 36
#define TXD1 4

String final_resault="";


void setup() {
  Serial.begin(115200);
  Serial1.begin(9600, SERIAL_8E1, RXD1, TXD1);
  // calibreate_scale();
  startup_as7341();
  setup_buttons();
}



void setup_buttons() {
  BUTTON_SCALE.setDebounceTime(50);      // set debounce time to 50 milliseconds
  BUTTON_AS7341.setDebounceTime(50);     // set debounce time to 50 milliseconds
  BUTTON_SEND_DATA.setDebounceTime(50);  // set debounce time to 50 milliseconds
}

void read_buttons() {
  BUTTON_SCALE.loop();
  BUTTON_AS7341.loop();
  BUTTON_SEND_DATA.loop();
  BUTTON_SCALE_state = BUTTON_SCALE.getState();
  BUTTON_AS7341_state = BUTTON_AS7341.getState();
  BUTTON_SEND_DATA_state = BUTTON_SEND_DATA.getState();
}
void startup_as7341() {
  while (as7341.begin() != 0) {
    Serial.println("IIC init failed, please check if the wire connection is correct");
    delay(1000);
  }
  //Integration time = (ATIME + 1) x (ASTEP + 1) x 2.78µs
  //Set the value of register ATIME(1-255), through which the value of Integration time can be calculated. The value represents the time that must be spent during data reading.
  as7341.setAtime(29);
  //Set the value of register ASTEP(0-65534), through which the value of Integration time can be calculated. The value represents the time that must be spent during data reading.
  as7341.setAstep(1000);
  //Set gain value(0~10 corresponds to X0.5,X1,X2,X4,X8,X16,X32,X64,X128,X256,X512)
  as7341.setAGAIN(2);

  // //  Enable LED
  as7341.enableLed(true);
  // //  Set pin current to control brightness (1~20 corresponds to current 4mA,6mA,8mA,10mA,12mA,......,42mA)
  as7341.controlLed(1);
}

t_as7341 read_as7341(bool detailed_print) {
  t_as7341 local_as7341_data;

  DFRobot_AS7341::sModeOneData_t data1;
  DFRobot_AS7341::sModeTwoData_t data2;

  //Start spectrum measurement
  //Channel mapping mode: 1.eF1F4ClearNIR,2.eF5F8ClearNIR
  as7341.startMeasure(as7341.eF1F4ClearNIR);
  //Read the value of sensor data channel 0~5, under eF1F4ClearNIR
  data1 = as7341.readSpectralDataOne();
  as7341.startMeasure(as7341.eF5F8ClearNIR);
  //Read the value of sensor data channel 0~5, under eF5F8ClearNIR
  data2 = as7341.readSpectralDataTwo();
  if (detailed_print) {
    Serial.print("F1(405-425nm):");
    Serial.println(data1.ADF1);
    Serial.print("F2(435-455nm):");
    Serial.println(data1.ADF2);
    Serial.print("F3(470-490nm):");
    Serial.println(data1.ADF3);
    Serial.print("F4(505-525nm):");
    Serial.println(data1.ADF4);

    Serial.print("F5(545-565nm):");
    Serial.println(data2.ADF5);
    Serial.print("F6(580-600nm):");
    Serial.println(data2.ADF6);
    Serial.print("F7(620-640nm):");
    Serial.println(data2.ADF7);
    Serial.print("F8(670-690nm):");
    Serial.println(data2.ADF8);
    Serial.print("Clear:");
    Serial.println(data2.ADCLEAR);
    Serial.print("NIR:");
    Serial.println(data2.ADNIR);
  }
  local_as7341_data.as7341_string = String(data1.ADF2) + ";" + String(data1.ADF4) + ";" + String(data1.ADF3) + ";" + String(data2.ADF5) + ";" + String(data2.ADF7) + ";" + String(data2.ADF8) + ";" + String(data2.ADCLEAR) + ";" + String(data2.ADF6) + ";" + String(data2.ADNIR);

  local_as7341_data.spectralData[0] = data1.ADF1;
  local_as7341_data.spectralData[1] = data1.ADF2;
  local_as7341_data.spectralData[2] = data1.ADF3;
  local_as7341_data.spectralData[3] = data1.ADF4;
  local_as7341_data.spectralData[4] = data2.ADF5;
  local_as7341_data.spectralData[5] = data2.ADF6;
  local_as7341_data.spectralData[6] = data2.ADF7;
  local_as7341_data.spectralData[7] = data2.ADF8;


  return local_as7341_data;
}

String calculateColor(int spectralData[]) {
  // Define RGB values for each band based on wavelengths
  float wavelengthToRGB[8][3] = {
    { 0.5, 0, 1 },  // F1 (405-425nm): violet
    { 0, 0, 1 },    // F2 (435-455nm): blue
    { 0, 0.5, 1 },  // F3 (470-490nm): blue-cyan
    { 0, 1, 0.5 },  // F4 (505-525nm): green-cyan
    { 0, 1, 0 },    // F5 (545-565nm): green
    { 1, 1, 0 },    // F6 (580-600nm): yellow
    { 1, 0.5, 0 },  // F7 (620-640nm): orange
    { 1, 0, 0 }     // F8 (670-690nm): red
  };

  // Find the maximum spectral value for normalization
  int maxValue = 0;
  for (int i = 0; i < 8; i++) {
    if (spectralData[i] > maxValue) {
      maxValue = spectralData[i];
    }
  }

  // Normalize and calculate weighted RGB
  float weightedRGB[3] = { 0, 0, 0 };
  for (int i = 0; i < 8; i++) {
    float normalizedValue = (float)spectralData[i] / maxValue;
    weightedRGB[0] += normalizedValue * wavelengthToRGB[i][0];
    weightedRGB[1] += normalizedValue * wavelengthToRGB[i][1];
    weightedRGB[2] += normalizedValue * wavelengthToRGB[i][2];
  }

  // Clamp RGB values to [0, 1]
  for (int i = 0; i < 3; i++) {
    if (weightedRGB[i] > 1) {
      weightedRGB[i] = 1;
    }
  }

  // Convert to HEX color format
  int r = round(weightedRGB[0] * 255);
  int g = round(weightedRGB[1] * 255);
  int b = round(weightedRGB[2] * 255);

  char hexColor[8];
  sprintf(hexColor, "#%02X%02X%02X", r, g, b);
  // Show to LCD

  return String(hexColor);
}


void calibreate_scale() {
  while (!MyScale.begin()) {
    Serial.println("The initialization of the chip is failed, please confirm whether the chip connection is correct");
    delay(1000);
  }
  //Set the calibration weight when the weight sensor module is automatically calibrated (g)
  MyScale.setCalWeight(100);
  // Set the trigger threshold (G) for automatic calibration of the weight sensor module. When only the weight of the object on the scale is greater than this value, the module will start the calibration process
  // This value cannot be greater than the calibration weight of the setCalWeight() setting
  MyScale.setThreshold(30);

  //Start sensor calibration
  Serial.println("Please put the object within 5S");
  MyScale.enableCal();
  long time1 = millis();
  //Wait for sensor calibration to complete
  while (!MyScale.getCalFlag()) {
    delay(1000);
    if ((millis() - time1) > 7000) {
      Serial.println("The calibration failed this time, and no object is detected within 5S. The last calibration value will be used.");
      break;
    }
  }
  //Obtain the calibration value. The accurate calibration value can be obtained after the calibration operation is completed
  Serial.print("the calibration value of the sensor is: ");
  Serial.println(MyScale.getCalibration());
  MyScale.setCalibration(MyScale.getCalibration());
}

void read_scale() {
  float weight = MyScale.readWeight();
  // Show to LCD

  // Serial.print("weight is: ");
  // if (Weight > 0.5) {
  //   Serial.print(Weight, 1);
  // } else {
  //   Serial.print(0, 1);
  // }
  // Serial.println(" g");
}



void loop() {
  read_buttons();

  if (BUTTON_SCALE.isPressed()) {
    read_scale();
  }

  weight = 342.6;

  // if (BUTTON_AS7341.isPressed()) {
      if (1) {
    as7341_data = read_as7341(false);
    // as7341_string="as7341:123,456,789,101,112,131,415,161,718";
    // Serial.println(as7341_data.as7341_string);
    // hex_color = calculateColor(as7341_data.spectralData);
    // Serial.print("Hex color: ");
    // Serial.println(hex_color);
  }

  // if (BUTTON_SEND_DATA.isPressed()) {
    if(1){
    final_resault="#1;"+String(weight) + ";" + as7341_data.as7341_string+ "#";
    Serial.println(final_resault);
    Serial1.println(final_resault);
  }


  delay(1);
}
