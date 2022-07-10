# The Lakeview Project
ESP32 solar powered weather station. More to come soon!

## Config
### Select Features
Edit the [`CMakeLists.txt`](CMakeLists.txt) file to select the temperature scale and other optional features.
The project version will be used to check if a newer version is available during the OTA update process.
```
# OPTIONAL: Set the temperature scale (C | F | K)
set(SENSORS_SCALE "C")

# OPTIONAL: Enable Over The Air (OTA) updates (ON | OFF)
set(OTA_UPDATES ON)

# OPTIONAL: Set the project version
set(PROJECT_VER "1.0.0.0")
```

## Setup
### Espressif SDK
```
git clone -b v4.4 --recursive https://github.com/espressif/esp-idf.git esp-idf-v4.4
cd esp-idf-v4.4
bash install.sh
source export.sh
```

### Lakeview
```
cd $IDF_PATH/../
git clone --recursive https://github.com/ma-lwa-re/Lakeview.git
cd lakeview
idf.py set-target esp32s3
export ESPPORT=/dev/cu.usbserial-0001
idf.py build flash
```

### Wi-Fi
The wifi credentials were previously hardcoded in the [`wifi.h`](main/wifi.h) header, but are now directly written in a standalone partition on the ESP32.

It has the advantage that no secrets are stored in the code section anymore, and thus allows the same code to run on multiple devices, over-the-air (OTA) updates, or modification of wifi credentials without having to reflash the device.

```
cp wifi.csv $IDF_PATH/components/nvs_flash/nvs_partition_generator
cd $IDF_PATH/components/nvs_flash/nvs_partition_generator
# Edit the wifi.csv and replace the DEFAULT_SSID and DEFAULT_PASSWORD strings
python3 nvs_partition_gen.py generate wifi.csv wifi.bin 0x3000
esptool.py -p $ESPPORT write_flash 0x340000 wifi.bin
```

### Code Signing
The integrity of the application can be secure and checked using an RSA signature scheme. The binary is signed after compilation with the private key that can be generated with `espsecure.py` or `openssl`, and the corresponding public key is embedded into the binary for verification.

```
espsecure.py generate_signing_key --version 2 secure_boot_signing_key.pem
```

The bootloader will be compiled with code to verify that an app is signed before booting it. In addition, the signature will be also proofed before updating the firmware and adds significant security against network-based attacks by preventing spoofing of OTA updates.

### Octal SPI Flash
If you're using a chip version that uses an Octal SPI interface to connect Flash/PSRAM, like the ESP32-S3-WROOM-2, you need to enable its support using the command below.

```
idf.py menuconfig
Serial flasher config ---> Enable Octal Flash
```

## Console Output
```
sudo cu -l $ESPPORT -s 115200
```

## Project
```
lakeview
├── CMakeLists.txt
├── LICENSE
├── README.md
├── lib
│   └── ds18b20
│       ├── CMakeLists.txt
│       ├── LICENSE
│       ├── README.md
│       ├── component.mk
│       ├── ds18b20.c
│       ├── ds18b20.h
│       └── main.c
├── main
│   ├── CMakeLists.txt
│   ├── backend.c
│   ├── backend.h
│   ├── main.c
│   ├── ota.c
│   ├── ota.h
│   ├── sensors.c
│   ├── sensors.h
│   ├── wifi.c
│   └── wifi.h
├── partitions.csv
├── sdkconfig
├── sdkconfig.defaults
└── wifi.csv
```

## TODO
- [x] DS18B20 water sensor
- [ ] SHT30 I2C air sensor
- [ ] CO2 sensor
- [ ] Solar panel
- [x] OTA updates

## Feedback & Issues

Issues and feature requests should be raised on GitHub using

- [`https://github.com/ma-lwa-re/lakeview/issues/new`](https://github.com/ma-lwa-re/lakeview/issues/new)
