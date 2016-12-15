# IoT tiva template

This application:
- A websocket client that receive data from heart monitor using websocket protocol
and plot a graph

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
