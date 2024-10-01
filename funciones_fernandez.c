#include "estructuras.h"
#include "funciones_bernal.h"

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
            printf("\n\n*** Error: Ha ingresado un parametro invalido ***");
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

    if(*numArchivos > 0) {
        mostrarArgumentos(nombreArchivoBMP, filtros, *numFiltros, *numArchivos);
    }
}

void mostrarArgumentos(char *nombreArchivoBMP[], char *filtros[], int numFiltros, int numArchivos)
{
    printf("\nArchivos leidos:");
    for(int i = 0; i < numArchivos; i++)
    {
        printf("\nArchivo %d: %s", i + 1, nombreArchivoBMP[i]);
    }
    puts("\n\n----------------------------------");
    printf("\nFiltros ingresados:");
    for(int i = 0; i < numFiltros; i++)
    {
        printf("\nFiltro %d: %s", i + 1, filtros[i]);
    }
    puts("\n\n----------------------------------");
}

t_pixel **leerImagenBMP(char *nombreArchivoBMP, t_metadata *metadata)
{
    FILE * archivo = fopen(nombreArchivoBMP, "rb");

    if(!archivo)
    {
        perror("\n\nError: No es posible abrir el archivo.");
        return NULL;
    }

    leerEncabezado(archivo, metadata);

    if(metadata->profundidad != 24)
    {
        perror("\n\nError: Solo se admiten imágenes de 24 bits.");
        fclose(archivo);
        return NULL;
    }

    t_pixel **matriz = (t_pixel **)malloc(metadata->alto * sizeof(t_pixel *));
    if(matriz == NULL)
    {
        perror("\n\nError: No es posible asignar espacio de memoria");
        fclose(archivo);
        return NULL;
    }

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

void liberarMemoriaMetadata(t_metadata *metadata)
{
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
        perror("\n\nError: El archivo indicado no es de tipo bit map.");
    }
}

void escribirImagenBMP(const char *nombreArchivo, t_metadata *metadata, t_pixel **matriz, char *nombreArchivoBMP[])
{
    char * nombreArchivoCompleto = (char *)malloc(256);
    strcpy(nombreArchivoCompleto, "DIGNIDAD_");
    strcat(nombreArchivoCompleto, nombreArchivo);
    strcat(nombreArchivoCompleto, "_");
    strcat(nombreArchivoCompleto, nombreArchivoBMP[0]);

    FILE *archivo = fopen(nombreArchivoCompleto, "wb");
    if (!archivo)
    {
        perror("\n\nError: No es posible abrir el archivo para escritura.");
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

    printf("\n\nSe creo el archivo: %s", nombreArchivoCompleto);

//    liberarMemoriaImg(matriz, metadata->alto);

    fclose(archivo);
}

int concatenarVertical(t_pixel **imagen, t_pixel **imagen2, t_metadata *metadata, t_metadata *metadata2, char * nombreArchivoBMP[])
{

    unsigned int nuevo_ancho = metadata->ancho > metadata2->ancho ? metadata->ancho : metadata2->ancho;
    unsigned int nuevo_alto = (metadata->alto + metadata2->alto);

    t_pixel **nueva_imagen = (t_pixel**)malloc(nuevo_alto * sizeof(t_pixel*));
    if(nueva_imagen == NULL)
    {
        printf("\n\n*** Error: No es posible asignar memoria para la nueva imagen. ***");
        return ERR_MEMORIA;
    }


    for (int i = 0; i < nuevo_alto; i++)
    {
        nueva_imagen[i] = (t_pixel*)malloc(nuevo_ancho * sizeof(t_pixel));
        if (nueva_imagen[i] == NULL)
        {
            printf("\n\n*** Error: No es posible asignar memoria para las filas de la nueva imagen. ***");
            liberarMemoriaImg(nueva_imagen, i);
            return ERR_MEMORIA;
        }
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

    escribirImagenBMP("concatenar-vertical", metadata, nueva_imagen, nombreArchivoBMP);

    liberarMemoriaImg(nueva_imagen, metadata->alto);

    return TODO_OK;
}

int concatenarHorizontal(t_pixel **imagen, t_pixel **imagen2, t_metadata *metadata, t_metadata *metadata2, char * nombreArchivoBMP[])
{
    unsigned int nuevo_ancho = metadata->ancho + metadata2->ancho;
    unsigned int nuevo_alto = metadata->alto > metadata2->alto ? metadata->alto : metadata2->alto;

    t_pixel **nueva_imagen = (t_pixel**)malloc(nuevo_alto * sizeof(t_pixel*));
    if(nueva_imagen == NULL)
    {
        printf("\n\n*** Error: No es posible asignar memoria para la nueva imagen. ***");
        return ERR_MEMORIA;
    }

    for (int i = 0; i < nuevo_alto; i++)
    {
        nueva_imagen[i] = (t_pixel*)malloc(nuevo_ancho * sizeof(t_pixel));
        if (nueva_imagen[i] == NULL)
        {
            printf("\n\n*** Error: No es posible asignar memoria para las filas de la nueva imagen. ***");
            liberarMemoriaImg(nueva_imagen, i);
            return ERR_MEMORIA;
        }
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

    escribirImagenBMP("concatenar-horizontal", metadata, nueva_imagen, nombreArchivoBMP);

    liberarMemoriaImg(nueva_imagen, metadata->alto);

    return TODO_OK;
}

int ajustarContraste(t_pixel **imagen, t_metadata *metadata, int parametro, char * nombreArchivoBMP[], char tipoDeAjuste[])
{
    if (parametro <= 0 || parametro > 100)
    {
        printf("\n\n*** Error: Debe ingresar un parametro comprendido entre 0 y 100 ***");
        return ERR_PARAMETRO;
    }

    if(strcmp(tipoDeAjuste, "reducir") == 0)
    {
        parametro = -parametro;
    }

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

    escribirImagenBMP(strcmp(tipoDeAjuste, "aumentar") == 0 ? "aumentar-contraste" : "reducir-contraste", metadata, imagen, nombreArchivoBMP);

    return TODO_OK;
}

int rotarImagenDerecha(t_pixel** imagen, t_metadata *metadata, char * nombreArchivoBMP[])
{
    unsigned int nuevo_ancho = metadata->alto;
    unsigned int nuevo_alto = metadata->ancho;

    t_pixel **imagenRotada = (t_pixel **)malloc(nuevo_alto * sizeof(t_pixel *));
    if(imagenRotada == NULL)
    {
        printf("\n\n*** Error: No es posible asignar memoria para la nueva imagen. ***");
        return ERR_MEMORIA;
    }

    for(unsigned int i = 0; i < nuevo_alto; i++)
    {
        imagenRotada[i] = (t_pixel *)malloc(nuevo_ancho * sizeof(t_pixel));
        if (imagenRotada[i] == NULL)
        {
            printf("\n\n*** Error: No es posible asignar memoria para las filas de la nueva imagen. ***");
            liberarMemoriaImg(imagenRotada, i);
            return ERR_MEMORIA;
        }
    }

    for(unsigned int i = 0; i < nuevo_alto; i++)
    {
        for(unsigned int j = 0; j < nuevo_ancho; j++)
        {
            imagenRotada[i][j] = imagen[metadata->alto - 1 - j][i];
        }
    }

    liberarMemoriaImg(imagen, metadata->alto);

    imagen = imagenRotada;

    metadata->ancho = nuevo_ancho;
    metadata->alto = nuevo_alto;

    escribirImagenBMP("rotar-derecha", metadata, imagen, nombreArchivoBMP);

    liberarMemoriaImg(imagenRotada, metadata->alto);

    return 0;
}

int rotarImagenIzquierda(t_pixel** imagen, t_metadata *metadata, char * nombreArchivoBMP[])
{
    unsigned int nuevo_ancho = metadata->alto;
    unsigned int nuevo_alto = metadata->ancho;

    t_pixel **imagenRotada = (t_pixel **)malloc(nuevo_alto * sizeof(t_pixel *));
    if(imagenRotada == NULL)
    {
        printf("\n\n*** Error: No es posible asignar memoria para la nueva imagen. ***");
        return ERR_MEMORIA;
    }

    for(unsigned int i = 0; i < nuevo_alto; i++)
    {
        imagenRotada[i] = (t_pixel *)malloc(nuevo_ancho * sizeof(t_pixel));
        if (imagenRotada[i] == NULL)
        {
            printf("\n\n*** Error: No es posible asignar memoria para las filas de la nueva imagen. ***");
            liberarMemoriaImg(imagenRotada, i);
            return ERR_MEMORIA;
        }
    }

    for(unsigned int i = 0; i < nuevo_alto; i++)
    {
        for(unsigned int j = 0; j < nuevo_ancho; j++)
        {
            imagenRotada[i][j] = imagen[j][metadata->ancho - 1 - i];
        }
    }

    liberarMemoriaImg(imagen, metadata->alto);

    imagen = imagenRotada;

    metadata->ancho = nuevo_ancho;
    metadata->alto = nuevo_alto;

    escribirImagenBMP("rotar-izquierda", metadata, imagen, nombreArchivoBMP);

    liberarMemoriaImg(imagenRotada, metadata->alto);
    return 0;
}

int tonalidadRoja(t_pixel **imagen, t_metadata *metadata, int param, char * nombreArchivoBMP[])
{
    if (param <= 0 || param > 100)
    {
        printf("\n\n*** Error: Debe ingresar un parametro comprendido entre 0 y 100 ***");
        return ERR_PARAMETRO;
    }

    for (unsigned int i = 0; i < metadata->alto; i++)
    {
        for (unsigned int j = 0; j < metadata->ancho; j++)
        {
            int valor = imagen[i][j].pixel[2] + (imagen[i][j].pixel[2] / 2);
            imagen[i][j].pixel[2] = (valor > 255) ? 255 : valor;
        }
    }

    escribirImagenBMP("tonalidad-roja", metadata, imagen, nombreArchivoBMP);
    return TODO_OK;
}

int tonalidadVerde(t_pixel **imagen, t_metadata *metadata, int param, char * nombreArchivoBMP[])
{
    if (param <= 0 || param > 100)
    {
        printf("\n\n*** Error: Debe ingresar un parametro comprendido entre 0 y 100 ***");
        return ERR_PARAMETRO;
    }

    for (unsigned int i = 0; i < metadata->alto; i++)
    {
        for (unsigned int j = 0; j < metadata->ancho; j++)
        {
            int valor = imagen[i][j].pixel[1] + (imagen[i][j].pixel[1] / 2);
            imagen[i][j].pixel[1] = (valor > 255) ? 255 : valor;
        }
    }
    escribirImagenBMP("tonalidad-verde", metadata, imagen, nombreArchivoBMP);
    return TODO_OK;
}

int tonalidadAzul(t_pixel **imagen, t_metadata *metadata, int param, char * nombreArchivoBMP[])
{
    if (param <= 0 || param > 100)
    {
        printf("\n\n*** Error: Debe ingresar un parametro comprendido entre 0 y 100 ***");
        return ERR_PARAMETRO;
    }

    for (unsigned int i = 0; i < metadata->alto; i++)
    {
        for (unsigned int j = 0; j < metadata->ancho; j++)
        {
            int valor = imagen[i][j].pixel[0] + (imagen[i][j].pixel[0] / 2);
            imagen[i][j].pixel[0] = (valor > 255) ? 255 : valor;
        }
    }
    escribirImagenBMP("tonalidad-azul", metadata, imagen, nombreArchivoBMP);
    return TODO_OK;
}

int espejarHorizontal(t_pixel **imagen, t_metadata *metadata, char * nombreArchivoBMP[])
{

    t_pixel **imagen_temporal = (t_pixel **)malloc(sizeof(t_pixel *) * metadata->alto);
    if(imagen_temporal == NULL)
    {
        printf("\n\n*** Error: No es posible asignar memoria para la nueva imagen. ***");
        return ERR_MEMORIA;
    }

    for(unsigned int i = 0; i < metadata->alto; i++)
    {
        imagen_temporal[i] = (t_pixel *)malloc(metadata->ancho * sizeof(t_pixel));
        if (imagen_temporal[i] == NULL)
        {
            printf("\n\n*** Error: No es posible asignar memoria para las filas de la nueva imagen. ***");
            liberarMemoriaImg(imagen_temporal, i);
            return ERR_MEMORIA;
        }
    }

    for(int i = 0; i < metadata->alto; i++)
    {
        for(int j = 0; j < metadata->ancho/2; j++)
        {
            imagen_temporal[i][j] = imagen[i][j];
            imagen[i][j] = imagen[i][metadata->ancho - j];
            imagen[i][metadata->ancho - j] = imagen_temporal[i][j];
        }
    }

    escribirImagenBMP("espejar-horizontal", metadata, imagen, nombreArchivoBMP);
    liberarMemoriaImg(imagen_temporal, metadata->alto);

    return TODO_OK;
}

int ruido(t_pixel **imagen, t_metadata *metadata, char * nombreArchivoBMP[])
{
    srand(time(NULL)); // Generar el ruido aleatoriamente

    for (int i = 0; i < metadata->alto; i++)
    {
        for (int j = 0; j < metadata->ancho; j++)
        {
            // Generar un valor aleatorio entre -intensidad e intensidad para cada canal de color
            int r = rand() % (2 * 21) - 20;
            int g = rand() % (2 * 21) - 20;
            int b = rand() % (2 * 21) - 20;

            // Ajustar los valores de los píxeles para que queden dentro del rango válido (0-255)
            imagen[i][j].pixel[0] = limitador(imagen[i][j].pixel[0] + r, 0, 255);
            imagen[i][j].pixel[1] = limitador(imagen[i][j].pixel[1] + g, 0, 255);
            imagen[i][j].pixel[2] = limitador(imagen[i][j].pixel[2] + b, 0, 255);
        }
    }

    escribirImagenBMP("comodin", metadata, imagen, nombreArchivoBMP);
    return TODO_OK;
}

int limitador(int value, int min, int max)
{
    return value < min ? min : (value > max ? max : value);
}
