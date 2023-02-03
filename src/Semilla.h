#ifndef SEMILLA_H
#define SEMILLA_H

#include <cstddef>
#include <ostream>

#include "Imagen.h"

struct Semilla {
    size_t x;
    size_t y;
    Imagen::pixel_t intensidad;
    int etiqueta;

    Semilla(size_t x, size_t y, Imagen::pixel_t intensidad, int etiqueta = 0);

    bool operator==(const Semilla &other) const;
    bool operator!=(const Semilla &other) const;

    bool operator<(const Semilla &other) const;
    bool operator>(const Semilla &other) const;
    bool operator<=(const Semilla &other) const;
    bool operator>=(const Semilla &other) const;

    friend std::ostream &operator<<(std::ostream &os, const Semilla &rhs);
};

#endif  // SEMILLA_H
