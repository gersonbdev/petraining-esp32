#include "PT_Constants.h"
#include "PT_LoadCell.h"

// Dispensation QUANTITY control variables
int QuantityToDispense; // quantity to dispense in one try
int InternalWeight;
int ExternalWeight;
int ActualQuantityToDispense;

void dispense(int QuantityToDispense){

  InternalWeight = internal.get_units(10); //Internal weight sense
  ExternalWeight = external.get_units(10); //External weight sense
  ActualQuantityToDispense = QuantityToDispense - ExternalWeight;

  int timeCounting = 0;
  while((InternalWeight < ActualQuantityToDispense) && (timeCounting <= TIMEOUT_DISPENSE) && (ActualQuantityToDispense > 0)){

    // Enable SCREW_MOTOR to dispense
    digitalWrite(SCREW_MOTOR, HIGH);
    InternalWeight = internal.get_units(10);
    ExternalWeight = external.get_units(10);

    //////////// VISUALIZING TESTS //////////////// 
    delay(500);
    Serial.println("DISPENSING FOOD, SCREW_MOTOR WORKING !!!!");
    Serial.println("");
    Serial.println("Cantidad a dispensar = "+String(QuantityToDispense)+" grams");
    Serial.println("Cantidad real a dispensar = "+String(ActualQuantityToDispense)+" grams");
    Serial.println("Peso interno = "+String(InternalWeight)+" grams");
    Serial.println("Peso externo = "+String(ExternalWeight)+" grams");
    Serial.println("Time out counting = "+String(timeCounting)+"/"+String(TIMEOUT_DISPENSE)+" seconds");
    Serial.println("");
    delay(500);
    //////////// VISUALIZING TESTS ////////////////

    timeCounting += 1;
  }

  if(timeCounting >= TIMEOUT_DISPENSE){
    // Notify to user app
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