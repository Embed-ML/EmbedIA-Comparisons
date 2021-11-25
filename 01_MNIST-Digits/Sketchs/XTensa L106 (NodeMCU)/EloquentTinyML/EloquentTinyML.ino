#define TFLITE_EMULATE_FLOAT
#include <EloquentTinyML.h>
#include "mnist_model.h"

// Definicion de constantes
#define CHANNELS 	1
#define WIDTH 		8
#define HEIGHT 		8
#define NUMBER_OF_INPUTS CHANNELS*WIDTH*HEIGHT  // Número de entradas (cantidad de elementos de una muetra de test)
#define NUMBER_OF_OUTPUTS 10   					// Número de posibles valores de salida / cantidad de clases

// in future projects you may need to tweek this value: it's a trial and error process
#define TENSOR_ARENA_SIZE 3*1024

Eloquent::TinyML::TfLite<NUMBER_OF_INPUTS, NUMBER_OF_OUTPUTS, TENSOR_ARENA_SIZE> model;

/* Variables globales */
float input_data[CHANNELS*WIDTH*HEIGHT];
float resultados[NUMBER_OF_OUTPUTS];


// Prototipos de función
int ingresar_Datos(void);

/* MAIN */
void setup(){
	  Serial.begin(19200);
  
    model.begin(mnist_model);
    
	  pinMode(LED_BUILTIN, OUTPUT);
	  Serial.println("Esperando Datos:");	
}

void loop() {
 
	if(ingresar_Datos()){

    
		  unsigned long tiempo = micros();
		  model.predict(input_data, resultados);
      tiempo = micros()-tiempo;
      
		  int prediccion = model.probaToClass(resultados);

		
        // Retorna los resultados en una linea
        // prediccion, tiempo, clase de salida
        Serial.print( prediccion);
        Serial.print(",");
        Serial.print(tiempo);
        Serial.print(",");
        Serial.println(resultados[prediccion]); 
	}  
	delay(100);
  
}

int ingresar_Datos(){
  uint16_t campos_completados = 0; 

  if ( !Serial.available() )
   
  digitalWrite(LED_BUILTIN, HIGH);
  
  // espera infinitamente hasta encontrar inicio 
  while (Serial.available() && Serial.read() != '['){
      // descarta lo que sea
  }
  
  while(Serial.available() && Serial.peek()!=']') {
    input_data[campos_completados++] = Serial.parseFloat();
  }

  digitalWrite(LED_BUILTIN, LOW);
  if ( Serial.peek()==']' ){
      Serial.read();
      if (campos_completados == NUMBER_OF_INPUTS) {
          return 1;
      } else {
          return 0;
      }
  }
  return 0;
}
