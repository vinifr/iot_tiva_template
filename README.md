# IoT tiva template

This application:
- toggles PA1 depending on IP status; and
- sends out a UDP package every five seconds containg "Hello World!"

Compile:
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
