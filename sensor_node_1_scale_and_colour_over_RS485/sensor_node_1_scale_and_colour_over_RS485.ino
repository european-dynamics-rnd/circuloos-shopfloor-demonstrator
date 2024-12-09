#include "DFRobot_HX711_I2C.h"
#include "DFRobot_AS7341.h"

//DFRobot_HX711_I2C MyScale(&Wire,/*addr=*/0x64);
DFRobot_HX711_I2C MyScale;

DFRobot_AS7341 as7341;


void setup() {
  Serial.begin(115200);
  calibreate_scale();
}




void startup_as7341(){
    while (as7341.begin() != 0) {
    Serial.println("IIC init failed, please check if the wire connection is correct");
    delay(1000);
  }
//  //Integration time = (ATIME + 1) x (ASTEP + 1) x 2.78µs
//  //Set the value of register ATIME(1-255), through which the value of Integration time can be calculated. The value represents the time that must be spent during data reading.
//  as7341.setAtime(29);
//  //Set the value of register ASTEP(0-65534), through which the value of Integration time can be calculated. The value represents the time that must be spent during data reading.
//  as7341.setAstep(599);
//  //Set gain value(0~10 corresponds to X0.5,X1,X2,X4,X8,X16,X32,X64,X128,X256,X512)
//  as7341.setAGAIN(7);

//  Enable LED
 as7341.enableLed(true);
//  Set pin current to control brightness (1~20 corresponds to current 4mA,6mA,8mA,10mA,12mA,......,42mA)
 as7341.controlLed(10);
}

void read_as7341(){
    DFRobot_AS7341::sModeOneData_t data1;
  DFRobot_AS7341::sModeTwoData_t data2;

  //Start spectrum measurement 
  //Channel mapping mode: 1.eF1F4ClearNIR,2.eF5F8ClearNIR
  as7341.startMeasure(as7341.eF1F4ClearNIR);
  //Read the value of sensor data channel 0~5, under eF1F4ClearNIR
  data1 = as7341.readSpectralDataOne();

  Serial.print("F1(405-425nm):");
  Serial.println(data1.ADF1);
  Serial.print("F2(435-455nm):");
  Serial.println(data1.ADF2);
  Serial.print("F3(470-490nm):");
  Serial.println(data1.ADF3);
  Serial.print("F4(505-525nm):");   
  Serial.println(data1.ADF4);
  //Serial.print("Clear:");
  //Serial.println(data1.ADCLEAR);
  //Serial.print("NIR:");
  //Serial.println(data1.ADNIR);
  as7341.startMeasure(as7341.eF5F8ClearNIR);
  //Read the value of sensor data channel 0~5, under eF5F8ClearNIR
  data2 = as7341.readSpectralDataTwo();
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

  String output =  String(data1.ADF2) + "," + String(data1.ADF4) + "," + String(data1.ADF3) + "," + String(data2.ADF5) + "," + String(data2.ADF7) + "," + String(data2.ADF8) + "," + String(data2.ADCLEAR) + "," + String(data2.ADF6) + "," + String(data2.ADNIR);


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





void loop() {
  float Weight = MyScale.readWeight();
  Serial.print("weight is: ");
  if (Weight > 0.5) {
    Serial.print(Weight, 1);
  } else {
    Serial.print(0, 1);
  }
  Serial.println(" g");
  delay(1000);
}
