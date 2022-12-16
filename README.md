# punch-speedometer-ble

Speedometer made by [Arduino Nano 33 BLE](https://www.switch-science.com/products/7667)  
Sends acceleration data to sever via BLE

## Setup

1. Install Arduino libraries
- [ArduinoBLE](https://www.arduino.cc/reference/en/libraries/arduinoble/)
- [SparkFunLSM9DS1](https://github.com/sparkfun/SparkFun_LSM9DS1_Arduino_Library)

2. Edit source code of the `SparkFunLSM9DS1` library (Don't forget to backup!)
- Comment out `initSPI()` in `SparkFunLSM9DS1.cpp`
- Change `initGyro()` and `initAccel()` to public in `SparkFunLSM9DS1.h`

[reference](https://github.com/arduino-libraries/Arduino_LSM9DS1/issues/2)

## Run

```
$ cd ble_central
$ node app.js
```

### app.js

A window will open to show real-time acceleration data

- ctrl + C: terminate process
- S:        saves data as csv