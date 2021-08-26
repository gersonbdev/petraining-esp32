// Libraries
#include <ESP32Servo.h> // Servomotors

#define InternalSensor 34 // ADC pin Output D34
#define ExternalSensor 35 // ADC pin Output D35
#define Motor 27 // Digital pin Output D27
#define LeftServo 25 // PWM pin Output D25
#define RightServo 26 // PWM pin Output D26

Servo ServoLeft;
Servo ServoRight;

// CONSTATNS

#define MAXGRAMS 55
#define ADCBITS 4096

int quantityoffood = 250;
int foodrations = 5;
int ExternalWeight;
int InternalWeight;
int ExternalWeightRead;
int InternalWeightRead;

float QuantityToDispense;

void setup(){
  pinMode(Motor, OUTPUT);

  initServos();
  
  Serial.begin(115200);
}

void loop(){
  
  QuantityToDispense = quantityoffood/foodrations;

  Serial.println("----------------------SIMULATION STARS-------------------------");
  delay(5000);
  
  for(int x=20; x>0; x--){
    // Weight reading !!
    // Let's suppose we can read until 500 grams in both plates
    InternalWeightRead = analogRead(InternalSensor);
    InternalWeight = map(InternalWeightRead, 0, ADCBITS, 0, MAXGRAMS); // ADC D34
    
    ExternalWeightRead = analogRead(ExternalSensor);
    ExternalWeight = map(ExternalWeightRead, 0, ADCBITS, 0, MAXGRAMS); // ADC D35
    Serial.println("SET EXTERNAL AND INTERNAL WEIGHT, YOU HAVE "+String(x)+" SECONDS LEFT");
    Serial.println("");
    Serial.println("Quantity we'd like to dispense = "+String(QuantityToDispense)+" grams");
    Serial.println("");
    Serial.println("External weight = "+String(ExternalWeight)+" grams");
    Serial.println("Internal weight = "+String(InternalWeight)+" grams");
    Serial.println("");
    
    delay(1000);
  }
  
  dispense();

  Serial.println("-----------------SIMULATION TERMINATED----------------------");
  while(1){
    delay(5000);
  }
}

void dispense(){

  InternalWeightRead = analogRead(InternalSensor);
  InternalWeight = map(InternalWeightRead, 0, ADCBITS, 0, MAXGRAMS); // ADC D34
  ExternalWeightRead = analogRead(ExternalSensor);
  ExternalWeight = map(ExternalWeightRead, 0, ADCBITS, 0, MAXGRAMS); // ADC D35
  
  // Quantity of food to dispense calculation
  float RealDispense = QuantityToDispense - ExternalWeight;

  // Retract servos
  ServoLeft.write(180);
  ServoRight.write(0);
  delay(1000);
  
  if(RealDispense > 0){
    
    while(InternalWeight < RealDispense){
      // Enable motor to dispense
      digitalWrite(Motor, HIGH);
      
      //////////// VISUALIZING TESTS ////////////////
      InternalWeightRead = analogRead(InternalSensor);
      InternalWeight = map(InternalWeightRead, 0, ADCBITS, 0, MAXGRAMS); // ADC D34
      ExternalWeightRead = analogRead(ExternalSensor);
      ExternalWeight = map(ExternalWeightRead, 0, ADCBITS, 0, MAXGRAMS); // ADC D35
      Serial.println("DISPENSING FOOD, MOTOR WORKING !!!!");
      Serial.println("");
      Serial.println("Real quantity to dispense = "+String(RealDispense)+" grams");
      Serial.println("");
      Serial.println("External weight = "+String(ExternalWeight)+" grams");
      Serial.println("Internal weight = "+String(InternalWeight)+" grams");
      Serial.println("");
      delay(2000);
      //////////// VISUALIZING TESTS ////////////////
    }
    // Disable motor 
    digitalWrite(Motor, LOW);
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
    for(int x=10; x>0; x--){
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

void initServos(){
  
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);

  ServoLeft.setPeriodHertz(50); // 20 ms
  ServoRight.setPeriodHertz(50); // 20 ms
  
  ServoLeft.attach(LeftServo, 500, 2400);
  ServoRight.attach(RightServo, 500, 2400);
}
