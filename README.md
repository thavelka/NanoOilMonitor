# NanoOilMonitor
Sketch to monitor oil temp and pressure and write to 128x64 display

![PXL_20231220_041108299](https://github.com/thavelka/NanoOilMonitor/assets/8907132/9478a4eb-ad96-4311-bf8b-823497758c42)

# Components
* [1.3 in SPI OLED](https://www.microcenter.com/product/643965/inland-iic-spi-13-128x64-oled-v20-graphic-display-module-for-arduino-uno-r3) ($8)
* [Arduino Nano](https://www.microcenter.com/product/615097/inland-nano-development-board-arduino-compatible) ($5-10)
* [NTC Temp Sensor](https://www.mlperformanceusa.com/products/canchecked-cc22902-ntc-temperature-sensor-1-8-npt-thread-for-oil-water) ($30)
* [5v pressure transducer](https://www.amazon.com/Universal-Pressure-Transducer-Sender-Connector/dp/B09VRDM71G) ($16)
* Momentary push button
* 5v source (e.g. USB or voltage regulator)

# Hookup
```
5v source: VIN  
Ground: GND  

Temp sensor positive: A4  
Temp sensor value: A6  
Temp sensor ground: GND  

Pressure sensor positive: A5  
Pressure sensor value: A7  
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

![PXL_20231220_221435072](https://github.com/thavelka/NanoOilMonitor/assets/8907132/9d96a823-0542-44d6-9151-d9b6e78d09e3)
