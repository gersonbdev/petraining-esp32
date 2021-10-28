#include "PT_Constants.h"
#include "PT_LoadCell.h"

// Dispensation QUANTITY control variables
int QuantityToDispense; // quantity to dispense in one try
int InternalWeight = 15;// TEST!!!!!!!!!

/*
 * Actuators function
 */
void dispense(int QuantityToDispense){

  InternalWeight = internal.get_units(10);

  while(InternalWeight < QuantityToDispense){

    // Enable SCREW_MOTOR to dispense
    digitalWrite(SCREW_MOTOR, HIGH);

    //////////// VISUALIZING TESTS ////////////////    
    delay(50);
    Serial.println("DISPENSING FOOD, SCREW_MOTOR WORKING !!!!");
    Serial.println("");
    Serial.println("Real quantity to dispense = "+String(QuantityToDispense)+" grams");
    Serial.println("");
    Serial.println("Internal weight = "+String(InternalWeight)+" grams");
    Serial.println("");
    delay(500);
    //////////// VISUALIZING TESTS ////////////////

    InternalWeight = internal.get_units(10);
  }

  // Disable SCREW_MOTOR
  Serial.println("SCREW MOTOR STALLED !!!!");
  Serial.println("");
  digitalWrite(SCREW_MOTOR, LOW);
  delay(500);

  // Linear actuator starts pushing food out of the plate
  Serial.println("LINEAR ACTUATOR GOES FORWARD !!!!");
  Serial.println("");
  digitalWrite(LINEAR_MOTOR_R, HIGH); // Right move by using H-bridge (Linear Actuator Forwards)
  digitalWrite(LINEAR_MOTOR_L, LOW); // Right move by using H-bridge (Linear Actuator Forwards)
  delay(9000);
  digitalWrite(LINEAR_MOTOR_R, LOW); // Motor stalled
  digitalWrite(LINEAR_MOTOR_L, LOW);
  delay(100); // Shall be teste: time linear actuator takes to get plate border
  Serial.println("LINEAR ACTUATOR GOES BACKWARD !!!!");
  Serial.println("");
  digitalWrite(LINEAR_MOTOR_R, LOW); // Left move by using H-bridge (Linear Actuator Backwards)
  digitalWrite(LINEAR_MOTOR_L, HIGH); // Left move by using H-bridge (Linear Actuator Backwards)
  delay(9000);
  digitalWrite(LINEAR_MOTOR_R, LOW); // Motor stalled
  digitalWrite(LINEAR_MOTOR_L, LOW);

}