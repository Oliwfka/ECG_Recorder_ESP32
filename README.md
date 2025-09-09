# ECG_Recorder_ESP32
This repository contains code developed as part of my **engineering thesis** at AGH University of Science and Technology.   The project integrates the **BNO055 IMU** (accelerometer + gyroscope) and the **MAX30003 ECG** chip with an **ESP32**, sending sensor data over **Bluetooth Low Energy (BLE)** to an external device.  

This project uses the following open-source libraries:
- [Adafruit_BNO055](https://github.com/adafruit/Adafruit_BNO055)
- [ProtoCentral MAX30003](https://github.com/Protocentral/protocentral_max30003)
- [ESP32 BLE Arduino](https://github.com/nkolban/ESP32_BLE_Arduino)

Parts of the implementation are based on example sketches provided with these libraries.

This project consists of two parts:

BLE Sender (ESP32)

Reads ECG, accelerometer, and gyroscope data.

Sends the data over BLE using custom characteristics.

BLE Receiver (Python script)

Connects to the ESP32 BLE device.

Receives real-time notifications for ECG, accelerometer, and gyroscope.

Saves the collected data to ECG_results.txt in seven columns: ECG [mV], Acceleration [m²/s²] (x/y/z), Gyroscope [°/s] (x/y/z).
