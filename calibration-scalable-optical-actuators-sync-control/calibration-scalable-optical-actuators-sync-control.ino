/* Written by Firgelli Automations
 * Limited or no support: we do not have the resources for Arduino code support
 * This code exists in the public domain
 * 
 * Program requires two (or more) of our supported linear actuators:
 * FA-OS-35-12-XX
 * FA-OS-240-12-XX
 * FA-OS-400-12-XX
 * Products available for purchase from https://www.firgelliauto.com/collections/linear-actuators/products/optical-sensor-actuators
 */

#include <elapsedMillis.h>
elapsedMillis timeElapsed;

#define numberOfActuators 2               
int RPWM[numberOfActuators]={11, 9};        //PWM signal right side
int LPWM[numberOfActuators]={10,6};       
int opticalPins[numberOfActuators]={2,3};   //connect optical pins to interrupt pins on Arduino. More information: https://www.arduino.cc/reference/en/language/functions/external-interrupts/attachinterrupt/
volatile long lastDebounceTime_0=0;         //timer for when interrupt was triggered
volatile long lastDebounceTime_1=0;

int Speed = 255;                            //choose any speed in the range [0, 255]
int period=200;                             //interval of waiting time to verify actuator as reached its limit
int currentTime;

#define falsepulseDelay 20                  //noise pulse time, if too high, ISR will miss pulses.                          
volatile int counter[numberOfActuators]={};   
int prevCounter[numberOfActuators]={};     
int Direction=1;                            //-1 = retracting
                                            // 0 = stopped
                                            // 1 = extending

int extensionCount[numberOfActuators] = {};  
int retractionCount[numberOfActuators] = {};  
int pulseTotal[numberOfActuators]={};   //stores number of pulses in one full extension/actuation

void setup(){
  for(int i=0; i<numberOfActuators; i++){
    pinMode(RPWM[i],OUTPUT);
    pinMode(LPWM[i], OUTPUT);
    pinMode(opticalPins[i], INPUT_PULLUP);
  }
  attachInterrupt(digitalPinToInterrupt(opticalPins[0]), count_0, RISING);
  attachInterrupt(digitalPinToInterrupt(opticalPins[1]), count_1, RISING);  

  memset(counter, 0, numberOfActuators);              //initialize as an array of zeros 
  memset(prevCounter, 0, numberOfActuators);  
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
    Serial.print("\t\t\t\tActuator ");
    Serial.print(i);
  }
      
  moveTillLimit(1, 255); //extend fully and count pulses
  Serial.print("\nExtension Count:");
  for(int i=0; i<numberOfActuators; i++){
    extensionCount[i]=counter[i];
    Serial.print("\t\t");  
    Serial.print(extensionCount[i]);
    Serial.print("\t\t\t");  
  }
  delay(1000);
  
  moveTillLimit(-1, 255); //retract fully and count pulses
  Serial.print("\nRetraction Count:");
  for(int i=0; i<numberOfActuators; i++){
    retractionCount[i]=counter[i];
    Serial.print("\t\t");  
    Serial.print(retractionCount[i]);
    Serial.print("\t\t\t");  
  }
  delay(1000);

  for(int i=0; i<numberOfActuators; i++){
    Serial.print("\nActuator ");
    Serial.print(i);
    Serial.print(" average pulses: ");
    pulseTotal[i]=(extensionCount[i]+retractionCount[i])/2;   //takes the average of measurements
    Serial.print(pulseTotal[i]);  
  }  
  Serial.println("\n\nEnter these values in the synchronous control progam.");
}

void loop() {  
}

void moveTillLimit(int Direction, int Speed){
  //this function moves the actuator to one of its limits
  for(int i = 0; i < numberOfActuators; i++){
    counter[i] = 0;                                     //reset counter variables
    prevCounter[i] = 0;
  }  
  do {
    for(int i = 0; i < numberOfActuators; i++) {
      prevCounter[i] = counter[i];
    }
    timeElapsed = 0;
    while(timeElapsed < period){                       //keep moving until counter remains the same for the duration of "period" variable  
      for(int i = 0; i < numberOfActuators; i++) {
        driveActuator(i, Direction, Speed);
      }
    }
  } while(compareCounter(prevCounter, counter));      //loop until all counts remain the same
}

bool compareCounter(int prevCounter[], int counter[]){
  //compares two arrays and returns false when every element of one array is the same as its corresponding indexed element in the other array
  bool areUnequal = true;
  for(int i = 0; i < numberOfActuators; i++){
    if(prevCounter[i] == counter[i]){
      areUnequal = false;
    } 
    else{                                             //if even one pair of elements are unequal the entire function returns true
      areUnequal = true;
      break;
    }
  }
  return areUnequal;
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

void count_0(){
  //This interrupt function increments a counter corresponding to changes in the optical pin status
  if ((millis() - lastDebounceTime_0) > falsepulseDelay) {    //reduce noise by debouncing IR signal 
    lastDebounceTime_0 = millis();
    if(Direction==1){
      counter[0]++;
    }
    if(Direction==-1){
      counter[0]--;
    }
  }
}

void count_1(){
  if ((millis() - lastDebounceTime_1) > falsepulseDelay) {   
    lastDebounceTime_1 = millis();
    if(Direction==1){
      counter[1]++;
    }
    if(Direction==-1){
      counter[1]--;
    }
  }
}
