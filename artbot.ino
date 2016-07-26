// Include libraries for the OLED screen
// Some more doc here
#include <Adafruit_GFX.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>

// Include libraries for the Stepper motors
#include <AccelStepper.h>
#include <AFMotor.h>

#define OLED_RESET 4                  // this code resets the display.
Adafruit_SSD1306 display(OLED_RESET);  // setup up the OLED screen

// two stepper motors one on each port
AF_Stepper motor1(2048, 1);
AF_Stepper motor2(2048, 2);

// you can change these to DOUBLE or INTERLEAVE or MICROSTEP!
// wrappers for the first motor!
void forwardstep1() {
  motor1.onestep(FORWARD, SINGLE);
}
void backwardstep1() {
  motor1.onestep(BACKWARD, SINGLE);
}
// wrappers for the second motor!
void forwardstep2() {
  motor2.onestep(BACKWARD, SINGLE);
}
void backwardstep2() {
  motor2.onestep(FORWARD, SINGLE);
}

// Motor shield has two motor ports, now we'll wrap them in an AccelStepper object
AccelStepper stepper1(forwardstep1, backwardstep1);
AccelStepper stepper2(forwardstep2, backwardstep2);

float maxSpeedLeft = 400;
float accelerationLeft = 100;
float moveToLeft = 10000;

float maxSpeedRight = 400;
float accelerationRight = 100;
float moveToRight = 1000000;

int rotaryEncoder1_set_clkPin = 49;
int rotaryEncoder1_set_dtPin = 47;
int rotaryEncoder1_set_btnPin = 45;
int rotaryEncoder1_read_clkPin;
int rotaryEncoder1_read_dtPin;
long rotaryEncoder1_positionCount = 93; // distance
int rotaryEncoder1_previousRead_clkPin;
// boolean bCW_1; // this is never used

int rotaryEncoder2_set_clkPin = 40;
int rotaryEncoder2_set_dtPin = 38;
int rotaryEncoder2_set_btnPin = 36;
int rotaryEncoder2_read_clkPin;
int rotaryEncoder2_read_dtPin;
long rotaryEncoder2_positionCount = 25; // speed
int rotaryEncoder2_previousRead_clkPin;
// boolean bCW_2; // this is never used

int rotaryEncoder3_set_clkPin = 46;
int rotaryEncoder3_set_dtPin = 44;
int rotaryEncoder3_set_btnPin = 42;
int rotaryEncoder3_read_clkPin;
int rotaryEncoder3_read_dtPin;
long rotaryEncoder3_positionCount = 35; // speed (30)
int rotaryEncoder3_previousRead_clkPin;
// boolean bCW_3; // this is never used

int rotaryEncoder4_set_clkPin = 52;
int rotaryEncoder4_set_dtPin = 50;
int rotaryEncoder4_set_btnPin = 48;
int rotaryEncoder4_read_clkPin;
int rotaryEncoder4_read_dtPin;
long rotaryEncoder4_positionCount = 100; // distance (1000)
int rotaryEncoder4_previousRead_clkPin;
// boolean bCW_4; // this is never used

int increment = 1;

boolean isDrawing = false;
int sensor1pin;

int rotaryMode = 0;

void setup()
{
  pinMode(rotaryEncoder1_set_clkPin, INPUT); // clk
  pinMode(rotaryEncoder1_set_dtPin, INPUT); // dt
  pinMode(rotaryEncoder1_set_btnPin, INPUT); // btn

  pinMode(25, INPUT);
  pinMode(23, INPUT);

  // dispay config
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // its the 0x3C that says "128 * 64"

  display.clearDisplay();
  display.setTextColor(WHITE);
/*
  display.setTextSize(1);
  display.setCursor(10, 0); // this moves our cursor right back to the top left pixel.. we should talk about this.
  display.print("The Polygon Door"); //this copies some text to the screens memory
*/
  display.setTextSize(3);
  display.setCursor(10, 10); // this moves our cursor right back to the top left pixel.. we should talk about this.
  display.print("ARTBOT"); //this copies some text to the screens memory
  display.setTextSize(2);
  display.setCursor(20, 45);
  display.print("Hello :)"); //this copies some text to the screens memory
  display.display();

  display.setTextSize(3);

  // wait to see the above message
  delay(1000);
}

long steps = 0;

void stopAndResetSteppers() {
  // stop everything
  stepper1.stop(); // Stop as fast as possible: sets new target
  stepper1.runToPosition();
  stepper2.stop(); // Stop as fast as possible: sets new target
  stepper2.runToPosition();

  // reset the steppers to position 0
  stepper1.setCurrentPosition(0);
  stepper2.setCurrentPosition(0);

  // tell the system that we are no longer drawing
  isDrawing = false;
}

void captureSettings() {
  // each wheel is 64mm
  // so if I make the the knob think in mm, then
  // 1 turn is 201.06mm (64 * 3.1416)
  // So, if someone says 100mm, then how many turns is that?
  // turns = configuredDistance / (64 * 3.1416)
  // given that 1 turn is 2048 steps then
  // turns in steps = 2048 * configuredDistance / (64 * 3.1416)

  // RIGHT WHEEL (knobs 1 and 2 (speed))
  stepper1.setMaxSpeed(rotaryEncoder2_positionCount * 10); // max 400
  stepper1.setAcceleration(accelerationRight);
  // calculate how many steps to go (here we divide by 2 because the bounce goes fowards and backwards)
  steps = (rotaryEncoder1_positionCount * 2048 / (64 * 3.1416) / 2);
  stepper1.moveTo(steps);
  // message(String(steps) );

  // LEFT WHEEL (knobs 4 and 3(speed))
  stepper2.setMaxSpeed(rotaryEncoder3_positionCount * 10); // max 400
  stepper2.setAcceleration(accelerationLeft);
  steps = (rotaryEncoder4_positionCount * 2048 / (64 * 3.1416) / 2);
  stepper2.moveTo(steps);
}

void loop() {
  if (!isDrawing) {
    readRotaryEncoders();

    // *TEST* read the IR sensing
    digitalWrite(23, 255);
    sensor1pin = analogRead(23);
    message(analogRead(1) );

    Serial.print("sensor1:");
    Serial.println(sensor1pin);
    delay(100);

    // read the value of the knob
    if (digitalRead(rotaryEncoder1_set_btnPin) == LOW) {
      // changeMode
      rotaryMode = (rotaryMode + 1) % 4;
      delay(500);
    }

    //push button to change increment
    if (digitalRead(25) == HIGH) {
      if (increment == 1) {
        increment = 10;
        message("Increment 10");
      }
      else if (increment == 10) {
        increment = 100; message("Increment 100");
      }
      else if (increment == 100) {
        increment = 1000; message("Increment 1000");
      }
      else if (increment == 1000) {
        increment = 1; message("Increment 1");
      }
      delay(500);
      report();
    }

    //push button to start
    if (digitalRead(23) == HIGH) {
      isDrawing = true;
      displayStartMessage();
      captureSettings();
    }

  } else {
    if (stepper1.distanceToGo() == 0) {
      // this line resets the whole device (but user needs to wait till wheel bounces)
      if (digitalRead(23) == HIGH) {
        // stop and reset
        stopAndResetSteppers();
        report();
      }
      stepper1.moveTo(-stepper1.currentPosition());
    }
    if (stepper2.distanceToGo() == 0) {
      stepper2.moveTo(-stepper2.currentPosition());
    }
    stepper1.run();
    stepper2.run();
  }
}

void readRotaryEncoders() {
  rotaryEncoder1_read_clkPin = digitalRead(rotaryEncoder1_set_clkPin); // orange cable, CLK
  rotaryEncoder1_read_dtPin = digitalRead(rotaryEncoder1_set_dtPin);

  rotaryMode; // Can be 0 1 2 or 3 (depending on which value we are changing)

  if ((rotaryEncoder1_read_clkPin != rotaryEncoder1_previousRead_clkPin) && (rotaryEncoder1_read_clkPin == LOW)) { // Knob Rotated l when aVal changes, BUT use only if aVal is LOW.
    if (rotaryEncoder1_read_dtPin == LOW) {
      if (rotaryMode == 0) {
        rotaryEncoder1_positionCount += increment;
      }
      else if (rotaryMode == 1) {
        rotaryEncoder2_positionCount += increment;
      }
      else if (rotaryMode == 2) {
        rotaryEncoder3_positionCount += increment;
      }
      else {
        rotaryEncoder4_positionCount += increment;
      }
      report();
    }
    else {
      if (rotaryMode == 0) {
        rotaryEncoder1_positionCount -= increment;
      }
      else if (rotaryMode == 1) {
        rotaryEncoder2_positionCount -= increment;
      }
      else if (rotaryMode == 2) {
        rotaryEncoder3_positionCount -= increment;
      }
      else {
        rotaryEncoder4_positionCount -= increment;
      }
      report();
    }
  }

  rotaryEncoder1_previousRead_clkPin = rotaryEncoder1_read_clkPin; // Don’t forget this
}

void report() {
  display.clearDisplay();

  display.setTextSize(1);
  display.setCursor(30 , 0);
  display.print("Speed: 0-40");
  display.setCursor(25 , 36);
  display.print("Distance: mm");

  display.setTextSize(2);
  display.setCursor(70, 50);
  display.print(rotaryEncoder1_positionCount); //this copies some text to the screens memory

  display.setCursor(70, 12);
  display.print(rotaryEncoder2_positionCount); //this copies some text to the screens memory

  display.setCursor(0, 12);
  display.print(rotaryEncoder3_positionCount); //this copies some text to the screens memory

  display.setCursor(0, 50);
  display.print(rotaryEncoder4_positionCount); //this copies some text to the screens memory

  display.display();
}

void message(String text) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(20, 40);
  display.print(text); //this copies some text to the screens memory
  display.display();
}

void message(int text) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(20, 40);
  display.print(text); //this copies some text to the screens memory
  display.display();
}

void displayStartMessage() {
  displayCountDown(3);
  delay(1000);
  displayCountDown(2);
  delay(1000);
  displayCountDown(1);
  delay(1000);

  display.clearDisplay();
  display.setTextSize(4);
  display.setCursor(40, 20);
  display.print("(:"); //this copies some text to the screens memory

  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print(rotaryEncoder3_positionCount); //this copies some text to the screens memory

  display.setCursor(110, 0);
  display.print(rotaryEncoder2_positionCount); //this copies some text to the screens memory

  display.setCursor(0, 50);
  display.print(rotaryEncoder4_positionCount); //this copies some text to the screens memory

  display.setCursor(110, 50);
  display.print(rotaryEncoder1_positionCount); //this copies some text to the screens memory

  display.display();
}

void displayCountDown(int count) {
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(10 , 5);
  display.print("STEP BACK");

  display.setTextSize(1);
  display.setCursor(10 , 40);
  display.print("Starting in: "); //this copies some text to the screens memory

  display.setTextSize(4);
  display.setCursor(94 , 30);
  display.print(count); //this copies some text to the screens memory
  display.display();
}

