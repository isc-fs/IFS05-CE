ZEVA 12-CELL Battery Management Module
  Ian Hooper, August 2021

This archive contains all the files needed to reproduce the ZEVA 12-cell Battery Management Modules. Files are provided under the open source MIT license, found at the end of this file. The short version is that you may use these files for any purpose, free of charge, but they don’t come with any warranty or support.

PCB and schematic (.sch) files can be opened in Autodesk Eagle. If you don’t need to modify the design, the Gerber files in the PCB Assembly folder can be sent as-is to a PCB manufacturer to get boards made. Files for PCB assembly are also included, such as a bill of materials, centroid files (for pick-and-place machines), and example photo for reference.

The firmware is written is C and can be compiled using AVR-GCC, normally using an IDE such as AVR / Microchip Studio. No claims are made about the quality or readability of the code. A precompiled HEX file is also supplied, which can be loaded onto boards as-is if standard firmware is suitable.

You will need an AVRISP type programming device for transferring firmware to the board, such as AVRISP MK2, STK500 or USBASP. The programming port on the PCB is a row of 5 holes near the microcontroller, with pin order GND, MOSI, MISO, SCK, RESET (with GND end marked). It is different to the usual 3x2 or 5x2 pin port used by AVRISP programmers, so you will need to make an adapter using 5 wires and some 0.1” pin header. Normally the 5x1 pin header can be pressed against the PCB holes during programming (avoids the need to add any connector to the board). The board will need to be powered during programming via 12V supplied to a CAN bus port.

AVR microcontrollers use “fuses” for setting things like the clock speed and brownout detection. The required fuse settings are listed near the top of the C code file.

Housings can be 3D printed from the two STL model files. The two halves are fastened together, with the circuit board in between, using two M3x10 countersunk machine screws. The mating connectors for CAN bus ports are Molex 39500-0005 and the cell connector is Molex 39500-0013.

Once assembled and programmed, the BMS module should be ready to use. Please review the user manual for further information about installation and operation.

----------

LICENSE (MIT Open Source):

Copyright (c) 2021 Ian Hooper, ZEVA

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.