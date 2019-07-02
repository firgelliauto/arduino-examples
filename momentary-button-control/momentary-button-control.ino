/* Firgelli Automations
 * Limited or no support: we do not have the resources for Arduino code support
 * To report software bugs contact: support@firgelliauto.com
 * 
 * This progam extends allows the user the control the direction of motion of one of our linear
 * actuators using momentary push buttons. While one of the buttons is held down the actuator
 * will move in its respective direction. When released the actuator will stop moving.
 *  
 * This program requires a 2 momentary switches, 2 10kOhm resistors, IBT-2 H-Bridge, 
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
 * Wire 2 momentary buttons to digital pins 12, 13. 
 * Connect 5V and GND (via 10kOhm resistor) - see diagram for details
 */

int RPWM = 10;    //connect Arduino pin 10 to IBT-2 pin RPWM
int LPWM = 11;
int downPin = 12;
int upPin = 13;

int Speed = 255;  //choose any speed in the range [0, 255]

void setup() {
  pinMode(RPWM, OUTPUT);
  pinMode(LPWM, OUTPUT);
  pinMode(downPin, INPUT);
  pinMode(upPin, INPUT);
}

void loop() {  
  if(digitalRead(upPin)==HIGH){        //check if extension button is pressed
    analogWrite(RPWM, Speed);
    analogWrite(LPWM, 0);
  }    

  else if(digitalRead(downPin)==HIGH){ //check if retraction button is pressed
    analogWrite(RPWM, 0);
    analogWrite(LPWM, Speed);
  }    

  else{                              //remain stationary
    analogWrite(RPWM, 0);
    analogWrite(LPWM, 0);
  }
}
