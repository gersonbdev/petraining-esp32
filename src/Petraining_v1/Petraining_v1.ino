// Libraries
#include <WiFi.h> // WiFi
#include <WebServer.h> // WiFi
#include <RTClib.h> // Real Time Clock module
#include <EEPROM.h> // EEPROM partition from flash memory
#include <Wire.h> // I2C
#include <ESP32Servo.h> // ServoMOTORs

// CONSTANTS
#define MAXGRAMS 55
#define ADCBITS 4096

// Pinouts
#define SENSOR_INTERNAL 34 // ADC pin Output D34
#define SENSOR_EXTERNAL 35 // ADC pin Output D35
#define MOTOR 27 // Digital pin Output D27
#define SERVO_LEFT 25 // PWM pin Output D25
#define SERVO_RIGHT 26 // PWM pin Output D26

// Objects
RTC_DS3231 MyRtc;
Servo ServoLeft;
Servo ServoRight;


// Global varaibles
unsigned int TypeOfConfig; // If 1 -> easy configuration mode, if 0 -> customized configuration mode
unsigned int RealTimeApp[6]; // DateTime real time received from App. Values -> {year, month, day, hour, minute, second}
unsigned int PetBirthDay[6]; // DataTime pet's birthday received from App. Values -> {year, month, day, 0, 0, 0}
unsigned int QuantityOfFood[3]; // Quantity of food received from (should be between 50 gr and 500 gr)
unsigned int FoodRations[3]; // Food rations will divide the quantity of food daily (should be between 2 and 4)

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
      initLocalVariables(); // Here we get the EERPOM values to new manageable varaibles
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
  
  if(TypeOfConfig == 1){ // EASY CONFIGURATION CASE
    calculateTimeDispensation();
  }
  else if(TypeOfConfig == 2){ // CUSTOMIZABLE CONFIGURATION CASE
    Serial.println("Custom");
  }
  
  // Visualize information with serial
  delay(2000); 
}

/////////////////////////////////////////////////// CUSTOMIZED FUNCTIONS //////////////////////////////////////////////////////////// 
/*
 * Receives the App information from WiFi to storage it in the EEPROM
 */
void appDataReceiving(){
 /* TODO: set WiFi transmission to get TypeOfConfig, RealTimeApp, PetBirthDay, QuantityOfFood and FoodRations from user's app
  *       and storage that information in the EEPROM
  */
  
  EEPROM.write(0,1); // TypeOfConfig -> 1 easy configuration

  // SIMULATED DATE, NOT REAL DATETIME
  EEPROM.write(1,0x07); // RealTimeApp[0] MSB year 2021
  EEPROM.write(2,0xE5); // RealTimeApp[0] LSB year 2021
  EEPROM.write(3,12);   // RealTimeApp[1] month 12
  EEPROM.write(4,13);   // RealTimeApp[2] day 13
  EEPROM.write(5,20);   // RealTimeApp[3] hour 20
  EEPROM.write(6,30);   // RealTimeApp[4] minute 30
  EEPROM.write(7,15);   // RealTimeApp[5] sec 15
  
  /*
  // TEST: birthday 2020/12/21, older than 6 months
  EEPROM.write(8,0x07); // PetBirthDay[0] MSB year 2020
  EEPROM.write(9,0xE4); // PetBirthDay[0] LSB year 2020
  EEPROM.write(10,12);  // PetBirthDay[1] month 12
  EEPROM.write(11,21);  // PetBirthDay[2] day 21
  EEPROM.write(12,0);   // PetBirthDay[3] hour 0
  EEPROM.write(13,0);   // PetBirthDay[4] minute 0
  EEPROM.write(14,0);   // PetBirthDay[5] sec 0
  */

  /*
  // TEST: birthday 2021/4/21, between 3 and 6 months
  EEPROM.write(8,0x07); // PetBirthDay[0] MSB year 2021
  EEPROM.write(9,0xE5); // PetBirthDay[0] LSB year 2021
  EEPROM.write(10,4);  // PetBirthDay[1] month 4
  EEPROM.write(11,21);  // PetBirthDay[2] day 21
  EEPROM.write(12,0);   // PetBirthDay[3] hour 0
  EEPROM.write(13,0);   // PetBirthDay[4] minute 0
  EEPROM.write(14,0);   // PetBirthDay[5] sec 0
  */

  
  // TEST: birthday 2021/7/1, younger than 3 months 
  EEPROM.write(8,0x07); // PetBirthDay[0] MSB year 2020
  EEPROM.write(9,0xE5); // PetBirthDay[0] LSB year 2020
  EEPROM.write(10,7);  // PetBirthDay[1] month 12
  EEPROM.write(11,1);  // PetBirthDay[2] day 21
  EEPROM.write(12,0);   // PetBirthDay[3] hour 0
  EEPROM.write(13,0);   // PetBirthDay[4] minute 0
  EEPROM.write(14,0);   // PetBirthDay[5] sec 0
  
 
  EEPROM.write(15,0x00); // QuantityOfFood MSB 120 gr younger than 3 months
  EEPROM.write(16,0x78); // QuantityOfFood LSB 120 gr younger than 3 months
  EEPROM.write(17,0x00); // QuantityOfFood MSB 190 gr between 3 and 6 months
  EEPROM.write(18,0xBE); // QuantityOfFood LSB 190 gr between 3 and 6 months
  EEPROM.write(19,0x01); // QuantityOfFood MSB 260 gr older than 6 months
  EEPROM.write(20,0x04); // QuantityOfFood LSB 260 gr older than 6 months 
  
  EEPROM.write(21,4);   // FoodRations younger than 3 months
  EEPROM.write(22,3);   // FoodRations between 3 and 6 months
  EEPROM.write(23,2);   // FoodRations elder than 6 months
  EEPROM.commit();
  
}

/*
 * Initializes local variables by using EEPROM information
 */
void initLocalVariables(){

  TypeOfConfig = EEPROM.read(0);
  
  // We get values of real time and date from EEPROM
  RealTimeApp[0] = (EEPROM.read(1)<<8) | (EEPROM.read(2));
  RealTimeApp[1] = EEPROM.read(3);
  RealTimeApp[2] = EEPROM.read(4);
  RealTimeApp[3] = EEPROM.read(5);
  RealTimeApp[4] = EEPROM.read(6);
  RealTimeApp[5] = EEPROM.read(7);
  
  // We get values of pet's age form EEPROM to our calculation variables
  PetBirthDay[0] = (EEPROM.read(8)<<8) | (EEPROM.read(9));
  PetBirthDay[1] = EEPROM.read(10);
  PetBirthDay[2] = EEPROM.read(11);
  PetBirthDay[3] = EEPROM.read(12);
  PetBirthDay[4] = EEPROM.read(13);
  PetBirthDay[5] = EEPROM.read(14);

  // Quantity of food for everyone case
  QuantityOfFood[0] = (EEPROM.read(15)<<8) | (EEPROM.read(16)); // Quantity of food if younger than 3 months
  QuantityOfFood[1] = (EEPROM.read(17)<<8) | (EEPROM.read(18)); // Quantity of food if between 3 and 6 months
  QuantityOfFood[2] = (EEPROM.read(19)<<8) | (EEPROM.read(20)); // Quantity of food if elder than 6 months

  // Food rations for everyone case
  FoodRations[0] = EEPROM.read(21); // Food rations if younger than 3 months
  FoodRations[1] = EEPROM.read(22); // Food rations if between 3 and 6 months
  FoodRations[2] = EEPROM.read(23); // Food rations if elder than 6 months
}

/*
 * Initializes RTC configuration by using user's App information
 */
void initRTC(){
  //MyRtc.adjust( DateTime(RealTimeApp[0], RealTimeApp[1], RealTimeApp[2], RealTimeApp[3], RealTimeApp[4], RealTimeApp[5]) ); // Sets real time from user's app
  MyRtc.adjust(DateTime(2021, 8, 25, 8, 47, 50)); // TESTS
  //MyRtc.adjust( DateTime(__DATE__, __TIME__) ); // Sets real time with from our PC
}

/*
 * Calculates pet's age by using MyRtc instance
 */
void calculatePetAge(){
  // Calculation pet's age
  Age2Days = (MyRtc.now().year()*365 + MyRtc.now().month()*30 + MyRtc.now().day()+1) - (PetBirthDay[0]*365 + PetBirthDay[1]*30 + PetBirthDay[2]);

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
      quantityoffood = QuantityOfFood[0];
      foodrations = FoodRations[0];
    }
    else if(Age2Days>90 and Age2Days<=180){ // Case 2: the pet is between 90 and 180 days -> 3 and 6 months
      quantityoffood = QuantityOfFood[1];
      foodrations = FoodRations[1];
    }
    else if(Age2Days>180){ // Case 3: the pet is elder than 180 days -> 6 months  
      quantityoffood = QuantityOfFood[2];
      foodrations = FoodRations[2];
    }
  //}

  /*
   * Here we calculate in a "foodrations" rows and 2 columns matrix, the times that the system will dispense
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
  Serial.println("Configuration = "+String(TypeOfConfig));
  Serial.println("");
  
  Serial.println("User's date (simulated - NOT REAL DATE): "+String( RealTimeApp[0] )+"/"+String(RealTimeApp[1])+"/"+String(RealTimeApp[2])+" user's time: "+String(RealTimeApp[3])+":"+String(RealTimeApp[4])+":"+String(RealTimeApp[5]));
  Serial.println("");
  
  Serial.println("Pet's birthday: "+String( PetBirthDay[2] )+"/"+String(PetBirthDay[1])+"/"+String(PetBirthDay[0])+" time: "+String(PetBirthDay[3])+":"+String(PetBirthDay[4])+":"+String(PetBirthDay[5]));
  Serial.println("");
  
  Serial.println("QuantityOfFood if younger than 3 months: "+String( QuantityOfFood[0] ));
  Serial.println("QuantityOfFood if between 3 and 6 months: "+String( QuantityOfFood[1] ));
  Serial.println("QuantityOfFood if older than 6 months: "+String( QuantityOfFood[2] ));
  Serial.println("");
  
  Serial.println("FoodRations if younger than 3 months: "+String( FoodRations[0] ));
  Serial.println("FoodRations if between 3 and 6 months: "+String( FoodRations[1] ));
  Serial.println("FoodRations if older than 6 months: "+String( FoodRations[2] ));
  Serial.println("");
}
