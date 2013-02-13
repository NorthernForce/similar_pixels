#!/bin/bash
export PKG_CONFIG_PATH=/home/pi/OpenCV-2.4.3/release/unix-install/:${PKG_CONFIG_PATH}
export LD_LIBRARY_PATH=/home/pi/OpenCV-2.4.3/release/lib
g++ `pkg-config --cflags --libs opencv` targeting2.cpp -o targeting 
