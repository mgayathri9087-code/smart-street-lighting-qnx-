# Smart Street Lighting with Predictive Control

## Team Infinity Sparks

**Hackathon:** State Level Hackathon  
**Platform:** QNX 8.0 + Raspberry Pi 4  
**Domain:** Smart City – Embedded Systems & IoT

### Team Members
- Ashwithaa B
- Gayathri M
- Kamaladevi SP

**College:** Prince Shri Venkateshwara Padmavathy Engineering College

---

## Project Overview

Smart Street Lighting with Predictive Control is a QNX-based embedded system designed to reduce unnecessary street-light energy consumption while improving road visibility.

The prototype divides the road into three lighting zones. An LDR determines whether it is day or night, while IR sensors detect vehicle movement.

When a vehicle enters one zone, the system switches ON the current zone and also pre-illuminates the next zone before the vehicle reaches it.

---

## Working Principle

### Day Mode

When sufficient ambient light is detected:

- Zone 1 LED - OFF
- Zone 2 LED - OFF
- Zone 3 LED - OFF

This avoids unnecessary energy consumption during daytime.

### Night Mode

At night, the IR sensors monitor vehicle movement.

#### Vehicle detected in Zone 1

- Zone 1 - ON
- Zone 2 - ON (Predicted)
- Zone 3 - OFF

#### Vehicle detected in Zone 2

- Zone 1 - OFF
- Zone 2 - ON
- Zone 3 - ON (Predicted)

#### Vehicle detected in Zone 3

- Zone 1 - OFF
- Zone 2 - OFF
- Zone 3 - ON

#### No vehicle detected

All three LEDs remain OFF in the current prototype.

---

## Predictive Control

Instead of waiting for the vehicle to enter the next zone, the system pre-illuminates the upcoming zone.

Example:

Zone 1 Detection  
→ Zone 1 Light ON  
→ Zone 2 Light ON in advance  
→ Vehicle moves towards Zone 2

This provides lighting ahead of the moving vehicle while avoiding unnecessary continuous illumination.

---

## Hardware Used

- Raspberry Pi 4 Model B
- LDR sensor module
- 3 IR obstacle sensors
- 3 LEDs
- 220-ohm resistors
- Breadboard
- Jumper wires
- Power supply

---

## GPIO Connections

| Component | Raspberry Pi GPIO |
|---|---|
| LDR | GPIO26 |
| IR Sensor - Zone 1 | GPIO5 |
| IR Sensor - Zone 2 | GPIO6 |
| IR Sensor - Zone 3 | GPIO13 |
| LED - Zone 1 | GPIO23 |
| LED - Zone 2 | GPIO24 |
| LED - Zone 3 | GPIO25 |

---

## Software

- QNX Software Development Platform 8.0
- QNX Momentics IDE
- C Programming
- Raspberry Pi GPIO interface

---

## QNX Integration

The application runs on QNX 8.0 on a Raspberry Pi 4.

The system reads real GPIO sensor inputs and controls the LED outputs using the QNX Raspberry Pi GPIO interface.

The GPIO client library communicates with the GPIO resource manager using QNX message-passing mechanisms.

---

## Sensor Stability

IR sensors may occasionally produce fluctuating readings.

A software stability filter is therefore used before changing the lighting state. The detected condition must remain stable across consecutive sensor readings before the LED state is changed.

This prevents unwanted rapid switching of the street lights.

---

## Project Flow

LDR + IR Sensors  
↓  
QNX GPIO Input  
↓  
Day / Night Decision  
↓  
Vehicle Zone Detection  
↓  
Sensor Stability Filter  
↓  
Predictive Control Logic  
↓  
GPIO Output  
↓  
Zone 1 / Zone 2 / Zone 3 LEDs

---

## Future Scope

The prototype can be extended with:

- Additional road zones
- Industrial LED street-light drivers
- Wireless communication between zone controllers
- Traffic-density monitoring
- Cloud-based monitoring
- Emergency vehicle prioritization
- Adaptive brightness control
- Larger smart-city road networks

---

## Conclusion

The prototype demonstrates how QNX, Raspberry Pi and real-world sensors can be integrated to create an intelligent street-lighting system.

The system illuminates the road ahead of a detected vehicle while keeping unnecessary lights switched OFF.

**Light the road ahead, only when it is needed.**
