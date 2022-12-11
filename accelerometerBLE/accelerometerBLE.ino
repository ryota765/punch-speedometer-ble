#include <Arduino_LSM9DS1.h>
#include <ArduinoBLE.h>

#define SERVICE_UUID   "050332B0-7E59-4B01-B359-352779E72BD9"
#define CHAR_UUID      "050332B1-7E59-4B01-B359-352779E72BD9"
#define BLE_LOCAL_NAME "speedometer"

const int ledPin = LED_BUILTIN; // pin to use for the LED

BLEService bleService(SERVICE_UUID);
BLEStringCharacteristic accCharacteristic(CHAR_UUID, BLERead | BLEWrite | BLENotify, 16);

void setup() {
  Serial.begin(9600);
  while (!Serial);
  Serial.println("Started");

  // set LED pin to output mode
  pinMode(ledPin, OUTPUT);

  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");
    while (1);
  }

  Serial.println("BLE LED Peripheral");

  Serial.print("Accelerometer sample rate = ");
  Serial.print(IMU.accelerationSampleRate());
  Serial.println(" Hz");
  Serial.println();
  Serial.println("Acceleration in g's");
  Serial.println("X\tY\tZ");

  // begin initialization
  if (!BLE.begin()) {
    Serial.println("starting Bluetooth® Low Energy module failed!");

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

  digitalWrite(ledPin, HIGH); 
}

void loop() {
  float x, y, z;

  // listen for Bluetooth® Low Energy peripherals to connect:
  BLEDevice central = BLE.central();

  // if a central is connected to peripheral:
  if (central) {
    Serial.print("Connected to central: ");
    // print the central's MAC address:
    Serial.println(central.address());

    // while the central is still connected to peripheral:
    while (central.connected()) {
      if (IMU.accelerationAvailable()) {
        IMU.readAcceleration(x, y, z);
        String accConcatStr = String(x) + "," + String(y) + "," + String(z);
    
        Serial.println(accConcatStr);

        accCharacteristic.writeValue(accConcatStr);
      }
    }

    // when the central disconnects, print it out:
    Serial.print(F("Disconnected from central: "));
    Serial.println(central.address());
  }
}
