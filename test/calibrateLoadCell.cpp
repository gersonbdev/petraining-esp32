#include <Arduino.h>
#include "HX711.h"
const int DOUT=13; // 19
const int CLK=14;  // 18

HX711 balanza;

void setup() {
  Serial.begin(9600);
  balanza.begin(DOUT, CLK);
  Serial.print("Lectura del valor del ADC:  ");
  balanza.set_gain(128);
  Serial.println(balanza.read());
  Serial.println("No ponga ningun  objeto sobre la balanza");
  Serial.println("Destarando...");
  Serial.println("...");
  balanza.set_scale(548); // Escala de canal A = 548.5
  balanza.tare(10);  //El peso actual es considerado Tara.
  
  Serial.println("Listo para pesar");  
}

void loop() {
  Serial.print("Peso: ");
  Serial.print(balanza.get_units(10),0);
  Serial.println(" gr");
  delay(100);
}