
#include "embedia.h"
#include "emnist_model.h"

#define INPUT_SIZE (EMNIST_MODEL_CHANNELS*EMNIST_MODEL_WIDTH*EMNIST_MODEL_HEIGHT)

// Buffer with number 0 example for test
float input_data[INPUT_SIZE]= {
  0.0, 0.0, 5.0, 13.0, 9.0, 1.0, 0.0, 0.0, 0.0, 0.0, 13.0, 15.0, 10.0, 15.0, 5.0, 0.0, 0.0, 3.0, 15.0, 2.0, 0.0, 11.0, 8.0, 
  0.0, 0.0, 4.0, 12.0, 0.0, 0.0, 8.0, 8.0, 0.0, 0.0, 5.0, 8.0, 0.0, 0.0, 9.0, 8.0, 0.0, 0.0, 4.0, 11.0, 0.0, 1.0, 12.0, 7.0, 
  0.0, 0.0, 2.0, 14.0, 5.0, 10.0, 12.0, 0.0, 0.0, 0.0, 0.0, 6.0, 13.0, 10.0, 0.0, 0.0, 0.0}; 

// Structure with input data for the inference function
data_t input = { EMNIST_MODEL_CHANNELS, EMNIST_MODEL_WIDTH, EMNIST_MODEL_HEIGHT, input_data };

// Structure with inference output results
flatten_data_t results;

void setup(){
      
      Serial.begin(9600);
      
} 
        
void loop(){
      

    // model initialization
    model_init();

    

    // model inference
    int prediction = model_predict(input, &results);    
    
    // print predicted class id
    Serial.print("Prediction class id: ");
    Serial.println(prediction); 


}
  