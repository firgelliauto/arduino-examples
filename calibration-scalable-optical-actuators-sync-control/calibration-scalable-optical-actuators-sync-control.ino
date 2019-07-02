/* Firgelli Automations
 * Limited or no support: we do not have the resources for Arduino code support
 * To report software bugs contact: support@firgelliauto.com
 * 
 * This progam extends two optical series linear actuators in sync with each other.
 *  
 * This program requires two IBT-2 H-Bridge, Arduino Uno, and two of our supported linear actuators:
 * (Insert list of compatible actuators)
 * Products available for purchase from https://www.firgelliauto.com/
 * 
 * Wiring: (insert link to diagram here)
 * IBT-2 pin VCC -> Arduino Uno pin 5V
 * IBT-2 pin GND -> Arduino Uno pin GND
 * IBT-2 pin R_EN -> Arduino Uno pin 5V
 * IBT-2 pin L_EN -> Arduino Uno pin 5V
 * IBT-2 pin RPWM -> Arduino Uno pin 10 (PWM)
 * IBT-2 pin LPWM -> Arduino Uno pin 11 (PWM)
 * +5V actuator -> Arduino Uno pin 5V
 * GND actuator -> Arduino Uno pin GND
 * output actuator -> Arduino Uno pin 2
 */

#include <elapsedMillis.h>
elapsedMillis timeElapsed;

//The following section of code requires you to customize it to your specific scenario:
//Number of actuators and pins they are connected to. 
//After setting these variables you don't need to alter any of the other code unless you want to experiment and optimize.

#define numberOfActuators 2
#define strokeLength 3  //enter your stroke length here (in inches)
int RPWM[numberOfActuators]={11, 9};      //PWM signal right side, change to suit the number actuators used and the pins they connect to
int LPWM[numberOfActuators]={10,6};       //PWM signal left side
int opticalPins[numberOfActuators]={2,3};
volatile long lastDebounceTime_0=0;   // the last time the interrupt was triggered
volatile long lastDebounceTime_1=0;

//After customizing the above lines of code you do not need to alter any of the remaining lines unless you want to.
//Need to add comment about changing interrupt functions, debounce time

int Speed = 255;  //choose any speed in the range [0, 255]
int period=200;
int currentTime;

#define falsepulseDelay 20 //noise pulse time, if too high, ISR will miss pulses.                          
volatile int counter[numberOfActuators] = {};   
int prevCounter[numberOfActuators] = {};     
int Direction=1;    //-1 = retracting
                    // 0 = stopped
                    // 1 = extending

int extensionCount[numberOfActuators] = {};  
int retractionCount[numberOfActuators] = {};  
int pulseTotal[numberOfActuators]={};   //stores number of pulses in one full extension/actuation

void setup() {
  for(int i=0; i<numberOfActuators; i++){
    pinMode(RPWM[i],OUTPUT);
    pinMode(LPWM[i], OUTPUT);
    pinMode(opticalPins[i], INPUT_PULLUP);
  }
  
  attachInterrupt(digitalPinToInterrupt(opticalPins[0]), count_0, RISING);
  attachInterrupt(digitalPinToInterrupt(opticalPins[1]), count_1, RISING);  
  
  memset(counter, 0, numberOfActuators);  //initialize as an array of zeros 
  memset(prevCounter, 0, numberOfActuators);  
  memset(extensionCount, 0, numberOfActuators);  
  memset(retractionCount, 0, numberOfActuators);
  memset(extensionCount, 0, numberOfActuators);
  memset(retractionCount, 0, numberOfActuators);
  memset(pulseTotal, 0, numberOfActuators);
  
  Serial.begin(9600);
  Serial.println("Initializing calibration");
  Serial.println("Actuator retracting...");
  moveTillLimit(-1, 255);  
  Serial.println("Actuator fully retracted");
  delay(1000);

  for(int i=0; i<numberOfActuators; i++){
    Serial.print("\t\t\t\tActuator \t\t\t");
    Serial.print(i);
    Serial.print("\t\t\t");
  }
      
  moveTillLimit(1, 255); //extend fully and count pulses
  Serial.print("\nExtension Count:\t\t");
  for(int i=0; i<numberOfActuators; i++){
    extensionCount_[i]=counter_[i];
    Serial.print(extensionCount_[i]);
    Serial.print("\t\t\t\t");  
  }
  delay(1000);
  
  moveTillLimit(-1, 255); //retract fully and count pulses
  Serial.print("\nRetraction Count:\t\t");
  for(int i=0; i<numberOfActuators; i++){
    retractionCount_[i]=counter_[i];
    Serial.print(retractionCount_[i]);
    Serial.print("\t\t\t\t");  
  }
  delay(1000);

  for(int i=0; i<numberOfActuators; i++){
    Serial.print("\nActuator ");
    Serial.print(i);
    Serial.print(" average pulses: ");
    pulseTotal[i]=(extensionCount_[i]+retractionCount_[i])/2;   //takes the average of measurements
    Serial.print(pulseTotal[i]);  
  }
  
  Serial.println("\nEnter these values in the syncronous control progam.");
}

void moveTillLimit(int Direction, int Speed){
  //reset counter variables
  for(int i = 0; i < numberOfActuators; i++){
    counter[i] = 0;
    prevCounter[i] = 0;
  }  
  do {
    //keep move until counter remains the same for 100ms (i.e at the end)  
    for(int i = 0; i < numberOfActuators; i++) {
      prevCounter_[i] = counter_[i];
    }
    timeElapsed = 0;
    while(timeElapsed < period){
      for(int i = 0; i < numberOfActuators; i++) {
        driveActuator(i, Direction, Speed);
      }
    }
  } while(compareCounter(prevCounter, counter))

}

// If both arrays have a similar element in the same index than return false
bool compareCounter(int prevCounter, int counter){
  bool areEqual = true;
  for(int i = 0; i < numberOfActuators; i++){
    if(prevCounter[i] == counter[i] ){
      areEqual = false;
    }
  }
  return areEqual;
}

void driveActuator(int Actuator, int Direction, int Speed){
  int rightPWM=RPWM[Actuator];
  int leftPWM=LPWM[Actuator];

  switch(Direction){
    case 1:       //extension
      analogWrite(rightPWM, Speed);
      analogWrite(leftPWM, 0);
      break;
    
    case 0:       //stopping
      analogWrite(rightPWM, 0);
      analogWrite(leftPWM, 0);
      break;

    case -1:      //retraction
      analogWrite(rightPWM, 0);
      analogWrite(leftPWM, Speed);
      break;
  }
}

void loop() {  
}

void count_0(){
  //This interrupt function increments a counter corresponding to changes in the optical pin status
  if ((millis() - lastDebounceTime_0) > falsepulseDelay) {    //reduce noise by debouncing IR signal 
    lastDebounceTime_0 = millis();
    if(Direction==1){
      counter_[0]++;
    }
    if(Direction==-1){
      counter_[0]--;
    }
  }
}

void count_1(){
  if ((millis() - lastDebounceTime_1) > falsepulseDelay) {   
    lastDebounceTime_1 = millis();
    if(Direction==1){
      counter_[1]++;
    }
    if(Direction==-1){
      counter_[1]--;
    }
  }
}
