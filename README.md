# ESP32 funlab
## 1) Setting up the toolchain for _Max OS_
```
echo "esp-idf/" >> .gitignore
git clone --recursive https://github.com/espressif/esp-idf.git
```
Now follow the instructions described in "esp-idf/docs/macos-setup.rst", it goes like this:
1. Downloading binary toolchain
```
echo "esp/" >> .gitignore
mkdir esp
cd esp
curl https://dl.espressif.com/dl/xtensa-esp32-elf-osx-1.22.0-59.tar.gz | gzip -d | tar -xvf -
cd ..
```
2. Setting up environemnt vars
```
export PATH=$PATH:$PWD/esp/xtensa-esp32-elf/bin
```
