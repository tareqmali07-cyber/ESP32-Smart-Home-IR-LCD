#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <IRremote.hpp>

#define IR_RECEIVE_PIN   4  // IR Receiver Pin
#define RELAY_PIN       27  // Channel 1: Relay
#define RED_LED_PIN     12  // Channel 2: RED LED
#define YELLOW_LED_PIN 13  // Channel 3: YELLOW LED 
#define GREEN_LED_PIN  14  // Channel 4: GREEN LED
#define BUZZER_PIN      26  // The buzzer for sound alert

LiquidCrystal_I2C lcd(0x27, 16, 2);

#define HEX_BTN_1   0x45
#define HEX_BTN_2   0x46
#define HEX_BTN_3   0x47
#define HEX_BTN_4   0x44
#define HEX_BTN_5   0x19

bool relayState = false;
bool redState  = false;
bool yellowState  = false;
bool greenState  = false;

// Beep sound activation function
void triggerBeep() {
  digitalWrite(BUZZER_PIN, HIGH);
  delay(80);
  digitalWrite(BUZZER_PIN, LOW);
}

// Screen refresh function
void updateLCD(String title, String status){
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("ESP32 Smart Home");
  lcd.setCursor(0, 1);
  lcd.print(title + ": " + status);
}

void setup() {

  Serial.begin(115200);

  // Initialize output pins
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(YELLOW_LED_PIN, OUTPUT);
  pinMode(GREEN_LED_PIN,OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  // Set all outputs to the off position at the start
  digitalWrite(RELAY_PIN, LOW);
  digitalWrite(RED_LED_PIN, LOW);
  digitalWrite(YELLOW_LED_PIN,LOW);
  digitalWrite(GREEN_LED_PIN, LOW);

  // Configure the I2C screen on GPIO 21 (SDA) and GPIO 22 (SCL)
  Wire.begin(21, 22);
  lcd.init();
  lcd.backlight();

  // Welcome screen
  lcd.setCursor(0, 0);
  lcd.print("  ESP32 System  ");
  lcd.setCursor(0, 1);
  lcd.print(" Ready for IR.. ");
  delay(2000);
  lcd.clear();
  lcd.print("System Active");

  // IR reception has begun
  IrReceiver.begin(IR_RECEIVE_PIN, DISABLE_LED_FEEDBACK);
  Serial.println("ESP32 System Ready. Waiting for IR Signals...");
}

void loop() {
  
  if (IrReceiver.decode()) {

    // Formatting and ensuring the receipt of a valid code
    if(IrReceiver.decodedIRData.decodedRawData != 0){
      uint32_t receivedCode = IrReceiver.decodedIRData.command;

      Serial.print("Received Hex Command: 0x");
      Serial.println(receivedCode, HEX);

      // --- Channel 1: Relay Control (Button 1) ---
      if(receivedCode == HEX_BTN_1){
        relayState = !relayState;
        digitalWrite(RELAY_PIN, relayState ? HIGH : LOW);
        triggerBeep();
        updateLCD("Relay Ch1", relayState ? "ON " : "OFF");
      }
      
      // --- Channel 2: LED 1 Control (Button 2) --- 
      else if(receivedCode == HEX_BTN_2){
        redState = !redState;
        digitalWrite(RED_LED_PIN, redState ? HIGH : LOW);
        triggerBeep();
        updateLCD("RED Ch2", redState ? "ON " : "OFF");
      }

      // --- Channel 3: LED 2 Control (Button 3) ---
      else if(receivedCode == HEX_BTN_3){
        yellowState = !yellowState;
        digitalWrite(YELLOW_LED_PIN, yellowState ? HIGH : LOW);
        triggerBeep();
        updateLCD("YELLOW Ch3", yellowState ? "ON " : "OFF");
      }

      // --- Channel 4: LED 3 Control (Button 4) ---
      else if(receivedCode == HEX_BTN_4){
        greenState = !greenState;
        digitalWrite(GREEN_LED_PIN, greenState ? HIGH : LOW);
        triggerBeep();
        updateLCD("GREEN Ch4", greenState ? "ON " : "OFF");
      }
      else if(receivedCode == HEX_BTN_5){
        bool newState = !relayState;
        relayState = redState = yellowState = greenState = newState;

        digitalWrite(RELAY_PIN, relayState ? HIGH : LOW);
        digitalWrite(RED_LED_PIN, redState ? HIGH : LOW);
        digitalWrite(YELLOW_LED_PIN, yellowState ? HIGH : LOW);
        digitalWrite(GREEN_LED_PIN, greenState ? HIGH : LOW);

        triggerBeep();
        updateLCD("All Channels", newState ? "ON " : "OFF");
      }
    }
    IrReceiver.resume();
  }
}