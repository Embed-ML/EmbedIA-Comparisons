#include "mnist_digits_model.h"

/* DEFINE */
#define TAM_ENTRADA  (MNIST_DIGITS_MODEL_CHANNELS*MNIST_DIGITS_MODEL_WIDTH*MNIST_DIGITS_MODEL_HEIGHT)

/* Variables globales */
fixed input_data[TAM_ENTRADA];
data_t input = {MNIST_DIGITS_MODEL_CHANNELS, MNIST_DIGITS_MODEL_WIDTH, MNIST_DIGITS_MODEL_HEIGHT, input_data};

flatten_data_t resultados; 


// Prototipos de función
uint8_t ingresar_Datos(void);


/* MAIN */
void setup(){
  
    Serial.begin(115200);
  
    pinMode(LED_BUILTIN, OUTPUT); // Led para prender al iniciar la captura de datos

    Serial.println("Esperando datos:"); 
    
    // model initialization
    model_init();
    
}



/* MAIN */
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
        Serial.println(FIXED_TO_FLOAT(resultados.data[prediccion])); 
    
        digitalWrite(LED_BUILTIN, LOW);
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
        input_data[campos_completados] = FL2FX(valor);
        campos_completados+=1;
    }

    digitalWrite(LED_BUILTIN, LOW); // finaliza captura
     
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
