# POWERSTEP01 test utility
With this software you can talk to yout powerstep01 device via
spidev.

## Usage:
./powerstep <-D>[-sdi]
  -D --device   device to use (default /dev/spidev1.1)
  -s --speed    max speed (Hz)
  -d --delay    delay (usec)
  -i --ini      ini file
  
 With the ini file you can pass default settings to the device.

In the prompt, type help for a list of commands and
help <command> for details of the specific command.

## Build
Just type make in the powerstep folder

## Licence
This software is under GNU GPLv2



