# CIRCULOOS Shopfloor Demo

The current repo is acollection of hardware and software compoment ment to simulate the jurney of the measurements of a shopfloor to a local RAMP IIoT instance (on a RasberyPi) and on the centalazed CIRCULOOS Data platform.
The demo consist of a full end to end solution ie from the sensor to the communication inside the shopfloor to the local RAMP IIoT instance and to the centalazed CIRCULOOS Data platform.


## Measurements
The following measurements (from various sensors) are included:
- Air temperature (BME680)
- Air hummidity (BME680)
- Air presure (BME680)
- Surface temperature (Pt100, k-type thermocuple)
- Inductive proximity sensor
- Weight
- Colour


## Shopfloor communication protocols 
The following communication protocols are incuded:
- CAN
- RS485 
- MQTT 
- I2C

# Sensor nodes 

The base embeded computational unit for each node is the module based on ESP32 by Olimex.

A list of the different node with each sensor can be foound on [system.md](./system.md)

## Diagram
![diagram](./diagram.png)








Copyright © 2023-2024 European Dynamics Luxembourg S.A.

Licensed under the EUPL, Version 1.2.
You may not use this work except in compliance with the License.
You may obtain a copy of the License at https://joinup.ec.europa.eu/collection/eupl/eupl-text-eupl-12 


Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.