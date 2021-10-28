#include <EEPROM.h> // EEPROM partition from flash memory

struct ptBasicData{
  unsigned int TypeOfConfig; // If 1 -> easy configuration mode, if 0 -> customized configuration mode
  unsigned int RealTimeApp[6]; // DateTime real time received from App. Values -> {year, month, day, hour, minute, second}
  unsigned int PetBirthDay[6]; // DataTime pet's birthday received from App. Values -> {year, month, day, 0, 0, 0}
  unsigned int QuantityOfFood[3]; // Quantity of food received from (should be between 50 gr and 500 gr)
  unsigned int FoodRations[3]; // Food rations will divide the quantity of food daily (should be between 2 and 4)
};

// Struct definiton
  ptBasicData ptdata;

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

  ptdata.TypeOfConfig = EEPROM.read(0);
  
  // We get values of real time and date from EEPROM
  ptdata.RealTimeApp[0] = (EEPROM.read(1)<<8) | (EEPROM.read(2));
  ptdata.RealTimeApp[1] = EEPROM.read(3);
  ptdata.RealTimeApp[2] = EEPROM.read(4);
  ptdata.RealTimeApp[3] = EEPROM.read(5);
  ptdata.RealTimeApp[4] = EEPROM.read(6);
  ptdata.RealTimeApp[5] = EEPROM.read(7);
  
  // We get values of pet's age form EEPROM to our calculation variables
  ptdata.PetBirthDay[0] = (EEPROM.read(8)<<8) | (EEPROM.read(9));
  ptdata.PetBirthDay[1] = EEPROM.read(10);
  ptdata.PetBirthDay[2] = EEPROM.read(11);
  ptdata.PetBirthDay[3] = EEPROM.read(12);
  ptdata.PetBirthDay[4] = EEPROM.read(13);
  ptdata.PetBirthDay[5] = EEPROM.read(14);

  // Quantity of food for everyone case
  ptdata.QuantityOfFood[0] = (EEPROM.read(15)<<8) | (EEPROM.read(16)); // Quantity of food if younger than 3 months
  ptdata.QuantityOfFood[1] = (EEPROM.read(17)<<8) | (EEPROM.read(18)); // Quantity of food if between 3 and 6 months
  ptdata.QuantityOfFood[2] = (EEPROM.read(19)<<8) | (EEPROM.read(20)); // Quantity of food if elder than 6 months

  // Food rations for everyone case
  ptdata.FoodRations[0] = EEPROM.read(21); // Food rations if younger than 3 months
  ptdata.FoodRations[1] = EEPROM.read(22); // Food rations if between 3 and 6 months
  ptdata.FoodRations[2] = EEPROM.read(23); // Food rations if elder than 6 months

}