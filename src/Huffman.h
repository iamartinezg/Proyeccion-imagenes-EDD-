#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <cstddef>
#include <cstdint>
#include <map>
#include <queue>
#include <string>

#include "ArbolCodificacion.hxx"
#include "Imagen.h"

class Huffman {
   public:
    using byte_t = uint8_t;
    using word_t = uint16_t;
    using dword_t = uint32_t;
    using lword_t = uint64_t;

   public:
    word_t ancho = 0;
    word_t alto = 0;
    byte_t maximo = 255;

    const Imagen *imagen;
    ArbolCodificacion<byte_t> *arbol = nullptr;
    std::map<byte_t, CodigoElemento<byte_t>> codigos;

   public:
    /**
     * @brief Construye la codificación Huffman de una imágen
     * @param img Imagen
     */
    Huffman(const Imagen &img);

    /**
     * @brief Construye la codificación Huffman desde un archivo
     * y la guarda como imagen en el apuntador (no debe estar asignado)
     * @param nombre_archivo Archivo a leer
     * @param salida Archivo de salida
     */
    Huffman(const std::string nombre_archivo, std::string salida);

    ~Huffman();

    /**
     * @brief Guardar la imágen de Huffman en un archivo
     *
     * @param nombre_archivo
     */
    void guardar_archivo(std::string nombre_archivo);

    /**
     * @brief Convertir un archivo Huffman en una imágen
     *
     * @param nombre_archivo Nombre del archivo a leer
     * @return Imagen Imagen decodificada
     */
    static Imagen decodificar(std::string nombre_archivo);
};

#endif  // HUFFMAN_H
