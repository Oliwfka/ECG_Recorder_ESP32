#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <SPI.h>
#include "protocentral_max30003.h"

#define SERVICE_UUID "53d1aafe-ddef-4165-bf9e-4d06efac243c"
#define ACC_x_UUID "8a611f55-6639-4b7c-ad98-30b59252692e"
#define ACC_y_UUID "5921c426-8576-45f1-aa6a-67677bc76d35"
#define ACC_z_UUID "620da595-293a-4f28-a52b-d24b9ead503b"
#define GYRO_x_UUID "a0f90571-4f82-4cf6-b502-228bb5c4090b"
#define GYRO_y_UUID "2b50ea61-f47e-4bb6-a352-bab845c4d6a5"
#define GYRO_z_UUID "9d3ca539-6f48-4c61-be81-1b97d65d16e9"
#define ECG_UUID "ef3d9410-d257-4693-a1eb-63e89274a848"

#define MAX30003_CS_PIN 5

MAX30003 max30003(MAX30003_CS_PIN);

BLECharacteristic ACC_char_x(ACC_x_UUID, BLECharacteristic::PROPERTY_NOTIFY | BLECharacteristic::PROPERTY_READ);
BLECharacteristic ACC_char_y(ACC_y_UUID,BLECharacteristic::PROPERTY_NOTIFY | BLECharacteristic::PROPERTY_READ);
BLECharacteristic ACC_char_z(ACC_z_UUID,BLECharacteristic::PROPERTY_NOTIFY | BLECharacteristic::PROPERTY_READ);
BLECharacteristic GYRO_char_x(GYRO_x_UUID, BLECharacteristic::PROPERTY_NOTIFY | BLECharacteristic::PROPERTY_READ);
BLECharacteristic GYRO_char_y(GYRO_y_UUID,BLECharacteristic::PROPERTY_NOTIFY | BLECharacteristic::PROPERTY_READ);
BLECharacteristic GYRO_char_z(GYRO_z_UUID,BLECharacteristic::PROPERTY_NOTIFY | BLECharacteristic::PROPERTY_READ);
BLECharacteristic ECG_char(ECG_UUID, BLECharacteristic::PROPERTY_NOTIFY | BLECharacteristic::PROPERTY_READ);

Adafruit_BNO055 bno = Adafruit_BNO055(55);

void setup(void) {
  Serial.begin(9600);

  BLEDevice::init("EKG");
  BLEServer *pServer = BLEDevice::createServer();
  BLEService *pService = pServer->createService(BLEUUID(SERVICE_UUID), 30, 0);
  pService->addCharacteristic(&ACC_char_x);
  pService ->addCharacteristic(&ACC_char_y);
  pService ->addCharacteristic(&ACC_char_z);
  pService->addCharacteristic(&GYRO_char_x);
  pService ->addCharacteristic(&GYRO_char_y);
  pService ->addCharacteristic(&GYRO_char_z);
  pService->addCharacteristic(&ECG_char);

  ACC_char_x.addDescriptor(new BLE2902());
  ACC_char_y.addDescriptor(new BLE2902());
  ACC_char_z.addDescriptor(new BLE2902());
  GYRO_char_x.addDescriptor(new BLE2902());
  GYRO_char_y.addDescriptor(new BLE2902());
  GYRO_char_z.addDescriptor(new BLE2902());
  ECG_char.addDescriptor(new BLE2902());

  BLEAdvertising *pAdvertising = pServer->getAdvertising();
  pService->start();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
  pAdvertising->setMaxPreferred(0x12);
  BLEDevice::startAdvertising();


  if (!bno.begin()) {
    Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
    while (1)
      ;
  }

  delay(100);

  bno.setExtCrystalUse(true);

  pinMode(MAX30003_CS_PIN, OUTPUT);

  digitalWrite(MAX30003_CS_PIN, HIGH);  //disable device

  SPI.begin();

  bool ret = max30003.max30003ReadInfo();
  if (ret) {
    Serial.println("Max30003 read ID Success");
  } else {

    while (!ret) {
      //stay here untill the issue is fixed.
      ret = max30003.max30003ReadInfo();
      Serial.println("Failed to read ID, please make sure all the pins are connected");
      delay(10000);
    }
  }

  Serial.println("Initialising the chip ...");
  max30003.max30003Begin();  // initialize MAX30003
}

void loop(void) {
  imu::Vector<3> acc = bno.getVector(Adafruit_BNO055::VECTOR_LINEARACCEL);
  imu::Vector<3> gyro = bno.getVector(Adafruit_BNO055::VECTOR_GYROSCOPE);

  double acc_x = acc.x();  //[m/s]
  double acc_y = acc.y();
  double acc_z = acc.z();
  ACC_char_x.setValue(acc_x);
  ACC_char_y.setValue(acc_y);
  ACC_char_z.setValue(acc_z);
  ACC_char_x.notify();
  ACC_char_y.notify();
  ACC_char_z.notify();

  double gyro_x = gyro.x() * 180 / 3.14;  //konwersja z [rad/s] na[stopnie/s]
  double gyro_y = gyro.y()*180/3.14;
  double gyro_z = gyro.z()*180/3.14;
  GYRO_char_x.setValue(gyro_x);
  GYRO_char_y.setValue(gyro_y);
  GYRO_char_z.setValue(gyro_z);
  GYRO_char_x.notify();
  GYRO_char_y.notify();
  GYRO_char_z.notify();

  /*Serial.print("Acceleration: \n");
  Serial.print("X: ");
  Serial.print(acc_x);
  Serial.print(" m^2/s");*/
  //Serial.print(" Y:");
  //Serial.print("\t");
  //Serial.print(acc_y);
  //Serial.print(" Z:");
  //Serial.print("\t");
  //Serial.print(acc_z);
  //Serial.print("\n");
  /*Serial.print("\nGyroscope: \n");
  Serial.print("X: ");
  Serial.print(gyro_x);
  Serial.print(" degree/s \n");*/
  //Serial.print(" Y:");
  //Serial.print("\t");
  //Serial.print(gyro_y);
  //Serial.print(" Z:");
  //Serial.print("\t");
  //Serial.print(gyro_z);
  //Serial.println("");*/

  max30003.getEcgSamples();  //It reads the ecg sample and stores it to max30003.ecgdata .
  Serial.println(max30003.ecgdata/10000);
  double ECG_data = max30003.ecgdata/10000;
  ECG_char.setValue(ECG_data);
  ECG_char.notify();

  delay(12);
}