"use strict";

const nodeplotlib = require("nodeplotlib");
const noble = require("@abandonware/noble");
const rxjs = require("rxjs");

const serviceUuid = `050332b07e594b01b359352779e72bd9`;
const localName = `speedometer`;

const dataArray = [];

const appendDataArray = (accConcatStr) => {
  const accArray = accConcatStr.split(",").map(parseFloat);
  dataArray.push(accArray);
};

const buildPlot = () => {
  const dataArrayTransposed = dataArray[0].map((col, i) =>
    dataArray.map((row) => row[i])
  );

  const plotData = [
    {
      x: dataArrayTransposed[3],
      y: dataArrayTransposed[0],
      type: "scatter",
      name: "x",
    },
    {
      x: dataArrayTransposed[3],
      y: dataArrayTransposed[1],
      type: "scatter",
      name: "y",
    },
    {
      x: dataArrayTransposed[3],
      y: dataArrayTransposed[2],
      type: "scatter",
      name: "z",
    },
  ];
  return plotData;
};

const stream = rxjs.interval(100).pipe(rxjs.map(buildPlot));

const accessCharacter = (character) => {
  character.notify(true, (err) => {
    if (err) {
      console.log("listen notif error", err);
    } else {
      console.log("listen notif");
    }
    nodeplotlib.plot(stream);
  });
  character.on("data", (data, isNotification) => {
    const dataConcatStr = data.toString("utf-8");
    appendDataArray(dataConcatStr);
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
