/*
ESP32 used pins:
2  -> AS7341_GPIO_0, IDC10
4  -> RS485_TX, IDC10
13 -> I2C_SDA, IDC10 + EXT_1
14 -> Potantiometer, EXT_1
15 -> AS7341_INT, IDC10
16 -> I2C_SCL, IDC10 + EXT_1
18  -> Blue button, EXT_1
19 -> Green button, EXT_1
22 -> Yellow button, EXT_1
36 -> RS485_RX, IDC10

*/
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);


#include <math.h>
// #include <ezButton.h>  // by ArduinoGetStarted.com

#define BUTTON_SCALE 22          //yellow button
#define BUTTON_AS7341 19         // green button
#define BUTTON_SEND_DATA 18      //blue buttoon
#define AS7341_POTENTIOMETER 14  // potentiometer for AS7341 LED level

#include "DFRobot_HX711_I2C.h"
#include "DFRobot_AS7341.h"

//DFRobot_HX711_I2C MyScale(&Wire,/*addr=*/0x64);
DFRobot_HX711_I2C MyScale;
float weight = -1;
bool is_scale_calibrated=false ; 


DFRobot_AS7341 as7341;
int as7341_led_level = 1;

struct t_as7341 {
  int spectralData[8];
  String as7341_string;
};

t_as7341 as7341_data;
String hex_color;
#define RXD1 36
#define TXD1 4

String final_resault = "";


void setup() {
  Serial.begin(115200);
  Serial1.begin(9600, SERIAL_8E1, RXD1, TXD1);
  startup_as7341();
  setup_buttons();
  setup_lcd();
  pinMode(AS7341_POTENTIOMETER, INPUT);
  Serial.println("starting....");
}

void setup_lcd() {
  lcd.init();  // initialize the lcd
  lcd.backlight();

  lcd.home();

  lcd.setCursor(0, 0);
  // lcd.print("1234567890123456"
  lcd.print("-+ CIRCULOOS +-");
  lcd.setCursor(0, 1);
  lcd.print("SHOPFLOOR Demo");
}

void setup_buttons() {
  pinMode(BUTTON_SCALE, INPUT);
  pinMode(BUTTON_AS7341, INPUT);
  pinMode(BUTTON_SEND_DATA, INPUT);
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
  as7341.controlLed(as7341_led_level);
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
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("SCALE init probl");
    delay(1000);
  }
  //Set the calibration weight when the weight sensor module is automatically calibrated (g)
  MyScale.setCalWeight(100);
  // Set the trigger threshold (G) for automatic calibration of the weight sensor module. When only the weight of the object on the scale is greater than this value, the module will start the calibration process
  // This value cannot be greater than the calibration weight of the setCalWeight() setting
  MyScale.setThreshold(30);

  //Start sensor calibration
  Serial.println("Please put the object within 5S");
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Please put the ");
  lcd.setCursor(0, 1);
  lcd.print("object within 5S");

  MyScale.enableCal();
  long time1 = millis();
  //Wait for sensor calibration to complete
  while (!MyScale.getCalFlag()) {
    delay(1000);
    if ((millis() - time1) > 7000) {
      Serial.println("The calibration failed this time, and no object is detected within 5S. The last calibration value will be used.");
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("The calibration failed");
      break;
    }
  }
  //Obtain the calibration value. The accurate calibration value can be obtained after the calibration operation is completed
  Serial.print("the calibration value of the sensor is: ");
  Serial.println(MyScale.getCalibration());
  MyScale.setCalibration(MyScale.getCalibration());
}

float read_scale() {
  float _weight = MyScale.readWeight();
  // Show to LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("weight is: ");
  lcd.setCursor(0, 1);
  if (_weight > 0.5) {
    lcd.print(_weight, 1);
  } else {
    lcd.print(0, 1);
  }
  lcd.print(" gr");
  return _weight;
}



void loop() {
  if (digitalRead(BUTTON_AS7341) == LOW) {
    Serial.println("BUTTON_AS7341");
    Serial.print("led_level: ");
    Serial.println(as7341_led_level);

    as7341_data = read_as7341(false);
    Serial.println(as7341_data.as7341_string);
    hex_color = calculateColor(as7341_data.spectralData);
    Serial.print("Hex color: ");
    Serial.println(hex_color);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Hex color:");
    lcd.setCursor(0, 1);
    lcd.print(hex_color);
    delay(10);
  }

  if (digitalRead(BUTTON_SCALE) == LOW) {
    Serial.println(" BUTTON_SCALE");
    if (!is_scale_calibrated) {
      calibreate_scale();
      is_scale_calibrated=true;
    }
    weight = read_scale();
  }

  if (digitalRead(BUTTON_SEND_DATA) == LOW) {
    final_resault="#1;"+String(weight) + ";" + as7341_data.as7341_string+ "#";
    Serial.println(final_resault);
    Serial1.println(final_resault);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Sending Data:");
    lcd.setCursor(0, 1);
    lcd.print(final_resault);
  }

  as7341_led_level = map(analogRead(AS7341_POTENTIOMETER), 100, 4095, 0, 18);
  as7341.controlLed(as7341_led_level);

  delay(10);
}
