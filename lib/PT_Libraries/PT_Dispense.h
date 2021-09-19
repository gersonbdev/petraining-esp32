#include <ESP32Servo.h> // Servos
#include "PT_LoadCell.h"

// Objects
Servo ServoLeft;
Servo ServoRight;

// Dispensation QUANTITY control variables
int QuantityToDispense; // quantity to dispense in one try
int ExternalWeight;
int InternalWeight;

/*
 * Routine of dispense
 */
void dispense(int QuantityToDispense){
  
  InternalWeight = internal.get_units(10);
  delay(50);
  ExternalWeight = external.get_units(10);
  
  // Quantity of food to dispense calculation
  float RealDispense = QuantityToDispense - ExternalWeight;

  // Retract servos (initial position)
  ServoLeft.write(180);
  ServoRight.write(0);
  delay(1000);
  
  if(RealDispense > 0){
    
    while(InternalWeight < RealDispense){
      // Enable MOTOR to dispense
      digitalWrite(MOTOR, HIGH);
      
      //////////// VISUALIZING TESTS ////////////////
      InternalWeight = internal.get_units(10);
      delay(50);
      ExternalWeight = external.get_units(10);
      Serial.println("DISPENSING FOOD, MOTOR WORKING !!!!");
      Serial.println("");
      Serial.println("Real quantity to dispense = "+String(RealDispense)+" grams");
      Serial.println("");
      Serial.println("External weight = "+String(ExternalWeight)+" grams");
      Serial.println("Internal weight = "+String(InternalWeight)+" grams");
      Serial.println("");
      delay(500);
      //////////// VISUALIZING TESTS ////////////////
    }
    // Disable MOTOR 
    digitalWrite(MOTOR, LOW);
    for(int x=5; x>0; x--){
      Serial.println("MOTOR STALLED, WAITING "+String(x)+" SECONDS TO DEPLOY SERVOS");
      delay(1000); // Dispenseation ends //////////////////////////
    }
    Serial.println("");
    
    // Deploy servos
    for(int pos=0; pos<=180; pos+=1){
      ServoLeft.write(180-pos);
      ServoRight.write(pos);
      delay(5);
    }
    for(int x=5; x>0; x--){
      Serial.println("LEAVING FOOD TO FALL DOWN. IN "+String(x)+" SECONDS SERVOS WILL RETRACT");
      delay(1000); // Dispenseation ends //////////////////////////
    }
   
    // Retract servos
    for(int pos=180; pos>=0; pos-=1){
      ServoLeft.write(180-pos);
      ServoRight.write(pos);
      delay(5);
    }
  }
}

/*
 * Servos initialization
 */
void initServos(){
  
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);

  ServoLeft.setPeriodHertz(50); // 20 ms
  ServoRight.setPeriodHertz(50); // 20 ms
  
  ServoLeft.attach(SERVO_LEFT, 500, 2400);
  ServoRight.attach(SERVO_RIGHT, 500, 2400);
}