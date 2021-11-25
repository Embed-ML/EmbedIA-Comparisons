/* 
 * EmbedIA 
 * LIBRERÍA ARDUINO QUE DEFINE FUNCIONES PARA LA IMPLEMENTACIÓN DE REDES NEURNOALES CONVOLUCIONALES
 * EN MICROCONTROLADORES Y LAS ESTRUCTURAS DE DATOS NECESARIAS PARA SU USO
 */

#include "Arduino.h"
#include "embedia.h"

/* IMPLEMENTACIÓN DE FUNCIONES DE LA LIBRERÍA EmbedIA DEFINIDAS EN embedia.h */

typedef struct{
	uint16_t size;
	void  * data;
} raw_buffer;


raw_buffer buffer1 = {0, NULL};
raw_buffer buffer2 = {0, NULL};

void * swap_alloc(size_t s){ // Tensor arena variable
	static raw_buffer * last_buff = &buffer2;
	last_buff = (last_buff==&buffer1) ? &buffer2 : &buffer1;
	
	if (last_buff->size < s){
		last_buff->data = realloc(last_buff->data, s);
		last_buff->size = s;
	}
	/*Serial.print("realloc ");
	Serial.print((last_buff==&buffer1) ? "1: " : "2 :" );
	Serial.println(last_buff->size);
	Serial.println(String((uint32_t)last_buff->data,16));*/

	return last_buff->data;
}

uint32_t swap_alloc_size(){
  return buffer1.size+buffer2.size;
}
/* 
 * conv2d()
 * Función que realiza la convolución entre un filtro y un conjunto de datos.
 * Parámetros:
 *             filter_t filter  =>  estructura filtro con pesos cargados
 *                data_t input  =>  datos de entrada de tipo data_t
 *             data_t * output  =>  puntero a la estructura data_t donde se guardará el resultado
 *                   uint16_t delta =>  posicionamiento de feature_map dentro de output.data
 */
void conv2d(filter_t filter, data_t input, data_t * output, uint16_t delta){
	uint16_t i,j,k,l,c;
	float suma;

	for (i=0; i<output->height; i++){
		for (j=0; j<output->width; j++){
			suma = 0;
			for (c=0; c<filter.channels; c++){
				for (k=0; k<filter.kernel_size; k++){
					for (l=0; l<filter.kernel_size; l++){
						//suma += float_mul(filter.weights[(c*filter.kernel_size*filter.kernel_size)+k*filter.kernel_size+l] , input.data[(c*input.height*input.width)+(i+k)*input.width+(j+l)]);			
						suma += (filter.weights[(c*filter.kernel_size*filter.kernel_size)+k*filter.kernel_size+l] * input.data[(c*input.height*input.width)+(i+k)*input.width+(j+l)]);
					}
				}
			}
			output->data[delta + i*output->width + j] = suma + filter.bias;
		}
	}

}

/* 
 * conv2d_layer()
 * Función que se encarga de aplicar la convolución de una capa de filtros (conv_layer_t)
 * sobre un determinado conjunto de datos de entrada.
 * Parámetros:
 *          conv_layer_t layer  =>  capa convolucional con filtros cargados
 *                data_t input  =>  datos de entrada de tipo data_t
 *             data_t * output  =>  puntero a la estructura data_t donde se guardará el resultado
 */
void conv2d_layer(conv_layer_t layer, data_t input, data_t * output){
	uint16_t delta;

	output->channels = layer.n_filters; //cantidad de filtros
	output->height   = input.height - layer.filters[0].kernel_size + 1;
	output->width    = input.width - layer.filters[0].kernel_size + 1;
    //output->data     = (float*)malloc(sizeof(float)*output->channels*output->height*output->width);
	output->data     = (float*)swap_alloc( sizeof(float)*output->channels*output->height*output->width );
	

	for(uint16_t i=0; i<layer.n_filters; i++){
		delta = i*(output->height)*(output->width);
		conv2d(layer.filters[i],input,output,delta);
	}

}

/* 
 * neuron_forward()
 * Función que realiza el forward de una neurona ante determinado conjunto de datos de entrada.
 * Parámetros:
 *             neuron_t neuron  =>  neurona con sus pesos y bias cargados
 *        flatten_data_t input  =>  datos de entrada en forma de vector (flatten_data_t)
 * Retorna:
 *                      float  =>  resultado de la operación             
 */
float neuron_forward(neuron_t neuron, flatten_data_t input){
	uint16_t i;
	float result = 0;

	for(i=0;i<input.length;i++){
		result += input.data[i]*neuron.weights[i];
		//result += float_mul(input.data[i],neuron.weights[i]);
		
	}

	return result + neuron.bias;
}

/* 
 * dense_forward()
 * Función que se encarga de realizar el forward de una capa densa (dense_layer_t)
 * sobre un determinado conjunto de datos de entrada.
 * Parámetros
 *          dense_layer_t dense_layer  =>  capa densa con neuronas cargadas
 *               flatten_data_t input  =>  datos de entrada de tipo flatten_data_t
 *            flatten_data_t * output  =>  puntero a la estructura flatten_data_t donde se guardará el resultado
 */
void dense_forward(dense_layer_t dense_layer, flatten_data_t input, flatten_data_t * output){
	uint16_t i;

	output->length = dense_layer.n_neurons;
    //output->data = (float*)malloc(sizeof(float)*dense_layer.n_neurons);
	output->data = (float*)swap_alloc(sizeof(float)*dense_layer.n_neurons);
	

	
	for(i=0;i<dense_layer.n_neurons;i++){
		output->data[i] = neuron_forward(dense_layer.neurons[i],input);
	}
}

/* 
 * max_pooling2d()
 * Función que se encargará de aplicar un max pooling a una entrada
 * con un tamaño de ventana de recibido por parámetro (uint16_t strides)
 * a un determinado conjunto de datos de entrada.
 * Parámetros:
 *                data_t input  =>  datos de entrada de tipo data_t
 *             data_t * output  =>  puntero a la estructura data_t donde se guardará el resultado
 */
void max_pooling_2d(uint16_t strides, data_t input, data_t* output){
	uint16_t c,i,j,aux1,aux2;
	float max = -INFINITY;
	float num;

  output->height = (input.height)/strides ;
  output->width =  (input.width )/strides ;

  output->channels = input.channels;
  //output->data = (float*)malloc(sizeof(float)*(output->channels)*(output->height)*(output->width));
  output->data = (float*)swap_alloc(sizeof(float)*(output->channels)*(output->height)*(output->width));

  for (c=0; c<output->channels; c++){
  	for (i=0; i<output->height; i++){
	  	for (j=0; j<output->width; j++){

	  		max = -INFINITY;

	  		for(aux1=0; aux1<strides; aux1++){
					for(aux2=0; aux2<strides; aux2++){
	  				
	  				num = input.data[c*input.width*input.height + (i*strides + aux1)*input.width + j*strides + aux2];
	  				
	  				if(num>max){
	  					max = num;
	  				}
	  			}	  			
	  		}

	  		output->data[c*output->width*output->height + i*output->width + j] = max;
	  	}
  	}	
  }
}

/* 
 * softmax()
 * Función que implementa la activación softmax.
 * Convierte un vector de entrada en na distribución de probabilidades
 * (aplicado a la salida de regresión lineal para obtener una distribución de probabilidades para de cada clase).
 * Parámetro:
 *         flatten_data_t data  =>  datos de entrada de tipo flatten_data_t
 */
void softmax(flatten_data_t data){
	float m = -INFINITY;
	for (size_t i = 0; i < data.length; i++) {
		if (data.data[i] > m) {
			m = data.data[i];
		}
	}

	float sum = (0.0);
	for (size_t i = 0; i < data.length; i++) {
		sum += exp(data.data[i] - m);
	}

	float offset = m + log(sum);
	for (size_t i = 0; i < data.length; i++) {
		data.data[i] = exp(data.data[i] - offset);
	}
}

/* 
 * relu(data_t)
 * Función que implementa la activación relu, convierte un vector de entrada en 
 * una distribución de probabilidades (aplicado a la salida de regresión lineal para
 * obtener una distribución de probabilidades para de cada clase).
 * Parámetro:
 *                 data_t data  =>  datos de tipo data_t a modificar
 */
void relu(data_t data){
	uint16_t i;

	for (i=0;i<(data.channels*data.height*data.width);i++){
		data.data[i] = data.data[i] < 0 ? 0 : data.data[i];
	}
}

/* 
 * relu(flatten_data_t)
 * Función que implementa la activación relu, convierte un vector de entrada en 
 * una distribución de probabilidades (aplicado a la salida de regresión lineal para
 * obtener una distribución de probabilidades para de cada clase).
 * Parámetro:
 *         flatten_data_t data  =>  datos de tipo flatten_data_t a modificar
 */
void relu_flatten(flatten_data_t data){
	uint16_t i;

	for (i=0;i<(data.length);i++){
		data.data[i] = data.data[i] < 0 ? 0 : data.data[i];
	}
}

/* 
 * flatten_layer()
 * Realiza un cambio de tipo de variable. 
 * Convierte el formato de los datos en formato de matriz data_t en vector flatten_data_t.
 * (prepara los datos para ingresar en una capa de tipo dense_layer_t)
 * Parámetros:
 *                data_t input  =>  datos de entrada de tipo data_t
 *     flatten_data_t * output  =>  puntero a la estructura flatten_data_t donde se guardará el resultado
 */
void flatten_layer(data_t input, flatten_data_t * output){
	uint16_t c,i,j;
	uint16_t cantidad = 0;

	output->length = input.channels * input.height * input.width;
    //output->data = (float*)malloc(sizeof(float)*output->length);
	output->data = (float*)swap_alloc(sizeof(float)*output->length);



	for(i=0;i<input.height;i++){
		for(j=0;j<input.width;j++){
			for(c=0;c<input.channels;c++){
				output->data[cantidad++] = input.data[(c*input.width*input.height)+(i*input.width)+j];
			}
		}
	}
}

/* 
 * argmax()
 * Busca el indice del valor mayor dentro de un vector de datos (flatten_data_t)
 * Parámetros:
 *         flatten_data_t data  =>  datos de tipo flatten_data_t a buscar máximo
 * Retorna
 *                         uint16_t  =>  resultado de la búsqueda - indice del valor máximo
 */
uint16_t argmax(flatten_data_t data){
	float max = data.data[0];
	uint16_t pos = 0;

	for(uint16_t i=1;i<data.length;i++){
		if(data.data[i]>max){
			max = data.data[i];
			pos = i;
		} 
	}
	return pos;
}
#if PRINT_RESULTS==1
/*
 * print_data_t()
 * Imprime los valores presentes en una matriz de datos y sus dimensiones
 * Parámetros: 
 *                  data_t matriz => matriz de datos a imprimir
 */ 
void print_data_t(data_t matriz){
	uint16_t c,h,w;

	Serial.print("MCh: "); Serial.print(matriz.channels);
	Serial.print(" - MHe: "); Serial.print(matriz.height);
	Serial.print(" - MWi: "); Serial.println(matriz.width);

	Serial.println("Contenido de la matriz: ");

	for(c=0;c<matriz.channels;c++){
		for(h=0;h<matriz.height;h++){
			for(w=0;w<matriz.width;w++){
				//Serial.print(FIXED_TO_FLOAT(matriz.data[c*matriz.height*matriz.width+h*matriz.width+w]), 5);
		        Serial.print( matriz.data[c*matriz.height*matriz.width+h*matriz.width+w] );
				Serial.print(" ");
			}
			Serial.println();
		}
		Serial.println();
	}
	Serial.println("----------------");

}


/*
 * print_flatten_data_t()
 * Imprime los valores presentes en un vector de datos y su largo
 * Parámetros: 
 *            flatten_data_t data => vector de datos a imprimir
 */ 
void print_flatten_data_t(flatten_data_t data){
	uint16_t i;

	Serial.print("Length: "); Serial.println(data.length);
	Serial.println("Contenido del vector: ");

	for(i=0;i<data.length;i++){
		Serial.print((data.data[i]));
		Serial.print(" ");
	}
	Serial.println();
	Serial.println("----------------");
}


/*
 * print_filter_t()
 * Imprime los valores de los pesos del filtro y sus dimensiones
 * Parámetros: 
 *                filter_t filtro => filtro a imprimir
 */ 
void print_filter_t(filter_t filtro){
	uint16_t c,h,w;

	Serial.print("FCh: "); Serial.print(filtro.channels);
	Serial.print(" - F.K_size: "); Serial.print(filtro.kernel_size);
	Serial.print(" - F.bias: "); Serial.println(filtro.bias);

	Serial.println("Pesos del filtro: ");

	for(c=0;c<filtro.channels;c++){
		for(h=0;h<filtro.kernel_size;h++){
			for(w=0;w<filtro.kernel_size;w++){
				Serial.print((filtro.weights[h*filtro.kernel_size+w]));
				Serial.print(" ");
			}
			Serial.println();
		}
		Serial.println();
	}
	Serial.println("----------------");
}
#endif
