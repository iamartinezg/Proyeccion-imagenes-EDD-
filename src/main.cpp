/**
 * @file main.cpp
 * @authors Angel Talero, Juan Urquijo, Iván Martínez
 * @brief Proyecto de proyecciones (Estructuras de datos 2022-1)
 * @version 0.1
 * @date 2022-02-08
 *
 * @copyright
 * Pontificia Universidad Javeriana
 * Facultad de Ingeniería
 * Departamento de Ingeniería de Sistemas
 * Bogotá D.C - Colombia
 *
 * Copyright © 2022
 */

#include <cstdlib>
#include <iostream>

#include "Consola.h"
#include "Controlador.h"


//* El controlador del sistema es 'Global'

//! Lista con los comandos a utilizar
std::vector<Comando> commands = {
    Comando("cargar_imagen", Controlador::cargar_imagen, "nombre_archivo.pgm",
            "Carga en memoria la imagen específicada"),

    Comando("cargar_volumen", Controlador::cargar_volumen, "nombre_base n_im",
            "Cargar en memoria una seria ordenada de imágenes con un nombre "
            "base y un tamaño 'n_im'"),

    Comando("info_imagen", Controlador::info_imagen, "",
            "Muestra información básica de la imagen actualmente carga en "
            "memoria"),

    Comando("info_volumen", Controlador::info_volumen, "",
            "Muestra información básica del volumen actualmente cargado en "
            "memoria"),

    Comando(
        "proyeccion2D", Controlador::proyeccion_2d,
        "dirección criterio nombre_archivo.pgm (la dirección puede ser "
        "'x|y|z' y el criterio puede ser 'minimo|maximo|promedio|mediana')",
        "Generar una proyección 2D a partir de un volumen cargado en memoria "
        "y guardarlo en 'nombre_archivo.pgm'"),

    Comando("codificar_imagen", Controlador::codificar_imagen,
            "nombre_archivo.huffman",
            "Generación de la codificación de Huffman para la imágen "
            "actualmente cargada en memoria"),

    Comando("decodificar_archivo", Controlador::decodificar_archivo,
            "nombre_archivo.huffman nombre_imagen.pgm",
            "Generar una imagen tipo PGM y almacenarla en disco "
            "'nombre_imagen.pgm' a partir de la información codificada por el "
            "archivo Huffman 'nombre_arhivo.huffman'"),

    Comando("segmentar", Controlador::segmentar,
            "salida_imagen.pgm sx1 sy1 sl1 sx2 sy2 sl2 ...",
            "Cargar la información del conjunto de semillas correspondiente a "
            "la imagen cargada en memoria")

};

//! main
int main(int argc, char const *argv[]) {
    // Crear un intérprete de línea de comandos
    Interprete interpreter{commands};
    // Correr el intérprete
    interpreter.cli();
    // Liberar los recursos utilizados
    Controlador::limpiar();
    return EXIT_SUCCESS;
}