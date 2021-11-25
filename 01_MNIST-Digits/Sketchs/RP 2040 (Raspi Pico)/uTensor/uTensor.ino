#include <stdio.h>

#include <cmath>


#include "mnist_cnn_model.hpp"  //gernerated model file"
#include "uTensor.h"

#define IMG_W 8
#define IMG_H 8

#define INPUT_SIZE (IMG_W*IMG_H)
#define OUTPUT_SIZE 10

// Prototipos de función
int8_t ingresar_Datos(void);

float input_image_buffer[INPUT_SIZE];


using namespace uTensor;

int argmax(const Tensor &logits) {
  uint32_t num_elems = logits->num_elems();
  float max_value = static_cast<float>(logits(0));
  int max_index = 0;
  for (uint32_t i = 1; i < num_elems; ++i) {
    float value = static_cast<float>(logits(i));
    if (value >= max_value) {
      max_value = value;
      max_index = i;
    }
  }
  return max_index;
}

static Mnist_cnn_model model;


// create the input/output tensor
Tensor input_image = new RomTensor({1, IMG_W, IMG_H, 1}, flt, input_image_buffer);
Tensor logits = new RamTensor({1, OUTPUT_SIZE}, flt);

void setup() {
  Serial.begin(9600);
  while (!Serial);

  Serial.println("Esperando Datos:");


  model.set_inputs({{Mnist_cnn_model::input_0, input_image}});
  model.set_outputs({{Mnist_cnn_model::output_0, logits}});
     

  int max_index = argmax(logits);

  Serial.print("pred label: ");
  Serial.println(max_index);

  
}

void loop() {

  int data_avail = ingresar_Datos();


  if(data_avail > 0){
    
   
    unsigned long tiempo = micros();
    model.eval();
    tiempo = micros() - tiempo;
    
    int prediccion = argmax(logits);
    
    // Retorna los resultados en una linea
    // prediccion, tiempo, clase de salida
    // Prediccion
    Serial.print(prediccion);
    Serial.print(",");
    Serial.print(tiempo);
    Serial.print(",");

  }  
  delay(100);
  
}


int8_t ingresar_Datos(){
    uint16_t campos_completados = 0; 
    float f;
    
    if ( !Serial.available() )
        return 0;
    digitalWrite(LED_BUILTIN, HIGH);
    // espera infinitamente hasta encontrar inicio 
    while (Serial.available() && Serial.read() != '['){
      // descarta lo que sea
    }

    while(Serial.peek()!=']') {
        f = Serial.parseFloat();
        input_image_buffer[campos_completados++] = f;
    }
    digitalWrite(LED_BUILTIN, LOW);
    
    if ( Serial.peek()==']' ){
        Serial.read();
        if (campos_completados == INPUT_SIZE) {
            return 1;
        } else {
            return -1;
        }
    }
    return -2;
}
