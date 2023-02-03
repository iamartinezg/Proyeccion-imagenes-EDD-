#ifndef ARISTA_HXX
#define ARISTA_HXX

#include <ostream>

/* --------- Declaraciones --------- */
template <typename T>
class Arista {
   public:
    /* --------- Constructores --------- */
    Arista() = delete;
    Arista(T, T, double = 1);
    ~Arista() = default;

    /* --------- Copy/Move constructors --------- */
    Arista(Arista &&) = default;
    Arista(const Arista &) = default;
    Arista &operator=(Arista &&) = default;
    Arista &operator=(const Arista &) = default;

    /* --------- Stream output operator --------- */
    template <typename K>
    friend std::ostream &operator<<(std::ostream &os, const Arista<K> &rhs);

    /* --------- Attributes --------- */
   protected:
    T desde;
    T hasta;
    double peso;

    /* --------- Methods --------- */
   public:
    /**
     * @brief Obtener una arista que es la inversa de esta (desde -> hasta,
     * hasta -> desde) con el mismo peso
     *
     * @return Arista<T> Nueva arista inversa de esta
     */
    Arista<T> inverso();

    /* --------- Getters & Setters --------- */
    T get_desde() const;
    T get_hasta() const;
    double get_peso() const;
};

template <typename T>
inline Arista<T>::Arista(T desde, T hasta, double peso)
    : desde{desde}, hasta{hasta}, peso{peso} {}

template <typename T>
inline T Arista<T>::get_hasta() const {
    return this->hasta;
}

template <typename T>
inline T Arista<T>::get_desde() const {
    return this->desde;
}

template <typename T>
inline std::ostream &operator<<(std::ostream &os, const Arista<T> &rhs) {
    os << "{\"from\": " << rhs.desde << ", \"to\": " << rhs.hasta
       << ", \"weight\": " << rhs.peso << "}";
    return os;
}

template <typename T>
inline Arista<T> Arista<T>::inverso() {
    return Arista<T>(this->hasta, this->desde, this->peso);
}

template <typename T>
inline double Arista<T>::get_peso() const {
    return this->peso;
}

/* --------- Definiciones --------- */

#endif  // ARISTA_HXX
