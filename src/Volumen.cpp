#include "Volumen.h"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <iomanip>
#include <limits>
#include <numeric>
#include <queue>
#include <sstream>
#include <stack>
#include <stdexcept>
#include <vector>

#include "Imagen.h"

Volumen::Volumen(std::string nombre_base, int tam)
    : nombre_base(nombre_base), tam_volumen(tam) {
    if (tam > MAX_IMAGENES_VOLUMEN || tam < MIN_IMAGENES_VOLUMEN)
        throw std::exception();

    int last_ancho = 0, last_alto = 0;

    // Por cada imágen que hay que cargar
    for (int i = 1; i <= tam; i++) {
        std::ostringstream temp;
        temp << std::setw(2) << std::setfill('0') << i;
        std::string numero_base = temp.str();
        this->volumen.push(Imagen(nombre_base + numero_base + EXTENSION_PGM));

        if (i == 1) {
            last_alto = this->volumen.front().get_alto();
            last_ancho = this->volumen.front().get_ancho();
        } else  // Si alguna imagen no tiene ancho y alto igual al anterior
                // ancho y alto
        {
            if ((last_ancho != this->volumen.front().get_ancho()) ||
                (last_alto != this->volumen.front().get_alto())) {
                throw std::exception();
            }
        }
    }

    // Ancho y alto son iguales al ancho y alto de las imágenes
    this->ancho = this->volumen.front().get_ancho();
    this->alto = this->volumen.front().get_alto();

    // El tamaño debe ser igual
    assert(static_cast<int>(this->volumen.size()) == tam_volumen);
}

std::string Volumen::to_string() const {
    return "(proceso satisfactorio) Volumen cargado en memoria: " +
           nombre_base + ", tamaño: " + std::to_string(tam_volumen) +
           ", ancho: " + std::to_string(ancho) +
           ",  alto: " + std::to_string(alto);
}

int Volumen::get_ancho() const { return ancho; }

void Volumen::set_ancho(int ancho) { this->ancho = ancho; }

int Volumen::get_alto() const { return alto; }

void Volumen::set_alto(int alto) { this->alto = alto; }

std::string Volumen::get_nombre_base() const { return nombre_base; }
void Volumen::set_nombre_base(std::string nombreBase) {
    nombre_base = nombreBase;
}

int Volumen::get_tam_volumen() const { return tam_volumen; }

void Volumen::set_tam_volumen(int tamVolumen) { tam_volumen = tamVolumen; }

std::queue<Imagen> Volumen::get_volumen() const { return volumen; }

void Volumen::set_volumen(std::queue<Imagen> volumen_) { volumen = volumen_; }

void Volumen::crear_proyeccion(std::string criterio, char direccion,
                               std::string nombre_archivo) {
    // Aquí queda el bloque con todas las imágenes
    int alto, ancho;
    std::queue<Imagen> imagenes{};

    // Rotar el bloque para que queden en el eje de las X
    switch (direccion) {
        case 'x':  // Se deja tal cual
            alto = this->alto;
            ancho = this->ancho;
            imagenes = this->get_volumen();
            break;

        case 'y':
            alto = this->tam_volumen;
            ancho = this->alto;

            // Por cada trozo vertical del volumen
            for (int k = 0; k < this->ancho; k++) {
                Imagen::matriz_t mtx = Imagen::matriz_vacia(ancho, alto);
                std::queue<Imagen> copia(this->get_volumen());
                // Por cada capa horizontal del volumen
                for (int c = 0; !copia.empty(); c++) {
                    Imagen::matriz_t capa = copia.front().get_pixeles();
                    // Por cada pixel en la capa
                    for (int i = 0; i < this->alto; i++) {
                        mtx[c][i] = capa[i][k];
                    }
                    copia.pop();
                }

                // Meter la nueva capa
                imagenes.push(Imagen{mtx});
            }
            break;

        case 'z':
            alto = this->tam_volumen;
            ancho = this->ancho;

            // Por cada trozo vertical del volumen
            for (int k = 0; k < this->alto; k++) {
                Imagen::matriz_t mtx = Imagen::matriz_vacia(ancho, alto);
                std::queue<Imagen> copia(this->get_volumen());
                // Por cada capa horizontal del volumen
                for (int c = 0; !copia.empty(); c++) {
                    Imagen::matriz_t capa = copia.front().get_pixeles();
                    // Por cada pixel en la capa
                    for (int i = 0; i < this->ancho; i++) {
                        mtx[c][i] = capa[k][i];
                    }
                    copia.pop();
                }

                // Meter la nueva capa
                imagenes.push(Imagen{mtx});
            }
            break;

        default:
            throw std::invalid_argument("Dirección errónea");
    }

    // Almacenar el resultado
    Imagen::matriz_t resultado = Imagen::matriz_vacia(ancho, alto);

    // Crear la proyección de la imágen
    if (criterio.compare("promedio") == 0) {
        // Buscar en cada imágen
        while (!imagenes.empty()) {
            Imagen::matriz_t pixeles = imagenes.front().get_pixeles();

            // Sumar cada imagen al total
            for (int i = 0; i < alto; i++)
                for (int j = 0; j < ancho; j++)
                    resultado[i][j] += pixeles[i][j];

            imagenes.pop();
        }

        // Sacar el promedio total
        for (int i = 0; i < alto; i++)
            for (int j = 0; j < ancho; j++)
                resultado[i][j] /= this->tam_volumen;
    } else if (criterio.compare("maximo") == 0) {
        // Buscar en cada imágen el máximo
        while (!imagenes.empty()) {
            Imagen::matriz_t pixeles = imagenes.front().get_pixeles();

            // Buscar el máximo de cada imágen
            for (int i = 0; i < alto; i++)
                for (int j = 0; j < ancho; j++)
                    if (pixeles[i][j] > resultado[i][j])
                        resultado[i][j] = pixeles[i][j];

            imagenes.pop();
        }
    }

    /* En el criterio mínimo hay que ignorar los 0's excepto en las áreas en
    las que no hay pixeles */
    else if (criterio.compare("minimo") == 0) {
        // Llenar la matriz con el maximo valor posible
        Imagen::llenar_matrix(resultado,
                              std::numeric_limits<Imagen::pixel_t>::max());
        // Buscar en cada imágen el máximo
        while (!imagenes.empty()) {
            Imagen::matriz_t pixeles = imagenes.front().get_pixeles();

            // Buscar el máximo de cada imágen
            for (int i = 0; i < alto; i++)
                for (int j = 0; j < ancho; j++)
                    // Colocar pixel mínimo (Diferente de 0)
                    if ((pixeles[i][j] < resultado[i][j]) &&
                        (pixeles[i][j] != 0))

                        resultado[i][j] = pixeles[i][j];

            imagenes.pop();
        }

        for (int i = 0; i < alto; i++)
            for (int j = 0; j < ancho; j++)
                if (resultado[i][j] ==
                    std::numeric_limits<Imagen::pixel_t>::max())
                    resultado[i][j] = 0;
    }

    else if (criterio.compare("mediana") == 0) {
        /*
        !Nota acerca del criterio mediana
        La mediana se calcula ordenando los datos en un arreglo y encontrando
        el dato de en medio.
        * Si el vector es de tamaño impar
        simplemente se coje el dato de en medio <dato at [ceil(tam / 2)]>
        * Si el vector es de tamaño par
        se cojen los dos datos de en medio y se promedian <promedio (dato at
        [tam / 2], dato at [tam / 2 + 1])>
        */

        // Matriz de vectores con todos los valores según el píxel
        std::vector<std::vector<std::vector<Imagen::pixel_t>>> valores{};
        for (int i = 0; i < alto; i++) {
            valores.push_back(std::vector<std::vector<Imagen::pixel_t>>{});
            for (int j = 0; j < ancho; j++) {
                valores[i].push_back(std::vector<Imagen::pixel_t>{});
                for (int k = 0; k < imagenes.size(); k++) {
                    valores[i][j].push_back(0);
                }
            }
        }

        // Colocar en cada imagen la mediana
        for (int k = 0; !imagenes.empty(); k++) {
            Imagen::matriz_t pixeles = imagenes.front().get_pixeles();

            // Por cada fila
            for (int i = 0; i < alto; i++)
                // Por cada columna
                for (int j = 0; j < ancho; j++)
                    valores[i][j][k] = pixeles[i][j];

            imagenes.pop();
        }

        // Organizar los vectores
        for (int i = 0; i < alto; i++)
            for (int j = 0; j < ancho; j++)
                std::sort(valores[i][j].begin(), valores[i][j].end());

        // Hallar el dato medio y promediarlo
        for (int i = 0; i < alto; i++)
            for (int j = 0; j < ancho; j++) {
                std::vector<std::vector<std::vector<Imagen::pixel_t>>>::
                    size_type size = valores[i][j].size();

                switch (size % 2) {
                    case 0:  // PAR <promedio (dato at [tam / 2], dato at [tam
                             // / 2 + 1])>
                        resultado[i][j] = static_cast<Imagen::pixel_t>(
                            (valores[i][j][size / 2] +
                             valores[i][j][size / 2 + 1]) /
                            2);
                        break;

                    default:  // IMPAR <dato at [ceil(tam / 2)]>
                        resultado[i][j] = static_cast<Imagen::pixel_t>(
                            valores[i][j][std::ceil(size / 2)]);
                        break;
                }
            }
    }

    else {
        throw std::invalid_argument("Criterio erróneo");
    }

    // Girar el resultado en caso de ser necesario
    Imagen guardar =
        (direccion != 'x' ? (Imagen::reflejo_vertical(Imagen{resultado}))
                          : Imagen{resultado});

    //! HARDCODED EXCEPTION
    // En caso de que sea mínimo colocar el máximo en 255
    if (criterio.compare("minimo") == 0) guardar.set_max_tam(255);

    // Guardar el resultado
    guardar.guardar_archivo(nombre_archivo);
}