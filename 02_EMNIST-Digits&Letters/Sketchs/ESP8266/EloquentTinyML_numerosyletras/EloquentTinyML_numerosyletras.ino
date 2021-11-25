#include <EloquentTinyML.h> // version 0.0.7
#include "mnist_model.h"

// Definicion de constantes
#define CHANNELS    1
#define WIDTH       16
#define HEIGHT      16
#define NUMBER_OF_INPUTS CHANNELS*WIDTH*HEIGHT  // Número de entradas (cantidad de elementos de una muetra de test)
#define NUMBER_OF_OUTPUTS 36                    // Número de posibles valores de salida / cantidad de clases
#define LED_BUILTIN 33

// in future projects you may need to tweek this value: it's a trial and error process
#define TENSOR_ARENA_SIZE 10*1024

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
        int prediccion = model.probaToClass(resultados);
        tiempo = micros()-tiempo;


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
    float valor; 

    if ( !Serial.available() )
        return 0;

    digitalWrite(LED_BUILTIN, HIGH); // inicia captura
    
    // espera infinitamente hasta encontrar inicio 
    while (Serial.available() && Serial.read() != '['){
        // descarta lo que sea
    }

    while(Serial.available() && Serial.peek()!=']') {
        valor = Serial.parseFloat();
        input_data[campos_completados] = (valor);
        campos_completados+=1;
    }

    digitalWrite(LED_BUILTIN, LOW); // finaliza captura

    if ( Serial.peek()==']' ){
        Serial.read();
        if (campos_completados == NUMBER_OF_INPUTS) {
            return 1;
        } else {
            // Faltan datos
            Serial.print(0);
            Serial.print(",");
            Serial.print(0);
            Serial.print(",");
            Serial.println(0);
            return 0;
        }
    }else{
        // No llego ']'
        return 0;
    }
}
