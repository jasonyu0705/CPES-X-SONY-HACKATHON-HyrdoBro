// Sensor code :DD
#include <LiquidCrystal.h>

#include "CuteBuzzerSounds.h"


// Pin definitions
#define trigPin 1
#define echoPin 0
#define led 2
#define led2 10
#define motorPin 3 // Motor control pin (PWM)
#define BUZZER_PIN 13
// Initialize LCD: RS, E, D4, D5, D6, D7
LiquidCrystal lcd(12, 11, 8, 9, 6, 4);

void setup() {  // assigning pins to inputs and outputs
  Serial.begin(9600);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(led, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(motorPin, OUTPUT);
  cute.init(BUZZER_PIN);

  lcd.begin(16, 2);
  lcd.print("hello, world!"); // making sure lcd is working 
  delay(1000);
  lcd.clear();
}

// setting constants for timer
const float DIST_THRESHOLD     = 5.0;     //(in cm)
const unsigned long REMIND_DELAY   = 10000;  //(in ms)
const unsigned long BLINK_INTERVAL =  500;  // distance between blinks (in ms) 

bool            bottlePresent = false;
unsigned long   presentStart  = 0;
unsigned long   lastBlink     = 0;
bool            blinkState    = false;

// the WORK
void loop() {
  // Sending a pulse and measuring distnace be timing how long the pulse takes to come back 
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH, 30000L); // making distance measuremneents
  float distance = (duration > 0)? (duration / 2.0) / 29.1: -1;

  lcd.setCursor(0, 0);
  if (distance < 0) {
    lcd.print("invalid");// error checking the proximity sensor 
  } else {
    lcd.print("Dist:");// writing the distance to the lcd so you know what its detecting :)
    lcd.print(distance, 1);
    lcd.print("cm   ");
  }
// driving the blinking leds
  if (distance > 0 && distance < DIST_THRESHOLD) {
    if (!bottlePresent) {
      bottlePresent = true;
      presentStart  = millis();
      lastBlink     = millis();
      blinkState    = false;
    }

    unsigned long elapsed = millis() - presentStart;// time elapsed from the start 
    if (elapsed >= REMIND_DELAY) {
      if (millis() - lastBlink >= BLINK_INTERVAL) { //timing for blinking
        blinkState = !blinkState;
        lastBlink  = millis();
        // LEDs
        digitalWrite(led,  blinkState ? HIGH : LOW); // switching led states
        digitalWrite(led2, blinkState ? LOW  : HIGH);
        // message
        lcd.setCursor(0, 1);
        if (blinkState) {// flashing the drink water text on one blink
          lcd.print("Drink water    ");
          cute.play(S_SUPER_HAPPY);

        } else {
          lcd.print("Go take a walk ");// flashing text on other blink to alow for multiple lines of text (sychroonized with leds)
          cute.play(S_HAPPY);
        }
      }
    } else {
      digitalWrite(led,  HIGH);// if the bottle is senses turn on leds and display text  
      digitalWrite(led2, HIGH);
      lcd.setCursor(0, 1);
      lcd.print("Bottle placed   ");
    }

  } else {
    bottlePresent = false; // reset for the timer for when you drinking water 
    digitalWrite(led,  LOW);
    digitalWrite(led2, LOW);
    lcd.setCursor(0, 1);
    lcd.print("You're drinking     ");
  
  }

  delay(100); // give time for the code to run
}