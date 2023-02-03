#include "Huffman.h"

#include <cstdlib>
#include <cstring>
#include <exception>
#include <fstream>
#include <map>
#include <queue>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>

#include "ArbolCodificacion.hxx"
#include "CodigoElemento.hxx"
#include "Imagen.h"
#include "NodoCodificacion.hxx"
#include "NodoElemento.hxx"
#include "NodoFrecuencia.hxx"

Huffman::Huffman(const Imagen &img)
    : ancho(img.get_ancho()),
      alto(img.get_alto()),
      maximo(img.get_max_tam()),
      imagen(&img) {
    // Verificar normalización
    if (img.get_max_tam() > 255)
        throw std::invalid_argument("La imagen no está normalizada");

    // Construir mapa de frecuencias
    std::map<byte_t, freq_t> frecuencias{};
    for (int i = 0; i <= 255; i++) frecuencias[i] = 0;

    // Calcular frecuencias en la imágen
    Imagen::matriz_t pixeles = img.get_pixeles();
    for (int i = 0; i < img.get_alto(); i++)
        for (int j = 0; j < img.get_ancho(); j++)
            frecuencias[(byte_t)pixeles[i][j]]++;
    // Cada vez que se encuentra un
    // pixel, le suma 1 a su frecuencia

    // Se crea el árbol de codificacion
    this->arbol = new ArbolCodificacion<byte_t>{frecuencias};
    std::vector<CodigoElemento<byte_t>> datos = arbol->codigos_elementos();

    // Se obtienen las frecuencias y valores de los bytes
    for (int i = 0; i < datos.size(); i++)
        codigos[datos[i].elemento] = datos[i];
}

Huffman::Huffman(const std::string nombre_archivo, std::string salida)
    : imagen{nullptr} {
    //* Abrir el archivo
    std::ifstream archivo(nombre_archivo, std::ios::binary | std::ios::in);

    // Verificar que se pudo abrir
    if (!archivo.is_open())
        throw std::runtime_error("No se pudo abrir el archivo");

    //* Header
    archivo.read((char *)(&this->ancho), sizeof(word_t));
    archivo.read((char *)(&this->alto), sizeof(word_t));
    archivo.read((char *)(&this->maximo), sizeof(byte_t));

    //* Frecuencias
    // Escribir todas las frecuencias
    for (byte_t i = 0; i < 255; i++) {
        if (!codigos.count(i)) codigos[i] = CodigoElemento<byte_t>{i, 0, ""};

        // Read binary
        archivo.read((char *)&codigos[i].frecuencia, sizeof(lword_t));
    }

    //* Construir el árbol
    std::map<byte_t, freq_t> frecuencias{};
    for (std::pair<byte_t, CodigoElemento<byte_t>> codigo : codigos)
        frecuencias[codigo.first] = codigo.second.frecuencia;

    this->arbol = new ArbolCodificacion<byte_t>{frecuencias};

    //* Obtener los bits
    std::vector<byte_t> bytes{};

    std::queue<int> cadenabits{};
    while (!archivo.eof()) {
        byte_t byte = 0x0000;
        archivo.read((char *)&byte, sizeof(byte_t));

        // If there's nothing to read
        if (archivo.gcount() == 0) break;

        // Transform every bit
        for (int k = 8; k > 0; k--)
            cadenabits.push(static_cast<int>((byte >> (k - 1)) & 1));
    }

    std::queue<byte_t> pixeles{};
    NodoCodificacion<byte_t> *actual = arbol->raiz;

    std::vector<CodigoElemento<byte_t>> codigos = arbol->codigos_elementos();
    int total_leer = 0;
    for (auto cod : codigos) total_leer += cod.codigo.size() * cod.frecuencia;

    for (int leidos = 0; leidos <= total_leer;) {
        // Si es una hoja
        if (typeid(*actual) == typeid(NodoElemento<byte_t>)) {
            pixeles.push(dynamic_cast<NodoElemento<byte_t> *>(actual)->dato);
            actual = arbol->raiz;
        } else {
            // Si es 1
            int bit = cadenabits.front();
            if (bit == 1)
                actual =
                    dynamic_cast<NodoFrecuencia<byte_t> *>(actual)->hijoDer;
            // Si es 0
            else if (bit == 0)
                actual =
                    dynamic_cast<NodoFrecuencia<byte_t> *>(actual)->hijoIzq;
            cadenabits.pop();
            leidos++;
        }
    }

    // Pasar los datos a la imagen
    Imagen::matriz_t matriz = Imagen::matriz_vacia(this->ancho, this->alto);
    for (int i = 0; i < this->alto; i++)
        for (int j = 0; j < this->ancho; j++) {
            matriz[i][j] = pixeles.front();
            pixeles.pop();
        }

    Imagen imgp{matriz};
    imgp.set_max_tam(static_cast<int>(this->maximo));
    imgp.set_nombre_archivo(salida);
    imgp.guardar_archivo(salida);

    //* Cerrar archivos
    archivo.close();
}

Huffman::~Huffman() {
    if (this->arbol != nullptr) {
        delete arbol;
        arbol = nullptr;
    }
}

void Huffman::guardar_archivo(std::string nombre_archivo) {
    //* Abrir el archivo
    std::ofstream archivo(nombre_archivo, std::ios::binary | std::ios::out);
#ifdef _DEBUG_
    std::ofstream debugf(nombre_archivo + ".debug.txt", std::ios::out);
#endif

    // Verificar que se pudo abrir
    if (!archivo.is_open())
        throw std::runtime_error("No se pudo abrir el archivo");

    //* Header
    archivo.write((char *)(&this->ancho), sizeof(word_t));
    archivo.write((char *)(&this->alto), sizeof(word_t));
    archivo.write((char *)(&this->maximo), sizeof(byte_t));

#ifdef _DEBUG_
    debugf << "# HEADER #\n";
    debugf << "WIDTH\t" << (int)this->ancho << '\t' << "WORD (2)" << '\n';
    debugf << "HEIGHT\t" << (int)this->alto << '\t' << "WORD (2)" << '\n';
    debugf << "VALMAX\t" << (int)this->maximo << '\t' << "BYTE (1)" << '\n';
    debugf << "# FREQ TABLE #\n";
#endif

    //* Frecuencias
    // Escribir todas las frecuencias
    for (byte_t i = 0; i < 255; i++) {
        if (!codigos.count(i)) codigos[i] = CodigoElemento<byte_t>{i, 0, ""};

        // Write binary
        archivo.write((char *)&codigos[i].frecuencia, sizeof(lword_t));

#ifdef _DEBUG_
        debugf << static_cast<int>(codigos[i].elemento) << "\tBYTE (1)\t"
               << codigos[i].frecuencia << "\t\tDWORD (4)\t"
               << codigos[i].codigo << "\t\t\t\t BITS("
               << codigos[i].codigo.size() << ")\n";
#endif
    }

    //* Calcular codigo general
    std::stringstream generalss;
    Imagen::matriz_t pixels = imagen->get_pixeles();

    // Tomar todos los elementos de la imagen y colocarlos en codigo
    for (int i = 0; i < imagen->get_alto(); i++)
        for (int j = 0; j < imagen->get_ancho(); j++)
            generalss << codigos[pixels[i][j]].codigo;
    const std::string general = generalss.str();

#ifdef _DEBUG_
    debugf << "# CODE #\n" << general << "\n# END #";
#endif

    //* Transformar a bytes
    for (int i = 0; i < general.size();) {
        byte_t byte = 0x0000;
        // Transform every bit
        for (int k = 8; k > 0; k--, i++) {
            if (i >= general.size()) break;
            if (general.at(i) == '1') (byte |= (1 << (k - 1)));
        }

        // Escribir el bit
        archivo.write((char *)&byte, sizeof(byte_t));
    }

    //* Cerrar archivos
    archivo.close();
#ifdef _DEBUG_
    debugf.close();
#endif
}
