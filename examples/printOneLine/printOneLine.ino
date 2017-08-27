/*
printOneLine

Simulate printing with one manual line.
With this example you can see how the printer works, printing any simulated line.
When line printing is finished, the printer changes the line position to the next line
and you can start printing another one.

Pins:
  you need define a few pins to each one of two EasyDriver controllers
  The order is:
  - sleep pin
  - enable pin
  - microstep 1 pin
  - microstep 2 pin
  - direction pin
  - steps pin

  You need define 4 interrupt pins for  each one of 4 limiters (left, right, forward and
  backward).

  You need define a pin to servo motor.

  After that, instantiate the class Printer and call setupMotorColumn(), setupMotorLine and begin() function inside of
  setup() function.
  The parameters to both, setupMotor columns and line makes the same reference to respective pins:
  sleep      - used to 'turn off' the  motor while the another motor is working
  enable     - used as a trick to lets the motor cold while it's working
  microstep1 - used to set step type
  microstep2 - used to set step type
  direction  - LEFT or RIGHT to column, FORWARD or BACKWARD to line
  step       - step pin, used to pulse each one of N steps.

  The parameters to begin() function are:
  byte stepTypeLine        - the examples are below, in setup() function
  byte stepTypeColumn      - the same as above.
  byte servoDegrees        - angle needed to release the pen enough to touch the paper.
  byte pinServo            - servo pin.
  int  stepsPerPixelColumn - how many steps are needed to shift one pixel.
  int  stepsPerPixelLine   - how many steps are needed to shift one pixel.

The circuit:
  Inputs:
    You can use anything you wish to limiters, for example, reed switches. The pins
    needs accept interrupt in. Arduino Mega seems the better to that.

  Outputs:
    The outputs are only the pins to control motors. 6 to each one of step
    motors and 1 to servo motor.

  Power:
    I suggest you to use a switched-mode power supplies 12V with a lot of current.
    Configure the EasyDriver to minimal current usage. If you don't know how to
    wiring and adjust EasyDriver, I recommend you this article about:
    http://www.dobitaobyte.com.br/como-utilizar-o-easydriver-para-controlar-motor-de-passo/

    The servo motor can be connected directly to Arduino. As you need to use
    many pins, I suggest to use an Arduino Mega.

  Printer project:
    If you need the printer project, just read this article about:
     http://www.dobitaobyte.com.br/impressora-com-arduino-opencv/ ‎


Created 25 Jul 2017
By Djames Suhanko <djames.suhanko@gmail.com>
Modified 25 Jul 2017
By Djames Suhanko <djames.suhanko@gmail.com>

http://www.dobitaobyte.com.br

*/

#include <bbPrinter.h>

//left/right
#define PIN_SLEEP_LR        13
#define PIN_ENABLE_LR       8
#define PIN_MS1_LR          9
#define PIN_MS2_LR          10
#define PIN_DIR_LR          11
#define PIN_STEP_LR         12
//backward/forward
#define PIN_SLEEP_BF         52
#define PIN_ENABLE_BF        53
#define PIN_MS1_BF           4
#define PIN_MS2_BF           5
#define PIN_DIR_BF           6
#define PIN_STEP_BF          7
//servo pin and degrees needed to release the pen
#define SERVO_PIN            51
#define SERVO_DEGREES        45
//limiters uses interrupts, so in Arduino Mega you can use pins 2,3,[18,19 are generating serial interrupts],21 and 21
#define SENSOR_LEFT          2
#define SENSOR_RIGHT         3
#define SENSOR_FORWARD       20
#define SENSOR_BACKWARD      21
//steps per line and steps per column
#define STEPS_PER_PIXEL_LINE 500
#define STEPS_PER_PIXEL_COL  30

//instance of Printer class
Printer printer;

void isrLeft(){
  printer.column_left_limit_found = true;
  printer.goTo = LEFT; //left is direction of motor. The gear is running to
                       //opposite side. See the article above to
                       //understand how my printer is working.
}

void isrRight(){
  printer.column_right_limit_found = true;
  printer.goTo = RIGHT;
}

void isrForward(){
  printer.line_forward_limit_found = true;
}

void isrBackward(){
  printer.line_backward_limit_found = true;
}

void setup(){
  delay(2000);

  //configure isr pins
  printer.isrPinsLrBf(SENSOR_LEFT, SENSOR_RIGHT, SENSOR_FORWARD, SENSOR_BACKWARD);

  //pinMode is used by the function above. Now, connect the ISR functions
  attachInterrupt(digitalPinToInterrupt(SENSOR_LEFT),     isrLeft,     FALLING);
  attachInterrupt(digitalPinToInterrupt(SENSOR_RIGHT),    isrRight,    FALLING);
  attachInterrupt(digitalPinToInterrupt(SENSOR_FORWARD),  isrForward,  FALLING);
  attachInterrupt(digitalPinToInterrupt(SENSOR_BACKWARD), isrBackward, FALLING);

  //Setup pins of each one of two motors
  // Ordering pins:        sleep         enable         microstep1  microstep2  direction   step
  printer.setupMotorColumn(PIN_SLEEP_LR, PIN_ENABLE_LR, PIN_MS1_LR, PIN_MS2_LR, PIN_DIR_LR, PIN_STEP_LR);
  printer.setupMotorLine(  PIN_SLEEP_BF, PIN_ENABLE_BF, PIN_MS1_BF, PIN_MS2_BF, PIN_DIR_BF, PIN_STEP_BF);

  //this function  will starts the alignment before print anyhing, no matter the printer size.
  //step types can be one of FULL_STEP, HALF_STEP, FOURTH_STEP, EIGHTH_STEP
  printer.begin(FULL_STEP, FOURTH_STEP, SERVO_DEGREES, SERVO_PIN, STEPS_PER_PIXEL_COL, STEPS_PER_PIXEL_LINE);

  //define one line to simulate a printing
  char oneLine[]        = {"111111111100000000001000000000011000000000011100000000001111\0"};

  /*prints the line and  stops here, because there is nothing in loop. If you are receiving lines from network
   * or serial, just get waiting lines inside the loop, then call this same function below:
   */
  printer.writeLine(oneLine);
}

void loop(){
    //nothing here for now
}
