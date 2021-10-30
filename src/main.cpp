// Libraries
#include <RTClib.h> // Real Time Clock module
#include <EEPROM.h> // EEPROM partition from flash memory
#include <Wire.h> // I2C

// User Libraries
#include "PT_Constants.h"
#include "PT_Dispense.h"
#include "PT_EEPROM.h"

// Objects
RTC_DS3231 MyRtc;

// Pet's age calculation control variables
unsigned int Age2Days; // Pet's age in days

// Dispensation TIME control variables
byte cont = 0; // counter of dispensation times throughout the day
int foodrations; // rations we will dispense throughout the day
int quantityoffood; // quantity of food (gr) we'd like to dispense per day
int DispenseTimes[10][2]; // times dispensation matrix

/////////////////////////////////////////////////// USER FUNCTIONS //////////////////////////////////////////////////////////// 

/*
 * Initializes RTC configuration by using user's App information
 */
void initRTC(){

  MyRtc.begin();
  //MyRtc.adjust( DateTime(ptdata.RealTimeApp[0], ptdata.RealTimeApp[1], ptdata.RealTimeApp[2], ptdata.RealTimeApp[3], ptdata.RealTimeApp[4], ptdata.RealTimeApp[5]) ); // Sets real time from user's app
  //MyRtc.adjust(DateTime(2021, 8, 25, 8, 47, 50)); // TESTS
  MyRtc.adjust( DateTime(__DATE__, __TIME__) ); // Sets real time with from our PC
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

void calculateEasyTimeDispensation(){
  /* 
   *  This guarantees to us that calculation of Age2Days won't affect to
   *  calculation time dispensation
   */
//if( (MyRtc.now().hour()>=23) && (MyRtc.now().hour()<=1) ){ // This line causes problems 
  if(Age2Days<=90){ // Case 1: the pet is younger than 90 days -> 3 months
    quantityoffood = ptdata.QuantityOfFood[0];
    foodrations = ptdata.FoodRations[0];
  }
  else if(Age2Days>90 && Age2Days<=180){ // Case 2: the pet is between 90 and 180 days -> 3 and 6 months
    quantityoffood = ptdata.QuantityOfFood[1];
    foodrations = ptdata.FoodRations[1];
  }
  else if(Age2Days>180){ // Case 3: the pet is elder than 180 days -> 6 months  
    quantityoffood = ptdata.QuantityOfFood[2];
    foodrations = ptdata.FoodRations[2];
  }
//}

  /*
   * Here we calculate in a "quantityoffood" rows and 2 columns matrix, the times that the system will dispense
   * from 6:00 to 20:00 range excluding 6:00 and 20:00
   */
  for(int x=0; x<foodrations; x++){
    DispenseTimes[x][1] = ((TIME_WINDOW)/float(foodrations+1)*(x+1)+6)*60; // All time in minutes
    DispenseTimes[x][0] = (DispenseTimes[x][1])/60; // Only the hour of the time (->hh:mm:ss)
    DispenseTimes[x][1] = (DispenseTimes[x][1] - DispenseTimes[x][0]*60); //Only the minute of the time (hh:->mm:ss)
  }
  
  QuantityToDispense = quantityoffood/foodrations; // Quantity to dispense in one try
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
  if( (MyRtc.now().hour()>=TIME_INF) && (MyRtc.now().hour()<=TIME_SUP) ){
    if( (MyRtc.now().hour() == DispenseTimes[cont][0]) && (MyRtc.now().minute() == DispenseTimes[cont][1])){
      cont++;
      Serial.println("");
      Serial.println("Cont: "+String(cont));
      dispense(QuantityToDispense); // Routine that will move the linear actuator and the DC motor in order to dispense food
      
      if(cont >= foodrations)
        cont=0;
    } 
  }
}

//////////////////////////////////////////// VISUALIZING FUNCTIONS //////////////////////////////////////////////////////

void printRtcConfig(){

  Serial.println("REAL TIME////////// BEGIN ///////////");
  
  DateTime realDateTime = MyRtc.now();
  Serial.print("DATE: ");
  Serial.print(realDateTime.day());
  Serial.print("/");
  Serial.print(realDateTime.month());
  Serial.print("/");
  Serial.print(realDateTime.year());
  Serial.print(" || TIME: ");
  Serial.print(realDateTime.hour());
  Serial.print(":");
  Serial.print(realDateTime.minute());
  Serial.print(":");
  Serial.println(realDateTime.second());

  Serial.println("REAL TIME////////// END ///////////");
  Serial.println("");
}

void printEEPROMInfo(){
  Serial.println("EEPROM INFORMATION////////// BEGIN ///////////");
  Serial.println("");
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
  Serial.println("EEPROM INFORMATION////////// END ///////////");
  Serial.println("");
}

////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////-----SETUP-----///////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////

void setup(){
  Serial.begin(115200);

  // Initialize dispensation times matrix in 0
  for(int n=0; n<6; n++){
    for(int m=0; m<2; m++){
      DispenseTimes[n][m] = 0;
    }
  }  

  // Output Motor Pins (possible to use PWM)
  pinMode(SCREW_MOTOR, OUTPUT);
  pinMode(LINEAR_MOTOR_R, OUTPUT);
  pinMode(LINEAR_MOTOR_L, OUTPUT);

  initLoadCells();

  EEPROM.begin(50); // Gives us 50 bytes from the eeprom (flash memory o ESP32)
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
  if( (!SETUP_EASY) && (!SETUP_CUSTOM) ){ // Only if EEPROM.read(0) is 1 or 0 we go forward

    Serial.print("Waiting for receiving WiFi transmission...");
    
    while( (!SETUP_EASY) && (!SETUP_CUSTOM) ){
      
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
  
  Serial.println("");
  Serial.println("");
  printEEPROMInfo();
  printRtcConfig();
  
  // DISPENSATION STAGE !!
  calculatePetAge(); // We always need to calculate pet's age
  
  if(ptdata.TypeOfConfig == CONFIG_EASY){ // EASY CONFIGURATION CASE
    calculateEasyTimeDispensation();
  }
  else if(ptdata.TypeOfConfig == CONFIG_CUSTOM){ // CUSTOMIZABLE CONFIGURATION CASE
    Serial.println("Custom");
  }
  
  // Visualize information with serial
  delay(10000); 
}