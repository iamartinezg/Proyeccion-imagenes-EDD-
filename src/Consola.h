/**
 * @file Consola.h
 * @authors Angel Talero, Juan Urquijo, Iván Martínez
 * @brief Clase encargada de manejar la línea de comandos
 * @version 0.1
 * @date 2022-02-08
 *
 * @copyright
 * Pontificia Universidad Javeriana
 * Facultad de Ingeniería
 * Departamento de Ingeniería de Sistemas
 * Bogotá D.C - Colombia
 *
 * Copyright © 2022
 */

#ifndef CONSOLE_H
#define CONSOLE_H

#include <exception>
#include <functional>
#include <string>
#include <vector>

// Comandos obligatorios
constexpr const char *_COMMAND_EXIT = "salir"; /**< Comando de salir */
constexpr const char *_COMMAND_HELP = "ayuda"; /**< Comando de ayuda */

/**
 * @class Comando
 * @brief Abstracción de un comando
 */
class Comando {
    /* ---- Definiciones de tipo  ---- */
   public:
    using arguments_t = std::vector<std::string>; /**< Vector de argumentos */

    /**
     * @typedef caller_t
     * @brief Función que un comando puede llamar
     * @param arguments_t Vector de argumentos
     * @throw Command::Error error en el comando
     */
    using caller_t = std::function<void(arguments_t)>;

    /**
     * @class Error
     * @brief Excepción que puede elevarse cuando un comando falla
     */
    class Error : public std::exception {
       public:
        /**
         * @enum Type
         * @brief tipo de fallo que se generó
         */
        enum Type {
            DEFAULT,
            INVALID_ARGS,
            DOES_NOT_EXIST,
            FILE_ERROR,
            BAD_USE
        };

       private:
        Type type;
        std::string _what;

       public:
        Error(Type type, std::string what = "Exception thrown");
        const char *what() const noexcept override;
        const Type get_type() const;
    };

    /* ---- Variables privadas ---- */
   protected:
    std::string command;     /**< Nombre del comando */
    caller_t caller;         /**< Función que el comando llama */
    std::string usage;       /**< Información de cómo usar el comando */
    std::string description; /**< Descripción del comando */

    /* ---- Constructores ---- */
   public:
    /**
     * @brief Construir un comando
     *
     * @param command Nombre con el cual llamar al comando
     * @param caller Función que el comando llama
     * @param usage [Opcional] Uso del comando Información del uso
     * @param description [Opcional] Descripción del comando
     */
    Comando(std::string command, caller_t caller, std::string usage = "",
            std::string description = "No hay descipción...");

    /* ---- Métodos públicas ---- */
   public:
    /**
     * @brief Obtener el nombre del comando
     * @return std::string Nombre del comando
     */
    std::string get_command() const;

    /**
     * @brief Obtener información sobre el uso del comando
     * @return std::string Texto
     */
    std::string get_usage() const;

    /**
     * @brief Obtener una descipción del comando
     * @return std::string Texto
     */
    std::string get_description() const;

    /**
     * @brief Llama a la función ligada al comando
     *
     * @param args Argumentos del comando
     * @throw Command::Error error en el comando
     */
    void call(arguments_t args) const;

    /**
     * @brief Información del comando
     * @return std::string Información
     */
    std::string to_string() const;
};

/**
 * @class Interpreter
 * @brief Intérprete de comandos de la línea de comando
 */
class Interprete {
    /* ---- Constantes privadas ---- */
   protected:
    static constexpr char indicator =
        '$'; /**< Indicador de la línea de comandos */

    /* ---- Variables privadas ---- */
   protected:
    std::vector<Comando> commands; /**< Lista de comandos disponibles */
    bool _exit_ = false;           /**< Salir de la línea de comandos */

    /* ---- Constructor ---- */
   public:
    /**
     * @brief Construir un intérprete de comandos
     * @param commands Lista con comandos disponibles
     */
    Interprete(std::vector<Comando> commands = std::vector<Comando>());

    /* ---- Métodos privados ---- */
   protected:
    /**
     * @brief Función que muestra información de todos los comandos ayuda sobre
     * un comando en específico
     * @return Command comando de ayuda
     * @throw Command::Error error en el comando
     */
    void show_help(Comando::arguments_t args) const;

    /* ---- Métodos públicos ---- */
   public:
    /**
     * @brief Añadir un comando a la lista
     * @param command Comando a añadir
     */
    void add_command(Comando command);

    /**
     * @brief Llamar a la línea de comandos
     * Crea un bucle infinito que recibe comandos hasta que se reciba el
     * comando especial de salida
     */
    void cli();
};

#endif  // CONSOLE_H