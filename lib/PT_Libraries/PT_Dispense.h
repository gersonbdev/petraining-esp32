#include "PT_Constants.h"
#include "PT_LoadCell.h"

// Dispensation QUANTITY control variables
int QuantityToDispense; // quantity to dispense in one try
int InternalWeight;

void dispense(int QuantityToDispense){

  InternalWeight = internal.get_units(10); // Internal weight sense
  
  float timeCounting = 0;
  while((InternalWeight < QuantityToDispense) && (timeCounting <= TIMEOUT_DISPENSE)){

    // Enable SCREW_MOTOR to dispense
    digitalWrite(SCREW_MOTOR, HIGH);
    InternalWeight = internal.get_units(10);

    //////////// VISUALIZING TESTS ////////////////    
    delay(50);
    Serial.println("DISPENSING FOOD, SCREW_MOTOR WORKING !!!!");
    Serial.println("");
    Serial.println("Real quantity to dispense = "+String(QuantityToDispense)+" grams");
    Serial.println("");
    Serial.println("Internal weight = "+String(InternalWeight)+" grams");
    Serial.println("");
    delay(50);
    //////////// VISUALIZING TESTS ////////////////

    timeCounting =+ 0.1;
  }

  // Disable SCREW MOTOR
  Serial.println("SCREW MOTOR STALLED !!!!");
  Serial.println("");
  digitalWrite(SCREW_MOTOR, LOW);
  delay(500);

  // Right move by using H-bridge (Linear Actuator Forwards) [1 0]
  Serial.println("LINEAR ACTUATOR GOES FORWARD !!!!");
  Serial.println("");
  digitalWrite(LINEAR_MOTOR_R, HIGH); 
  digitalWrite(LINEAR_MOTOR_L, LOW); 
  delay(13500);

  // Motor stalled [0 0]
  digitalWrite(LINEAR_MOTOR_R, LOW); 
  digitalWrite(LINEAR_MOTOR_L, LOW);
  delay(100);

  // Left move by using H-bridge (Linear Actuator Backwards) [0 1]
  Serial.println("LINEAR ACTUATOR GOES BACKWARD !!!!");
  Serial.println("");
  digitalWrite(LINEAR_MOTOR_R, LOW); 
  digitalWrite(LINEAR_MOTOR_L, HIGH);
  delay(13500);

  // Motor stalled
  digitalWrite(LINEAR_MOTOR_R, LOW); 
  digitalWrite(LINEAR_MOTOR_L, LOW);

}