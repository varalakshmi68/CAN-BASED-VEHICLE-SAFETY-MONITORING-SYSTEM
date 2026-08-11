# CAN-Based Vehicle Safety & Monitoring System
A multi-node automotive safety and monitoring system developed using the LPC2129 microcontroller and CAN communication.

The system monitors engine temperature, controls vehicle indicators, detects obstacles during reverse operation, and provides safety alerts through communication between three CAN nodes.

## 1. Project Overview

This project uses three LPC2129-based CAN nodes:

1. Main Node
2. Indicator Node
3. Reverse Node

The Main Node acts as the central controller. It monitors engine temperature using the DS18B20 sensor, handles vehicle mode and indicator switches, displays information on an LCD, and communicates with the other nodes through CAN.

The Indicator Node receives indicator commands from the Main Node and controls the indicator LEDs.

The Reverse Node measures obstacle distance using an ultrasonic sensor and sends the distance and alert information to the Main Node through CAN communication.

## 2.Main Features

- Engine temperature monitoring using DS18B20
- Ultrasonic obstacle detection during reverse mode
- Left and right indicator control
- CAN-based communication between three nodes
- Forward and Reverse driving modes
- LCD-based system status display
- Reverse obstacle warning using buzzer
- Custom LCD indicator symbols using CGRAM
- External interrupt-based switch control
- Active-low indicator LED control
- Distance averaging using three ultrasonic readings
- Sensor failure and timeout handling


## 3.System Architecture
,
                    +----------------------+
                    |      MAIN NODE       |
                    |----------------------|
                    | LPC21xx              |
                    | LCD                  |
                    | DS18B20              |
                    | Buzzer               |
                    | Mode Switch          |
                    | Indicator Switches   |
                    +----------+-----------+
                               |
                               | CAN BUS
                 +-------------+-------------+
                 |                           |
                 v                           v
       +-------------------+       +-------------------+
       |   REVERSE NODE    |       |  INDICATOR NODE   |
       |-------------------|       |-------------------|
       | LPC21xx           |       | LPC21xx           |
       | Ultrasonic Sensor |       | Indicator LEDs    |
       | Distance Measure  |       | Left / Right      |
       +-------------------+       +-------------------+ 

Communication Flow
Main Node
   |
   | CAN ID 0x102
   | Vehicle Mode
   v
Reverse Node
   |
   | CAN ID 0x201
   | Distance + Alert
   v
Main Node


Main Node
   |
   | CAN ID 0x101
   | Indicator Command
   v
Indicator Node

4. Node Description
4.1 Main Node
   The Main Node is the primary monitoring and display node.

It performs the following functions:

Reads engine temperature from the DS18B20 sensor.
Detects Forward/Reverse mode using an external interrupt switch.
Detects Left/Right indicator switch operations.
Sends indicator commands through CAN.
Sends vehicle mode information through CAN.
Receives obstacle distance from the Reverse Node.
Displays temperature, indicator status, mode and distance on the LCD.
Activates the buzzer when the obstacle distance is less than 20 cm.
Main Node CAN Messages
| CAN ID  | Direction        | Data              | Purpose                                    |
| ------- | ---------------- | ----------------- | ------------------------------------------ |
| `0x101` | Main → Indicator | Indicator command | Controls Left/Right/OFF indicators         |
| `0x102` | Main → Reverse   | Mode              | Sends Forward/Reverse mode                 |
| `0x201` | Reverse → Main   | Distance + Alert  | Sends ultrasonic distance and alert status |

4.2 Reverse Node

The Reverse Node is responsible for obstacle detection.

It performs the following functions:

Receives vehicle mode information from the Main Node.
Activates ultrasonic measurement only in Reverse mode.
Measures obstacle distance using the ultrasonic sensor.
Takes three distance readings and calculates their average.
Generates an alert when the measured distance is less than 20 cm.
Sends distance and alert information to the Main Node through CAN.
Vehicle Modes:
| Value | Mode    |
| ----- | ------- |
| `0`   | Forward |
| `1`   | Reverse |


4.3 Indicator Node

The Indicator Node controls the vehicle indicator LEDs.

It performs the following functions:

Receives indicator commands from the Main Node through CAN.
Controls the left indicator LEDs.
Controls the right indicator LEDs.
Turns all indicator LEDs OFF when the command is S.
Indicator commands:
'L' → Left Indicator
'R' → Right Indicator
'S' → Indicators OFF

5.Sensor Details
DS18B20 Temperature Sensor

The DS18B20 is connected to the Main Node and communicates using the 1-Wire protocol.

The sensor is used to:

Start temperature conversion
Read the temperature value
Display engine temperature on the LCD.

Ultrasonic Sensor

The ultrasonic sensor is connected to the Reverse Node.

The Reverse Node:

Sends a trigger pulse.
Waits for the echo signal.
Measures the echo duration using Timer0.
Converts the measured time into distance.
Averages three distance measurements.
Sends the result to the Main Node through CAN.

6.Hardware
Microcontroller:
LPC21xx / ARM7-based microcontroller
Sensors:
DS18B20 temperature sensor
Ultrasonic distance sensor
Output Devices:
16x2/20x4 LCD
Indicator LEDs
Buzzer
Communication:
CAN Bus
Input Devices:
Mode switch
Left indicator switch
Right indicator switch

7.Development Tools
Keil µVision
Proteus
Embedded C
LPC21xx ARM7 microcontroller
CAN communication






