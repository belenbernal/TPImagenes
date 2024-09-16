/*
    Integrantes del grupo. En caso de ser un grupo de dos integrantes, no completar el último campo.
    Si alguno de los integrantes del grupo dejara la materia, completar de todos modos sus datos, aclarando que no entrega.
    -----------------
    Apellido: Fernandez
    Nombre: Thomas Uriel
    DNI: 42838357
    Entrega: SI
    -----------------
    Apellido:Bernal
    Nombre:Ana Belen
    DNI:41713487
    Entrega:SI
    -----------------
    Apellido: Mariani
    Nombre: Antonella Lucia
    DNI:41586421
    Entrega: NO
    -----------------
*/

#include "funciones_grupo.h"
#include "constantes.h"
#include "estructuras.h"

int solucion(int argc, char* argv[])
{
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
        printf("\n\n*** Error: No se ha ingresado ninguna imagen BMP ***");
        return ERR_ARCHIVO_BMP;
    }

    t_metadata *metadata = malloc(sizeof(t_metadata));
    t_pixel **imagen = leerImagenBMP(*nombreArchivoBMP, metadata);

    if(imagen == NULL)
    {
        perror("\n\nError: No ha sido posible leer la imagen BMP.");
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
            res = escalaDeGrises(imagen, metadata, nombreArchivoBMP);
        }
        else if(strcmp(filtro, "--rotar-derecha") == 0)
        {
            res = rotarImagenDerecha(imagen, metadata, nombreArchivoBMP);
        }
        else if(strcmp(filtro, "--rotar-izquierda") == 0)
        {
            res = rotarImagenIzquierda(imagen, metadata, nombreArchivoBMP);
        }
        else if(strcmp(filtro, "--concatenar-horizontal") == 0)
        {
            if(numArchivos > 1)
            {
                t_metadata *metadata2 = malloc(sizeof(t_metadata));
                t_pixel **imagen2 = leerImagenBMP(nombreArchivoBMP[1], metadata2);
                res = concatenarHorizontal(imagen, imagen2, metadata, metadata2, nombreArchivoBMP);
                liberarMemoriaImg(imagen2, metadata2->alto);
                liberarMemoriaMetadata(metadata2);
            }
            else
            {
                printf("\n\n*** Error: Debe ingresar otra imagen BMP como argumento ***");
                res = ERR_ARCHIVO_BMP;
            }
        }
        else if(strcmp(filtro, "--concatenar-vertical") == 0)
        {
            if(numArchivos > 1)
            {
                t_metadata *metadata2 = malloc(sizeof(t_metadata));
                t_pixel **imagen2 = leerImagenBMP(nombreArchivoBMP[1], metadata2);
                res = concatenarVertical(imagen, imagen2, metadata, metadata2, nombreArchivoBMP);
                liberarMemoriaImg(imagen2, metadata2->alto);
                liberarMemoriaMetadata(metadata2);
            }
            else
            {
                printf("\n\n*** Error: Debe ingresar otra imagen BMP como argumento ***");
                res = ERR_ARCHIVO_BMP;
            }
        }
        else if(strcmp(filtro, "--aumentar-contraste") == 0)
        {
            if(parametro != -1)
            {
                res = ajustarContraste(imagen, metadata, parametro, nombreArchivoBMP, "aumentar");
            }
            else
            {
                printf("\n\n*** Error: Debe ingresar un parametro comprendido entre 0 y 100 ***");
                res = ERR_PARAMETRO;
            }
        }
        else if(strcmp(filtro, "--reducir-contraste") == 0)
        {
            if(parametro != -1)
            {
                res = ajustarContraste(imagen, metadata, parametro, nombreArchivoBMP, "reducir");
            }
            else
            {
                printf("\n\n*** Error: Debe ingresar un parametro comprendido entre 0 y 100 ***");
                res = ERR_PARAMETRO;
            }
        }
        else if(strcmp(filtro, "--tonalidad-verde") == 0)
        {
            if(parametro != -1)
            {
                res = tonalidadVerde(imagen, metadata, parametro, nombreArchivoBMP);
            }
            else
            {
                printf("\n\n*** Error: Debe ingresar un parametro comprendido entre 0 y 100 ***");
                res = ERR_PARAMETRO;
            }
        }
        else if(strcmp(filtro, "--tonalidad-azul") == 0)
        {
            if(parametro != -1)
            {
                res = tonalidadAzul(imagen, metadata, parametro, nombreArchivoBMP);
            }
            else
            {
                printf("\n\n*** Error: Debe ingresar un parametro comprendido entre 0 y 100 ***");
                res = ERR_PARAMETRO;
            }
        }
        else if(strcmp(filtro, "--tonalidad-roja") == 0)
        {
            if(parametro != -1)
            {
                res = tonalidadRoja(imagen, metadata, parametro, nombreArchivoBMP);
            }
            else
            {
                printf("\n\n*** Error: Debe ingresar un parametro comprendido entre 0 y 100 ***");
                res = ERR_PARAMETRO;
            }
        }
        else if(strcmp(filtro, "--espejar-horizontal") == 0)
        {
            res = espejarHorizontal(imagen, metadata, nombreArchivoBMP);
        }
        else if(strcmp(filtro, "--espejar-vertical") == 0)
        {
            res = espejarVertical(imagen, metadata, nombreArchivoBMP);
        }
        else if(strcmp(filtro, "--recortar") == 0)
        {
            if(parametro != -1)
            {
                res = recortar(&imagen, metadata, parametro, nombreArchivoBMP);
            }
            else
            {
                printf("\n\n*** Error: Debe ingresar un parametro comprendido entre 0 y 100 ***");
                res = ERR_PARAMETRO;
            }
        }
        else if(strcmp(filtro, "--achicar") == 0)
        {
            if(parametro != -1)
            {
                res = achicar(imagen, metadata, parametro, nombreArchivoBMP);
            }
            else
            {
                printf("\n\n*** Error: Debe ingresar un parametro comprendido entre 0 y 100 ***");
                res = ERR_PARAMETRO;
            }
        }
        else if(strcmp(filtro, "--comodin") == 0)
        {
            res = ruido(imagen, metadata, nombreArchivoBMP);
        }
        else if(strcmp(filtro, "--negativo") == 0)
        {
            res = negativo(imagen, metadata, nombreArchivoBMP);
        }
        else
        {
            printf("\n\n*** Error: Filtro inexistente ***");
            res = ERR_NO_FILTRO;
        }

        if(res != TODO_OK)
        {
            printf("\n*** No ha sido posible aplicar el filtro %s ***\n----------------------------------\n", filtros[i]);
        }

        imagen = leerImagenBMP(*nombreArchivoBMP, metadata);
    }

    liberarMemoriaImg(imagen, metadata->alto);
    liberarMemoriaMetadata(metadata);

    return 0;
}
