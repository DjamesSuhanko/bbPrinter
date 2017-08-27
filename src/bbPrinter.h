#include <Arduino.h>
#include <Servo.h>

//TODO: do change to const int?
#define FULL_STEP   0
#define HALF_STEP   1
#define FOURTH_STEP 2
#define EIGHTH_STEP 3
#define MS0         0
#define MS1         1
#define FORWARD     0
#define BACKWARD    1
#define RIGHT       1
#define LEFT        0
#define INITIAL_POS 1
#define SLEEP       0
#define WAKE        1
#define ENABLE      0 //HIGH disable, LOW enable
#define DISABLE     1

class Printer
{
private:
  Servo pen;

  //boolean to check if setupMotor* is done. If don't, the begin() will not starts.
  bool setupMotorColumnIsOk = false;
  bool setupMotorLineIsOk   = false;
  //the printer will not start to print if begin() wasn't called before all
  bool setupDone = false;
  //column
  //pins
  byte pin_step_column;
  byte pin_direction_column;
  byte pin_ms2_column;
  byte pin_ms1_column;
  byte pin_enable_column;
  byte pin_sleep_column;
  //flag of direction
  byte direction_column;
  //type of step (full, half...)
  byte stepTypeColumnSelected;
  //how many steps are needed to shifting one pixel
  byte step_to_another_pixel_column = 0;
  //memory of actual position in column
  int  steps_column = 0;
  //memory of maximum steps per line. Measurement executed in alignColumn() function
  int column_width  = 0;
  //how many pixels are possible to write per line
  int pixels_in_line = 0;
  //The user needs to select how many steps per pixel in begin() function
  int global_spp_col= 0;

  //isr pins just to check status in alignment
  byte isr_pin_right = 0;
  byte isr_pin_left  = 0;
  byte isr_pin_fw    = 0;
  byte isr_pin_bw    = 0;

  //line
  byte           pin_step_line;
  byte           pin_direction_line;
  byte           pin_ms2_line;
  byte           pin_ms1_line;
  byte           pin_enable_line;
  byte           direction_line;
  byte           stepTypeLineSelected;
  byte           pin_sleep_line;
  unsigned long  steps_line;

  //how many steps are needed to shifting one pixel
  int global_spp_line = 0;

  //servo pin
  byte pinServoSelected;

  //servo degrees
  byte servoDegreesSelected;

  /*!If using a key limiter, sometimes when unlocking it can generate a false interrupt.
   To ensure if interrupt is correct, I'm using a time limit to accept a second interrupt.
   When interrupt occurs, the ISR will check if time_increased is greather than
   time_limit_to_another_interrupt. If true, time_increased gets 0 and interrupt is accepted
   inside moveLine or moveColumn.
  */

  int time_limit_to_another_interrupt = 5000; //5 seconds
  int time_increased                  = 0;
  unsigned long int initial_millis    = 0;

  const byte microSteps[4][2] = { {LOW,LOW},
                         {HIGH,LOW},
                         {LOW,HIGH},
                         {HIGH,HIGH}
  };

public:
    //interrupt from limit sensor
    bool column_left_limit_found  = false;
    bool column_right_limit_found = false;
    //interrupt from limit sensor
    bool line_backward_limit_found  = false;
    bool line_forward_limit_found   = false;

    //used to inform direction to writePixel() function.
    volatile byte goTo = 255; //0 or 1. Any other is a mistake. This will changed by ISR in sketch

   Printer();
  ~Printer();
  /*!This function moves the pen's car to left or right N pixels. One pixels is the same
  as the pen touching the paper one time in a respective position. The vars are:
  - pixelsToMove - number of pixels to move for
  - downTime_ms  - how much time to stay down touching the paper (to absorve ink)
  - directionFor - direction can be LEFT, RIGHT, BACKWARD or FORWARD
  */
  void moveColumn(int pixelsToMove, int downTime_ms, byte directionFor, int stepsPerPixel);

  /*! This function controlling behavior of servo motor and it's called to write a pixel. You could
  just call pen.write(), but this way you need implement delay between any degrees, if you wish a
  smooth movement. Delay between any servo movement depends weighted of pen. If really heavy, up the pen
  fast isn't a good idea. Try it.

  The variables are:
  - delayBetweenServoStepsDown_ms - How fast the arm needs go down (in ms). Probably so fast as possible so, 0ms
  - delayBetweenServoStepsUp_ms   - How fast the arm needs go up (in ms). A bit slow can be a good idea (5ms between each step?)
  - downTime_ms                   - Time to waiting ink absorption to paper (in ms). Depends the type of pen
  */
  void writePixel(int delayBetweenServoStepsDown_ms, int delayBetweenServoStepsUp_ms, int downTime_ms);

  /*! Move to selected line. This control is the simplest, because it just move forward and backward. The
  variables of this function are:
  - numberOfLinesToMove - number of lines to move for
  - timeToWaiting - delay before move again (probably not useful)
  - directionFor  - FORWARD to next, BACKWARD to prev
  - stepsPerPixel - steps needed to move 1 pixel
  - stepType      - FULL_STEP, HALF_STEP, FOURTH_STEP, EIGHTH_STEP
  */
  void moveLine(int numberOfLinesToMove, int timeToWaiting, byte directionFor, int stepsPerPixel);

  /*! This function receives  lines to write, does the parsing to call writePixel function, passing each
  pixel to servo motor. After that, call de pixel or line movement. This is the main function to print,
  writing line by line until the job is finished. When job is done, starts the re-alignment.*/
  void writeLine(char *line);

  /*! function to make the column alignment*/
  void alignColumn();

  /*! Function to make the line alignment*/
  void alignLine();

  /*! Pass an array of bytes for motorLine and motorColumn. Example:
  byte motorOnePins = {2,3,4,5,6,7};
  printer.begin(motorOnePins,...) and so on.
  You must pass a fixed size array with 6 pins for each one of two motors
  to controling EasyDriver correctly.
  The order of pins are:
  - pin sleep
  - pin enable
  - pin microstep 1
  - pin microstep 2
  - pin direction
  - pin step
  These pins must be connected correctly from Arduino to EasyDriver controller.

  The stepType can be selected from anyone of 4 steps type:
  FULL_STEP, HALF_STEP, FOURTH_STEP, EIGHTH_STEP

  The servoDegrees is how much you wish that your servo motor move away from the
  pen (how much your servo needs move away to release the pen). Using 45 degrees
  seems good enough, but it's related to your mechanism. Use the servoTest()
  function to ensure which is the correct position for.
  */
 void begin(byte stepTypeLine, byte stepTypeColumn, byte servoDegrees, byte pinServo, int stepsPerPixelColumn, int stepsPerPixelLine);

  /* Self test to check if everything is working fine. This function traces a
  retangle rX,rY starting in pX,pY. Example:
  printerSelfTest(20,50,30,30);
  This example prints a retangle size of 20(X) per 50(Y), starting in 30(X) per 30(Y)
  */
  void printerSelfTest(int rX, int rY, int pX, int pY);

  //! setup motor column pins
  void setupMotorColumn(byte p_sleep, byte p_enable, byte p_ms1, byte p_ms2, byte p_dir, byte p_step);

  //! setup motor line pins
  void setupMotorLine(byte p_sleep, byte p_enable, byte p_ms1, byte p_ms2, byte p_dir, byte p_step);

  //! settings of isr pins
  void isrPinsLrBf(byte p_left, byte p_right, byte p_backward, byte p_forward);

  //! calibrate servo - just touching a pixel in the paper and adjust the servo angle.
  void calibrateServo(int downTime_ms, byte degreesDown, byte servoPin);

  //!Adjustment of steps per pixel in column. Just a test, the final values needs be passed in begin() function.
  void calibratePixelStepsInColumn(int pixelsToMove, byte directionFor, int stepsPerPixel, int downTime_ms, byte degreesDown, byte servoPin);

  void calibratePixelStepsInLine(int pixelsToMove, byte directionFor, int stepsPerPixel, int downTime_ms, byte degreesDown, byte servoPin);
};
