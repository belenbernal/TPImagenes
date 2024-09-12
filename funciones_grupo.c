/*
    Integrantes del grupo. En caso de ser un grupo de dos integrantes, no completar el último campo.
    Si alguno de los integrantes del grupo dejara la materia, completar de todos modos sus datos, aclarando que no entrega.
    -----------------
    Apellido: Fernandez
    Nombre: Thomas Uriel
    DNI: 42838357
    Entrega: SI
    -----------------
    Apellido:
    Nombre:
    DNI:
    Entrega:
    -----------------
    Apellido:
    Nombre:
    DNI:
    Entrega:
    -----------------
*/
#include <stdio.h>
#include<stdlib.h>
#include<string.h>
#include <stdbool.h>
#include "funciones_grupo.h"
#include "constantes.h"
#include "estructuras.h"

int solucion(int argc, char* argv[])
{
    /*
        Aquí deben hacer el código que solucione lo solicitado.
        Todas las funciones utilizadas deben estar declaradas en este archivo, y en su respectivo .h
    */

    /* Bitmap file format
    *
    * SECTION
    * Address:Bytes	Name
    *
    * HEADER:
    *	  0:	2		"BM" magic number
    *	  2:	4		file size
    *	  6:	4		junk
    *	 10:	4		Starting address of image data
    * BITMAP HEADER:
    *	 14:	4		header size
    *	 18:	4		width  (signed)
    *	 22:	4		height (signed)
    *	 26:	2		Number of color planes
    *	 28:	2		Bits per pixel
    *	[...]
    * [OPTIONAL COLOR PALETTE, NOT PRESENT IN 32 BIT BITMAPS]
    * BITMAP DATA:
    *	DATA:	X	Pixels
    */

    char *nombreArchivoBMP[argc];
    char *filtros[argc];
    int numFiltros = 0;
    int numArchivos = 0;

    obtenerArgumentos(nombreArchivoBMP, filtros, argc, argv, &numFiltros, &numArchivos);

    if(numArchivos < 1)
    {
        printf("Ejecucion finalizada. No se ha ingresado ninguna imagen BMP");
        return ERR_ARCHIVO_BMP;
    }

    t_metadata *metadata = malloc(sizeof(t_metadata));
    t_pixel **imagen = leerImagenBMP(*nombreArchivoBMP, metadata);

    if(imagen == NULL)
    {
        perror("\nNo ha sido posible leer la imagen");
        return ERR_ARCHIVO_BMP;
    }

    int res = 0;
    char filtro[256];
    float parametro = 0;

    for(int i = 0; i < numFiltros; i++)
    {
        extraerFiltroYParametro(filtros[i], filtro, &parametro);
        if(strcmp(filtro, "--escala-de-grises") == 0)
        {

        }
        else if(strcmp(filtro, "--rotar-derecha") == 0)
        {
            res = rotarImagenDerecha(imagen, metadata);
        }
        else if(strcmp(filtro, "--rotar-izquierda") == 0)
        {
            res = rotarImagenIzquierda(imagen, metadata);
        }
        else if(strcmp(filtro, "--concatenar-horizontal") == 0)
        {
            if(numArchivos > 0)
            {
                t_metadata *metadata2 = malloc(sizeof(t_metadata));
                t_pixel **imagen2 = leerImagenBMP(nombreArchivoBMP[1], metadata2);
                res = concatenarHorizontal(imagen, imagen2, metadata, metadata2);
                liberarMemoriaImg(imagen2, metadata2->alto);
                liberarMemoriaMetadata(metadata2);
            }
            else
            {
                res = ERR_ARCHIVO_BMP;
            }
        }
        else if(strcmp(filtro, "--concatenar-vertical") == 0)
        {
            if(numArchivos > 0)
            {
                t_metadata *metadata2 = malloc(sizeof(t_metadata));
                t_pixel **imagen2 = leerImagenBMP(nombreArchivoBMP[1], metadata2);
                res = concatenarVertical(imagen, imagen2, metadata, metadata2);
                liberarMemoriaImg(imagen2, metadata2->alto);
                liberarMemoriaMetadata(metadata2);
            }
            else
            {
                res = ERR_ARCHIVO_BMP;
            }
        }
        else if(strcmp(filtro, "--aumentar-contraste") == 0)
        {
            if(parametro != -1)
            {
                res = ajustarContraste(imagen, metadata, parametro);
            }
            else
            {
                res = ERR_PARAMETRO;
            }
        }
        else if(strcmp(filtro, "--tonalidad-verde") == 0)
        {
            res = tonalidadVerde(imagen, metadata, parametro);
        }
        else if(strcmp(filtro, "--tonalidad-azul") == 0)
        {
            res = tonalidadAzul(imagen, metadata, parametro);
        }
        else if(strcmp(filtro, "--tonalidad-roja") == 0)
        {
            res = tonalidadRoja(imagen, metadata, parametro);
        }
        else if(strcmp(filtro, "--espejar-horizontal") == 0)
        {
            res = espejarHorizontal(imagen, metadata);
        }
        else if(strcmp(filtro, "--espejar-vertical") == 0)
        {

        }
        else if(strcmp(filtro, "--recortar") == 0)
        {

        }
        else if(strcmp(filtro, "--achicar") == 0)
        {

        }
        else if(strcmp(filtro, "--comodin") == 0)
        {

        }

        if(res != TODO_OK)
        {
            printf("Error al aplicar el filtro %s\n", filtros[i]);
        }

        imagen = leerImagenBMP(*nombreArchivoBMP, metadata);
    }

    liberarMemoriaImg(imagen, metadata->alto);
    liberarMemoriaMetadata(metadata);

    return 0;
}

void extraerFiltroYParametro(const char *entrada, char *filtro, float *parametro)
{
    char buffer[256];
    strncpy(buffer, entrada, 255);

    char *igualSigno = strchr(buffer, '=');
    if (igualSigno)
    {
        *igualSigno = '\0';
        *parametro = atof(igualSigno + 1);
        if (*parametro < 0.00 || *parametro > 100.00)
        {
            printf("parametro invalido\n");
            *parametro = ERR_NO_FILTRO;
        }

    }
    else
    {
        *parametro = -1;
    }

    strcpy(filtro, buffer);
}

void obtenerArgumentos(char *nombreArchivoBMP[], char *filtros[], int argc, char *argv[], int *numFiltros, int *numArchivos)
{
    for(int i=1; i < argc; i++)
    {
        if(strstr(argv[i], ".bmp"))
        {
            nombreArchivoBMP[*numArchivos] = argv[i];
            (*numArchivos)++;
        }
        else if(strstr(argv[i], "--"))
        {
            filtros[*numFiltros] = argv[i];
            (*numFiltros)++;
        }
    }
    mostrarArgumentos(nombreArchivoBMP, filtros, *numFiltros, *numArchivos);
}

void mostrarArgumentos(char *nombreArchivoBMP[], char *filtros[], int numFiltros, int numArchivos)
{
    for(int i = 0; i < numArchivos; i++)
    {
        printf("Archivo: %d: %s\n", i + 1, nombreArchivoBMP[i]);
    }
    for(int i = 0; i < numFiltros; i++)
    {
        printf("Filtro %d: %s\n", i + 1, filtros[i]);
    }
}

t_pixel **leerImagenBMP(char *nombreArchivoBMP, t_metadata *metadata)
{
    FILE * archivo = fopen(nombreArchivoBMP, "rb");

    if(!archivo)
    {
        perror("\nError. No es posible abrir el archivo.");
        return NULL;
    }

    leerEncabezado(archivo, metadata);

    if(metadata->profundidad != 24)
    {
        perror("\nError. Solo se admiten imágenes de 24 bits.");
        fclose(archivo);
        return NULL;
    }

    t_pixel **matriz = (t_pixel **)malloc(metadata->alto * sizeof(t_pixel *));
    for(unsigned int i = 0; i < metadata->alto; i++)
    {
        matriz[i] = (t_pixel *)malloc(metadata->ancho * sizeof(t_pixel));
    }

    fseek(archivo, metadata->comienzoImagen, SEEK_SET);

    for(int i = metadata->alto - 1; i >= 0; i--)
    {
        for(unsigned int j = 0; j < metadata->ancho; j++)
        {
            fread(matriz[i][j].pixel, 3, 1, archivo);
            matriz[i][j].profundidad = metadata->profundidad;
        }
    }

    fclose(archivo);
    return matriz;
}

void liberarMemoriaImg(t_pixel **matriz, unsigned int alto)
{
    for (unsigned int i = 0; i < alto; i++)
    {
        free(matriz[i]);
    }
    free(matriz);
}

void liberarMemoriaMetadata(t_metadata *metadata) {
    free(metadata);
}

void leerEncabezado(FILE *archivo, t_metadata *metadata)
{
    if(getc(archivo) == 'B' && getc(archivo) == 'M')
    {
        fread(&metadata->tamArchivo, sizeof(unsigned int), 1, archivo);
        fseek(archivo, 4, SEEK_CUR);
        fread(&metadata->comienzoImagen, sizeof(unsigned int), 1, archivo);
        fread(&metadata->tamEncabezado, sizeof(unsigned int), 1, archivo);
        fread(&metadata->ancho, sizeof(unsigned int), 1, archivo);
        fread(&metadata->alto, sizeof(unsigned int), 1, archivo);
        fseek(archivo, 2, SEEK_CUR);
        fread(&metadata->profundidad, sizeof(unsigned short), 1, archivo);
    }
    else
    {
        printf("\n *** Error: El archivo indicado no es de tipo bit map. ***");
    }
}

void escribirImagenBMP(const char *nombreArchivo, t_metadata *metadata, t_pixel **matriz)
{

    FILE *archivo = fopen(nombreArchivo, "wb");
    if (!archivo)
    {
        perror("Error al abrir el archivo para escritura");
        return;
    }

    int padding = (4 - (metadata->ancho * 3) % 4) % 4;

    metadata->tamArchivo = 54 + (metadata->ancho * metadata->alto * 3) + (padding * metadata->alto);

    fwrite("BM", sizeof(char), 2, archivo);
    fwrite(&metadata->tamArchivo, sizeof(unsigned int), 1, archivo);
    fseek(archivo, 4, SEEK_CUR);
    fwrite(&metadata->comienzoImagen, sizeof(unsigned int), 1, archivo);
    fwrite(&metadata->tamEncabezado, sizeof(unsigned int), 1, archivo);
    fwrite(&metadata->ancho, sizeof(unsigned int), 1, archivo);
    fwrite(&metadata->alto, sizeof(unsigned int), 1, archivo);
    fseek(archivo, 2, SEEK_CUR);
    fwrite(&metadata->profundidad, sizeof(unsigned short), 1, archivo);
    fseek(archivo, metadata->comienzoImagen, SEEK_SET);

    //Escribir los datos de la imagen con padding
    for (int i = metadata->alto - 1; i >= 0; i--)
    {
        for (unsigned int j = 0; j < metadata->ancho; j++)
        {
            fwrite(matriz[i][j].pixel, 3, 1, archivo);
        }
        // Escribir el padding
        for (int p = 0; p < padding; p++)
        {
            fputc(0, archivo);
        }
    }

    printf("\n\nEscribe: %s", nombreArchivo);

    fclose(archivo);
}

int concatenarVertical(t_pixel **imagen, t_pixel **imagen2, t_metadata *metadata, t_metadata *metadata2)
{

    unsigned int nuevo_ancho = metadata->ancho > metadata2->ancho ? metadata->ancho : metadata2->ancho;
    unsigned int nuevo_alto = (metadata->alto + metadata2->alto);

    t_pixel **nueva_imagen = (t_pixel**)malloc(nuevo_alto * sizeof(t_pixel*));

    if(nueva_imagen == NULL)
    {
        return ERR_MEMORIA;
    }


    for (int i = 0; i < nuevo_alto; i++)
    {
        nueva_imagen[i] = (t_pixel*)malloc(nuevo_ancho * sizeof(t_pixel));
    }

    // Copiar los píxeles de la primera imagen
    for (int i = 0; i < metadata->alto; i++)
    {
        for (int j = 0; j < metadata->ancho; j++)
        {
            nueva_imagen[i][j] = imagen[i][j];
        }
        // Rellenar los bordes si es necesario
        for (int j = metadata->ancho; j < nuevo_ancho; j++)
        {
            nueva_imagen[i][j].pixel[0] = 100;
            nueva_imagen[i][j].pixel[1] = 150;
            nueva_imagen[i][j].pixel[2] = 200;
        }
    }

    // Copiar los píxeles de la segunda imagen
    for (int i = 0; i < metadata2->alto; i++)
    {
        for (int j = 0; j < metadata2->ancho; j++)
        {
            nueva_imagen[i + metadata->alto][j] = imagen2[i][j];
        }
        // Rellenar los bordes si es necesario
        for (int j = metadata2->ancho; j < nuevo_ancho; j++)
        {
            nueva_imagen[i + metadata->alto][j].pixel[0] = 100;
            nueva_imagen[i + metadata->alto][j].pixel[1] = 150;
            nueva_imagen[i + metadata->alto][j].pixel[2] = 200;
        }
    }

    metadata->ancho = nuevo_ancho;
    metadata->alto = nuevo_alto;

    escribirImagenBMP("concatenar_vertical.bmp", metadata, nueva_imagen);

    return TODO_OK;
}

int concatenarHorizontal(t_pixel **imagen, t_pixel **imagen2, t_metadata *metadata, t_metadata *metadata2)
{

    unsigned int nuevo_ancho = metadata->ancho + metadata2->ancho;
    unsigned int nuevo_alto = metadata->alto > metadata2->alto ? metadata->alto : metadata2->alto;

    t_pixel **nueva_imagen = (t_pixel**)malloc(nuevo_alto * sizeof(t_pixel*));

    for (int i = 0; i < nuevo_alto; i++)
    {
        nueva_imagen[i] = (t_pixel*)malloc(nuevo_ancho * sizeof(t_pixel));
    }

    // Copiar los píxeles de la primera imagen
    for (int i = 0; i < metadata->alto; i++)
    {
        for (int j = 0; j < metadata->ancho; j++)
        {
            nueva_imagen[i][j] = imagen[i][j];
        }
    }

    // Rellenar los bordes si es necesario
    for (int i = metadata->alto; i < nuevo_alto; i++)
    {
        for (int j = 0; j < metadata->ancho; j++)
        {
            nueva_imagen[i][j].pixel[0] = 100;
            nueva_imagen[i][j].pixel[1] = 150;
            nueva_imagen[i][j].pixel[2] = 200;
        }
    }

    // Copiar los píxeles de la segunda imagen
    for (int i = 0; i < metadata2->alto; i++)
    {
        for (int j = 0; j < metadata2->ancho; j++)
        {
            nueva_imagen[i][j + metadata->ancho] = imagen2[i][j];
        }
    }

    // Rellenar los bordes si es necesario
    for (int i = metadata2->alto; i < nuevo_alto; i++)
    {
        for (int j = 0; j < metadata2->ancho; j++)
        {
            nueva_imagen[i][j + metadata->ancho].pixel[0] = 100;
            nueva_imagen[i][j + metadata->ancho].pixel[1] = 150;
            nueva_imagen[i][j + metadata->ancho].pixel[2] = 200;
        }
    }

    // Actualizar el metadata de la nueva imagen
    metadata->ancho = nuevo_ancho;
    metadata->alto = nuevo_alto;

    escribirImagenBMP("concatenar_horizontal.bmp", metadata, nueva_imagen);

    return TODO_OK;
}

int ajustarContraste(t_pixel **imagen, t_metadata *metadata, int parametro)
{
    float porcentaje = (259.0 * (parametro + 255.0)) / (255.0 * (259.0 - parametro));
    float valor_original = 0;
    float valor_nuevo = 0;

    for (unsigned int i = 0; i < metadata->alto; i++)
    {
        for (unsigned int j = 0; j < metadata->ancho; j++)
        {
            for (int k = 0; k < 3; k++)
            {
                valor_original = imagen[i][j].pixel[k];
                valor_nuevo = porcentaje * (valor_original - 128) + 128;
                if(valor_nuevo < 0)
                {
                    valor_nuevo = 0;
                }
                else if(valor_nuevo > 255)
                {
                    valor_nuevo = 255;
                }
                imagen[i][j].pixel[k] = (unsigned char)valor_nuevo;
            }
        }
    }

    escribirImagenBMP("aumentar_contraste.bmp", metadata, imagen);
    return TODO_OK;
}

int rotarImagenDerecha(t_pixel** imagen, t_metadata *metadata)
{
    unsigned int nuevo_ancho = metadata->alto;
    unsigned int nuevo_alto = metadata->ancho;

    t_pixel **imagenRotada = (t_pixel **)malloc(nuevo_alto * sizeof(t_pixel *));

    if(imagenRotada == NULL)
    {
        return ERR_MEMORIA;
    }

    for(unsigned int i = 0; i < nuevo_alto; i++)
    {
        imagenRotada[i] = (t_pixel *)malloc(nuevo_ancho * sizeof(t_pixel));
    }

    for(unsigned int i = 0; i < nuevo_alto; i++)
    {
        for(unsigned int j = 0; j < nuevo_ancho; j++)
        {
            imagenRotada[i][j] = imagen[metadata->alto - 1 - j][i];
        }
    }

    for(unsigned int i = 0; i < metadata->alto; i++)
    {
        free(imagen[i]);
    }

    free(imagen);

    imagen = imagenRotada;

    metadata->ancho = nuevo_ancho;
    metadata->alto = nuevo_alto;

    escribirImagenBMP("rotar_derecha.bmp", metadata, imagen);

    return 0;
}

int rotarImagenIzquierda(t_pixel** imagen, t_metadata *metadata)
{
    unsigned int nuevo_ancho = metadata->alto;
    unsigned int nuevo_alto = metadata->ancho;

    t_pixel **imagenRotada = (t_pixel **)malloc(nuevo_alto * sizeof(t_pixel *));

    if(imagenRotada == NULL)
    {
        return ERR_MEMORIA;
    }

    for(unsigned int i = 0; i < nuevo_alto; i++)
    {
        imagenRotada[i] = (t_pixel *)malloc(nuevo_ancho * sizeof(t_pixel));
    }

    for(unsigned int i = 0; i < nuevo_alto; i++)
    {
        for(unsigned int j = 0; j < nuevo_ancho; j++)
        {
            imagenRotada[i][j] = imagen[j][metadata->ancho - 1 - i];
        }
    }

    for(unsigned int i = 0; i < metadata->alto; i++)
    {
        free(imagen[i]);
    }

    free(imagen);

    imagen = imagenRotada;

    metadata->ancho = nuevo_ancho;
    metadata->alto = nuevo_alto;

    escribirImagenBMP("rotar_izquierda.bmp", metadata, imagen);

    return 0;
}

int tonalidadRoja(t_pixel **imagen, t_metadata *metadata, int param)
{
    for (unsigned int i = 0; i < metadata->alto; i++)
    {
        for (unsigned int j = 0; j < metadata->ancho; j++)
        {
            int valor = imagen[i][j].pixel[2] + (imagen[i][j].pixel[2] / 2);
            imagen[i][j].pixel[2] = (valor > 255) ? 255 : valor;
        }
    }

    escribirImagenBMP("tonalidad_roja.bmp", metadata, imagen);
    return TODO_OK;
}

int tonalidadVerde(t_pixel **imagen, t_metadata *metadata, int param)
{
    for (unsigned int i = 0; i < metadata->alto; i++)
    {
        for (unsigned int j = 0; j < metadata->ancho; j++)
        {
            int valor = imagen[i][j].pixel[1] + (imagen[i][j].pixel[1] / 2);
            imagen[i][j].pixel[1] = (valor > 255) ? 255 : valor;
        }
    }
    escribirImagenBMP("tonalidad_verde.bmp", metadata, imagen);
    return TODO_OK;
}

int tonalidadAzul(t_pixel **imagen, t_metadata *metadata, int param)
{
    for (unsigned int i = 0; i < metadata->alto; i++)
    {
        for (unsigned int j = 0; j < metadata->ancho; j++)
        {
            int valor = imagen[i][j].pixel[0] + (imagen[i][j].pixel[0] / 2);
            imagen[i][j].pixel[0] = (valor > 255) ? 255 : valor;
        }
    }
    escribirImagenBMP("tonalidad_azul.bmp", metadata, imagen);
    return TODO_OK;
}

int espejarHorizontal(t_pixel **imagen, t_metadata *metadata) {

    t_pixel **imagen_temporal = (t_pixel **)malloc(sizeof(t_pixel *) * metadata->alto);
    for(unsigned int i = 0; i < metadata->alto; i++)
    {
        imagen_temporal[i] = (t_pixel *)malloc(metadata->ancho * sizeof(t_pixel));
    }

    for(int i = 0; i < metadata->alto; i++) {
        for(int j = 0; j < metadata->ancho/2; j++) {
            imagen_temporal[i][j] = imagen[i][j];
            imagen[i][j] = imagen[i][metadata->ancho - j];
            imagen[i][metadata->ancho - j] = imagen_temporal[i][j];
        }
    }

    escribirImagenBMP("espejar_horizontal.bmp", metadata, imagen);
    liberarMemoriaImg(imagen_temporal, metadata->alto);

    return TODO_OK;
}
