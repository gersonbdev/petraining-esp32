#include <EEPROM.h> // EEPROM partition from flash memory

struct ptBasicData{
  unsigned int QuantityOfFood; // Quantity of food received from (should be between 50 gr and 500 gr)
  unsigned int FoodRations; // Food rations will divide the quantity of food daily (should be between 2 and 4)
};

// Struct definiton
  ptBasicData ptdata;

/**
 * Receives the App information from WiFi to storage it in the EEPROM
 */
void appDataReceiving(){

  EEPROM.write(0, 0x01); // QuantityOfFood MSB -> example: 260
  EEPROM.write(1, 0x04); // QuantityOfFood LSB -> example: 260
  EEPROM.write(2, 4);   // FoodRations younger than 3 months
  EEPROM.commit();
}

/**
 * Initializes local variables by using EEPROM information
 */
void initLocalVariables(){
  ptdata.QuantityOfFood = (EEPROM.read(1)<<8) | (EEPROM.read(2));
  ptdata.FoodRations = EEPROM.read(3); // Food rations if younger than 3 months
}