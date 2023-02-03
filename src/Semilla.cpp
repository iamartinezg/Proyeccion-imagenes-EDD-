#include "Semilla.h"

Semilla::Semilla(size_t x, size_t y, Imagen::pixel_t intensidad, int etiqueta)
    : x{x}, y{y}, intensidad{intensidad}, etiqueta{etiqueta} {}

bool Semilla::operator<(const Semilla &other) const {
    if (this->x < other.x) return true;
    if (other.x < this->x) return false;
    return this->y < other.y;
}

bool Semilla::operator>(const Semilla &other) const { return other < *this; }

bool Semilla::operator<=(const Semilla &other) const {
    return !(other < *this);
}

bool Semilla::operator>=(const Semilla &other) const {
    return !(*this < other);
}

bool Semilla::operator==(const Semilla &other) const {
    return this->x == other.x && this->y == other.y;
}

bool Semilla::operator!=(const Semilla &other) const {
    return !(*this == other);
}

std::ostream &operator<<(std::ostream &os, const Semilla &rhs) {
    os << "{\"x\": " << rhs.x << ", \"y\": " << rhs.y
       << ", \"intensidad\": " << rhs.intensidad
       << ", \"etiqueta\": " << rhs.etiqueta << '}';
    return os;
}
