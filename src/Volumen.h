#ifndef VOLUMEN_H
#define VOLUMEN_H

#include <queue>
#include <string>

#include "Imagen.h"


#define MAX_IMAGENES_VOLUMEN 99
#define MIN_IMAGENES_VOLUMEN 2
#define EXTENSION_PGM ".pgm"

/**
 * @class Volumen
 * @brief Representación de un volumen 3D
 */
class Volumen {
   private:
    std::string nombre_base; /**< Nombre base de los archivos*/
    int tam_volumen;         /**< Tamaño del volumen (Número de capas que lo
                                componen)*/
    int ancho;               /**< Ancho del volumen */
    int alto;                /**< Alto del volumen */
    std::queue<Imagen> volumen /**< Capas del volumen*/;

   public:
    /**
     * @brief Construir un volúmen vacío
     */
    Volumen() = default;

    /**
     * @brief Construir un volúmen a partir de una serie de imágenes con un
     * nombre base
     *
     * @param nombre_base Nombre base de los archivos
     * @param tam Tamaño del volumen (Imágenes a leer)
     */
    Volumen(std::string nombre_base, int tam);

    std::string to_string() const;

    std::string get_nombre_base() const;
    void set_nombre_base(std::string nombreBase);

    int get_tam_volumen() const;
    void set_tam_volumen(int tamVolumen);

    int get_ancho() const;
    void set_ancho(int ancho);

    int get_alto() const;
    void set_alto(int alto);

    std::queue<Imagen> get_volumen() const;
    void set_volumen(std::queue<Imagen> volumen_);

    /**
     * @brief Crea una proyección 2D del volumen
     *
     * @param criterio puede ser 'promedio', 'maximo', 'minimo', 'mediana'
     * @param direccion
     * @param nombre_archivo
     */
    void crear_proyeccion(std::string criterio, char direccion,
                          std::string nombre_archivo);
};

#endif  // VOLUMEN_H
