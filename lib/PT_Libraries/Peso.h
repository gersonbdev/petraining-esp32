#include "HX711.h" //la libreria la puedes instalar en el IDE de arduino desde aqui: http://librarymanager/All#Avia_HX711

#define LOADCELL_DOUT_PIN  A1 //pin DT del modulo HX711 ira conectado al pin A0 de arduino.
#define LOADCELL_SCK_PIN  A0  //pin SCK del modulo HX711 ira conectado al pin A1 de arduino.

HX711 scale;

float calibration_factor = -951; // funciona para mi celda de carga de 1 kgs

void pesoIN() {
  Serial.begin(9600);
  //Serial.println("Peso entrada parqueadero");
  


  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale();
  scale.tare();  //Reset the scale to 0

  long zero_factor = scale.read_average(); //Obten una lectura de referencia
  //Serial.print("Zero factor: "); //Puede ser utilizado para  quitar la necesidad de calibrar la bascula, muy util en proyectos permamentes.
  //Serial.println(zero_factor); //solo poner el valor cuando ya tengas bien definido el valor del factor cero
}

void CalcularPeso() {

  scale.set_scale(calibration_factor); //Ajuste del factor de calibración

  Serial.print("Peso: ");
  Serial.print(scale.get_units(), 1);
  Serial.print(" Gramos"); //Cambia A KILOGRAMOS Y REAJUSTA EL VALOR DE CALIBRACIÓN
  Serial.println();

}
