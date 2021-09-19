#include "PT_Constants.h"
#include "HX711.h"

HX711 internal;
HX711 external;

void initLoadCells(){

    internal.begin(DOUT_INT, CLK_INT);
    internal.set_gain(128);
    internal.set_scale(548); // Load cell internal (A) calibration
    internal.tare(10);

    delay(250);

    external.begin(DOUT_EXT, CLK_EXT);
    external.set_gain(128);
    external.set_scale(612); // Load cell external (B) calibration
    external.tare(10);
}

/*
Serial.print("Peso: ");
Serial.print(internal.get_units(10),0);
Serial.println(" gr");
        
Serial.print("Peso: ");
Serial.print(external.get_units(10),0);
Serial.println(" gr");
*/