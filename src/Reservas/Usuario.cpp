#include "Usuario.h"
#include <iostream>


Usuario::Usuario(const std::string& nombre, const std::string& pass, const std::string& rol)
    : nombreUsuario(nombre), password(pass), rol(rol) {}


std::string Usuario::getNombreUsuario() const {
    return nombreUsuario;
}

std::string Usuario::getPassword() const {
    return password;
}

std::string Usuario::getRol() const {
    return rol;
}


Admin::Admin(const std::string& nombre, const std::string& pass)
    : Usuario(nombre, pass, "admin") {}

void Admin::mostrarMenu() const {
    std::cout << "\n--- Menú Admin ---\n";
    std::cout << "1. Mostrar mesas disponibles\n";
    std::cout << "2. Reservar una mesa\n";
    std::cout << "3. Liberar una mesa\n";
    std::cout << "4. Crear una nueva mesa\n";
    std::cout << "5. Salir\n";
}


