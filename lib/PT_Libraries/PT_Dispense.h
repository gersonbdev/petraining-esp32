#include "PT_Constants.h"
#include "PT_LoadCell.h"

// Dispensation QUANTITY control variables
int QuantityToDispense; // quantity to dispense in one try
int InternalWeight;
int ExternalWeight;
int ActualQuantityToDispense;

void linearActuatorForwards(){
  Serial.println("LINEAR ACTUATOR GOES FORWARD !!!!"); Serial.println("");

  // Time = LINEAR_ACTUATOR_TIME_DELAY x delay(ms) [Seconds]
  for(int i=0; i<=LINEAR_ACTUATOR_TIME_DELAY; i++){
    digitalWrite(LINEAR_MOTOR_R, HIGH);
    digitalWrite(LINEAR_MOTOR_L, LOW);
    delay(1000);
    Serial.println("Peso externo = "+String(ExternalWeight)+" grams"); Serial.println("");
  }

  Serial.println("LINEAR ACTUATOR STOPPED !!!!"); Serial.println("");
  digitalWrite(LINEAR_MOTOR_R, LOW); 
  digitalWrite(LINEAR_MOTOR_L, LOW);
  delay(100);
}

void linearActuatorBackwards(){
  Serial.println("LINEAR ACTUATOR GOES BACKWARD !!!!"); Serial.println(""); Serial.println("");

  // Time = LINEAR_ACTUATOR_TIME_DELAY x delay(ms) [Seconds]
  for(int i=0; i<=LINEAR_ACTUATOR_TIME_DELAY; i++){
    digitalWrite(LINEAR_MOTOR_R, LOW); 
    digitalWrite(LINEAR_MOTOR_L, HIGH);
    delay(1000);
    Serial.println("Peso externo = "+String(ExternalWeight)+" grams"); Serial.println("");
  }

  Serial.println("LINEAR ACTUATOR STOPPED !!!!"); Serial.println("");
  digitalWrite(LINEAR_MOTOR_R, LOW); 
  digitalWrite(LINEAR_MOTOR_L, LOW);
  delay(100);
}


void dispense(int QuantityToDispense){

  // Left move by using H-bridge (Linear Actuator Backwards) [0 1]
  digitalWrite(LINEAR_MOTOR_R, LOW); 
  digitalWrite(LINEAR_MOTOR_L, HIGH);
  delay(500);

  // Motor stopped
  digitalWrite(LINEAR_MOTOR_R, LOW); 
  digitalWrite(LINEAR_MOTOR_L, LOW);

  int timeCounting = 0;
  InternalWeight = internal.get_units(10); //Internal weight sense
  ExternalWeight = external.get_units(10); //External weight sense
  ActualQuantityToDispense = QuantityToDispense - ExternalWeight;

  while((InternalWeight < ActualQuantityToDispense) && (timeCounting <= TIMEOUT_DISPENSE) && (ActualQuantityToDispense > 0)){

    // Enable SCREW MOTOR to dispense
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
  Serial.println("SCREW MOTOR STOPPED !!!!"); Serial.println("");
  digitalWrite(SCREW_MOTOR, LOW);
  delay(500);

  // Right move by using H-bridge (Linear Actuator Forwards) [1 0]
  linearActuatorForwards();
  // Left move by using H-bridge (Linear Actuator Backwards) [0 1]
  linearActuatorBackwards();

}