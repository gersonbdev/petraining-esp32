// Libraries
#include <RTClib.h> // Real Time Clock module
#include <EEPROM.h> // EEPROM partition from flash memory
#include <Wire.h> // I2C
#include <WiFi.h>
#include <WebServer.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// User Libraries
#include "PT_Constants.h"
#include "PT_Dispense.h"
#include "PT_EEPROM.h"

// Objects
RTC_DS3231 MyRtc;

// Dispensation TIME control variables
byte cont = 0; // counter of dispensation times throughout the day
int DispenseTimes[MAX_DISPENSE_PER_DAY][2]; // times dispensation matrix

/////////////////////////////////////////////////// USER FUNCTIONS //////////////////////////////////////////////////////////// 

/*
 * Initializes RTC configuration by using user's App information
 */
void initRTC(){

  MyRtc.begin();
  delay(10);
  MyRtc.adjust( DateTime(2021, 11, 6, ptdata.ServerHour, ptdata.ServerMinute, ptdata.ServerSecond) ); // Sets real time from user's app
  //MyRtc.adjust(DateTime(2021, 11, 6, 11, 35, 49)); // TESTS
  //MyRtc.adjust( DateTime(__DATE__, __TIME__) ); // Sets real time with from our PC
}

void dispenseCalculation(){

  /**
   * Here we calculate in a "quantityoffood" rows and 2 columns matrix, the times that the system will dispense
   * from 6:00 to 20:00 range excluding 6:00 and 20:00
   */
  for(int x=0; x < ptdata.FoodRations; x++){
    DispenseTimes[x][1] = ((TIME_WINDOW)/float(ptdata.FoodRations+1)*(x+1)+6)*60; // All time in minutes
    DispenseTimes[x][0] = (DispenseTimes[x][1])/60; // Only the hour of the time (->hh:mm:ss)
    DispenseTimes[x][1] = (DispenseTimes[x][1] - DispenseTimes[x][0]*60); //Only the minute of the time (hh:->mm:ss)
  }
  
  int QuantityToDispense = ptdata.QuantityOfFood/ptdata.FoodRations; // Quantity to dispense in one try
  Serial.println("DISPENSATION TIMES");
  
  for(int y=0; y < ptdata.FoodRations; y++){
    Serial.println(String(DispenseTimes[y][0])+" hours and "+String(DispenseTimes[y][1])+" minutes");
  }
  
  /**
   * looking for no nulls values in DispenseTimes Matrix
   * and adjusting cont 
   */
  int i = 0;
  int bandera = 0;
  while(bandera == 0){

    if(MyRtc.now().hour() <= DispenseTimes[i][0]){

      if(MyRtc.now().hour() == DispenseTimes[i][0]){

        if(MyRtc.now().minute() <= DispenseTimes[i][1]){
          cont = i; bandera = 1;
        }else if(MyRtc.now().minute() > DispenseTimes[i][1]){
          i++;
        }

      }else if(MyRtc.now().hour() < DispenseTimes[i][0]){
        cont = i; bandera = 1;
      }

    }else{
      i++;
    }
    if(cont >= ptdata.FoodRations){
      cont = 0; bandera = 1;
    }
  }

  Serial.println("");
  Serial.println("Cont: "+String(cont));
  /*
   * Dispensation of food will only occurred between 6 am and 8 pm,
   * at leas for easy configuration case
   */
  if( (MyRtc.now().hour()>=TIME_INF) && (MyRtc.now().hour()<=TIME_SUP) ){
    if( (MyRtc.now().hour() == DispenseTimes[cont][0]) && (MyRtc.now().minute() == DispenseTimes[cont][1])){

      Serial.println("");
      Serial.println("Before dispense, Cont: "+String(cont));

      dispense(QuantityToDispense); // Routine that will move the linear actuator and the DC motor in order to dispense food
            
      cont++;
      if(cont >= ptdata.FoodRations){
        cont=0;
      }

      Serial.println("");
      Serial.println("After dispense, Cont: "+String(cont));
    } 
  }
}

//////////////////////////////////////////// VISUALIZING FUNCTIONS //////////////////////////////////////////////////////

void printRtcConfig(){

  Serial.println("////////// REAL TIME BEGIN ///////////");
  
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

  Serial.println("////////// REAL TIME END ///////////");
  Serial.println("");
}

void printEEPROMInfo(){
  Serial.println("////////// EEPROM INFORMATION BEGIN ///////////");

  Serial.println("Quantity of food per day: "+String( ptdata.QuantityOfFood ));
  Serial.println("Food rations per day: "+String( ptdata.FoodRations ));

  Serial.println("////////// EEPROM INFORMATION END ///////////");
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
  for(int n=0; n<MAX_DISPENSE_PER_DAY; n++){
    for(int m=0; m<2; m++){
      DispenseTimes[n][m] = 0;
    }
  }

  // Output Motor Pins
  pinMode(SCREW_MOTOR, OUTPUT);
  pinMode(LINEAR_MOTOR_R, OUTPUT);
  pinMode(LINEAR_MOTOR_L, OUTPUT);

  initLoadCells();

  EEPROM.begin(1000);
  EEPROM.commit();
  
  while(!MyRtc.begin()){
    Serial.println("");
    Serial.println("RTC module not found!!!");
    Serial.println("");
    delay(1000);
  }

}

void loop(){

  // DATA RECEIVING STAGE !!
  appDataReceiving(); // From here we should get the information we need from user's App and we storage it in EEPROM
  initLocalVariables(); // Here we get the EERPOM values to new manageable varaibles
  initRTC(); // Here we configurate the RTC using user information
  
  Serial.println("");
  Serial.println("");
  printEEPROMInfo();
  printRtcConfig();
  
  dispenseCalculation();
  
  // Visualize information with serial
  delay(10000); 
}