# Getting started
## 0) Prerequisites
python
pip

```
pip install pyserial
```

## 1) Setting up the toolchain for _Mac OS_
```
echo "esp-idf/" >> .gitignore
git clone --recursive https://github.com/espressif/esp-idf.git
```
Now follow the instructions described in "esp-idf/docs/macos-setup.rst", it goes like this:

- Downloading binary toolchain

```
echo "esp/" >> .gitignore
mkdir esp
cd esp
curl https://dl.espressif.com/dl/xtensa-esp32-elf-osx-1.22.0-59.tar.gz | gzip -d | tar -xvf -
cd ..
```

- Setting up environemnt vars

```
export PATH=$PATH:$PWD/esp/xtensa-esp32-elf/bin
export IDF_PATH=$PWD/esp-idf
```

## 2) Configure USB connectivity to your ESP32

- Now plug in your buddy via USB cable

list USB devices

```
ioreg -p IOUSB
``` 
there should be line like: 
_CP2102 USB to UART Bridge Controller_ 

download & install driver for this controller 
http://www.silabs.com/products/mcu/pages/usbtouartbridgevcpdrivers.aspx

check the serial special device name by running 
```
ls /dev/tty.*
```

on my Mac it is _/dev/tty.SLAB_USBtoUART_

- remember it's name since it will be needed to flash ESP32

## 3) Flash first app

- Clone and configure example app

```
git clone https://github.com/espressif/esp-idf-template.git TEST01-exampleapp
cd TEST01-exampleapp
make menuconfig
```

goto _Serial flasher config  --->_

change _Default serial port_ to the name of your serial special device (/dev/tty.SLAB_USBtoUART)

save changes and flash your ESP32

```
make flash
```

## 4) now you can connect to your buddy :)

```
screen /dev/tty.SLAB_USBtoUART 115200
```

exit scree using (ctrl-a) + (k)