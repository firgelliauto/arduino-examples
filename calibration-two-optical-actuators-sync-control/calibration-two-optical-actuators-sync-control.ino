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

int RPWM_1 = 11;  //connect Arduino pin 10 to IBT-2 pin RPWM
int LPWM_1 = 10;
int RPWM_2 = 9;  
int LPWM_2 = 6;
int opticalPin_1 = 2;
int opticalPin_2 = 3;

int Speed = 255;  //choose any speed in the range [0, 255]
int currentTime;
int period=200;

#define falsepulseDelay 20 // noise pulse time  , if too high, ISR will miss pulses.                          
volatile long lastDebounceTime_1 = 0;   // the last time the interrupt was triggered
volatile int counter_1=0;
int prevCounter_1=0;
int Direction=1;    //-1 = retracting
                    // 0 = stopped
                    // 1 = extending
volatile long lastDebounceTime_2 = 0;   // the last time the interrupt was triggered
volatile int counter_2=0;
int prevCounter_2=0;
//int Direction_2=1; 

int strokeLength = 3;  //enter your stroke length here (in inches)
float Position = 0.0;

int rightPWM;
int leftPWM;

int extensionCount_1;
int retractionCount_1;
int extensionCount_2;
int retractionCount_2;

int actuatorCount1;
int actuatorCount2;

void setup() {
  pinMode(RPWM_1, OUTPUT);
  pinMode(LPWM_1, OUTPUT);
  pinMode(opticalPin_1, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(opticalPin_1), count_1, RISING);
  pinMode(RPWM_2, OUTPUT);
  pinMode(LPWM_2, OUTPUT);
  pinMode(opticalPin_2, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(opticalPin_2), count_2, RISING);
  
  Serial.begin(9600);
  Serial.println("Initializing calibration");
  Serial.println("Actuator retracting...");

  moveTillLimit(-1, 255);
  /*do{
    //keep retracting until counter remains the same for 100ms (i.e at the end)    
    prevCounter_1=counter_1;
    prevCounter_2=counter_2;
    timeElapsed=0;
    while(timeElapsed<period){
      driveActuator(1,-1, 255);  //retract actuator
      driveActuator(2,-1, 255);  //retract actuator
    }
  }while(prevCounter_1!=counter_1 or prevCounter_2!=counter_2);
  */
  
  Serial.println("Actuator fully retracted");
  delay(1000);

  //reset variables
  timeElapsed=0;
  Direction=1;
  counter_1=0;
  counter_2=0;
  prevCounter_1=0;
  prevCounter_2=0;

  Serial.print("\n\t\t\t\tActuator 1\t\t\t");
  Serial.println("Actuator 2");
  
  Serial.print("Extension Count:\t\t");
  moveTillLimit(1, 255); //extend fully
  extensionCount_1=counter_1;
  extensionCount_2=counter_2;
  Serial.print(extensionCount_1);
  Serial.print("\t\t\t\t");  
  Serial.println(extensionCount_2);
  delay(500);

  counter_1=0;
  counter_2=0;
  
  Serial.print("Retraction Count:\t\t");
  moveTillLimit(-1, 255); //retract fully
  retractionCount_1=counter_1;
  retractionCount_2=counter_2;
  Serial.print(retractionCount_1);
  Serial.print("\t\t\t\t");  
  Serial.println(retractionCount_2);
  delay(500);

  Serial.print("\nActuator 1 average pulses: ");
  actuatorCount1=(extensionCount_1+retractionCount_1)/2;  //take average of two measurements
  Serial.println(actuatorCount1);

  Serial.print("Actuator 2 average pulses: ");
  actuatorCount2=(extensionCount_2+retractionCount_2)/2;
  Serial.println(actuatorCount2);

  Serial.println("\nEnter these values in the syncronous control progam.");
}

void moveTillLimit(int Direction, int Speed){
  do{
    //keep retracting until counter remains the same for 100ms (i.e at the end)    
    prevCounter_1=counter_1;
    prevCounter_2=counter_2;
    timeElapsed=0;
    while(timeElapsed<period){
      driveActuator(1,Direction, Speed);  //retract actuator
      driveActuator(2,Direction, Speed);  //retract actuator
    }
  }while(prevCounter_1!=counter_1 or prevCounter_2!=counter_2);
}

void driveActuator(int Actuator, int Direction, int Speed){
  if(Actuator==1){
    rightPWM=RPWM_1;
    leftPWM=LPWM_1;
  }
  else if(Actuator==2){
    rightPWM=RPWM_2;
    leftPWM=LPWM_2;
  }
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

/*  do{
    //keep retracting until counter remains the same for 100ms    
    prevCounter_1=counter_1;
    prevCounter_2=counter_2;
    timeElapsed=0;
    while(timeElapsed<period){   //ensures the end is reached and also
      driveActuator(1, Direction, Speed);  //move actuator
      driveActuator(2, Direction, Speed); 
    }
  }while(prevCounter_1!=counter_1 or prevCounter_2!=counter_2);
  
  delay(500);      //in addition to a pause the delay will count any brief overshoot after stopping the motor
  
  if(Direction==1){
    extensionCount_1=counter_1;
    extensionCount_2=counter_2;
    Serial.print(extensionCount_1);
    Serial.print(", ");
    Serial.print(extensionCount_2);
    Serial.print("\t\t\t\t");  
  }
  else if(Direction==-1){
    retractionCount_1=abs(counter_1);
    retractionCount_2=abs(counter_2);
    Serial.print(retractionCount_1);
    Serial.print(", ");
    Serial.print(retractionCount_2);
    Serial.println();
  }
    
  Direction*=-1; ///reverse the direction
  counter_1=0;   //reset counter
  counter_2=0;
}
*/

void count_1(){
  if ((millis() - lastDebounceTime_1) > falsepulseDelay) {    //reduce noise by debouncing IR signal 
    lastDebounceTime_1 = millis();
    if(Direction==1){
      counter_1++;
    }
    if(Direction==-1){
      counter_1--;
    }
  }
  //add in error if direction==0 and interrupt triggers; due to overshoot
}

void count_2(){
  if ((millis() - lastDebounceTime_2) > falsepulseDelay) {    //reduce noise by debouncing IR signal 
    lastDebounceTime_2 = millis();
    if(Direction==1){
      counter_2++;
    }
    if(Direction==-1){
      counter_2--;
    }
  }
  //add in error if direction==0 and interrupt triggers; due to overshoot
}
