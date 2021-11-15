#include <PT_Connection.h>

struct ptBasicData{
  unsigned int QuantityOfFood;
  unsigned int FoodRations;
  unsigned int ServerHour;
  unsigned int ServerMinute;
  unsigned int ServerSecond;
  char ServerUser[50];
  char ServerPassword[50];
};


// Struct definiton
  ptBasicData ptdata;

/**
 * Receives the App information from WiFi to storage it in the EEPROM
 */
void appDataReceiving(){

  // 255 and variable -> esto guarda los primeros 8 bits
  // (0xFF00) and variable -> esto guarda los siguientes 8 bits

  EEPROM.write(0, 0x01); // QuantityOfFood MSB -> example: 260
  EEPROM.write(1, 0x04); // QuantityOfFood LSB -> example: 260
  EEPROM.write(2, 4);   // FoodRations younger than 3 months
  EEPROM.write(3, 15);   // ServerHour
  EEPROM.write(4, 30);   // ServerMinute
  EEPROM.write(5, 10);   // ServerSecond


/**
 * Initializes local variables by using EEPROM information
 */
void initLocalVariables(){
  ptdata.QuantityOfFood = (EEPROM.read(0)<<8) | (EEPROM.read(1));
  ptdata.FoodRations = EEPROM.read(2);
  ptdata.ServerHour = EEPROM.read(3);
  ptdata.ServerMinute = EEPROM.read(4);
  ptdata.ServerSecond = EEPROM.read(5);
}




  /*
  for (int i = 6; i < (6+UserPasswordChar); i++) {
    ptdata.ServerUser[i-6] = EEPROM.read(i);
      if (ptdata.ServerUser[i] == 0){
          break;
      }
  }

  for (int i = (UserPasswordChar+6); i < (UserPasswordChar+UserPasswordChar+6); i++) {
  ptdata.ServerPassword[i-6-UserPasswordChar] = EEPROM.read(i);
    if (ptdata.ServerPassword[i] == 0){
        break;
    }
  }
  */

/*
  int addr = 6; // EEPROM address to storage ServerUser (up to 55)
  char ssidUser[UserPasswordChar] = "Usuario: El string que me envía el server";
  for (int i = 0; i < UserPasswordChar; i++){
      EEPROM.write(addr, ssidUser[i]);
      addr += 1;
  }
  char ssidPassword[UserPasswordChar] = "Password: El string que me envía el server";
  for (int i = 0; i < UserPasswordChar; i++){
      EEPROM.write(addr, ssidPassword[i]);
      addr += 1;
  }
  // addr equals to 106 at this point

  EEPROM.commit();
}
*/













