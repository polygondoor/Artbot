/* 
 *  This file hides the complexity of the hardware UI
 *  It handles reading the Rotary Encoders
 *  and writing messages to the OLED screen
 *  
 */

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

