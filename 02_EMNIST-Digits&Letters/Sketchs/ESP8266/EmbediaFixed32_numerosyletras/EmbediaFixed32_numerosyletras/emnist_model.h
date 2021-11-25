/* EmbedIA model - Autogenerado */
#ifndef EMNIST_MODEL_H
#define EMNIST_MODEL_H

#include "embedia.h"
#define EMNIST_MODEL_CHANNELS 1
#define EMNIST_MODEL_HEIGHT 16
#define EMNIST_MODEL_WIDTH 16
  
void model_init();

int model_predict(data_t input, flatten_data_t * results);

  
#endif
