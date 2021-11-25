#include "Arduino.h"


#include <TensorFlowLite_ESP32.h>

#include "tensorflow/lite/experimental/micro/kernels/all_ops_resolver.h"
#include "tensorflow/lite/experimental/micro/micro_error_reporter.h"
#include "tensorflow/lite/experimental/micro/micro_interpreter.h"
#include "tensorflow/lite/schema/schema_generated.h"
#include "tensorflow/lite/version.h"

#include "mnist_model.h"

// Define de valores necesarios para TinyML
#define NUMBER_OF_INPUTS 64       // Número de entradas (cantidad de elementos de una muetra de test)
#define NUMBER_OF_OUTPUTS 10      // Número de posibles valores de salida / cantidad de clases
#define LED_BUILTIN 33


// Globals, used for compatibility with Arduino-style sketches.
namespace {
tflite::ErrorReporter* error_reporter = nullptr;
const tflite::Model* model = nullptr;
tflite::MicroInterpreter* interpreter = nullptr;
TfLiteTensor* input = nullptr;
TfLiteTensor* output = nullptr;

// Create an area of memory to use for input, output, and intermediate arrays.
// Finding the minimum value for your model may require some trial and error.
constexpr int kTensorArenaSize = 3 * 1024;
uint8_t tensor_arena[kTensorArenaSize];
}  // namespace



  
void setup() {
    Serial.begin(19200);
  
    pinMode(LED_BUILTIN, OUTPUT); // Led para prender al iniciar la captura de datos

    Serial.println("Esperando datos...");  

  
    // Set up logging. Google style is to avoid globals or statics because of
    // lifetime uncertainty, but since this has a trivial destructor it's okay.
    // NOLINTNEXTLINE(runtime-global-variables)
    static tflite::MicroErrorReporter micro_error_reporter;
    error_reporter = &micro_error_reporter;
  
    // Map the model into a usable data structure. This doesn't involve any
    // copying or parsing, it's a very lightweight operation.
    model = tflite::GetModel(mnist_model);
    if (model->version() != TFLITE_SCHEMA_VERSION) {
        error_reporter->Report(
            "Model provided is schema version %d not equal "
            "to supported version %d.",
            model->version(), TFLITE_SCHEMA_VERSION);
        return;
    }

    // Pull in only the operation implementations we need.
    // This relies on a complete list of all the ops needed by this graph.
    // An easier approach is to just use the AllOpsResolver, but this will
    // incur some penalty in code space for op implementations that are not
    // needed by this graph.
    //
    // tflite::ops::micro::AllOpsResolver resolver;
    // NOLINTNEXTLINE(runtime-global-variables)
    static tflite::ops::micro::AllOpsResolver resolver;
  
    // Build an interpreter to run the model with.
    static tflite::MicroInterpreter static_interpreter(
        model, resolver, tensor_arena, kTensorArenaSize,
        error_reporter);
    interpreter = &static_interpreter;
  
    // Allocate memory from the tensor_arena for the model's tensors.
    TfLiteStatus allocate_status = interpreter->AllocateTensors();
    if (allocate_status != kTfLiteOk) {
      error_reporter->Report("AllocateTensors() failed");
      return;
    }
  
    // Get information about the memory area to use for the model's input.
    input = interpreter->input(0);  
}

void loop() {

 
    if(ingresar_Datos()){

        unsigned long tiempo = micros();
        /********************************************************************/
        // Run the model on this input and make sure it succeeds.
        if (kTfLiteOk != interpreter->Invoke()) {
            error_reporter->Report("Invoke failed.");
        }
  
        TfLiteTensor* output = interpreter->output(0);
  
        // Process the inference results.
        float max = 0;
        float aux = 0;
        uint16_t predicted = 0;
        for (uint16_t i = 0; i < NUMBER_OF_OUTPUTS; i++){
            aux = output->data.f[i];
            if (aux>max){
                max = aux;
                predicted = i;
            }
        }
        /********************************************************************/
        tiempo = micros() - tiempo;
  
        Serial.print(predicted);
        Serial.print(",");
        Serial.print(tiempo);
        Serial.print(",");
        Serial.println(max);  

    }  
    delay(100);
}

uint8_t ingresar_Datos(){
    uint16_t campos_completados = 0; 
    float valor; 
  
    if ( !Serial.available() ){
        return 0;
    }
    
    digitalWrite(LED_BUILTIN, HIGH);
    
    // espera infinitamente hasta encontrar inicio 
    while (Serial.available() && Serial.read() != '['){
        // descarta lo que sea
    }
  
    while(Serial.available() && Serial.peek()!=']') {
        valor = Serial.parseFloat();
        input->data.f[campos_completados] = valor;
        campos_completados+=1;
    }
  
    digitalWrite(LED_BUILTIN, LOW);
    
    if ( Serial.peek()==']' ){
        Serial.read();
        if (campos_completados == NUMBER_OF_INPUTS) {
            return 1;
        } else {
            return 0;
        }
    }else{
        return 0;
    }
    
    
    return 0;
}
