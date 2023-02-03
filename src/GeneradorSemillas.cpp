#include "GeneradorSemillas.h"

#include <cassert>
#include <cstddef>
#include <cstdlib>
#include <limits>
#include <stdexcept>
#include <utility>

#include "Arista.hxx"
#include "Grafo.hxx"
#include "Imagen.h"
#include "Semilla.h"

GeneradorSemillas::~GeneradorSemillas() {
    delete this->grafo;
    this->grafo = nullptr;
}

GeneradorSemillas::GeneradorSemillas(Imagen imagen) {
    // Obtener los pixeles
    Imagen::matriz_t matrix = imagen.get_pixeles();

    /*
    * > Generar el Grafo

    1. Tomar cada píxel y crear dos aristas, una con el nodo vecino derecho
    y otro con el nodo vecino de abajo

        - Para cada nodo calcular su vecino derecho e inferior

        Al ser NO_DIRIGIO creariamos una malla de nodos de conexiones
        desde cualquier nodo a cualquiera de sus vecinos

    */

    std::vector<Arista<Semilla>> aristas{};

    // Por cada fila
    for (size_t i = 0; i < matrix.size(); i++) {
        // Por cada columna
        for (size_t j = 0; j < matrix[i].size(); j++) {
            // Derecha
            if ((j + 1) < matrix[i].size())
                aristas.push_back({Semilla{j, i, matrix[i][j]},
                                   Semilla{(j + 1), i, matrix[i][j + 1]},
                                   static_cast<double>(
                                       abs(matrix[i][j + 1] - matrix[i][j]))});

            // Abajo
            if ((i + 1) < matrix.size()) {
                aristas.push_back({Semilla{j, i, matrix[i][j]},
                                   Semilla{j, (i + 1), matrix[i + 1][j]},
                                   static_cast<double>(
                                       abs(matrix[i + 1][j] - matrix[i][j]))});
            }
        }
    }

    this->n_aristas = aristas.size();

    // 2*(n-1)(m-1)+(n-1)+(m-1)
    assert(n_aristas ==
               (2 * (imagen.get_alto() - 1) * (imagen.get_ancho() - 1) +
                (imagen.get_alto() - 1) + (imagen.get_ancho() - 1)) &&
           "Cantidad de aristas no corresponde a la esperada "
           "(2(n-1)(m-1)+(n-1)+(m-1))");

    // Crear el grafo
    this->grafo = new Grafo<Semilla>{aristas};

    assert(grafo->get_vertices().size() ==
               (imagen.get_alto() * imagen.get_ancho()) &&
           "Tamaño del grafo no coincide con el esperado (n*m)");
}

std::vector<std::pair<Semilla, Grafo<Semilla>::dijkstra_path>>
GeneradorSemillas::generar_caminos(std::vector<Semilla> semillas) {
    // Resultado
    std::vector<std::pair<Semilla, Grafo<Semilla>::dijkstra_path>> caminos{};

    // Por cada semilla
    for (size_t i = 0; i < semillas.size(); i++) {
        // Generar el Dijkstra y Guardarlo
        caminos.push_back(std::make_pair(
            semillas[i], this->grafo->dijkstra_algorithm(semillas[i])));
    }

    return caminos;
}

std::vector<Semilla> GeneradorSemillas::generar_etiquetas(
    Imagen imagen,
    std::vector<std::pair<Semilla, Grafo<Semilla>::dijkstra_path>> caminos) {
    // Inicializar matrices
    Imagen::matriz_t matrix = imagen.get_pixeles();
    std::vector<Semilla> resultado{};

    // Por cada fila
    for (size_t i = 0; i < matrix.size(); i++) {
        // Por cada columna
        for (size_t j = 0; j < matrix[i].size(); j++) {
            // Generar la semilla inicial
            Semilla pixel{j, i, matrix[i][j], 0};

            double min_costo = std::numeric_limits<double>::infinity();
            int min_etiqueta = std::numeric_limits<double>::max();

            // Por cada camino en el arreglo
            for (size_t c = 0; c < caminos.size(); c++) {
                double peso = Grafo<Semilla, GRAFO_NO_DIRIGIDO>::weight_of(
                    caminos[c].second, pixel);

                // Hallar la etiqueta de menor costo
                if (peso < min_costo) {
                    min_costo = peso;
                    min_etiqueta = caminos[c].first.etiqueta;
                }
            }

            // Asignar la etiqueta de menor costo
            pixel.etiqueta = min_etiqueta;

            // Meter en la matriz la semilla
            resultado.push_back(pixel);
        }
    }

    return resultado;
}

Imagen::matriz_t GeneradorSemillas::generar_matrix(
    std::vector<Semilla> semillas, int ancho, int alto) {
    // Generar la matriz vacía
    Imagen::matriz_t mtx = Imagen::matriz_vacia(ancho, alto);

    for (size_t i = 0; i < semillas.size(); i++) {
        Semilla semilla = semillas[i];
        // Verificar boundaires
        if (semilla.x >= ancho || semilla.y >= alto)
            throw std::runtime_error(
                "Semilla fuera de los límites de la imágen");
        // Añadir a la matriz (Etiqueta)
        mtx[semilla.y][semilla.x] = semilla.etiqueta;
    }

    return mtx;
}
