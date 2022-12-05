'use strict';

const noble = require('@abandonware/noble');
const serviceuuid = `050332b17e594b01b359352779e72bd9`;
const localName = `speedometer`;

const accessChara = (chara) => {
    console.log('-----Start GATT Access-----')
    chara.notify(true, (err) => {
        if (err) {
          console.log('listen notif error', err)
        } else {
          console.log('listen notif')
        }
    });
    chara.on('data', (data, isNotif) => {
        const jsonStr = data.toString('utf-8');
        const jsonData = JSON.parse(jsonStr);
        console.log(jsonData);
    });
}


//discovered BLE device
const discovered = (peripheral) => {
    console.log(`BLE Device Found: ${peripheral.advertisement.localName}(${peripheral.uuid}) RSSI${peripheral.rssi}`);

    if(peripheral.advertisement.localName === localName){
        noble.stopScanning();
        console.log('device found');
        console.log(`service discover...`);

        peripheral.connect(error => {
            if (error) {
                console.log("connection error:", error)
            } else {
                console.log("device connected");
            }

            peripheral.discoverServices([],(error, services) => {
                if (error) {
                    console.log("discover service error", error)
                }
                console.log('discover service');               
                services.forEach(service => {
                    if(service.uuid === serviceuuid){
                        service.discoverCharacteristics([], (error, charas) => {
                            console.log('discover chara');
                            charas.forEach(chara => {
                                if(chara.uuid === serviceuuid){
                                    console.log("found chara: ", chara.uuid)
                                    accessChara(chara);
                                }
                            });
                        });
                    }
                });
            });
        });
    }
}

//BLE scan start
const scanStart = () => {
    noble.startScanning();
    noble.on('discover', discovered);
}

if(noble.state === 'poweredOn'){
    scanStart();
}else{
    noble.on('stateChange', scanStart);
}