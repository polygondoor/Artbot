/* 
 *  This Arduino project drives the Polygon Door Artbot
 *  Its aim is to empower as many drawing modes as possible.
 *
 *  The Artbot is of the ilk of drawing bots starting with MIT's turtle bot
 *  in which a pen is positioned between 2 wheels, each drien by a separate stepper motor
 */

// Include libraries for drawing to the OLED screen
#include <Adafruit_GFX.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>

// Include libraries for the stepper motors
#include <AccelStepper.h>
#include <AFMotor.h>

// Declare the OLED screen
#define OLED_RESET 4                  
Adafruit_SSD1306 display(OLED_RESET); 

// Declare the motors (for AFMotor lib)
AF_Stepper motor1(2048, 1);
AF_Stepper motor2(2048, 2);

// These methods are used as 'wrappers' so that we can use 2 motor libraries together
// Note that each step can be SINGLE, DOUBLE, INTERLEAVE or MICROSTEP
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

// Declare the AccelStepper motors (which 'wrap' the AFMotor lib motors)
AccelStepper stepper1(forwardstep1, backwardstep1);
AccelStepper stepper2(forwardstep2, backwardstep2);

// Default configurations of the motors
float maxSpeedLeft = 400;
float accelerationLeft = 100;
float moveToLeft = 10000;

float maxSpeedRight = 400;
float accelerationRight = 100;
float moveToRight = 1000000;

// Many values are required for the action of the rotary controllers
// TODO: Can these be abstracted out to a separate class or file?
int rotaryEncoder1_set_clkPin = 49;
int rotaryEncoder1_set_dtPin = 47;
int rotaryEncoder1_set_btnPin = 45;
int rotaryEncoder1_read_clkPin;
int rotaryEncoder1_read_dtPin;
long rotaryEncoder1_positionCount = 93;
int rotaryEncoder1_previousRead_clkPin;

int rotaryEncoder2_set_clkPin = 40;
int rotaryEncoder2_set_dtPin = 38;
int rotaryEncoder2_set_btnPin = 36;
int rotaryEncoder2_read_clkPin;
int rotaryEncoder2_read_dtPin;
long rotaryEncoder2_positionCount = 25;
int rotaryEncoder2_previousRead_clkPin;

int rotaryEncoder3_set_clkPin = 46;
int rotaryEncoder3_set_dtPin = 44;
int rotaryEncoder3_set_btnPin = 42;
int rotaryEncoder3_read_clkPin;
int rotaryEncoder3_read_dtPin;
long rotaryEncoder3_positionCount = 35;
int rotaryEncoder3_previousRead_clkPin;

int rotaryEncoder4_set_clkPin = 52;
int rotaryEncoder4_set_dtPin = 50;
int rotaryEncoder4_set_btnPin = 48;
int rotaryEncoder4_read_clkPin;
int rotaryEncoder4_read_dtPin;
long rotaryEncoder4_positionCount = 100;
int rotaryEncoder4_previousRead_clkPin;

// TODO: this needs a clearer name
int increment = 1;
// TODO: this needs either a clearer name or better expanation
boolean isDrawing = false;
// TODO: better nomenclature and explanation
int sensor1pin;
// TODO: explain
int rotaryMode = 0;

void setup()
{
  pinMode(rotaryEncoder1_set_clkPin, INPUT); // clk
  pinMode(rotaryEncoder1_set_dtPin, INPUT); // dt
  pinMode(rotaryEncoder1_set_btnPin, INPUT); // btn

  // TODO: These pinModes need to be abstracted to variables
  // OR they should be clearly marked.
  pinMode(25, INPUT);
  pinMode(23, INPUT);

  // Initialise the OLED display
  // Note: it is necessary to change a value in the Adafruit_SSD1306 library to set the screen size to 128x64
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();

  // Set some default values for writing to the OLED screen 
  display.setTextColor(WHITE);

  // Write "ARTBOT" in big
  display.setTextSize(3);
  display.setCursor(10, 10); // this moves our cursor right back to the top left pixel.. we should talk about this.
  display.print("ARTBOT"); //this copies some text to the screens memory

  // Write "Hello" in small
  display.setTextSize(2);
  display.setCursor(20, 45);
  display.print("Hello :)"); //this copies some text to the screens memory
  display.display();

  // TODO: Can we remove this?
  display.setTextSize(3);

  // Small delay to allow user to see this message
  // TODO: Is this necessary? I suspect not
  delay(1000);
}

// TODO: what is this value?
long steps = 0;

/*
 * This is the main block of  code
 * This code should:
 * a) be brief
 * b) describe the high level logic of the app
 * 
 */
void loop() {
  if (!isDrawing) {
    readRotaryEncoders();

    // TODO: remove test code (perhaps after IR is working)
    /*
    digitalWrite(23, 255);
    sensor1pin = analogRead(23);
    message(analogRead(1) );

    Serial.print("sensor1:");
    Serial.println(sensor1pin);
    delay(100);
    */

    // See if mode button has been pressed
    // This controls which value will be modified by the knob
    if (digitalRead(rotaryEncoder1_set_btnPin) == LOW) {
      // changeMode
      rotaryMode = (rotaryMode + 1) % 4;
      delay(500);
    }

    // push button to change increment
    // TODO: Use a parameter, not a value in the digitalRead param
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

    // Push button to start
    // TODO: Use a parameter, not a value in the digitalRead param
    if (digitalRead(23) == HIGH) {
      isDrawing = true;
      displayStartMessage();
      captureSettings();
    }

  } else {
    if (stepper1.distanceToGo() == 0) {
      // Reset the whole device (but user needs to wait till wheel bounces)
      // TODO: Use a parameter, not a value in the digitalRead param
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
