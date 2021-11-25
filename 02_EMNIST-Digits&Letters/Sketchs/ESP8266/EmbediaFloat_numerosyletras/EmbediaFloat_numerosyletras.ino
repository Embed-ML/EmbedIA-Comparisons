#include "emnist_model.h"

#define TAM_ENTRADA (EMNIST_MODEL_CHANNELS*EMNIST_MODEL_WIDTH*EMNIST_MODEL_HEIGHT)
#define LED_BUILTIN 33

// Prototipos de función
uint8_t ingresar_Datos(void);

/* Variables globales */
float* input_data = (float *)swap_alloc(sizeof(float)*TAM_ENTRADA);
data_t input = {EMNIST_MODEL_CHANNELS, EMNIST_MODEL_WIDTH, EMNIST_MODEL_HEIGHT, input_data};

flatten_data_t resultados; 
/* MAIN */
void setup(){
  
    Serial.begin(19200);
  
    pinMode(LED_BUILTIN, OUTPUT); // Led para prender al iniciar la captura de datos

    // model initialization
    model_init();
  
    Serial.println("Esperando datos:"); 
    
}



void loop() {
    if(ingresar_Datos()){
        unsigned long tiempo = micros();
        uint8_t prediccion = model_predict(input,&resultados);
        tiempo = micros() - tiempo;

        // Retorna los resultados en una linea
        // prediccion, tiempo, clase de salida
        // Prediccion
        Serial.print( prediccion);
        Serial.print(",");
        Serial.print(tiempo);
        Serial.print(",");
        Serial.println(resultados.data[prediccion]); 
        input.data = (float *)swap_alloc(sizeof(float)*TAM_ENTRADA);
    
    }  
    delay(100);
}

uint8_t ingresar_Datos(){
  
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
        input.data[campos_completados] = (valor);
        campos_completados+=1;
    }
    
    digitalWrite(LED_BUILTIN, LOW);
     
    if ( Serial.peek()==']' ){
        Serial.read();
        if (campos_completados == TAM_ENTRADA) {
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
