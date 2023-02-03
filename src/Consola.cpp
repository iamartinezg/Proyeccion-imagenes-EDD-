#include "Consola.h"

#include <exception>
#include <iostream>
#include <sstream>
#include <stdexcept>

// Definiciones de la clase Command

Comando::Comando(std::string command, caller_t caller, std::string usage,
                 std::string description)
    : command(command),
      caller(std::move(caller)),
      usage(command + " " + usage),
      description(description) {}

std::string Comando::get_command() const { return this->command; }

std::string Comando::get_usage() const { return this->usage; }

std::string Comando::get_description() const { return this->description; }

void Comando::call(arguments_t args) const { this->caller(args); }

std::string Comando::to_string() const {
    return "Command: " + this->command + "\n" +
           "Caller type: " + typeid(this->caller).name() + "\n" +
           "Usage: " + this->usage + ".\n" +
           "Description: " + this->description + "\n";
}

// Definiciones de la excepción
Comando::Error::Error(Comando::Error::Type type, std::string what)
    : type(type), _what(what) {}

const char *Comando::Error::what() const noexcept {
    switch (this->type) {
        case INVALID_ARGS:
            return "Argumentos inválidos\n";
            break;

        case DOES_NOT_EXIST:
            return "El comando no existe\n";
            break;

        case FILE_ERROR:
            return "Error en la lectura de archivos\n";
            break;

        case BAD_USE:
            return _what.c_str();
            break;

        default:
            return "Comando inválido\n";
            break;
    }
}

const Comando::Error::Type Comando::Error::get_type() const {
    return this->type;
}

// Definición de la clase Interpreter

Interprete::Interprete(std::vector<Comando> commands) : commands(commands) {
    Interprete *this_interpreter = this;

    // Agregar información del comando ayuda
    this->commands.push_back(Comando(
        _COMMAND_HELP,
        [=](Comando::arguments_t args) { this_interpreter->show_help(args); },
        "[comando a consultar]",
        "Muestra información de ayuda sobre un comando"));

    // Agregar información del comando salir
    this->commands.push_back(Comando(
        _COMMAND_EXIT,
        [=](Comando::arguments_t args) { this_interpreter->_exit_ = true; },
        "", "Salir de la línea de comandos"));
}

void Interprete::add_command(Comando command) {
    this->commands.push_back(command);
}

void Interprete::cli() {
    while (!_exit_) {
        // Mostrar el indicador
        std::cout << Interprete::indicator << " " << std::flush;

        // Leer el comando
        std::string input;
        std::getline(std::cin, input);

        if (input.empty()) continue;

        // Tokenizar el comando
        std::stringstream str_stream(input);
        std::vector<std::string> tokens;
        std::string temp_str;

        while (std::getline(str_stream, temp_str, ' '))
            tokens.push_back(temp_str);

        // Buscar el comando y ejecutarlo
        bool _found_ = false;
        for (Comando command : this->commands)
            if (command.get_command() == tokens[0]) {
                _found_ = true;
                tokens.erase(tokens.begin());
                //! Capturar cualquier excepción que eleve
                try {
                    command.call(tokens);
                } catch (Comando::Error &e) {
                    std::cerr << "(mensaje de error) " << e.what();
                    if (e.get_type() == Comando::Error::Type::INVALID_ARGS)
                        std::cerr << "\tUso: " << command.get_usage()
                                  << std::endl;
                } catch (std::runtime_error &e) {
                    std::cerr
                        << "(mensaje de error) Error del sistema: " << e.what()
                        << '\n';
                }

                catch (std::exception &e) {
                    std::cerr
                        << "(mensaje de error) Error del sistema: " << e.what()
                        << '\n';
                }

                catch (...) {
                    std::cerr << "(mensaje de error) Error del sistema: no "
                                 "identificado\n";
                }
                break;
            }

        if (!_found_)
            std::cerr << "(mensaje de error) "
                      << "Comando inválido" << std::endl;
    }
}

void Interprete::show_help(Comando::arguments_t args) const {
    // Mostrar todos los comandos línea por línea
    if (args.empty()) {
        std::cout << "Mostrando todos los comandos...\n";
        std::cout << "Para ver el uso de un comando: ayuda <comando>\n\n";
        for (Comando command : this->commands)
            std::cout << "> " << command.get_command() << ": "
                      << command.get_description() << "\n";
    }

    // Si se hace mal uso del comando
    else if (args.size() > 1)
        throw Comando::Error(Comando::Error::INVALID_ARGS);

    // Mostrar información de un comando
    else {
        // Buscar el comando especificado
        bool found = false;
        for (Comando command : this->commands)
            if (command.get_command() == args[0]) {
                std::cout << "> " << command.get_command() << ": "
                          << command.get_description()
                          << ".\n\tUso: " << command.get_usage() << '\n';
                found = true;
                break;
            }

        // Si no se encontró el comando
        if (!found) throw Comando::Error(Comando::Error::DOES_NOT_EXIST);
    }

    std::cout << std::endl;
}
