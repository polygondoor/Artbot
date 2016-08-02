/* 
 *  This file hides all the complexity involved in 
 *  using the Accell Stepper library with the 
 *  Adafruit AFMotor library
 *  
 */

/*
 * Utility to stop steppers, and set them back to a 0 position
 */
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
