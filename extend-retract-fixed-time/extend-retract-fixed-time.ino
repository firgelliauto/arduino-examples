/* Firgelli Automations
 * Limited or no support: we do not have the resources for Arduino code support
 * To report software bugs contact: support@firgelliauto.com
 * 
 * This progam extends a linear actuator for set time interval, defined by the variable "period" 
 * and then retracts for the same time interval. The variable "Speed" can be set as well.
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
 */

int RPWM = 10;  //connect Arduino pin 10 to IBT-2 pin RPWM
int LPWM = 11;

int Speed = 255;  //choose any speed in the range [0, 255]
unsigned long currentTime;
int period = 2500;

void setup() {
  pinMode(RPWM, OUTPUT);
  pinMode(LPWM, OUTPUT);
}

void loop() {  
  //Extension
  currentTime = millis();
  while(millis()<(period+currentTime)){
    analogWrite(RPWM, Speed);
    analogWrite(LPWM, 0);
  }    

  //Retraction
  currentTime = millis();
  while(millis()<(period+currentTime)){
    analogWrite(RPWM, 0);
    analogWrite(LPWM, Speed);
  }
}
