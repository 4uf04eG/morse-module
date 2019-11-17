#!/bin/bash
sudo make install &&
sudo modprobe -r morse-module &&
sudo modprobe morse-module &&
echo "sos" > /proc/morse
