#ifndef GRAFO_HXX
#define GRAFO_HXX

#include <exception>
#include <initializer_list>
#include <limits>
#include <list>
#include <map>
#include <ostream>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <typeinfo>
#include <utility>
#include <vector>

#include "Arista.hxx"

#define GRAFO_DIRIGIDO true
#define GRAFO_NO_DIRIGIDO false

/**
 * @class Grafo
 * @brief Clase que representa un grafo
 * @tparam T Tipo de dato a guardar en los grafos (Debe implementar operador
 * '<' y el operador '==') si se quiere imprimir a pantalla la información del
 * grafo el tipo también debe implementar el operador '<<'
 * @tparam Tipo puede ser GRAFO_DIRIGIDO(true) o GRAFO_NO_DIRIGIDO(false), si
 * no se especifica es por defecto GRAFO_NO_DIRIGIDO
 */
template <typename T, bool Tipo = GRAFO_NO_DIRIGIDO>
class Grafo {
    /* --------- Typedefs --------- */
   public:
    using lista_adyacencia_t = std::map<T, std::list<Arista<T>>>;
    using dijkstra_path = std::map<T, std::pair<double, T *>>;

    /* --------- Constructor/Destructor, Move, Copy and Assigment --------- */
   public:
    Grafo() = delete;
    Grafo(std::vector<Arista<T>>);
    Grafo(std::initializer_list<Arista<T>>);
    Grafo(Grafo &&) = default;
    Grafo(const Grafo &) = default;
    Grafo &operator=(Grafo &&) = default;
    Grafo &operator=(const Grafo &) = default;
    ~Grafo() = default;

    /* --------- Stream output operator --------- */
   public:
    /**
     * @brief Obtener la lista de adyacencia del grafo actual en formato de
     * texto
     *
     * @param os Output stream
     * @param rhs Grafo a codificar
     * @return std::ostream& Output stream de salira
     */
    template <typename K, bool Type>
    friend std::ostream &operator<<(std::ostream &os,
                                    const Grafo<K, Type> &rhs);

    /* --------- Private member variables --------- */
   private:
    std::set<T> vertices;
    lista_adyacencia_t lista_adyacencia;

    /* --------- Métodos --------- */
   public:
    /**
     * @brief Realiza el recorrido del algoritmo de menor costo de Dijkstra
     *
     * @param vertice_inicial Vertice desde el cual comenzar el algoritmo
     * @return dijkstra_path Retorna un
     * mapa cuyas llaves son los vértices del grafo, y cuyo valor es un par
     * (costo, predecesor)
     */
    dijkstra_path dijkstra_algorithm(T vertice_inicial);

    /**
     * @brief Eliminar un vértice del grafo
     *
     * @param remover Vertice a eliminar
     */
    void quitar_vertice(T remover);

    /**
     * @brief Verificar si el grafo está vacío
     *
     * @return true Es vacío
     * @return false No está vacío
     */
    bool es_vacio() const;

    /* --------- Miembros estáticos --------- */
   public:
    /**
     * @brief Convertir los caminos de dikjstra a un formato en string para ser
     * leído por humanos
     *
     * @param list Caminos de dijkstra
     * @return std::string Texto con los caminos de dijkstra
     */
    static std::string dijkstra_path_string(dijkstra_path list);

    /**
     * @brief Obtener el peso de un valor especificado en el algoritmo de
     * dijkstra, a diferencia de usar el valor como llave en los valores de
     * dijkstra, esta función utiliza el operador de comparación ==
     *
     * @param list caminos del algoritmo de dijkstra
     * @param value valor a buscar
     * @return double Peso de dikjstra
     */
    static double weight_of(dijkstra_path list, T value);

    std::set<T> get_vertices() const;
};

template <typename T, bool Tipo>
Grafo<T, Tipo>::Grafo(std::initializer_list<Arista<T>> conexiones)
    : Grafo{std::vector<Arista<T>>{conexiones}} {}

template <typename T, bool Tipo>
Grafo<T, Tipo>::Grafo(std::vector<Arista<T>> conexiones) {
    // Por cada arista que el usuario define
    for (Arista<T> arista : conexiones) {
        // Insertar la arista en la lista de adyacencia
        this->lista_adyacencia[arista.get_desde()].push_back(arista);
        // Si es no dirigido insertar el inverso
        if (Tipo == GRAFO_NO_DIRIGIDO)
            this->lista_adyacencia[arista.get_hasta()].push_back(
                arista.inverso());

        // Insertar los vertices
        this->vertices.insert(arista.get_hasta());
        this->vertices.insert(arista.get_desde());
    }
}

template <typename T, bool Tipo>
typename Grafo<T, Tipo>::dijkstra_path Grafo<T, Tipo>::dijkstra_algorithm(
    T vertice_inicial) {
    // Crear el mapa de pesos
    dijkstra_path pesos;
    std::set<T> unvisited{};  // Vertices a visitar
    std::set<T> visited{};    // Vertices visitados

    // Llenar el mapa de pesos
    for (auto vertice = this->vertices.begin();
         vertice != this->vertices.end(); vertice++) {
        pesos[*vertice] =
            (*vertice == vertice_inicial
                 ? std::make_pair(0.0, &const_cast<T &>(*vertice))
                 : std::make_pair(std::numeric_limits<double>::infinity(),
                                  nullptr));
        unvisited.insert(*vertice);
    }

    // Mientras que hayan nodos sin visitar
    for (int counter = 0;
         !unvisited.empty() && counter != this->vertices.size(); counter++) {
        // Obtener el nodo con el menor peso
        T *vertice = nullptr;
        double menor_peso = std::numeric_limits<double>::infinity();

        // AUTO es usado porque el compilador no puede generar un iterador
        // de un tipo plantilla T hasta el momento de compilación cuando
        // pueda ser deducido
        for (auto it = pesos.begin(); it != pesos.end(); it++)
            if ((it->second.first < menor_peso) &&
                (unvisited.find(it->first) != unvisited.end())) {
                vertice = &const_cast<T &>(it->first);
                menor_peso = it->second.first;
            }

        if (vertice == nullptr) break;

        // Realizar el cálculo de todas sus conexiones salientes
        std::list<Arista<T>> salientes = lista_adyacencia[*vertice];
        for (Arista<T> s : salientes) {
            if (pesos[s.get_hasta()].first >
                (pesos[s.get_desde()].first + s.get_peso())) {
                pesos[s.get_hasta()].first =
                    (pesos[s.get_desde()].first + s.get_peso());
                pesos[s.get_hasta()].second = vertice;
            }
        }

        // Actualizar listas
        visited.insert(*vertice);
        unvisited.erase(*vertice);
        vertice = nullptr;
    }

    return pesos;
}

template <typename T, bool Tipo>
inline void Grafo<T, Tipo>::quitar_vertice(T remover) {
    // Quitar el nodo de la lista
    this->lista_adyacencia.erase(remover);
    this->vertices.erase(remover);
    // Quitar las referencias al nodo en las demás listas
    // Nos da las listas de adyacencia
    for (auto &adyacente : this->lista_adyacencia) {
        // Por cada arista en la lista de adyacencia
        bool encontrado = false;
        do {
            encontrado = false;
            for (typename std::list<Arista<T>>::iterator arista =
                     adyacente.second.begin();
                 arista != adyacente.second.end(); arista++) {
                if (arista->get_desde() == remover ||
                    arista->get_hasta() == remover) {
                    adyacente.second.erase(arista);
                    encontrado = true;
                    break;
                }
            }
        } while (encontrado);
    }
}

template <typename T, bool Tipo>
inline bool Grafo<T, Tipo>::es_vacio() const {
    return this->vertices.empty();
}

template <typename T, bool Tipo>
inline std::set<T> Grafo<T, Tipo>::get_vertices() const {
    return this->vertices;
}

template <typename T, bool Tipo>
inline std::string Grafo<T, Tipo>::dijkstra_path_string(dijkstra_path list) {
    std::stringstream os;

    os << "Recorridos (Dijkstra):\n";
    for (std::pair<T, std::pair<double, T *>> vertice : list) {
        os << vertice.first << "\t<-\t";
        if (vertice.second.second != nullptr) {
            os << *vertice.second.second;
        } else {
            os << "null";
        }
        os << " [Costo: " << vertice.second.first << "]\n";
    }

    return os.str();
}

template <typename T, bool Tipo>
inline double Grafo<T, Tipo>::weight_of(dijkstra_path list, T value) {
    // Por cada elemento de dikjstra
    for (std::pair<T, std::pair<double, T *>> item : list)
        if (item.first == value) return item.second.first;

    throw std::runtime_error("Value does not exist");
}

template <typename K, bool Type>
inline std::ostream &operator<<(std::ostream &os, const Grafo<K, Type> &rhs) {
    // Mostrar detalles del grafo
    os << "{\n\t\"type\": "
       << (Type == GRAFO_DIRIGIDO ? "\"directed\"" : "\"undirected\"") << ','
       << "\n\t\"n_vertices\": " << rhs.vertices.size() << ','
       << "\n\t\"vertices\": [";

    for (K vertice : rhs.vertices) os << vertice << ',';

    os << "\b],\n\t\"adjacency_list\": [";

    // Mostrar la lista de adyacencia
    for (auto item = rhs.lista_adyacencia.begin();
         item != rhs.lista_adyacencia.end(); item++) {
        os << "\n\t\t{\"node\": " << item->first << ", \"adjacent\": [ ";
        // Por cada elemento en la lista de ese nodo
        for (Arista<K> arista : item->second) os << arista << ", ";
        os << "\b\b ]}";

        if (item != --rhs.lista_adyacencia.end()) os << ',';
    }

    os << "\b\n\t]\n}";
    return os;
}

#endif  // GRAFO_HXX
