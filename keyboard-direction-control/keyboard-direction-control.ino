/* Firgelli Automations
 * Limited or no support: we do not have the resources for Arduino code support
 * To report software bugs contact: support@firgelliauto.com
 * 
 * This progam extends allows the user to control the direction of motion of one of our linear
 * actuators using the "e"/"r"/"s" (extend/retract/stop) keys on a keyboard. 
 * Note: to use this function the keys "e", "r", or "s" must be entered one at a time in the 
 * Arduino serial monitor followed by pressing the "Enter" key. 
 *
 * To use keys on the keyboard without the "Enter" key requires additional software such as
 * Processing. We encourage you to try this if you want, but will require experience with
 * programming. Firgelli Automations will not provide support for this application.
 *  
 * This program requires a IBT-2 H-Bridge, Arduino Uno, 
 * and one of our supported linear actuators:
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

int Speed = 255;  //choose any speed in the range [0, 255]

void setup() {
  Serial.begin(9600);
  pinMode(RPWM, OUTPUT);
  pinMode(LPWM, OUTPUT);
}

void loop() {  
  if(Serial.available()>0){         //wait for user input via the keyboard
    char inChar = Serial.read();    //read incoming serial data
    if(inChar=='e'){                //extend actuator
      analogWrite(RPWM, Speed);
      analogWrite(LPWM, 0);
    }
    else if(inChar=='r'){           //retract actuator
      analogWrite(RPWM, 0);
      analogWrite(LPWM, Speed);
    }
    else if(inChar=='s'){           //stop actuator
      analogWrite(RPWM, 0);
      analogWrite(LPWM, 0);
    }
  }
}

