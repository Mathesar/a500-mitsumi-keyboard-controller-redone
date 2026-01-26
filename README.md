# Mitsumi PCB A-500 redone

This is a remake ("redone") of the Amiga 500 Mitsumi keyboard conntroller.

![alt text](pictures\Mitsumi-A-500-REDONE.jpg)

The original Amiga 500 uses a MOS 6570 microcontroller that is no longer produced and getting exceedingly rare. They are also starting to fail, resulting the dreaded "blinking CAPS-LOCK" issue. Since there is no open-source / open-hardware replacement design available I decided to make my own.

This is a completely new "from scratch" design that uses a Microchip [PIC18F4520](https://www.microchip.com/en-us/product/pic18f4520) microcontroller. I chose this type as it is available in a DIP40 package so easy to solder by hand. It also gives this new redone keyboard controller a look very similar to the original controller. 

Lastly, what I like about the PIC18F4520 is that it is an 8bit microcontroller with modest capabilities not unlike the MOS 6570. It would bother me a bit to use a 50MHz+ 32bit ARM processor that is magnitudes faster than the Amiga's 68000 CPU just to scan a keyboard.

## Features

* Completely independent design
* Fully compatible with the original Mitsumi keyboard controller.
* Cost effective 2-layer circuit board.
* Through-hole design using currently available components for a retro look.
* Uses three 5mm x 2mm x 7mm LEDs instead of the unobtanium Commodore 15mm LED.
* Optionally, a 3D printed lightpipe can be used to give a cleaner LED look.
* Anti-ghosting algorithm.

## Molex ZIF connector

The original keyboard controller uses a Molex right angle ZIF connector to connect to the keyboard matrix membrane. This connector is very hard (impossible?) to get. I therefore used the straight version of this connector, the [Molex 39-53-2315](https://www.molex.com/en-us/products/part-detail/39532315). This connector is soldered on the edge of the circuitboard. This causes the entry point of the connector to sit lower than the original. This puts some strain on the flat cable. One should thus take care when assembling.

## LED brightness tuning

Modern LEDs are way brighter than the LEDs that Commodore used back in the days.
If the LEDs turn out too bright, they can be tuned by changing some resistor values.

### Drive LED

The drive LED can be tuned by simply changing resistor R12. Higher values lower the brightness.

### Power LED

The power LED is a bit special. One cannot simply change the value of R8. This is because of the power LED "DIM" function on later revisions of the Amiga 500. Dor this dim function to work correctly, R8 must always be 22 Ohms. To solve this, the circuit around Q1 has been added. This circuit is a voltage "multiplier" that makes the parallel connected LEDS look like a single LED with a voltage drop similar to the original series connected Commodore LED. To make the LEDs less bright, this voltage drop must be increased (so the voltage drop over R8 is decreased, hence current and brightness are also decreased). This can be done by increasing the value of R10.

## Firmware and programming

The firmware was developed using [MPLAB X ide v6.25](https://www.microchip.com/en-us/tools-resources/develop/mplab-x-ide).
The code is written in C for the [XC8 compiler](https://www.microchip.com/en-us/tools-resources/develop/mplab-xc-compilers/xc8).

These tools are freely available from [Microchip](https://www.microchip.com).

## Licenses

The hardware is licensed under [CERN OHL-S V2.0](https://cern-ohl.web.cern.ch/).

The firmware is licensed under [GNU GPL V3](https://www.gnu.org/licenses/gpl-3.0.en.html).

## Acknowledgements

The KiCAD keyboard symbol was originally done by [Scott Lawrence](https://github.com/BleuLlama/AmigaSchematics/tree/master/A500_kyb_Mitsumi).


