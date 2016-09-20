/* 
 *  This file hides the complexity of the hardware UI
 *  It handles reading the Rotary Encoders
 *  and writing messages to the OLED screen
 *  
 */

/*
 * This method reads all the rotary encoders
 */
void readRotaryEncoders() {
  rotaryEncoder1_read_clkPin = digitalRead(rotaryEncoder1_set_clkPin); // orange cable, CLK
  rotaryEncoder1_read_dtPin = digitalRead(rotaryEncoder1_set_dtPin);

  // rotaryMode; // Can be 0 1 2 or 3 (depending on which value we are changing)

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

/*
 * This method captures all the knob settings
 * and gives them to the steppers.
 */
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

 /*
  * 
  *  SCREEN HANDLING
  * 
  */

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


