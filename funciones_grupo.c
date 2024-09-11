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

    char *nombreArchivoBMP;
    char *filtros[argc];
    int numFiltros = 0;

    obtenerArgumentos(&nombreArchivoBMP, filtros, argc, argv, &numFiltros);

    if(nombreArchivoBMP == NULL)
    {
        printf("Ejecucion finalizada. No se ha ingresado ninguna imagen BMP");
        return ERR_ARCHIVO_BMP;
    }

    t_metadata *metadata = malloc(sizeof(t_metadata));
    t_pixel **imagen = leerImagenBMP(nombreArchivoBMP, metadata);

    if(imagen == NULL)
    {
        perror("\nNo ha sido posible leer la imagen");
        return ERR_ARCHIVO_BMP;
    }

    rotarImagenDerecha(imagen, metadata);
    return 0;
}

void obtenerArgumentos(char **nombreArchivoBMP, char *filtros[], int argc, char *argv[], int *numFiltros)
{
    for(int i=1; i < argc; i++)
    {
        if(strstr(argv[i], ".bmp"))
        {
            *nombreArchivoBMP = argv[i];
        }
        else if(strstr(argv[i], "--"))
        {
            filtros[*numFiltros] = argv[i];
            (*numFiltros)++;
        }
    }
    mostrarArgumentos(nombreArchivoBMP, filtros, *numFiltros);
}

void mostrarArgumentos(char **nombreArchivoBMP, char *filtros[], int numFiltros)
{
    printf("%s\n", *nombreArchivoBMP);
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

void escribirImagenBMP(const char *nombreArchivo, t_metadata *metadata, t_pixel **matriz) {

    FILE *archivo = fopen(nombreArchivo, "wb");
    if (!archivo) {
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
    for (int i = metadata->alto - 1; i >= 0; i--) {
        for (unsigned int j = 0; j < metadata->ancho; j++) {
            fwrite(matriz[i][j].pixel, 3, 1, archivo);
        }
        // Escribir el padding
        for (int p = 0; p < padding; p++) {
            fputc(0, archivo);
        }
    }

    fclose(archivo);
}
