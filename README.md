# IoT tiva template

This application:
- toggles PA1 depending on IP status; and
- sends out a UDP package every five seconds containg "Hello World!"

Compile: (This project uses gcc-arm-none-eabi compiler)
```
git submodule init
git submodule update
cd src
make
```

Flash (using ARM-USB-OCD-H from Olimex):
```
make flash
```
