# Milk Spillover Prevention System

## Overview
An IoT-based system that prevents milk from boiling over using an ESP32-S3, VL53L0X ToF sensor, and SG90 servo motor.

## Features
- Detects rising milk level
- Automatically reduces stove flame
- Real-time monitoring
- Cost-effective solution

## Hardware Used
- ESP32-S3 T-Display AMOLED
- VL53L0X ToF Sensor
- SG90 Servo Motor
- Power Supply

## Working Principle
The VL53L0X sensor continuously measures the distance between the sensor and milk surface. When the distance decreases below a threshold, the ESP32 triggers the servo motor to rotate the gas knob and reduce the flame.


## Future Improvements
- Mobile notifications
- AI-based prediction
- Voice alerts
