#ifndef ARBOLCODIFICACION_HXX
#define ARBOLCODIFICACION_HXX

#include <map>
#include <queue>
#include <string>
#include <vector>

#include "CodigoElemento.hxx"
#include "NodoCodificacion.hxx"
#include "NodoElemento.hxx"
#include "NodoFrecuencia.hxx"

template <typename T>
class ArbolCodificacion {
    // Huffman tiene acceso
    friend class Huffman;

   private:
    NodoCodificacion<T> *raiz = nullptr;

   public:
    ArbolCodificacion() = delete;
    ArbolCodificacion(std::map<T, freq_t> frecuencias);

    ~ArbolCodificacion();

    /**
     * @brief Obtener el código de la codificación de huffman de todos los
     * elementos
     *
     * @return std::vector<CodigoElemento<T>> Vector con los CodigoElemento
     */
    std::vector<CodigoElemento<T>> codigos_elementos();
};

template <typename T>
inline ArbolCodificacion<T>::ArbolCodificacion(
    std::map<T, freq_t> frecuencias) {
    // Cola mínima con las frecuencias
    struct deref_less {
        bool operator()(const NodoCodificacion<T> *a,
                        const NodoCodificacion<T> *b) {
            return a->frecuencia > b->frecuencia;
        }
    };

    std::priority_queue<NodoCodificacion<T> *,
                        std::vector<NodoCodificacion<T> *>, deref_less>
        cola{};

    // Llenar la cola con los elementos
    for (std::pair<T, freq_t> x : frecuencias)
        if (x.second != 0)
            cola.push({dynamic_cast<NodoCodificacion<T> *>(
                new NodoElemento<T>{x.first, x.second})});

    // Insertar valores en el árbol
    while (cola.size() > 1) {
        // Crear un nuevo nodo (! memoria)
        NodoFrecuencia<T> *nodoZ = new NodoFrecuencia<T>{};

        // Hijo Izquierdo
        nodoZ->hijoIzq = cola.top();
        cola.pop();

        // Hijo derecho
        nodoZ->hijoDer = cola.top();
        cola.pop();

        // Frecuencia del nodo
        nodoZ->frecuencia =
            nodoZ->hijoIzq->frecuencia + nodoZ->hijoDer->frecuencia;

        // Guardar el nodo
        cola.push(nodoZ);
    }

    // El único elemento de la cola es la raíz
    this->raiz = cola.top();
    cola.pop();
}

template <typename T>
inline ArbolCodificacion<T>::~ArbolCodificacion() {
    if (this->raiz != nullptr) {
        delete raiz;
        raiz = nullptr;
    }
}

template <typename T>
inline std::vector<CodigoElemento<T>>
ArbolCodificacion<T>::codigos_elementos() {
    return this->raiz->codigos_elementos("");
}

#endif  // ARBOLCODIFICACION_HXX
