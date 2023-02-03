#ifndef GENERADORSEMILLAS_H
#define GENERADORSEMILLAS_H

#include <utility>
#include <vector>

#include "Grafo.hxx"
#include "Imagen.h"
#include "Semilla.h"

class GeneradorSemillas {
    /* --------- Atributos privados --------- */
   private:
    Grafo<Semilla, GRAFO_NO_DIRIGIDO> *grafo = nullptr;
	int n_aristas;

   public:
    /* --------- Constructores por defecto y copias --------- */
    GeneradorSemillas() = delete;  // No se permite constructor sin parámetros
    GeneradorSemillas(GeneradorSemillas &&) = default;
    GeneradorSemillas(const GeneradorSemillas &) = default;
    GeneradorSemillas &operator=(GeneradorSemillas &&) = default;
    GeneradorSemillas &operator=(const GeneradorSemillas &) = default;
    ~GeneradorSemillas();

    /* --------- Constructor a partir de una imagen --------- */
    GeneradorSemillas(Imagen imagen);

    /**
     * @brief Generar los caminos de dikjstra para toda la imagen tomando como
     * nodo inciail cada una de las semillas
     *
     * @param semillas Vector con las semillas de inicio
     * @return std::vector<std::pair<Semilla, Grafo<Semilla>::dijkstra_path>>
     * Retorna un vector de pares, el primer dato del par es la semilla de
     * inicio y el segundo dato son los caminos de dijkstra
     */
    std::vector<std::pair<Semilla, Grafo<Semilla>::dijkstra_path>>
    generar_caminos(std::vector<Semilla> semillas);

    /**
     * @brief generar la matriz de la imagen con sus etiquetas dados los
     * caminos de dijkstra de las semillas generadas por 'generar_caminos'
     *
     * @param imagen Imagen a partir de la cual se generaron los caminos
     * @param caminos Caminos generados por 'generar_caminos'
     * @return std::vector<Semilla> Vector de Semillas
     */
    static std::vector<Semilla> generar_etiquetas(
        Imagen imagen,
        std::vector<std::pair<Semilla, Grafo<Semilla>::dijkstra_path>>
            caminos);

    /**
     * @brief Generar la matriz (imagen) a partir de las etiquetas de las
     * semillas
     * @param semillas Vector con las semillas
     * @param ancho Ancho de la matriz
     * @param alto Alto de la matriz
     * @return Imagen::matriz_t Matriz generada
     */
    static Imagen::matriz_t generar_matrix(std::vector<Semilla> semillas,
                                           int ancho, int alto);
};

#endif  // GENERADORSEMILLAS_H
