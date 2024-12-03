
# RAMP IIoT computer

## Components 
- RaspberryPi 5 8GB
- Power supply 
- Case with fun
- SD card
## Communication interfaces
- RS485 ([waveshare](https://grobotronics.com/waveshare-isolated-rs485-rs232-hat-spi-control.html))
- CAN bus ([waveshare](https://www.waveshare.com/product/2-ch-can-hat-plus.htm))


### Optimal
HDMI to micro


# Sensor Node #1 scale and colour  - RS485/Modubus
- ESP32 ([olimex](https://www.olimex.com/Products/IoT/ESP32/ESP32-EVB/open-source-hardware))
- Scale kit ([gravity](https://grobotronics.com/gravity-i2c-1kg-weight-sensor-kit-hx711.html))
- Color sensors ([AS7341](https://grobotronics.com/fermion-as7341-11-channel-visible-light-sensor.html))
- TTL to RS485 module ([module](https://grobotronics.com/ttl-to-rs485-module.html))

# Sensor Node #2 Environmental mqtt over wifi
- ESP32 ([olimex](https://www.olimex.com/Products/IoT/ESP32/ESP32-EVB/open-source-hardware))
- Temperature, RH, Barometric Pressure ([BME680](https://grobotronics.com/waveshare-environmental-sensor-bme680.html) )
- PT1000 ([module](https://grobotronics.com/gravity-analog-high-temperature-sensor-30350c.html))
- K-type thermocouple ([modeule](https://grobotronics.com/pimoroni-mcp9600-thermocouple-amplifier-breakout.html))


# Sensor Node #3 Temperature - CAN bus
- ESP32 ([olimex](https://www.olimex.com/Products/IoT/ESP32/ESP32-EVB/open-source-hardware))
- PT1000 ([module](https://grobotronics.com/gravity-analog-high-temperature-sensor-30350c.html))
- K-type thermocouple ([modeule](https://grobotronics.com/pimoroni-mcp9600-thermocouple-amplifier-breakout.html))
- Inductive Proximity ([module](https://grobotronics.com/inductive-proximity-sensor-12mm-lj12a3-4-z-ex.html))



# Sensor Node #4 - OPC UA Server over wifi
- ESP32 ([olimex](https://www.olimex.com/Products/IoT/ESP32/ESP32-EVB/open-source-hardware))
- Temperature, RH, Barometric Pressure ([BME680](https://grobotronics.com/waveshare-environmental-sensor-bme680.html) )
