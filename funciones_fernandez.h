#ifndef FUNCIONES_FERNANDEZ_H_INCLUDED
#define FUNCIONES_FERNANDEZ_H_INCLUDED
#include "estructuras.h"

void obtenerArgumentos(char *nombreArchivoBMP[], char *filtros[], int argc, char *argv[], int *numFiltros, int *numArchivos);
void mostrarArgumentos(char *nombreArchivoBMP[], char *filtros[], int numFiltros, int numArchivos);
t_pixel **leerImagenBMP(char *nombreArchivoBMP, t_metadata *header);
void leerEncabezado(FILE *archivo, t_metadata *metadata);
int rotarImagenDerecha(t_pixel** imagen, t_metadata *metadata);
int rotarImagenIzquierda(t_pixel** imagen, t_metadata *metadata);
void escribirImagenBMP(const char *nombreArchivo, t_metadata *metadata, t_pixel **matriz);
void liberarMemoriaImg(t_pixel **matriz, unsigned int alto);
void liberarMemoriaMetadata(t_metadata *metadata);
int ajustarContraste(t_pixel **imagen, t_metadata *metadata , int parametro);
int tonalidadVerde(t_pixel **imagen, t_metadata *metadata, int param);
int tonalidadRoja(t_pixel **imagen, t_metadata *metadata, int param);
int tonalidadAzul(t_pixel **imagen, t_metadata *metadata, int param);
int concatenarVertical(t_pixel **imagen, t_pixel **imagen2, t_metadata *metadata, t_metadata *metadata2);
int concatenarHorizontal(t_pixel **imagen, t_pixel **imagen2, t_metadata *metadata, t_metadata *metadata2);
int espejarHorizontal(t_pixel **imagen, t_metadata *metadata);
void extraerFiltroYParametro(const char *entrada, char *filtro, float *parametro);
#endif // FUNCIONES_FERNANDEZ_H_INCLUDED
