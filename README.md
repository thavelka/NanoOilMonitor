# NanoOilMonitor
Sketch to monitor oil temp and pressure and write to 128x64 OLED display.  
Also includes STL files for display/button housing and a board housing for the arduino. 

https://github.com/thavelka/NanoOilMonitor/assets/8907132/da23afdf-3d59-4e5f-9234-c822fb60fd93

# Applications
I am using this in a 2016 Mazda MX-5. You can use this in any vehicle with provisions for oil sensors. You can purchase a [sandwich plate](https://www.amazon.com/GlowShift-Filter-Sandwich-Adapter-Thread/dp/B007SXGAZQ) for your vehicle if you do not have ports you can tap into. I am using a sandwich plate that was included with my oil cooler.

Recommended temperatures and pressures may be different for your vehicle. Check your factory service manual for the correct values and update the constants in the sketch.

# Components
* [1.3 in SPI OLED](https://www.microcenter.com/product/643965/inland-iic-spi-13-128x64-oled-v20-graphic-display-module-for-arduino-uno-r3) (~$8 from MC or Amazon)
* [Arduino Nano](https://www.microcenter.com/product/615097/inland-nano-development-board-arduino-compatible) ($5-10 from MC or Amazon)
* [NTC Temp Sensor](https://www.mlperformanceusa.com/products/canchecked-cc22902-ntc-temperature-sensor-1-8-npt-thread-for-oil-water) ($30, get a good one)
* [5v pressure Sensor](https://www.amazon.com/Universal-Pressure-Transducer-Sender-Connector/dp/B09VRDM71G) ($16)
* [Momentary push button](https://www.amazon.com/Cylewet-Momentary-Button-Switch-CYT1078/dp/B0752RMB7Q) (for mode select)
* [3 conductor cable for sensors](https://www.amazon.com/gp/product/B0B7L1TQ33) ($15)
* 8 conductor cable for display and button (ethernet cable works)
* 5v source (e.g. [Dashcam hardwire kit](https://www.amazon.com/Hardwire-Dashcam-Camera-Charger-%EF%BC%8813ft%EF%BC%89/dp/B07YBDMD8V) or buck converter, L7805, etc.)

Total cost: $80-100

# Hookup
```
5v source: VIN  
Ground: GND  

Temp sensor positive: A4  
Temp sensor output: A6  
Temp sensor ground: GND  

Pressure sensor positive: A5  
Pressure sensor output: A7  
Pressure sensor ground: GND  

Momentary button: A3, GND  

Display VCC: 5V  
Display GND: GND  
Display MOSI: D11  
Display CLK: D13  
Display CS: A0  
Display DC: A1  
Display RES: A2
```
Run the ethernet cable through the back of the display housing and solder to the display.  
Install the button in the back of the display housing. Solder one leg to the remaining wire on the ethernet cable and the other leg to the ground wire in the ethernet cable.

# Usage
Press button to cycle mode
#### Modes
1. Standby (default) - Display for 10 seconds then go dark unless alert thresholds exceeded.
2. Unit - Display temp in degrees F and pressure in PSI
3. Idiot light - Display temp ranges (COLD, COOL, OK, HOT), pressure ranges (ALERT, LOW, OK, HIGH).

# Improvements
* You could compact this into just the display housing if you used an ATTiny and moved the voltage regulation outside the unit. You would want to use an I2C display instead of SPI.
* I used screw terminal blocks for this prototype since I had them, but I would recommend JST connectors or soldering instead for motorsports usage.

# Photos
![PXL_20231220_221435072](https://github.com/thavelka/NanoOilMonitor/assets/8907132/9d96a823-0542-44d6-9151-d9b6e78d09e3)

![PXL_20231220_041108299](https://github.com/thavelka/NanoOilMonitor/assets/8907132/aa5d515f-0b29-4a54-a31a-ff45861646f5)

![PXL_20231219_033741559](https://github.com/thavelka/NanoOilMonitor/assets/8907132/aa55d550-5427-4ae7-84f1-3d83e92c2a1c)

# Credit
* Orignal display housing: https://www.thingiverse.com/thing:4680559
* Display lib: https://github.com/olikraus/u8g2
