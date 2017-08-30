
#include "bbPrinter.h"

//Constructor
Printer::Printer()
{

}

/*In first, needs to found right corner. After that, starts movement to left, increasing
steps_column, to found how many pixels can write per line.*/
void Printer::alignColumn()
{
    //found where starts in right...
    if (digitalRead(isr_pin_left) == LOW){
        /*motor to left, runs gear to right.
         If the motor is stopped in the left side, will back a few steps.
        */
        this->moveColumn(50, 0, LEFT, 2);
    }
    this->moveColumn(300, 0, RIGHT, 400); //motor to right move car to left
    delay(500);
    //...so, starts counting...
    this->moveColumn(300, 0, LEFT, 400);
}
/* This alignment is like above, changing the vars. */
void Printer::alignLine()
{
    //TODO: calculte steps to move based in ppl*steps
    this->moveLine(10000, 0, FORWARD, 10);
    delay(500);
    this->moveLine(10000, 0, BACKWARD, 10);
}

void Printer::isrPinsLrBf(byte p_left, byte p_right, byte p_backward, byte p_forward)
{
    this->isr_pin_left  = p_left;
    this->isr_pin_right = p_right;
    this->isr_pin_fw    = p_forward;
    this->isr_pin_bw    = p_backward;

    pinMode(this->isr_pin_left,    INPUT_PULLUP);
    pinMode(this->isr_pin_right,   INPUT_PULLUP);
    pinMode(this->isr_pin_fw,      INPUT_PULLUP);
    pinMode(this->isr_pin_bw,      INPUT_PULLUP);
}

void Printer::setupMotorColumn(byte p_sleep, byte p_enable, byte p_ms1, byte p_ms2, byte p_dir, byte p_step)
{
    /*
    You must pass  6 pins for control the step motor with EasyDriver.
    The order of pins is:
    - pin sleep
    - pin enable
    - pin microstep 1
    - pin microstep 2
    - pin direction
    - pin step
    These pins must be connected correctly from Arduino to EasyDriver controller.
    */
    this->pin_sleep_column     = p_sleep;
    this->pin_enable_column    = p_enable;
    this->pin_ms1_column       = p_ms1;
    this->pin_ms2_column       = p_ms2;
    this->pin_direction_column = p_dir;
    this->pin_step_column      = p_step;

    pinMode(this->pin_sleep_column,OUTPUT);
    pinMode(this->pin_direction_column,OUTPUT);
    pinMode(this->pin_step_column,OUTPUT);
    pinMode(this->pin_ms1_column,OUTPUT);
    pinMode(this->pin_ms2_column,OUTPUT);
    pinMode(this->pin_enable_column,OUTPUT);

    digitalWrite(this->pin_enable_column,DISABLE);
    digitalWrite(this->pin_sleep_column,SLEEP);
    this->setupMotorColumnIsOk = true;
}

void Printer::setupMotorLine(byte p_sleep, byte p_enable, byte p_ms1, byte p_ms2, byte p_dir, byte p_step)
{
    /*
    You must pass  6 pins for control the step motor with EasyDriver.
    The order of pins is:
    - pin sleep
    - pin enable
    - pin microstep 1
    - pin microstep 2
    - pin direction
    - pin step
    These pins must be connected correctly from Arduino to EasyDriver controller.
    */
    this->pin_sleep_line     = p_sleep;
    this->pin_enable_line    = p_enable;
    this->pin_ms1_line       = p_ms1;
    this->pin_ms2_line       = p_ms2;
    this->pin_direction_line = p_dir;
    this->pin_step_line      = p_step;

    pinMode(this->pin_sleep_line,OUTPUT);
    pinMode(this->pin_direction_line,OUTPUT);
    pinMode(this->pin_step_line,OUTPUT);
    pinMode(this->pin_ms1_line,OUTPUT);
    pinMode(this->pin_ms2_line,OUTPUT);
    pinMode(this->pin_enable_line,OUTPUT);

    digitalWrite(this->pin_enable_line,DISABLE);
    digitalWrite(this->pin_sleep_line,SLEEP);

    this->setupMotorLineIsOk = true;
}

void Printer::begin(byte stepTypeLine, byte stepTypeColumn, byte servoDegrees, byte pinServo, int stepsPerPixelColumn, int stepsPerPixelLine)
{
    /* ATTENTION! Before call begin(), you need setup the motor pins. To do that,
    use the setupMotorLine and setupMotorColumn methods.
    TODO: is a good idea a visual feedback
    */
    if (!this->setupMotorLineIsOk || !this->setupMotorColumnIsOk){
        return;
    }
    /*
    In this parameter, you need to pass one of:
    FULL_STEP   - Using a step motor with 1.8 degrees per step, 200 steps is like
    360 degrees
    HALF_STEP   - 200 steps runs 180 degrees
    FOURTH_STEP - 200 steps runs 90 degrees
    EIGHTH_STEP - 200 steps runs  45 degrees
    This choice is related to speed  too.
    */
    this->stepTypeLineSelected   = stepTypeLine;
    this->stepTypeColumnSelected = stepTypeColumn;

    /*The servo degress depends of your mechanism. In good mechanism, 45 degrees is
    enough. This way, you will have a minimal effort to get up the pen. Less is better.*/
    this->servoDegreesSelected = servoDegrees;

    //pin servo to attach
    this->pinServoSelected = pinServo;

    //steps per pixel selected
    this->global_spp_col  = stepsPerPixelColumn;
    this->global_spp_line = stepsPerPixelLine;

    /*Now, connect the servo motor and set pen positionto  up. The 0 is up so, you
    should starts the servo setting write to 0. With servoDegreesSelected the  pen
    will gets down.
    */
    pen.attach(pinServoSelected);
    pen.write(0);

    /*Make alignment. Both line motor and column motor will move to right, then
    both motors will  move to left, couting  how many steps are available in line
    and column.
    */
    //this->alignColumn();
    //this->alignLine();
    this->setupDone = true;
}

//TODO: turn generic this function to move both, line and column
void Printer::moveColumn(int pixelsToMove, int downTime_ms, byte directionFor, int stepsPerPixel)
{
    digitalWrite(this->pin_ms2_column,this->microSteps[this->stepTypeColumnSelected][MS1]); // setup second bit of step type
    digitalWrite(this->pin_ms1_column,this->microSteps[this->stepTypeColumnSelected][MS0]); // setup first bit of step type

    digitalWrite(this->pin_direction_column,directionFor); //set direction of

    digitalWrite(this->pin_sleep_column,WAKE); //wake up the EasyDriver controller
    digitalWrite(this->pin_enable_column,ENABLE); //pin_enable in LOW enables motor usage

    /*stepsPerPixel is how many steps are needed to set the next pixel position.
    The numberOfLinesToMove var is how much lines you wish move forward. To backword, just change  the
    directionFor var, you don't need a loop decrasing position.
    */
    unsigned long int task = pixelsToMove * stepsPerPixel;

    this->initial_millis   = millis();

    for (unsigned long int i=0; i<task;i++){
        /*The limiter can be anything; a button, a reedwitch and so on. If step forward gets the limits and
        the limit sensor detect it, the motor will be stopped imediatly.
        The interrupt  will not start again because ISR is getting FALLING state only.
        Don't forget to attach interrupt using FALLING. DON'T use CHANGE state or other one than FALLING!
        */
        this->time_increased = (millis()-this->initial_millis)/1000;
        if (this->column_left_limit_found && this->time_increased > 5){
            this->column_left_limit_found = false;
            digitalWrite(this->pin_enable_column,DISABLE); //unlock tracking (disable tracking)
            digitalWrite(this->pin_sleep_column, SLEEP);  //put EasyDriver to sleep. The motor gets really cold this way
            this->time_increased = 0;
            break;
        }
        /* If a false interrupt occurs, this flag needs be changed to initial state again*/
        else if (this->column_left_limit_found){
            this->column_left_limit_found = false;
        }

        /* In alignment function in the first step, is needed make this event to get start counting of total
        steps. But the changes in steps_column can be started only one time. To do that, the boolean
        setupDone cannot be true.*/
        else if (this->column_right_limit_found && this->time_increased > 5){
            this->column_right_limit_found = false;
            digitalWrite(this->pin_enable_column,DISABLE); //unlock tracking (disable tracking)
            digitalWrite(this->pin_sleep_column,SLEEP);  //put EasyDriver to sleep. The motor gets really cold this way
            this->time_increased = 0;
            break;
        }
        else if (this->column_right_limit_found){
            this->column_right_limit_found = false;
        }

        //do one step...
        digitalWrite(this->pin_step_column,HIGH);
        delay(5);
        digitalWrite(this->pin_step_column,LOW);

        /* While setupDone is not true, the steps_column will be increased. No matter if right corner wasn't
        touched yet, because when the right corner is touched, steps_column is reseted.
        */
        if (directionFor == LEFT && !setupDone){
            this->steps_column += 1;
        }

        //...and disable the locking of motor for a short time. This way the motor will gets not very hot
        digitalWrite(this->pin_enable_column,DISABLE);
        delay(1);
        digitalWrite(this->pin_enable_column,ENABLE);
    }
    digitalWrite(this->pin_enable_column,DISABLE); //unlock tracking (disable tracking)
    digitalWrite(this->pin_sleep_column,SLEEP);  //put EasyDriver to sleep. The motor gets really cold this way
}

void Printer::moveLine(int numberOfLinesToMove, int timeToWaiting, byte directionFor,int stepsPerPixel)
{
    digitalWrite(this->pin_ms2_line,this->microSteps[this->stepTypeLineSelected][MS1]); // setup second bit of step type
    digitalWrite(this->pin_ms1_line,this->microSteps[this->stepTypeLineSelected][MS0]); // setup first bit of step type

    digitalWrite(this->pin_direction_line,directionFor); //set direction of

    digitalWrite(this->pin_sleep_line,WAKE); //wake up EasyDriver controller
    digitalWrite(this->pin_enable_line,ENABLE); //enable in LOW enables motor usage

    /*stepsPerPixel is how many steps are needed to set the next pixel position.
    The numberOfLinesToMove var is how much lines you wish move forward. To backword, just change  the
    directionFor var, you don't need a loop decrasing position.
    */
    unsigned long int task = (unsigned long)numberOfLinesToMove * (unsigned long)stepsPerPixel;

    this->initial_millis   = millis();
    //unsigned long int task = 100000;
    while (task > 1){
        task -= 1;
        /*The limiter can be anything; a button, a reedwitch and so on. If step forward gets the limits and
        the limit sensor detect it, the motor will be stopped imediatly.
        The interrupt  will not start again because ISR is getting FALLING state only.
        Don't forget to attach interrupt using FALLING. DON'T use CHANGE state or other one than FALLING!
        */
        this->time_increased = (millis()-this->initial_millis)/1000;
        if (this->line_forward_limit_found && this->time_increased > 5){
            this->line_forward_limit_found = false;
            digitalWrite(this->pin_enable_line,DISABLE); //unlock tracking (disable tracking)
            digitalWrite(this->pin_sleep_line, SLEEP);  //put EasyDriver to sleep. The motor gets really cold this way
            this->time_increased = 0;
            break;
        }

        else if (this->line_forward_limit_found){
            this->line_forward_limit_found = false;
        }

        else if (this->line_backward_limit_found && this->time_increased > 5){
            this->line_backward_limit_found = false;
            if (!this->setupDone){
                this->steps_line = 0;
            }
            digitalWrite(this->pin_enable_line,DISABLE); //unlock tracking (disable tracking)
            digitalWrite(this->pin_sleep_line, SLEEP);  //put EasyDriver to sleep. The motor gets really cold this way
            this->time_increased = 0;
            break;
        }

        else if (this->line_backward_limit_found){
            this->line_backward_limit_found = false;
        }

        //do one step...
        digitalWrite(this->pin_step_line,HIGH);
        delay(5);
        digitalWrite(this->pin_step_line,LOW);

        /* While setupDone is not true, the steps_line will be increased. No matter if forward corner wasn't
        touched yet, because when the forward corner is touched, steps_line is reseted.
        */
        if (!this->setupDone){
            this->steps_line += 1;
        }

        //...and disable the locking of motor for a short time. This way the motor will not gets very hot
        digitalWrite(this->pin_enable_line,DISABLE);
        delay(1);
        digitalWrite(this->pin_enable_line,ENABLE);
    }
    //when finished the work, the motor stays unlocked
    digitalWrite(this->pin_enable_line,DISABLE); //this way the motor will not waiting to tracking
    digitalWrite(this->pin_sleep_line,SLEEP); //put EasyDriver to sleep. The motor gets really cold this way
}

/*In Arduino sketch, everything you need to do is call this function to write a line.
spp means steps-per-pixel.
*/
void Printer::writeLine(char *line)
{
    /* First step - check if overall length of line is equal or litle than maximum size.
    The maximum size is stored in steps_column, inside moveColumn() function, when begin()
    function is called.
    */
    this->direction_column = this->goTo;
    int chars_in_line = strlen(line);
    int check_size    = chars_in_line*global_spp_col;

    /*
    if (check_size > this->steps_column){
        //TODO: colocar um LED pra alarmar
        return;
    }
    */
    //write pixels from array
    if (this->direction_column == LEFT){
        for (int i=0; i<chars_in_line;i++){
            //using if inside is better than two instructions every time
            if (line[i] == '1'){
                writePixel(0, 10, 300);
                if (i < chars_in_line-1){
                    moveColumn(1, 0, this->direction_column, this->global_spp_col);
                }
            }
            //move to the next pixel
            //using if inside is better than two instructions every time
            if (line[i] == '0'){
                if (i != chars_in_line-1){
                    moveColumn(1, 0, this->direction_column, this->global_spp_col);
                }
            }
        }
    }
    else{ //RIGHT
        for (int i=chars_in_line;i>-1;i--){
            //using if inside is better than two instructions every time
            if (line[i] == '1'){
                writePixel(0, 0, 100);
                //move to the next pixel
                if (i > 1){
                    moveColumn(1, 0, this->direction_column, this->global_spp_col);
                }
            }
            //using if inside is better than two instructions every time
            else if (line[i] == '0'){
                if (i > 1){
                    moveColumn(1, 0, this->direction_column, this->global_spp_col);
                }
            }
        }
        //first move, after that write. so, we need to advance one step before change direction
        moveColumn(1, 0, this->direction_column, this->global_spp_col);
    }
    this->goTo = !this->direction_column;

    //finished all pixels in this line. Now you can start printing a new image.
    moveLine(1,100, FORWARD, this->global_spp_line);
}

//details in header file
void Printer::writePixel(int delayBetweenServoStepsDown_ms, int delayBetweenServoStepsUp_ms, int downTime_ms)
{
    //if 0ms, write position directly
    if (this->servoDegreesSelected == 0){
        pen.write(this->servoDegreesSelected);
    }
    //else, make a loop degree-by-degree with delay between each step
    else{
        for (int i=1; i<this->servoDegreesSelected+1;i++){
            pen.write(i);
            delay(delayBetweenServoStepsDown_ms);
        }
    }
    //delay to ink absorption
    delay(downTime_ms);

    //if 0ms, write position directly
    if (delayBetweenServoStepsUp_ms){
        pen.write(0); //remember: 0 is initial position, where pen don't touch the paper
        return;
    }
    //if the condition above is true, the code below will not be executed (because we have a 'return' statement there)
    for (int j=this->servoDegreesSelected;j >-1;j--){
        pen.write(j);
        delay(delayBetweenServoStepsUp_ms);
    }
}

void Printer::calibrateServo(int downTime_ms, byte degreesDown, byte servoPin)
{
    pen.attach(servoPin);
    this->pen.write(0);
    delay(downTime_ms);
    this->pen.write(degreesDown);
    delay(downTime_ms);
    this->pen.write(0);
    pen.detach();
}

//TODO: make a generic method to both, lines and columns
void Printer::calibratePixelStepsInColumn(int pixelsToMove, byte directionFor, int stepsPerPixel, int downTime_ms, byte degreesDown, byte servoPin)
{
    this->calibrateServo(downTime_ms, degreesDown, servoPin);
    this->moveColumn(pixelsToMove,downTime_ms,directionFor,stepsPerPixel);
    this->calibrateServo(downTime_ms, degreesDown, servoPin);
}

void Printer::calibratePixelStepsInLine(int pixelsToMove, byte directionFor, int stepsPerPixel, int downTime_ms, byte degreesDown, byte servoPin)
{
    this->calibrateServo(downTime_ms, degreesDown, servoPin);
    this->moveLine(pixelsToMove,downTime_ms,directionFor,stepsPerPixel);
    this->calibrateServo(downTime_ms, degreesDown, servoPin);
}

Printer::~Printer()
{

}
