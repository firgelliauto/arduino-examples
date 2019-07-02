/* Firgelli Automations
 * Limited or no support: we do not have the resources for Arduino code support
 * To report software bugs contact: support@firgelliauto.com
 * 
 * This progam extends a measures the number of pulses per cycle in our
 * optical linear actuator series.
 *  
 * This program requires a IBT-2 H-Bridge, Arduino Uno, and one of our supported linear actuators:
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

int RPWM = 11;  //connect Arduino pin 10 to IBT-2 pin RPWM
int LPWM = 10;
int opticalPin = 2;

int Speed = 255;  //choose any speed in the range [0, 255]
int currentTime;
int period=200;

#define falsepulseDelay 20 // noise pulse time  , if too high, ISR will miss pulses.                          
volatile long lastDebounceTime = 0;   // the last time the interrupt was triggered
volatile int counter=0;
int prevCounter=0;
int Direction=1;    //-1 = retracting
                    // 0 = stopped
                    // 1 = extending

int strokeLength = 3;  //enter your stroke length here (in inches)
float Position = 0.0;

int extensionCount;
int retractionCount;

void setup() {
  pinMode(RPWM, OUTPUT);
  pinMode(LPWM, OUTPUT);
  pinMode(opticalPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(opticalPin), count, FALLING);
  Serial.begin(9600);
  Serial.println("Initializing calibration");
  Serial.println("Actuator retracting...");
  
  do{
    //keep retracting until counter remains the same for 100ms (i.e at the end)    
    prevCounter=counter;
    timeElapsed=0;
    while(timeElapsed<period){
      driveActuator(-1, 255);  //retract actuator
    }
  }while(prevCounter!=counter);
  
  Serial.println("Actuator fully retracted");
  delay(1000);

  //reset variables
  timeElapsed=0;
  counter=0;
  prevCounter=0;
  Serial.print("\n\nExtension Count\t\t");
  Serial.println("Retraction Count");
}

void driveActuator(int Direction, int Speed){
  switch(Direction){
    case 1:       //extension
      analogWrite(RPWM, Speed);
      analogWrite(LPWM, 0);
      break;
    
    case 0:       //stopping
      analogWrite(RPWM, 0);
      analogWrite(LPWM, 0);
      break;

    case -1:      //retraction
      analogWrite(RPWM, 0);
      analogWrite(LPWM, Speed);
      break;
  }
}

void loop() {  
  do{
    //keep retracting until counter remains the same for 100ms    
    prevCounter=counter;
    timeElapsed=0;
    while(timeElapsed<period){   //ensures the end is reached and also
      driveActuator(Direction, Speed);  //move actuator
    }
  }while(prevCounter!=counter);
  
  delay(500);      //in addition to a pause the delay will count any brief overshoot after stopping the motor
  
  if(Direction==1){
    extensionCount=counter;
    Serial.print(extensionCount);
    Serial.print("\t\t\t\t");  
  }
  else if(Direction==-1){
    retractionCount=abs(counter);
    Serial.print(retractionCount);
    Serial.println();
  }
    
  Direction*=-1; ///reverse the direction
  counter=0;   //reset counter
}

void count(){
  if ((millis() - lastDebounceTime) > falsepulseDelay) {    //reduce noise by debouncing IR signal 
    lastDebounceTime = millis();
    if(Direction==1){
      counter++;
    }
    if(Direction==-1){
      counter--;
    }
  }
  //add in error if direction==0 and interrupt triggers; due to overshoot
}

