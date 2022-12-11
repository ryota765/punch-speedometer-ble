"use strict";

const noble = require("@abandonware/noble");
const serviceUuid = `050332b07e594b01b359352779e72bd9`;
const localName = `speedometer`;

const parseAccConcatStr = (accConcatStr) => {
    const accArray = accConcatStr.split(',').map(parseFloat)
    console.log(accArray);
}

const accessCharacter = (character) => {
  character.notify(true, (err) => {
    if (err) {
      console.log("listen notif error", err);
    } else {
      console.log("listen notif");
    }
  });
  character.on("data", (data, isNotification) => {
    const accConcatStr = data.toString("utf-8");
    parseAccConcatStr(accConcatStr);
  });
};

//discovered BLE device
const discovered = (peripheral) => {
  console.log(
    `BLE Device Found: ${peripheral.advertisement.localName}(${peripheral.uuid})`
  );

  if (peripheral.advertisement.localName === localName) {
    noble.stopScanning();
    console.log("device found");

    peripheral.connect((error) => {
      if (error) {
        console.log("connection error:", error);
      } else {
        console.log("device connected");
      }

      // Services can not be discoveres also by iOS app
      peripheral.discoverServices([], (error, services) => {
        if (error) {
          console.log("discover service error", error);
        }
        console.log("discover service");
        services.forEach((service) => {
          console.log(service.uuid);
          if (service.uuid === serviceUuid) {
            service.discoverCharacteristics([], (error, characters) => {
              console.log("discover chara");
              characters.forEach((character) => {
                console.log("found character: ", character.uuid);
                accessCharacter(character);
              });
            });
          }
        });
      });
    });
  }
};

//BLE scan start
const scanStart = () => {
  noble.startScanning();
  noble.on("discover", discovered);
};

if (noble.state === "poweredOn") {
  scanStart();
} else {
  noble.on("stateChange", scanStart);
}
