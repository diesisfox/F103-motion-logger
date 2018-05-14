#include <SD.h>
#include <Wire.h>

const int MPU_addr=0x68;  // I2C address of the MPU-6050
int16_t AcX=0,AcY=0,AcZ=0,Tmp,GyX=0,GyY=0,GyZ=0;
int loopCounter = 0;
File dataFile;

void setup(){
  pinMode(13,OUTPUT);
  Serial.begin(38400);
  
  Wire.begin();
  
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
  Serial.println("transmission begun");

  Wire.beginTransmission(MPU_addr);
  Wire.write(0x1C);  // PWR_MGMT_1 register
  Wire.write(0b00011000);     // set to 1 8192 LSB/g
  Wire.endTransmission(true);

  Wire.beginTransmission(MPU_addr);
  Wire.write(0x1B);  // PWR_MGMT_1 register
  Wire.write(0b00000000);     // set to 1 8192 LSB/°/s
  Wire.endTransmission(true);
  Serial.println("transmission end");

  Serial.print("Initializing SD card...");

  // see if the card is present and can be initialized:
  if (!SD.begin()) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    while (1);
  }
  Serial.println("card initialized.");

  Serial.println("opening File...");
  dataFile = SD.open("datalog.txt", FILE_WRITE);
  if (dataFile) {
    dataFile.print("\n\n\nAcX\tAcY\tAcZ\ttemp\tGyX\tGyY\tGyZ\n");
    dataFile.flush();
    Serial.println("File opened");
  }
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening datalog.txt");
    for(;;);
  }
  
//  
//  digitalWrite(13,1);
//  delay(1000);
//  digitalWrite(13,0);
//  delay(1000);
//
//  digitalWrite(13,1);

  getValues();

  loopCounter=0;
}

void loop(){
  
  getValues();
  logStuff();
  if((loopCounter%100)==0){
    printStuff();
  }
//  digitalWrite(13,(loopCounter%3 > 0));
  loopCounter++;
}

void logStuff(){
  String dataString = "";
  dataString += String(AcX);
  dataString += "\t";
  dataString += String(AcY);
  dataString += "\t";
  dataString += String(AcZ);
  dataString += "\t";
  dataString += String(Tmp/340.00+36.53);
  dataString += "\t";
  dataString += String(GyX);
  dataString += "\t";
  dataString += String(GyY);
  dataString += "\t";
  dataString += String(GyZ);
  dataString += "\n";

  dataFile.print(dataString);
  dataFile.flush();
}

void printStuff(){
  Serial.print(loopCounter);Serial.print(": ");
  Serial.print("AcX="); Serial.print(AcX);
  Serial.print(" | AcY="); Serial.print(AcY);
  Serial.print(" | AcZ="); Serial.print(AcZ);
  Serial.print(" | Tmp="); Serial.print(Tmp/340.00+36.53);  //equation for temperature in degrees C from datasheet
  Serial.print(" | GyX="); Serial.print(GyX);
  Serial.print(" | GyY="); Serial.print(GyY);
  Serial.print(" | GyZ="); Serial.print(GyZ);
  Serial.println();
}

void getValues(){
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr,14,true);  // request a total of 14 registers
  AcX=Wire.read()<<8|Wire.read();  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)    
  AcY=Wire.read()<<8|Wire.read();  // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  AcZ=Wire.read()<<8|Wire.read();  // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
  Tmp=Wire.read()<<8|Wire.read();  // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
  GyX=Wire.read()<<8|Wire.read();  // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
  GyY=Wire.read()<<8|Wire.read();  // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
  GyZ=Wire.read()<<8|Wire.read();  // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)
}

