// Libraries
#include <WiFi.h> // WiFi
#include <WebServer.h> // WiFi
#include <RTClib.h> // Real Time Clock module
#include <EEPROM.h> // EEPROM partition from flash memory
#include <Wire.h> // I2C
#include <ESP32Servo.h> // ServoMOTORs

// User Libraries
#include "inc/PT_Libraries/PT_Defines.h"
#include "inc/PT_Libraries/PT_EEPROM.h"
#include "inc/PT_Libraries/PT_WIFI.h"

// Objects
RTC_DS3231 MyRtc;
Servo ServoLeft;
Servo ServoRight;

ptBasicData ptdata;

// Pet's age calculation control variables
unsigned int Age2Days; // Pet's age in days

// Dispensation TIME control variables
byte cont = 0;
int foodrations; // rations we will dispense throughout the day
int quantityoffood; // quantity of food (gr) we'd like to dispense per day
int DispenseTimes[6][2];


// Dispensation QUANTITY control variables
float QuantityToDispense; // quantity to dispense in one attemp
int ExternalWeight;
int InternalWeight;
int ExternalWeightRead;
int InternalWeightRead;

/////////////////////////////// END VARIABLES ///////////////////////////////////
void setup(){
  
  for(int n=0; n<6; n++){
    for(int m=0; m<2; m++){
      DispenseTimes[n][m] = 0;
    }
  }  
  pinMode(MOTOR, OUTPUT);
  initServos();
  Serial.begin(115200);
  EEPROM.begin(50);
  EEPROM.write(0,0); // Test
  EEPROM.commit(); // Test
  
  while(!MyRtc.begin()){
    Serial.println("RTC module not found!");
    Serial.println("");
    delay(1000);
  }
}

void loop(){

  // DATA RECEIVING STAGE !!
  if( (EEPROM.read(0)!=1) && (EEPROM.read(0)!=2) ){ // Only if EEPROM.read(0) is 1 or 0 we go forward
    Serial.print("Waiting for receiving WiFi transmission...");
    while( (EEPROM.read(0)!=1) && (EEPROM.read(0)!=2) ){
      
      appDataReceiving(); // From here we should get the information we need from user's App and we storage it in EEPROM
      ptdata = initLocalVariables(); // Here we get the EERPOM values to new manageable varaibles
      initRTC(); // Here we configurate the RTC using user information
      
      delay(1000);
      Serial.println(".");
    }
    Serial.println("");
    Serial.println("Wifi transmission received -> OK");
    Serial.println("");
  }
  
  Serial.println("////////////////////////////////////////////////////////////////////////////");
  printEEPROMInfo();
  printRtcConfig();
  
  // DISPENSATION STAGE !!
  calculatePetAge(); // We always need to calculate pet's age
  
  if(ptdata.TypeOfConfig == 1){ // EASY CONFIGURATION CASE
    calculateTimeDispensation();
  }
  else if(ptdata.TypeOfConfig == 2){ // CUSTOMIZABLE CONFIGURATION CASE
    Serial.println("Custom");
  }
  
  // Visualize information with serial
  delay(2000); 
}

/////////////////////////////////////////////////// CUSTOMIZED FUNCTIONS //////////////////////////////////////////////////////////// 


/*
 * Initializes RTC configuration by using user's App information
 */
void initRTC(){
  //MyRtc.adjust( DateTime(ptdata.RealTimeApp[0], ptdata.RealTimeApp[1], ptdata.RealTimeApp[2], ptdata.RealTimeApp[3], ptdata.RealTimeApp[4], ptdata.RealTimeApp[5]) ); // Sets real time from user's app
  MyRtc.adjust(DateTime(2021, 8, 25, 8, 47, 50)); // TESTS
  //MyRtc.adjust( DateTime(__DATE__, __TIME__) ); // Sets real time with from our PC
}

/*
 * Calculates pet's age by using MyRtc instance
 */
void calculatePetAge(){
  // Calculation pet's age
  Age2Days = (MyRtc.now().year()*365 + MyRtc.now().month()*30 + MyRtc.now().day()+1) - (ptdata.PetBirthDay[0]*365 + ptdata.PetBirthDay[1]*30 + ptdata.PetBirthDay[2]);

  // Prints total days and pet's age
  Serial.println("PET'S AGE");
  Serial.println("Total Days: "+String(Age2Days));
  Serial.println("");
}

void calculateTimeDispensation(){
  /* 
   *  This guarantees to us that calculation of Age2Days won't affect to
   *  calculation time dispensation
   */
  //if( (MyRtc.now().hour()>=0) && (MyRtc.now().hour()<=1) ){ 
    if(Age2Days<=90){ // Case 1: the pet is younger than 90 days -> 3 months
      quantityoffood = ptdata.QuantityOfFood[0];
      foodrations = ptdata.FoodRations[0];
    }
    else if(Age2Days>90 and Age2Days<=180){ // Case 2: the pet is between 90 and 180 days -> 3 and 6 months
      quantityoffood = ptdata.QuantityOfFood[1];
      foodrations = ptdata.FoodRations[1];
    }
    else if(Age2Days>180){ // Case 3: the pet is elder than 180 days -> 6 months  
      quantityoffood = ptdata.QuantityOfFood[2];
      foodrations = ptdata.FoodRations[2];
    }
  //}

  /*
   * Here we calculate in a "ptdata.FoodRations" rows and 2 columns matrix, the times that the system will dispense
   * from 6:00 to 20:00 range excluding 6:00 and 20:00
   */
  for(int x=0; x<foodrations; x++){
    DispenseTimes[x][1] = (14/float(foodrations+1)*(x+1)+6)*60; // All time in minutes
    DispenseTimes[x][0] = (DispenseTimes[x][1])/60; // Just the hour of the time (->hh:mm:ss)
    DispenseTimes[x][1] = (DispenseTimes[x][1] - DispenseTimes[x][0]*60); //Just the minute of the time (hh:->mm:ss)
  }
  Serial.println("DISPENSATION TIMES");
  
  for(int y=0; y<foodrations; y++){
    Serial.println(String(DispenseTimes[y][0])+" hours and "+String(DispenseTimes[y][1])+" minutes");
  }
  
  Serial.println("");
  Serial.println("Cont: "+String(cont));
  /*
   * Dispensation of food will only occurred between 6 am and 8 pm,
   * at leas for easy configuration case
   */
  if( (MyRtc.now().hour()>=6) && (MyRtc.now().hour()<=20) ){
    if( (MyRtc.now().hour() == DispenseTimes[cont][0]) && (MyRtc.now().minute() == DispenseTimes[cont][1])){ // DispenseTimes[cont][0]
      cont++;
      Serial.println("");
      Serial.println("Cont: "+String(cont));
      dispense();
      
      if(cont>=foodrations)
        cont=0;
    } 
  }
}
/*
 * Routine of dispense
 */
void dispense(){

  QuantityToDispense = quantityoffood/foodrations;
  
  InternalWeightRead = analogRead(SENSOR_INTERNAL);
  InternalWeight = map(InternalWeightRead, 0, ADCBITS, 0, MAXGRAMS); // ADC D34
  ExternalWeightRead = analogRead(SENSOR_EXTERNAL);
  ExternalWeight = map(ExternalWeightRead, 0, ADCBITS, 0, MAXGRAMS); // ADC D35
  
  // Quantity of food to dispense calculation
  float RealDispense = QuantityToDispense - ExternalWeight;

  // Retract servos
  ServoLeft.write(180);
  ServoRight.write(0);
  delay(1000);
  
  if(RealDispense > 0){
    
    while(InternalWeight < RealDispense){
      // Enable MOTOR to dispense
      digitalWrite(MOTOR, HIGH);
      
      //////////// VISUALIZING TESTS ////////////////
      InternalWeightRead = analogRead(SENSOR_INTERNAL);
      InternalWeight = map(InternalWeightRead, 0, ADCBITS, 0, MAXGRAMS); // ADC D34
      ExternalWeightRead = analogRead(SENSOR_EXTERNAL);
      ExternalWeight = map(ExternalWeightRead, 0, ADCBITS, 0, MAXGRAMS); // ADC D35
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
//////////////////////////////////////////// VISUALIZING FUNCTIONS //////////////////////////////////////////////////////

void printRtcConfig(){
  DateTime realDateTime = MyRtc.now();

  Serial.print("REAL DATE: ");
  Serial.print(realDateTime.day());
  Serial.print("/");
  Serial.print(realDateTime.month());
  Serial.print("/");
  Serial.print(realDateTime.year());
  Serial.print(" REAL TIME: ");
  Serial.print(realDateTime.hour());
  Serial.print(":");
  Serial.print(realDateTime.minute());
  Serial.print(":");
  Serial.println(realDateTime.second());
  Serial.println("");
}

void printEEPROMInfo(){
  Serial.println("EEPROM INFORMATION");
  Serial.println("If 1 -> easy configuration mode");
  Serial.println("if 2 -> customized configuration mode");
  Serial.println("Configuration = "+String(ptdata.TypeOfConfig));
  Serial.println("");
  
  Serial.println("User's date (simulated - NOT REAL DATE): "+String( ptdata.RealTimeApp[0] )+"/"+String(ptdata.RealTimeApp[1])+"/"+String(ptdata.RealTimeApp[2])+" user's time: "+String(ptdata.RealTimeApp[3])+":"+String(ptdata.RealTimeApp[4])+":"+String(ptdata.RealTimeApp[5]));
  Serial.println("");
  
  Serial.println("Pet's birthday: "+String( ptdata.PetBirthDay[2] )+"/"+String(ptdata.PetBirthDay[1])+"/"+String(ptdata.PetBirthDay[0])+" time: "+String(ptdata.PetBirthDay[3])+":"+String(ptdata.PetBirthDay[4])+":"+String(ptdata.PetBirthDay[5]));
  Serial.println("");
  
  Serial.println("ptdata.QuantityOfFood if younger than 3 months: "+String( ptdata.QuantityOfFood[0] ));
  Serial.println("ptdata.QuantityOfFood if between 3 and 6 months: "+String( ptdata.QuantityOfFood[1] ));
  Serial.println("ptdata.QuantityOfFood if older than 6 months: "+String( ptdata.QuantityOfFood[2] ));
  Serial.println("");
  
  Serial.println("ptdata.FoodRations if younger than 3 months: "+String( ptdata.FoodRations[0] ));
  Serial.println("ptdata.FoodRations if between 3 and 6 months: "+String( ptdata.FoodRations[1] ));
  Serial.println("ptdata.FoodRations if older than 6 months: "+String( ptdata.FoodRations[2] ));
  Serial.println("");
}