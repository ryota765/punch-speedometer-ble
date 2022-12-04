#include <Arduino_LSM9DS1.h>
#include <ArduinoBLE.h>

#define SERVICE_UUID   "050332B1-7E59-4B01-B359-352779E72BD9"
#define BLE_LOCAL_NAME "speedometer"

const int ledPin = LED_BUILTIN; // pin to use for the LED

BLEService bleService(SERVICE_UUID);
BLEFloatCharacteristic xAxisCharacteristic(SERVICE_UUID, BLERead | BLEWrite);
BLEFloatCharacteristic yAxisCharacteristic(SERVICE_UUID, BLERead | BLEWrite);
BLEFloatCharacteristic zAxisCharacteristic(SERVICE_UUID, BLERead | BLEWrite);

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
  bleService.addCharacteristic(xAxisCharacteristic);
  bleService.addCharacteristic(yAxisCharacteristic);
  bleService.addCharacteristic(zAxisCharacteristic);

  // add service
  BLE.addService(bleService);

  // set the initial value for the characeristic:
  xAxisCharacteristic.writeValue(0);
  yAxisCharacteristic.writeValue(0);
  zAxisCharacteristic.writeValue(0);

  // start advertising
  BLE.advertise();

  digitalWrite(ledPin, HIGH); 
}

void loop() {
  float x, y, z;

  if (IMU.accelerationAvailable()) {
    IMU.readAcceleration(x, y, z);

    Serial.print(x);
    Serial.print('\t');
    Serial.print(y);
    Serial.print('\t');
    Serial.println(z);

    xAxisCharacteristic.writeValue(x);
    yAxisCharacteristic.writeValue(y);
    zAxisCharacteristic.writeValue(z);
  }
}
