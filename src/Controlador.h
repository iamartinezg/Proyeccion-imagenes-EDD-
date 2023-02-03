#ifndef CONTROLADOR_H
#define CONTROLADOR_H

#include <memory>
#include <vector>

#include "Consola.h"
#include "Huffman.h"
#include "Imagen.h"
#include "Volumen.h"

//! Esta clase es estática, todo dentro de ella debe ser estática
class Controlador {
   private:
    // Apuntador inteligente a una imagen
    static Imagen *imagen_cargada;
    static Volumen *volumen_cargado;

   public:
    static void limpiar();

    //* Componente 1

    static void cargar_imagen(Comando::arguments_t args);

    static void cargar_volumen(Comando::arguments_t args);

    static void info_imagen(Comando::arguments_t args);

    static void info_volumen(Comando::arguments_t args);

    static void proyeccion_2d(Comando::arguments_t args);

    //* Componente 2

    static void codificar_imagen(Comando::arguments_t args);

    static void decodificar_archivo(Comando::arguments_t args);

    //* Componente 3
    static void segmentar(Comando::arguments_t args);
};

#endif  // CONTROLADOR_H
