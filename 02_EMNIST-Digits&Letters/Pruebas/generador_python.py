#!/usr/bin/env python
# coding: utf-8

# In[1]:


print('Test EMNIST 36 classes')


# # Generador Nuevo

# In[2]:


# Importar librerías
import pandas as pd
import numpy as np
import serial


# In[18]:


###################################################
#   definiciones de constantes y configuracion

(MCU_PICO, MCU_ESP32, MCU_ESP8266, MCU_STM32, MCU_MEGA) = (0,1,2,3,4)

# velocidades mas convenientes segun el dispositivo de prueba
MCU_SERIAL_SPEED = [115200, 19200, 19200, 9600, 4800]

# seleccione del MCU de prueba
MCU_SELECT = MCU_PICO

# prefijo del nombre del dataset para la prueba
#DATASET = "emnist_mayus"
#DATASET = "digits"
DATASET = "numerosyletras"

# cantidad de muestras para probar. 0 o menos => todas
DATASET_COUNT = 0


# Establecer comunicación Serial
SERIAL_SPEED = MCU_SERIAL_SPEED[MCU_SELECT]    # Velocidad de transmisión
SERIAL_PORT  = 'COM6'  # Puerto
SERIAL_TOUT  = 3.0     # Timeout en segundos


# Cargar datos
data_test = np.load(f"datos_formato_numpy/{DATASET}_x_test.npy")
data_class = np.load(f"datos_formato_numpy/{DATASET}_y_test.npy")
test_image = str(list(data_test[0].flatten()))


csv_filename = "output.csv"

#csv = open(csv_filename, 'w')

if DATASET_COUNT > 0:
    data_test  = data_test[0:DATASET_COUNT]
    data_class = data_class[0:DATASET_COUNT]

# In[19]:


try:
    print("Abriendo puerto %s a %d baudios..." % (SERIAL_PORT, SERIAL_SPEED) )
    serial_dev = serial.Serial(port=SERIAL_PORT, baudrate=SERIAL_SPEED, timeout=SERIAL_TOUT)
    
    compilacion = input("Ingrese resultados de la compilación: ")
    compilacion = "Compilación: "+compilacion
    print()
    
    ok_count = 0
    cant_piolas = 0
    datos = []
    
    if len(data_test) != len(data_class):
        raise Exception("La cantidad de ejemplos (%d) no coincide con la de clases (%d)" %(len(data_test), len(data_class)))
    
    # Por si se envio algo antes de empezar y quedo en el buffer
    #serial_dev.reset_input_buffer() 
    serial_dev.flushInput()
    
    for i in range(len(data_test)):
        # Enviar datos
        
        #test_image = str(list(data_test[i].flatten()))
        #test_image = str([ "{:0.7f}".format(x) for x in data_test[i].flatten() ])
        strings = [ format(x,".7f") for x in data_test[i].flatten() ]
        string = '['
        for s in strings:
            string = string + s + ', '
        string = string[:-2] + ']'
        
        test_image = string
        csv.write( str(data_class[i])+': '+str(test_image)+'\n' )

        #print(test_image)
        serial_dev.write(test_image.encode('ascii'))

        # Recibir respuesta
        resp_str = serial_dev.readline().decode()
        if resp_str == "":
            raise Exception("No se recibio respuesta con resultado")
        resp = resp_str[:-2].split(',') # saca los 2 caracteres \r\n

        # Campos a guardar
        clase_imagen = str(data_class[i])
        clase_inferida =  resp[0]
        tiempo = "%5s" % resp[1]
        confianza = resp[2]
        correcto = clase_imagen == clase_inferida
        resp.insert(0,clase_imagen)

        # Analizar resultados
        if tiempo!='0':
            cant_piolas+=1
        if correcto: 
            correcto = "SI"
            ok = " "
            if tiempo != '0':
                ok_count+=1
        else: 
            correcto = "NO"
            ok = "X"
        resp.append(correcto)

        # Imprimir resultados
        datos.append(resp)
        print("---------------------------------------------")
        print("Imagen %4d ==> " % (i), end='' )
        print(resp)
        print("Clase %5s == %2s - us: %5s - out: %2s  \t%s - rate: %3.2f%%" % (clase_imagen, clase_inferida, tiempo, confianza, ok,100.0*ok_count/(i+1)))
    
except :
    serial_dev.close()
    csv.close()
    raise

# Cerrar comunicación
serial_dev.close()
print("---------------------------------------------")
print("\nListo")


# In[20]:


# Completar campos
cant_total = len(data_test)
cant_aciertos = f'Aciertos: {ok_count}/{cant_total}'
acc = "Efectividad: %3.2f%%" % (100.0*ok_count/(cant_piolas))
error_comm = "Error de comunicacion: %3.2f%%" % ((len(data_class)-cant_piolas)/len(data_class) * 100.0)

# Crear DataFrame
df = pd.DataFrame(datos, columns = ['ClaseImagen','ClaseInferida','Tiempo','Confianza','Correcto'])
df['ClaseImagen'] = df['ClaseImagen'].astype(np.int)
df['ClaseInferida'] = df['ClaseInferida'].astype(np.int)
tiempo_mean = "Tiempo promedio: %5.2f us" % (df['Tiempo'].astype(np.int).mean())
df['-'] = ' '
df['Metricas'] = ' '
df['Metricas'][0] = acc
df['Metricas'][1] = cant_aciertos
df['Metricas'][2] = tiempo_mean
df['Metricas'][3] = error_comm
df['Metricas'][4] = compilacion

# Imprimir resultados
print("===========================================")
print(f"\t{acc}")
print(f"\t{cant_aciertos}")
print(f"\t{tiempo_mean}")
print(f"\t{error_comm}")
print("===========================================")

df.head()


# In[21]:


# Crear archivo
out_fname = input("Ingrese el nombre de archivo para resultados (prueba.xlsx por defecto): ")
if out_fname == "":
    out_fname = "prueba"
out_fname="resultados_formato_excel/"+out_fname+".xlsx"
df.to_excel(out_fname)
print("Archivo creado.")


# Todo terminó
