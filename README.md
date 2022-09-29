# ArduinoYun-IoT
IoT project featuring Arduino Yùn for "Networking Technologies and Protocols" course at Univerisity of Modena and Reggio Emilia ("Enzo Ferrari" Eng. Dep.)


## Electronic Model

This project is built on the Arduino Yùn board. The system includes a series of sensors
and actuators, each of which execute a specific task. In particular, the system implements:

• 2x 3 mm Green LED

• 1x 3 mm Red LED

• 4x 3 mm Yellow LED

• 4x HC-SR04 Ultrasonic Sensors

• 2x KY-008 Laser Modules

• 1x Piezo-Buzzers

• 2x Photo-resistors

• 8x 220 Ω resistors

• 1x 100 Ω resistors

## System behavior

The whole system behavior can be split into three semi-autonomous sub-systems:

### 1. Level Sensor Sub-system. 

It consists in an HC-SR04 Ultrasonic sensor that
continuously measures the distance from the raw material in the silos, thus knowing
whether it is nearly empty or not. In the former condition, Yellow LEDs on the silos
and on its respective deposit will light up.

### 2. Laser Sub-system.

It consists in a KY-008 Laser module pointing a photo-resistor,
that is connected to a voltage divider and to a YUN’ analog pin. Doing so, the system
is able to detect whenever the laser beam is interrupted or not and, if it is, the Green
LED on the silos will light up.

### 3. Alarm Sub-system.

It consists in an HC-SR04 Ultrasonic sensor that continuously
measures the distance between the silos and the excavator. If the latter is too nearbyto the wrong silos, the sensor detects it and it will activate a Red LED and a Piezo-Buzzer to warn the operator.
