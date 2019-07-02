/* Firgelli Automations
 * Limited or no support: we do not have the resources for Arduino code support
 * To report software bugs contact: support@firgelliauto.com
 * 
 * This progam extends allows the user the control the speed of motion of one of our linear
 * actuators using a potentiometer. 
 *  
 * This program requires a potentiometer, 2 10kOhm resistors, IBT-2 H-Bridge, 
 * Arduino Uno, and one of our supported linear actuators:
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
 * 
 * Wire one side of the potentiometer to 5V and the other side to GND
 * Wire the middle pin of the potentiometer to pin A0 on the Arduino
 */

int RPWM = 10;    //connect Arduino pin 10 to IBT-2 pin RPWM
int LPWM = 11;
int potPin = A0;

int Speed, sensorVal;

void setup() {
  pinMode(RPWM, OUTPUT);
  pinMode(LPWM, OUTPUT);
  pinMode(potPin, INPUT);
}

void loop() {  
  sensorVal = analogRead(potPin);       //read user input from the potentiometer

  if(sensorVal >= 512){
    Speed = map(sensorVal, 512, 1023, 0, 255);
    analogWrite(RPWM, Speed);
    analogWrite(LPWM, 0);
  }

  else{
    Speed = map(sensorVal, 0, 511, 255, 0);
    analogWrite(RPWM, 0);
    analogWrite(LPWM, Speed);
  }
}
