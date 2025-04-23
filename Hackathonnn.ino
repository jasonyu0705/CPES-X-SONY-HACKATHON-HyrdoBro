// Sensor code :DD
#include <LiquidCrystal.h>

#include "CuteBuzzerSounds.h"


// ----Pin definitions--------------------------------------

// Ultra Sonic Sensor
#define trigPin 1
#define echoPin 0

//Leds
#define led 2
#define led2 10

//Motor pin (unable to implement into the final design ): )
#define motorPin 3 // Motor control pin (PWM)

//Buzzer pin 
#define BUZZER_PIN 13

// Initialize LCD: RS, E, D4, D5, D6, D7
LiquidCrystal lcd(12, 11, 8, 9, 6, 4);

//--------------------------------------------------------------------------
void setup() {  // assigning pins to inputs and outputs
  Serial.begin(9600);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(led, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(motorPin, OUTPUT);
  cute.init(BUZZER_PIN);

  lcd.begin(16, 2);
  lcd.print("Booting up..."); // making sure lcd is working on startup
  delay(1000);
  lcd.clear();
}

// setting constants for timer
const float DIST_THRESHOLD     = 5.0;     //(in cm)  -- how far the water bottle needs to be for  it to be sensed
const unsigned long REMIND_DELAY   = 10000;  //(in ms) -- The length of time between hydration reminders
const unsigned long BLINK_INTERVAL =  500;  // distance between blinks (in ms) -- flashrate of the leds

//intializing variables
bool            bottlePresent = false;
unsigned long   presentStart  = 0;
unsigned long   lastBlink     = 0;
bool            blinkState    = false;

// Setting of the Hydration Reminder
void loop() {
  // Sending a pulse and measuring distance by timing how long the pulse takes to come back 
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH, 30000L); // making distance measuremneents
  float distance = (duration > 0)? (duration / 2.0) / 29.1: -1;

  lcd.setCursor(0, 0);
  // error checking the proximity sensor 
  if (distance < 0) {
    lcd.print("invalid");
  } else {
    // writing the distance to the lcd so you know what its detecting :)
    lcd.print("Dist:");
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
        digitalWrite(led,  blinkState ? HIGH : LOW); // switching between led states ON or OFF to create flashing
        digitalWrite(led2, blinkState ? LOW  : HIGH);
        // message
        lcd.setCursor(0, 1);
        if (blinkState) {// flashing text on a blink of teh LED's
          lcd.print("Drink water    ");
          cute.play(S_SUPER_HAPPY);

        } else {
          lcd.print("Go take a walk ");// flashing text on other blink to allow for multiple lines of text (sychronized with leds)
          cute.play(S_HAPPY);
        }
      }
    } else {
      digitalWrite(led,  HIGH);// if the bottle is sensed turn on leds and display text until the timer is up   
      digitalWrite(led2, HIGH);
      lcd.setCursor(0, 1);
      lcd.print("Bottle placed   ");
    }

  } else {
    bottlePresent = false; // reset the timer for when you drinking water 
    digitalWrite(led,  LOW);
    digitalWrite(led2, LOW);
    lcd.setCursor(0, 1);
    lcd.print("You're drinking     ");
  
  }

  delay(100); // give time for the code to run
}
