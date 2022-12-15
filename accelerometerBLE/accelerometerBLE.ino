#include <ArduinoBLE.h>
#include <SPI.h>
#include <Wire.h>
#include <SparkFunLSM9DS1.h>

#define SERVICE_UUID   "050332B0-7E59-4B01-B359-352779E72BD9"
#define CHAR_UUID      "050332B1-7E59-4B01-B359-352779E72BD9"
#define BLE_LOCAL_NAME "speedometer"

#define LSM9DS1_M   0x1E // Would be 0x1C if SDO_M is LOW
#define LSM9DS1_AG  0x6B // Would be 0x6A if SDO_AG is LOW

const int ledPin = LED_BUILTIN; // pin to use for the LED

// Temporary workaround
// This differs by imu.settings.accel.scale value
const int accCoefficient = 4;

BLEService bleService(SERVICE_UUID);
BLEStringCharacteristic accCharacteristic(CHAR_UUID, BLERead | BLENotify, 24);

LSM9DS1 imu;

void setup() {
  // set LED pin to output mode
  pinMode(ledPin, OUTPUT);

  // Ref: https://github.com/arduino-libraries/Arduino_LSM9DS1/issues/2
  Wire1.begin();
  Wire1.setClock(400000);

  if (imu.begin(LSM9DS1_AG, LSM9DS1_M, Wire1) == false) {
    delay(1000);
  }
  imu.settings.accel.scale = 8; // 16g (max)
  imu.settings.accel.sampleRate = 6; // 952 Hz (max)
  imu.initAccel();

  // begin initialization
  if (!BLE.begin()) {
    while (1);
  }

  // set advertised local name and service UUID:
  BLE.setLocalName(BLE_LOCAL_NAME);
  BLE.setAdvertisedService(bleService);

  // add the characteristic to the service
  bleService.addCharacteristic(accCharacteristic);

  // add service
  BLE.addService(bleService);

  // set the initial value for the characeristic:
  accCharacteristic.writeValue("init");

  // start advertising
  BLE.advertise();
}

void loop() {
  // listen for Bluetooth® Low Energy peripherals to connect:
  BLEDevice central = BLE.central();

  // if a central is connected to peripheral:
  if (central) {
    digitalWrite(ledPin, HIGH); 

    // while the central is still connected to peripheral:
    while (central.connected()) {
      if (imu.accelAvailable()) {
        imu.readAccel();
        int elapsedTime = millis();
        
        String accConcatStr = String(imu.calcAccel(imu.ax) * accCoefficient) + "," +
                              String(imu.calcAccel(imu.ay) * accCoefficient) + "," +
                              String(imu.calcAccel(imu.az) * accCoefficient) + "," +
                              String(elapsedTime);

        accCharacteristic.writeValue(accConcatStr);
      }
    }
    digitalWrite(ledPin, LOW); 
  }
}
