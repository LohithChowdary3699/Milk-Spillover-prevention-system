#define BLYNK_TEMPLATE_ID "TMPL3AYqsPTBh"
#define BLYNK_TEMPLATE_NAME "MilkBoilPrevention"
#define BLYNK_AUTH_TOKEN "gZZVnGlBFf_bcEAnMLL-5uqV1lRS6F96"

#include <Wire.h>
#include <VL53L0X.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <ESP32Servo.h>  // Added Servo Library
#include "rm67162.h"
#include "TFT_eSPI.h"
#include "NotoSansBold15.h"
#include "NotoSansBold36.h"  

// Blynk authentication and WiFi credentials
char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "Lohith";       
char pass[] = "Lohith__99";   

// Variables for controlling the motor and distance from the milk surface
TFT_eSPI tft = TFT_eSPI();
TFT_eSprite sprite = TFT_eSprite(&tft);

// Create VL53L0X object
VL53L0X sensor;
Servo myServo;  // Create Servo object

float distanceFromMilk;
int motorPin = 2;           // Pin for motor/servo control
int distanceThreshold = 280; // Distance threshold in mm
bool motorStatus = false;   // Motor status (ON/OFF)

BlynkTimer timer;  // Blynk timer for sending data

void setup() {
  // Initialize Serial and Blynk
  Serial.begin(115200);
  Blynk.begin(auth, ssid, pass);  // Connect to Blynk Cloud

  // Initialize motor (Servo) and display
  myServo.attach(motorPin);  // Attach servo to the defined pin
  myServo.write(90);  // Set servo to neutral position initially

  sprite.createSprite(536, 241);
  sprite.setSwapBytes(1);
  rm67162_init();  // AMOLED LCD initialization
  lcd_setRotation(1);

  // Initialize VL53L0X sensor
  Wire.begin(42, 41);  // SDA = GPIO42, SCL = GPIO41
  sensor.init();
  sensor.setTimeout(500);
  sensor.startContinuous();

  // Set up timer to send sensor data every second
  timer.setInterval(1000L, sendSensorData);
}

void sendSensorData() {
  // Read distance from VL53L0X sensor
  int distanceValue = sensor.readRangeContinuousMillimeters();

  // Send distance data to Blynk (Virtual Pin V1)
  distanceFromMilk = distanceValue;
  Blynk.virtualWrite(V1, distanceFromMilk);

  // Send motor status to Blynk (Virtual Pin V2)
  Blynk.virtualWrite(V2, motorStatus ? 1 : 0);

  // Update display with distance and motor status
  drawDisplay();
}

void drawDisplay() {
  // Clear the screen
  sprite.fillSprite(TFT_BLACK);

  // Display title
  sprite.setTextColor(TFT_WHITE, TFT_BLACK);
  sprite.loadFont(NotoSansBold15);
  sprite.drawString("Milk Boil Prevention System", 10, 0);
  sprite.unloadFont();

  // Display distance from milk surface
  sprite.loadFont(NotoSansBold36);
  sprite.setTextColor(TFT_GREEN, TFT_BLACK);
  sprite.drawString("Distance: " + String(distanceFromMilk) + "mm", 10, 50);
  sprite.unloadFont();

  // Display motor status (ON or OFF)
  sprite.loadFont(NotoSansBold15);
  sprite.setTextColor(TFT_RED, TFT_BLACK);
  sprite.drawString(motorStatus ? "Motor Status: ON" : "Motor Status: OFF", 10, 120);
  sprite.unloadFont();

  // Push the updated display to the screen
  lcd_PushColors(0, 0, 536, 240, (uint16_t*)sprite.getPointer());
}

void monitorDistance() {
  // Read distance sensor data
  int distanceValue = sensor.readRangeContinuousMillimeters();
  distanceFromMilk = distanceValue;
}

void controlMotor() {
  if (distanceFromMilk < distanceThreshold && !motorStatus) {
    motorStatus = true;
    myServo.write(0);  // Move servo to turn off the stove
    Blynk.logEvent("motor_on", "Servo moved to prevent boiling");
    Serial.println("Servo moved to 0° (OFF position)");
  } 
  else if (distanceFromMilk >= distanceThreshold && motorStatus) {
    motorStatus = false;
    myServo.write(90);  // Move servo back to neutral position
    Blynk.logEvent("motor_off", "Servo returned to neutral position");
    Serial.println("Servo moved to 90° (Neutral position)");
  }
}

void loop() {
  Blynk.run();   // Run Blynk client
  timer.run();   // Run timer
  
  monitorDistance();  // Monitor the current distance from the milk surface
  controlMotor();     // Control the motor based on the distance
}
