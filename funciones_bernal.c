#include "estructuras.h"
#include "constantes.h"
#include "funciones_fernandez.h"
#include "funciones_fernandez.c"

int escalaDeGrises(t_pixel **imagen, t_metadata *metadata, char * nombreArchivoBMP[])
{
    for (unsigned int i = 0; i < metadata->alto; i++)
    {
        for (unsigned int j = 0; j < metadata->ancho; j++)
        {
            // Calculo del promedio de los valores RGB
            unsigned char gris = (imagen[i][j].pixel[0] + imagen[i][j].pixel[1] + imagen[i][j].pixel[2]) / 3;

            imagen[i][j].pixel[0] = gris;  // B
            imagen[i][j].pixel[1] = gris;  // G
            imagen[i][j].pixel[2] = gris;  // R
        }
    }

    escribirImagenBMP("escala-de-grises", metadata, imagen, nombreArchivoBMP);

    return TODO_OK;
}

int espejarVertical(t_pixel **imagen, t_metadata *metadata, char * nombreArchivoBMP[])
{
    // Recorremos solo la mitad de las filas
    for (unsigned int i = 0; i < metadata->alto / 2; i++)
    {
        // Intercambo cada píxel de la fila 'i' con la fila 'alto - i - 1'
        for (unsigned int j = 0; j < metadata->ancho; j++)
        {
            // Guardamos el pixel de la fila superior en una variable temporal
            t_pixel temp = imagen[i][j];

            // Espejamos el pixel de la fila inferior en la fila superior
            imagen[i][j] = imagen[metadata->alto - i - 1][j];

            // Colocamos el pixel guardado temporalmente en la fila inferior
            imagen[metadata->alto - i - 1][j] = temp;
        }
    }
    escribirImagenBMP("espejar-vertical", metadata, imagen, nombreArchivoBMP);
    return TODO_OK;
}

int negativo(t_pixel **imagen, t_metadata *metadata, char * nombreArchivoBMP[])
{
    for (unsigned int i = 0; i < metadata->alto; i++)
    {
        for (unsigned int j = 0; j < metadata->ancho; j++)
        {
            // Inverto los valores RGB de cada píxel para obtener el negativo
            imagen[i][j].pixel[0] = 255 - imagen[i][j].pixel[0];  // B
            imagen[i][j].pixel[1] = 255 - imagen[i][j].pixel[1];  // G
            imagen[i][j].pixel[2] = 255 - imagen[i][j].pixel[2];  // R
        }
    }

    escribirImagenBMP("negativo", metadata, imagen, nombreArchivoBMP);
    return TODO_OK;
}

int recortar(t_pixel ***imagen, t_metadata *metadata, float parametro, char * nombreArchivoBMP[])
{
    // Valido que el parametro esté en el rango de 0 a 100
    if (parametro <= 0 || parametro > 100)
    {
        printf("\n\n*** Error: Parametro fuera de rango. Debe estar entre 0 y 100. ***");
        return ERR_PARAMETRO;
    }

    // Calculo las nuevas dimensiones basadas en el parametro
    unsigned int nuevo_ancho = (unsigned int)(metadata->ancho * (parametro / 100));
    unsigned int nuevo_alto = (unsigned int)(metadata->alto * (parametro / 100));

    // Creo una nueva matriz de píxeles para la imagen recortada
    t_pixel **nueva_imagen = malloc(nuevo_alto * sizeof(t_pixel *));
    if (nueva_imagen == NULL)
    {
        printf("\n\n*** Error: No es posible asignar memoria para la nueva imagen. ***");
        return ERR_MEMORIA;
    }

    for (unsigned int i = 0; i < nuevo_alto; i++)
    {
        nueva_imagen[i] = malloc(nuevo_ancho * sizeof(t_pixel));
        if (nueva_imagen[i] == NULL)
        {
            printf("\n\n*** Error: No es posible asignar memoria para las filas de la nueva imagen. ***");
            liberarMemoriaImg(nueva_imagen, i);
            return ERR_MEMORIA;
        }
    }

    // Copio los píxeles dentro de las nuevas dimensiones
    for (unsigned int i = 0; i < nuevo_alto; i++)
    {
        for (unsigned int j = 0; j < nuevo_ancho; j++)
        {
            nueva_imagen[i][j] = (*imagen)[i][j];  // Copio el píxel original
        }
    }

    // Actualizo los metadatos
    metadata->ancho = nuevo_ancho;
    metadata->alto = nuevo_alto;

    liberarMemoriaImg(*imagen, metadata->alto);

    // Asigno la nueva imagen al puntero original
    *imagen = nueva_imagen;

    escribirImagenBMP("recortar", metadata, *imagen, nombreArchivoBMP);

    liberarMemoriaImg(nueva_imagen, metadata->alto);

    return TODO_OK;
}

int achicar(t_pixel **imagen, t_metadata *metadata, unsigned int parametro, char * nombreArchivoBMP[])
{

    if (parametro <= 0 || parametro > 100)
    {
        printf("\n\n*** Error: Parametro fuera de rango. Debe estar entre 0 y 100. ***");
        return ERR_PARAMETRO;
    }

    // Calculo las nuevas dimensiones basadas en el porcentaje
    unsigned int nuevo_ancho = (metadata->ancho * parametro) / 100;
    unsigned int nuevo_alto = (metadata->alto * parametro) / 100;

    // Creo una nueva matriz de píxeles con las nuevas dimensiones
    t_pixel **nuevaImagen = (t_pixel **)malloc(nuevo_alto * sizeof(t_pixel *));
    if (!nuevaImagen)
    {
        printf("\n\n*** Error: No es posible asignar memoria para la nueva imagen. ***");
        return ERR_MEMORIA; // Error al asignar memoria
    }

    for (unsigned int i = 0; i < nuevo_alto; i++)
    {
        nuevaImagen[i] = (t_pixel *)malloc(nuevo_ancho * sizeof(t_pixel));

        if (!nuevaImagen[i])
        {
            return ERR_MEMORIA; // Error al asignar memoria
        }
    }

    // Modifico los píxeles de la imagen original a la nueva imagen

    for (unsigned int i = 0; i < nuevo_alto; i++)
    {
        for (unsigned int j = 0; j < nuevo_ancho; j++)
        {
            // Obtengo el índice del píxel original más cercano (vecino más cercano)
            unsigned int origen_x = (j * metadata->ancho) / nuevo_ancho;
            unsigned int origen_y = (i * metadata->alto) / nuevo_alto;

            // Copioel píxel de la imagen original a la nueva imagen
            nuevaImagen[i][j] = imagen[origen_y][origen_x];
        }
    }

    // Metadatos con el nuevo tamaño

    metadata->ancho = nuevo_ancho;

    metadata->alto = nuevo_alto;

    escribirImagenBMP("achicar", metadata, nuevaImagen, nombreArchivoBMP);

    liberarMemoriaImg(nuevaImagen, metadata->alto);

    return TODO_OK;
}
