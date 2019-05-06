#define I2C_SLAVE_ADDRESS 0x4 // the 7-bit address (remember to change this when adapting this example)
// Get this from https://github.com/rambo/TinyWire
#include <TinyWireS.h>
#include <Stepper.h>
// The default buffer size, Can't recall the scope of defines right now
#ifndef TWI_RX_BUFFER_SIZE
#define TWI_RX_BUFFER_SIZE ( 16 )
#endif
// Get this library from http://bleaklow.com/files/2010/Task.tar.gz 
// and read http://bleaklow.com/2010/07/20/a_very_simple_arduino_task_manager.html for background and instructions
#include <Task.h>
#include <TaskScheduler.h>

// The led is connected so that the tiny sinks current
#define LED_ON LOW
#define LED_OFF HIGH
#define I2CStopCheck_YIELD_TICKS 2
bool lB = true;
int readout = 0;

/**
 * BEGIN: sipper
 */
class sipper : public Task
{
public:
    sipper ();
    virtual void run(uint32_t now);
    virtual bool canRun(uint32_t now);
    void sipperIn();
    void sipperOut();
    void setTask(char);
private:
    volatile char task = '0'; // changed by master to move sipper in/out
    uint8_t yield_counter; // Incremented on each canRun call, used to yield to other tasks.
    const int stepsPerRevolution = 200;  // change this to fit the number of steps per revolution
    unsigned long t = 0;
    Stepper myStepper = Stepper(stepsPerRevolution, 10, 9, 8, 7);

};

sipper::sipper()
: Task()
{
  myStepper.setSpeed(60);
  digitalWrite(1, INPUT); // endstop
  digitalWrite(6, OUTPUT); // toggle h-bridge on-off
}

// We can't just return true since then no other task could ever run (since we have the priority)
bool sipper::canRun(uint32_t now)
{
    yield_counter++;
    bool ret = false;
    if (yield_counter == I2CStopCheck_YIELD_TICKS)
    {
        ret = true;
        yield_counter = 0;
    }
    return ret;
}

void sipper::run(uint32_t now)
{
    if (millis()>=t){ // turn off H-Bridge after 6 seconds
      digitalWrite(6,LOW);
      t = millis();
    }
    if (task != '0'){
      if (task == 'i'){
        sipperIn();
      }
      if (task == 'o'){
        sipperOut();
      }
      task = '0';
    }
}

void sipper::sipperIn(){
  digitalWrite(6,HIGH);
  myStepper.step(5*stepsPerRevolution);
  t = millis()+6000;
}

void sipper::sipperOut(){
  digitalWrite(6,HIGH);
  myStepper.step(-5*stepsPerRevolution);
  t = millis()+6000;
}

void sipper::setTask(char t){
  task = t;
}

/**
 * END: sipper
 */

 
/**
 * BEGIN: solenoids
 */
class solenoids : public Task
{
public:
    solenoids ();
    virtual void run(uint32_t now);
    virtual bool canRun(uint32_t now);
    void giveAccess();
    void fill();
    void topOpen();
    void topClose();
    void bottomOpen();
    void bottomClose();
    void setTask(char);
private:
    volatile char task = 0; // changed by master to control liquid
    uint8_t yield_counter; // Incremented on each canRun call, used to yield to other tasks.
};

solenoids::solenoids()
: Task()
{
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
}

// We can't just return true since then no other task could ever run (since we have the priority)
bool solenoids::canRun(uint32_t now)
{
    yield_counter++;
    bool ret = false;
    if (yield_counter == I2CStopCheck_YIELD_TICKS)
    {
        ret = true;
        yield_counter = 0;
    }
    return ret;
}

void solenoids::run(uint32_t now)
{
    if (task != '0'){
      if (task == 'a'){
        giveAccess();
      }
      if (task == 'f'){
        fill();
      }
      if (task == 'A'){
        topOpen();
      }
       if (task == 'B'){
        topClose();
      }
       if (task == 'C'){
        bottomOpen();
      }
       if (task == 'D'){
        bottomClose();
      }
      task = '0';
    }
}

void solenoids::giveAccess(){
  digitalWrite(2,LOW);
  digitalWrite(3,HIGH);
  
}

void solenoids::fill(){
  bottomClose();
  delay(6000);
  while (analogRead(A0) < 580){
    topOpen();
    delay(200);
    topClose();
    delay(1000);
  }
  topClose();
  delay(1);
  bottomOpen();
}

// for debugging purposes

void solenoids::topOpen(){
    digitalWrite(2,HIGH);
}

void solenoids::topClose(){
    digitalWrite(2,LOW);
}


void solenoids::bottomOpen(){
    digitalWrite(3,LOW);
}


void solenoids::bottomClose(){
    digitalWrite(3,HIGH);
}

// end for debugging purposes

void solenoids::setTask(char t){
  task = t;
}

/**
 * END: solenoids
 */


/**
 * BEGIN: I2C Stop flag checker
 *
 * This task needs to run almost all the time due to the USI I2C implementation limitations
 *
 * So I2CStopCheck_YIELD_TICKS below is used to specify how often the task is run, not it's every 4 ticks
 */
#define I2CStopCheck_YIELD_TICKS 4
class I2CStopCheck : public Task
{
public:
    I2CStopCheck();
    virtual void run(uint32_t now);
    virtual bool canRun(uint32_t now);
private:
    uint8_t yield_counter; // Incremented on each canRun call, used to yield to other tasks.
};

I2CStopCheck::I2CStopCheck()
: Task()
{
}

// We can't just return true since then no other task could ever run (since we have the priority)
bool I2CStopCheck::canRun(uint32_t now)
{
    yield_counter++;
    bool ret = false;
    if (yield_counter == I2CStopCheck_YIELD_TICKS)
    {
        ret = true;
        yield_counter = 0;
    }
    return ret;
}

void I2CStopCheck::run(uint32_t now)
{
    TinyWireS_stop_check();
}
/**
 * END: I2C Stop flag checker
 */

// Create the tasks.

I2CStopCheck checker;
sipper sip;
solenoids sol;
// Tasks are in priority order, only one task is run per tick
//Task *tasks[] = { &checker, &blinker, };
Task *tasks[] = { &checker, &sip, &sol};
TaskScheduler sched(tasks, NUM_TASKS(tasks));


/**
 * This is called for each read request we receive, never put more than one byte of data (with TinyWireS.send) to the 
 * send-buffer when using this callback
 */
void requestEvent()
{  
    //TinyWireS.send(i2c_regs[reg_position]);
    int data = analogRead(A0);
    TinyWireS.send(highByte(data));
    TinyWireS.send(lowByte(data));
   
}

/**
 * The I2C data received -handler
 *
 * This needs to complete before the next incoming transaction (start, data, restart/stop) on the bus does
 * so be quick, set flags for long running tasks to be called from the mainloop instead of running them directly,
 */
void receiveEvent(uint8_t howMany)
{
  char task = TinyWireS.receive();
    if (task == 'i' || 'o'){ // simple in/out
      sip.setTask(task);
    }
    if (task == 'a' || 'f' || 'A' || 'B' || 'C' || 'D' ){
      sol.setTask(task);
    }
}


void setup()
{
    // TODO: Tri-state this and wait for input voltage to stabilize 
    pinMode(3, OUTPUT); // OC1B-, Arduino pin 3, ADC
    digitalWrite(3, LED_ON); // Note that this makes the led turn on, it's wire this way to allow for the voltage sensing above.

    pinMode(1, OUTPUT); // OC1A, also The only HW-PWM -pin supported by the tiny core analogWrite

    /**
     * Reminder: taking care of pull-ups is the masters job
     */

    TinyWireS.begin(I2C_SLAVE_ADDRESS);
    TinyWireS.onReceive(receiveEvent);
    TinyWireS.onRequest(requestEvent);

    
    // Whatever other setup routines ?
    
}

void loop()
{
    // Run the scheduler - never returns.
    sched.run();
}
