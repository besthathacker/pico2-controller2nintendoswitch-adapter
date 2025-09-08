# This is **UNTESTED**
> [!Warning]
> This Is **NOT** For __pico 2, pico w, or pico 2w__

> [!Caution]
> This Might **NOT** Work. **USE AT YOUR OWN RISK**

> To Build From Source, run These commands In Order

```
sudo apt update
sudo apt install -y build-essential cmake gcc-arm-none-eabi libnewlib-arm-none-eabi
git clone https://github.com/raspberrypi/pico-sdk.git
git -C pico-sdk submodule update --init
echo "PICO_SDK_PATH=$GITHUB_WORKSPACE/pico-sdk" >> $GITHUB_ENV
export PICO_SDK_PATH=$PWD/pico-sdk
mkdir build
cd build
cmake .. \
  -DPICO_SDK_PATH=$GITHUB_WORKSPACE/pico-sdk \
  -DCMAKE_BUILD_TYPE=Release \
  -G Ninja
cmake --build buildnone
```
