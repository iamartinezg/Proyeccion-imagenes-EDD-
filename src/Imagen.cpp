#include "Imagen.h"

#include <fstream>
#include <functional>
#include <sstream>
#include <stdexcept>

Imagen::Imagen(matriz_t matriz_pixeles) : matriz_pixeles{matriz_pixeles} {
    int valor_maximo = 0;

    // Verificar que la matriz sea correcta (Todos los vectores de adentro
    // deben tener el mismo tamaño)
    for (matriz_t::iterator it1 = matriz_pixeles.begin();
         it1 != matriz_pixeles.end(); it1++) {
        // Recorrer nuevamente la matriz de pixeles
        for (matriz_t::iterator it2 = matriz_pixeles.begin();
             it2 != matriz_pixeles.end(); it2++) {
            // Si todos los vectores no tienen el mismo tamaño
            if (it1->size() != it2->size()) throw std::exception();
        }

        // Por cada elemento de la fila
        for (fila_t::iterator ft = it1->begin(); ft != it1->end(); ft++) {
            if (*ft > valor_maximo) valor_maximo = *ft;
        }
    }

    this->max_tam = valor_maximo;
    this->alto = matriz_pixeles.size();
    this->ancho = matriz_pixeles[0].size();
}

Imagen::Imagen(std::string nombre_archivo) : nombre_archivo(nombre_archivo) {
    // Verificar que el nombre_archivo termine por .pgm
    if (nombre_archivo.substr(nombre_archivo.find_last_of(".") + 1) != "pgm") {
        throw std::exception();
    }

    // Abrir el archivo
    std::fstream archivo(nombre_archivo);

    // Si no se pudo abrir
    if (!archivo.is_open()) {
        throw std::exception();
    }

    // Arreglo donde se van a guardar los valores
    std::vector<Imagen::pixel_t> pixeles;

    // Leer línea por línea
    std::string linea;
    // i lleva la cuenta de las líneas leídas
    for (int i = 0; std::getline(archivo, linea); i++) {
        // Ignorar comentarios
        if (linea[0] == '#') {
            i--;  // No contar esta línea como lineas_leidas
            continue;
        }

        // Interpretar
        if (i == 0)  // Formato
        {
            this->formato_imagen = linea;
        } else if (i == 1)  // Dimensiones
        {
            // Tokenizar, deben haber 2 números
            std::vector<int> enteros;
            std::stringstream linea2(linea);
            std::string temp;

            while (std::getline(linea2, temp, ' '))
                enteros.push_back(std::stoi(temp));

            // Guardar cada número ANCHO x ALTO
            this->ancho = enteros[0];
            this->alto = enteros[1];
        } else if (i == 2)  // MaxTam
        {
            this->max_tam = std::stoi(linea);
        } else  // Pixéles
        {
            std::stringstream linea_pixeles(linea);
            std::string aux;

            // Empujar todos los tokens a la lista
            while (std::getline(linea_pixeles, aux, ' '))
                pixeles.push_back(std::stoi(aux));
        }
    }

    // Convertir todos los pixeles en arreglo
    try {
        int k = 0;
        for (int i = 0; i < this->alto; i++) {
            // Crear cada fila
            this->matriz_pixeles.push_back(Imagen::fila_t());
            for (int j = 0; j < this->ancho; j++) {
                this->matriz_pixeles[i].push_back(pixeles[k]);
                k++;
            }
        }
    } catch (...) {
        // Si hay algún acceso a memoria ilegal
        throw std::exception();
    }

    // Cerrar el archivo
    archivo.close();
}

bool Imagen::guardar_archivo(std::string nombre_archivo) {
    // Abrir el archivo (crearlo si no existe)
    std::ofstream salida(nombre_archivo, std::ios::out);

    if (!salida.is_open()) return false;

    // Guardar la cabecera
    salida << this->formato_imagen << '\n'  // Formato
           << "# Archivo generado por ProyeccionesEDD"
           << '\n'  // Comentario generado por el sistema
           << this->ancho << ' ' << this->alto << '\n'  // Ancho x Alto
           << this->max_tam << '\n';                    // Tamaño máximo

    // Por cada fila
    for (matriz_t::iterator it_fila = this->matriz_pixeles.begin();
         it_fila != this->matriz_pixeles.end(); it_fila++) {
        // Por cada elemento de la fila
        for (fila_t::iterator it_pixel = it_fila->begin();
             it_pixel != it_fila->end(); it_pixel++) {
            salida << *it_pixel << ' ';
        }
        salida << '\n';
    }

    // Cerrar el archivo
    salida.close();
    return true;
}

std::string Imagen::get_formato() const { return formato_imagen; }

void Imagen::set_formato(const std::string &formatoImagen) {
    formato_imagen = formatoImagen;
}

int Imagen::get_ancho() const { return ancho; }
void Imagen::set_ancho(int ancho_) { ancho = ancho_; }

int Imagen::get_alto() const { return alto; }
void Imagen::set_alto(int alto_) { alto = alto_; }

int Imagen::get_max_tam() const { return max_tam; }
void Imagen::set_max_tam(int maxTam) { max_tam = maxTam; }

Imagen::matriz_t Imagen::get_pixeles() const { return matriz_pixeles; }
void Imagen::set_pixeles(Imagen::matriz_t matrizPixeles) {
    matriz_pixeles = matrizPixeles;
}

std::string Imagen::get_nombre_archivo() const { return this->nombre_archivo; }

void Imagen::set_nombre_archivo(std::string nombre) {
    this->nombre_archivo = nombre;
}

std::string Imagen::to_string() {
    return "(proceso satisfactorio) Imagen cargada en memoria: " +
           nombre_archivo + ", ancho: " + std::to_string(ancho) +
           ", alto: " + std::to_string(alto);
}

Imagen::matriz_t Imagen::matriz_vacia(int ancho, int alto) {
    Imagen::matriz_t vacia;
    for (int i = 0; i < alto; i++) {
        vacia.push_back(std::vector<pixel_t>());
        for (int j = 0; j < ancho; j++) {
            vacia[i].push_back(0);
        }
    }

    return vacia;
}

void Imagen::llenar_matrix(matriz_t &mtx, pixel_t value) {
    for (int i = 0; i < mtx.size(); i++)
        for (int j = 0; j < mtx[i].size(); j++) mtx[i][j] = value;
}

Imagen Imagen::reflejo_vertical(const Imagen &img) {
    Imagen::matriz_t nueva = matriz_vacia(img.get_ancho(), img.get_alto());
    Imagen::matriz_t vieja = img.get_pixeles();

    for (int i = 0; i < img.alto; i++)
        for (int j = 0; j < img.ancho; j++)
            nueva[img.alto - i - 1][j] = vieja[i][j];

    return Imagen{nueva};
}

void Imagen::normalizar() {
    int tam_max = this->max_tam;
    // Convierte el pixel a valores de 0 hasta 255
    std::function<int(int)> map_pixel = [tam_max](int valor) -> int {
        return static_cast<int>((valor * 255) / tam_max);
    };

    for (int i = 0; i < this->alto; i++)
        for (int j = 0; j < this->ancho; j++)
            this->matriz_pixeles[i][j] = map_pixel(this->matriz_pixeles[i][j]);

    this->max_tam = 255;
}