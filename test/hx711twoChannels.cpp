#include "HX711.h"
const int DOUT_INT = 19;
const int CLK_INT = 18;

const int DOUT_EXT = 13;
const int CLK_EXT = 14;

HX711 internal;
HX711 external;

void setup() {
  Serial.begin(9600);

  internal.begin(DOUT_INT, CLK_INT);
  internal.set_gain(128);
  internal.set_scale(548);
  internal.tare(10);

  external.begin(DOUT_EXT, CLK_EXT);
  external.set_gain(128);
  external.set_scale(612);
  external.tare(10);
  
}

void loop() {

  Serial.print("Peso: ");
  Serial.print(internal.get_units(10),0);
  Serial.println(" gr");
            
  Serial.print("Peso: ");
  Serial.print(external.get_units(10),0);
  Serial.println(" gr");

  delay(100);
}