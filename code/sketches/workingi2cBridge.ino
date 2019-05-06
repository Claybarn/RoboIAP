
#define I2C_SLAVE_ADDRESS 0x4 // the 7-bit address (remember to change this when adapting this example)
// Get this from https://github.com/rambo/TinyWire
#include <TinyWireM.h> // master i2c

#include <TinyWireS.h> // slave i2c

volatile byte i2c_regs[2] = {
     0x00, 
      0x00, 
    };
volatile int reg_position = 0;
const byte reg_size = 2;
volatile bool readFlag = false;

void requestEvent(){ 
  //TinyWireS.send(i2c_regs[0]);
  //TinyWireS.send(i2c_regs[1]);
  //readFlag = true; // Dont think want this here
   
    TinyWireS.send(i2c_regs[reg_position]);
    reg_position++;
    if (reg_position >= reg_size)
    {
        reg_position = 0;
        readFlag = true;
    }

     
}
/**
 * The I2C data received -handler
 *
 * This needs to complete before the next incoming transaction (start, data, restart/stop) on the bus does
 * so be quick, set flags for long running tasks to be called from the mainloop instead of running them directly,
 */
void receiveEvent(uint8_t howMany)
{
  getSlaveData(); // refresh stores sensor data
}



void setup() {
  // put your setup code here, to run once:
TinyWireM.begin();
delay(100);
TinyWireS.begin(I2C_SLAVE_ADDRESS);
TinyWireS.onReceive(receiveEvent);
TinyWireS.onRequest(requestEvent);

}

void getSlaveData(){
    //uint8_t error = 1;
    //while(error){
      TinyWireM.begin();
      TinyWireM.beginTransmission(uint8_t(0x6c)); // problem, gets hung up on
      TinyWireM.write(0x30);
      TinyWireM.endTransmission(false);
      TinyWireM.requestFrom(uint8_t(0x6c),uint8_t(2),uint8_t(true));
      //while(TinyWireM.available()<2){
        // do nothing
      //}
      TinyWireM.endTransmission(true);
      i2c_regs[0] = TinyWireM.read();
      i2c_regs[1] = TinyWireM.read();
      
      while(TinyWireM.available()){ // clear buffer
        TinyWireM.read();
      }
}

void loop() {
  // put your main code here, to run repeatedly:
    getSlaveData();
    readFlag = false;
  while(!readFlag){
       //do nothing
    }
}
