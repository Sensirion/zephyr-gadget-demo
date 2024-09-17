# Zephyr Demoproject

## BLE Gadget

This is the Code for reading out an SHT4x and an SCD41 over I2C. And advertise the data afterwards to make it visible 
on Sensirion's MyAmbience app.

This code should work on every board that has Bluetooth support in Zephyr.

## BLE Display

This is basically the same as the BLE Gadget but additionally with a Display.

This code only works with the TTGO T-Display v1.1

The integartion with the TTGO T-Display was done based on the work of
Mike Szczys. The tutorial can be found [here](https://jumptuck.com/blog/2023-03-30-using-ttgo-t-display-with-zephyr/)
