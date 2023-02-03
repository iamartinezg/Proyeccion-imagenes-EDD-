#include "Controlador.h"

#include <cmath>
#include <cstring>
#include <exception>
#include <iomanip>
#include <iostream>
#include <string>

#include "Consola.h"
#include "GeneradorSemillas.h"
#include "Huffman.h"
#include "Imagen.h"
#include "Semilla.h"

//* Inicializar variables estáticas
Imagen *Controlador::imagen_cargada = nullptr;
Volumen *Controlador::volumen_cargado = nullptr;

//! Componente 1

void Controlador::cargar_imagen(Comando::arguments_t args) {
    if (args.size() != 1)
        throw Comando::Error(Comando::Error::Type::INVALID_ARGS);

    if (args[0].substr(args[0].find_last_of(".") + 1) != "pgm") {
        throw Comando::Error(Comando::Error::BAD_USE,
                             "Extensión del archivo errónea\n");
    }

    // Cargar la imagen en memoria
    try {
        // Si ya hay un volume cargado
        if (imagen_cargada != nullptr) {
            throw Comando::Error(Comando::Error::BAD_USE,
                                 "ya hay una imagen cargada\n");
        }

        // Crear la imagen
        imagen_cargada = new Imagen(args[0]);
    } catch (std::exception &e) {
        std::cerr << "(mensaje de error) La imagen " + args[0] +
                         " no ha podido ser cargada\n";
        return;
    }

    std::cout << "(proceso satisfactorio) La imagen '" + args[0] +
                     "' ha sido cargada"
              << std::endl;
}

void Controlador::limpiar() {
    if (imagen_cargada != nullptr) {
        delete imagen_cargada;
        imagen_cargada = nullptr;
    }

    if (volumen_cargado != nullptr) {
        delete volumen_cargado;
        volumen_cargado = nullptr;
    }
}

void Controlador::cargar_volumen(Comando::arguments_t args) {
    if (args.size() != 2)
        throw Comando::Error(Comando::Error::Type::INVALID_ARGS);

    // Cargar volumen
    try {
        // Si ya hay un volumen cargado
        if (volumen_cargado != nullptr) {
            throw Comando::Error(Comando::Error::BAD_USE,
                                 "ya hay un volumen cargado\n");
        }

        volumen_cargado = new Volumen(args[0], std::stoi(args[1]));
    }

    catch (std::exception &e) {
        std::cerr << "(mensaje de error) El volumen " + args[0] +
                         " no ha podido ser cargado\n";
        volumen_cargado = nullptr;
        return;
    }

    std::cout << "(proceso satisfactorio) El volumen " + args[0] +
                     " ha sido cargado"
              << std::endl;
}

void Controlador::info_imagen(Comando::arguments_t args) {
    if (!args.empty())
        throw Comando::Error(Comando::Error::Type::INVALID_ARGS);

    if (imagen_cargada == nullptr) {
        throw Comando::Error(Comando::Error::BAD_USE,
                             "No hay imagen cargada en memoria\n");
    }

    std::cout << imagen_cargada->to_string() << std::endl;
}

void Controlador::info_volumen(Comando::arguments_t args) {
    if (!args.empty())
        throw Comando::Error(Comando::Error::Type::INVALID_ARGS);

    if (volumen_cargado == nullptr) {
        throw Comando::Error(Comando::Error::BAD_USE,
                             "No hay volumen cargado en memoria\n");
    }

    std::cout << volumen_cargado->to_string() << std::endl;
}

void Controlador::proyeccion_2d(Comando::arguments_t args) {
    if (args.size() != 3)
        throw Comando::Error(Comando::Error::Type::INVALID_ARGS);

    //* Validar la dirección
    if (args[0].size() != 1)
        throw Comando::Error(Comando::Error::Type::INVALID_ARGS);
    switch (args[0].c_str()[0])  // Criterio
    {
            // Casos válidos
        case 'x':
        case 'y':
        case 'z':
            break;

        default:
            throw Comando::Error(Comando::Error::Type::INVALID_ARGS);
            break;
    }

    //* Validar el criterio
    if (args[1] != "minimo" && args[1] != "maximo" && args[1] != "promedio" &&
        args[1] != "mediana") {
        throw Comando::Error(Comando::Error::Type::INVALID_ARGS);
    }

    //* Crear la proyección
    try {
        // Si el volumen aún no se carga
        if (volumen_cargado == nullptr) throw std::exception();

        // Crear la proyeccion
        volumen_cargado->crear_proyeccion(args[1], args[0][0], args[2]);
    } catch (std::exception &e) {
        std::cerr << "(mensajes de error)\n"
                  << "El volumen aún no ha sido cargado en memoria\n"
                  << "La proyección 2D del volumen en memoria no ha podido "
                     "ser generada\n";
        return;
    }

    std::cout << "(proceso satisfactorio) La proyección 2D del volumen en "
                 "memoria ha sido generada"
              << std::endl;
}

//! Componente 2

void Controlador::codificar_imagen(Comando::arguments_t args) {
    if (args.size() != 1)
        throw Comando::Error(Comando::Error::Type::INVALID_ARGS);

    if (imagen_cargada == nullptr) {
        throw Comando::Error(Comando::Error::BAD_USE,
                             "No hay imagen cargada en memoria\n");
    }

    // Verificar que el nombre_archivo termine por .pgm
    if (args[0].substr(args[0].find_last_of(".") + 1) != "huffman") {
        throw Comando::Error(Comando::Error::BAD_USE,
                             "Extensión del archivo errónea\n");
    }

    try {
        // Normalizar la imagen de ser necesario
        Imagen normalizada{*imagen_cargada};
        if (normalizada.get_max_tam() > 255) normalizada.normalizar();

        // Construir la codificación de Huffman
        Huffman huff{normalizada};
        // Guardar el archivo
        huff.guardar_archivo(args[0]);
    } catch (...) {
        throw Comando::Error(Comando::Error::BAD_USE,
                             "No se pudo codificar la imágen\n");
    }

    // Mensaje de éxito
    std::cout << "(proceso satisfactorio) La imagen en memoria ha sido "
                 "codificada exitosamente"
              << std::endl;
}

void Controlador::decodificar_archivo(Comando::arguments_t args) {
    if (args.size() != 2)
        throw Comando::Error(Comando::Error::Type::INVALID_ARGS);

    // Verificar que el nombre_archivo termine por .pgm
    if ((args[0].substr(args[0].find_last_of(".") + 1) != "huffman") ||
        (args[1].substr(args[1].find_last_of(".") + 1) != "pgm")) {
        throw Comando::Error(Comando::Error::BAD_USE,
                             "Extensión del archivo errónea\n");
    }

    try {
        Huffman huffman{args[0], args[1]};
    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
        throw Comando::Error(Comando::Error::BAD_USE,
                             "(proceso satisfactorio) El archivo " + args[0] +
                                 " no ha podido ser decodificado\n");
    }

    std::cout << "(proceso satisfactorio) El archivo " + args[0] +
                     " ha sido decodificado exitosamente\n";
}

//! Componente 3

void Controlador::segmentar(Comando::arguments_t args) {
    // Validación de argumentos
    if (args.size() <= 1)
        throw Comando::Error(Comando::Error::Type::INVALID_ARGS);

    // Valida que la imágen está cargada
    if (imagen_cargada == nullptr) {
        throw Comando::Error(Comando::Error::BAD_USE,
                             "No hay imagen cargada en memoria\n");
    }

    // Obtener el primer argumento
    std::string archivo = args[0];

    // Semillas
    std::vector<Semilla> semillas{};

    for (size_t i = 1; i < args.size(); i++) {
        // Crear la semilla
        Semilla semilla{std::stoul(args[i]), std::stoul(args[++i]), 0,
                        std::stoi(args[++i])};
        // Verificar que no esté fuera de rango
        if (semilla.x >= imagen_cargada->get_ancho() ||
            semilla.y >= imagen_cargada->get_alto())
            throw Comando::Error(
                Comando::Error::BAD_USE,
                "Una semilla tiene coordenadas que exceden a la imagen\n");

        semillas.push_back(semilla);
    }

    // Generar la semilla
    GeneradorSemillas generador(*imagen_cargada);

    // Mostrar advertencia
    std::cout << "(advertencia) Calculando las rutas con el algoritmo de "
                 "Dijkstra, este proceso puede tardar bastante tiempo...\n";

    // Obtener los caminos de Dijkstra
    std::vector<std::pair<Semilla, Grafo<Semilla>::dijkstra_path>> caminos =
        generador.generar_caminos(semillas);

    // Generar las etiquetas
    std::vector<Semilla> etiquetas =
        generador.generar_etiquetas(*imagen_cargada, caminos);

    // Generar la matriz
    Imagen::matriz_t mtx_imagen = generador.generar_matrix(
        etiquetas, imagen_cargada->get_ancho(), imagen_cargada->get_alto());

    // Generar la imágen
    Imagen imagen{mtx_imagen};
    imagen.guardar_archivo(archivo);

    std::cout
        << "(proceso satisfactorio) La imagen en memoria fue segmentada "
           "correctamente y guardada en " +
               args[0] + "\n";
}
