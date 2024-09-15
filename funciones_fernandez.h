#ifndef FUNCIONES_FERNANDEZ_H_INCLUDED
#define FUNCIONES_FERNANDEZ_H_INCLUDED
#include "estructuras.h"

int rotarImagenDerecha(t_pixel** imagen, t_metadata *metadata, char * nombreArchivoBMP[]);
int ajustarContraste(t_pixel **imagen, t_metadata *metadata , int parametro, char * nombreArchivoBMP[], char tipoDeAjuste[]);
int tonalidadVerde(t_pixel **imagen, t_metadata *metadata, int param, char * nombreArchivoBMP[]);
int tonalidadRoja(t_pixel **imagen, t_metadata *metadata, int param, char * nombreArchivoBMP[]);
int concatenarHorizontal(t_pixel **imagen, t_pixel **imagen2, t_metadata *metadata, t_metadata *metadata2, char * nombreArchivoBMP[]);
int espejarHorizontal(t_pixel **imagen, t_metadata *metadata, char * nombreArchivoBMP[]);
int negativo(t_pixel **imagen, t_metadata *metadata, char * nombreArchivoBMP[]);
int tonalidadAzul(t_pixel **imagen, t_metadata *metadata, int param, char * nombreArchivoBMP[]);
int ruido(t_pixel **imagen, t_metadata *metadata, char * nombreArchivoBMP[]);

void obtenerArgumentos(char *nombreArchivoBMP[], char *filtros[], int argc, char *argv[], int *numFiltros, int *numArchivos);
void mostrarArgumentos(char *nombreArchivoBMP[], char *filtros[], int numFiltros, int numArchivos);
t_pixel **leerImagenBMP(char *nombreArchivoBMP, t_metadata *header);
void leerEncabezado(FILE *archivo, t_metadata *metadata);
void liberarMemoriaImg(t_pixel **matriz, unsigned int alto);
void liberarMemoriaMetadata(t_metadata *metadata);
void escribirImagenBMP(const char *nombreArchivo, t_metadata *metadata, t_pixel **matriz, char *nombreArchivoBMP[]);
void extraerFiltroYParametro(const char *entrada, char *filtro, float *parametro);
int limitador(int value, int min, int max);
#endif // FUNCIONES_FERNANDEZ_H_INCLUDED
