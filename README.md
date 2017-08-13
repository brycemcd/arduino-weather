# Arduino Weather

> A silly app to measure the weather and other environmental conditions

<img src="images/20170730_153250.jpg" width=500 />

## Changelog

### 2017-08-13

14:20 Eastern: moved the whole thing outside again

### 2017-08-04

10:30pm Eastern: swap out 22K resistors for 33K resistors

### 2017-08-02

At around 9pm Eastern, I'm swapping out all the resistors for 22K

### 2017-08-01

At around 11pm Eastern, I noticed that the sensors were consistently
reporting values of 4095 which is their error value. I reset the system
and also noticed that I was powering the gas sensors with 3.3V instead
of the 5V required per the data sheet.

I swapped out the resistor on the MQ-135 to use th  last 10k resistor I
had and plugged the gas sensor breadboard into a 5V supply.

### 2017-07-30

Release 0.0.3

Adds gas sensors with 4.7k resistors
