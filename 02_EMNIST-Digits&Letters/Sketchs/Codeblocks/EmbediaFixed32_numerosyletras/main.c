#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "emnist_model.h"
#define TAM_ENTRADA (EMNIST_MODEL_CHANNELS*EMNIST_MODEL_WIDTH*EMNIST_MODEL_HEIGHT)




// Prototipos de función
uint8_t ingresar_Datos(void);


const char* getfield(char* line, int num)
{
	const char* tok;
	for (tok = strtok(line, ";");
			tok && *tok;
			tok = strtok(NULL, ";\n"))
	{
		if (!--num)
			return tok;
	}
	return NULL;
}

int main()
{
    /* Variables globales */
    fixed* input_data = (fixed *)swap_alloc(sizeof(fixed)*TAM_ENTRADA);
    data_t input = {EMNIST_MODEL_CHANNELS, EMNIST_MODEL_WIDTH, EMNIST_MODEL_HEIGHT, input_data};
    flatten_data_t resultados;

    //Archivo donde guardo lso datos
    FILE * outFile = fopen("FROMPC_EmbediaFixed32_numerosyletras-all.csv","w");
    fprintf(outFile,"nroEj;ClaseImagen;ClaseInferida;Tiempo;Confianza;Correcto;;Metricas\n");
    // model initialization
    model_init();
    printf("Bienvenidx a la generacion del futuro!\n");
    FILE* stream = fopen("test.csv", "r"); //Abro el archivo de entrada
    char line[20000];
    int j=0,aciertos=0;
    fgets(line,10000,stream); //Descarto primera linea
    while (fgets(line, 10000, stream) && j<20000) //Leo lineas hasta que no hayan mas
    {
        j++;
        //Guardo el input y salida esperada (alambre)
        char* tmp = strdup(line);
        char* myInput = getfield(tmp, 2);
        tmp = strdup(line);
        char* expectedOutput = getfield(tmp, 3);
        //printf("%s -->", myInput);
        //printf("%s\n\n", expectedOutput);

        //Formateo input de la forma [x.y, z.w, ....]
        //Casts a floats y seteado de input.data
        char* res = strtok(myInput,"[, ");
        input.data[0] = FLOAT_TO_FIXED(atof(res));
        //printf("Input data -> %s\n",res);
        for(int i=1;i<TAM_ENTRADA;i++){
            res = strtok(NULL,", ]");
            input.data[i]=FLOAT_TO_FIXED(atof(res));
        }
        //Realizo la predicción
        uint8_t prediccion = model_predict(input,&resultados);
        fprintf(outFile,"%2d ; %2d ; %2d ; N/A ; %2.2f",j,atoi(expectedOutput),prediccion,FIXED_TO_FLOAT(resultados.data[prediccion]));
        printf("%2d ; %2d ; %2d ; N/A ; %2.2f",j,atoi(expectedOutput),prediccion,FIXED_TO_FLOAT(resultados.data[prediccion]));
        if(prediccion==atoi(expectedOutput)){
            fprintf(outFile,";SI;;\n");
            aciertos++;
        }
        else{
            fprintf(outFile,";NO;;\n");
        }
        printf("-->  Era: %2d y Dije: %2d\n",atoi(expectedOutput),prediccion);
        free(tmp);
    }
    printf("\n=========\nAccuracy: %.2f\n=========\n",(float)aciertos / j);
    fclose(stream); //Cierro el archivo
    fclose(outFile);
    scanf("%d"); //Pa ver algo
    return 0;
}


