#include "SistemaReservas.h"
#include <iostream>

// Constructor del sistema de reservas
SistemaReservas::SistemaReservas(const std::vector<Mesa>& listaMesas, const std::vector<Usuario*>& listaUsuarios)
    : mesas(listaMesas), usuarios(listaUsuarios) {}

// Mostrar todas las mesas disponibles
void SistemaReservas::mostrarMesasDisponibles() const {
    std::cout << "Mesas disponibles:\n";
    for (const auto& mesa : mesas) {
        if (mesa.estaDisponible()) {
            std::cout << "Mesa #" << mesa.getNumeroMesa() << " - Capacidad: " << mesa.getCapacidad() << "\n";
        }
    }
}

// Reservar una mesa
bool SistemaReservas::reservarMesa(int numeroMesa) {
    for (auto& mesa : mesas) {
        if (mesa.getNumeroMesa() == numeroMesa) {
            if (mesa.estaDisponible()) {
                mesa.reservar();
                std::cout << "Mesa #" << numeroMesa << " ha sido reservada.\n";
                return true;
            } else {
                std::cout << "Mesa #" << numeroMesa << " no está disponible.\n";
                return false;
            }
        }
    }
    std::cout << "Mesa #" << numeroMesa << " no existe.\n";
    return false;
}

// Liberar una mesa
void SistemaReservas::liberarMesa(int numeroMesa) {
    for (auto& mesa : mesas) {
        if (mesa.getNumeroMesa() == numeroMesa) {
            mesa.liberar();
            std::cout << "Mesa #" << numeroMesa << " ha sido liberada.\n";
            return;
        }
    }
    std::cout << "Mesa #" << numeroMesa << " no existe.\n";
}

// Crear una nueva mesa
void SistemaReservas::crearMesa(int numeroMesa, int capacidad) {
    for (const auto& mesa : mesas) {
        if (mesa.getNumeroMesa() == numeroMesa) {
            std::cout << "Mesa #" << numeroMesa << " ya existe.\n";
            return;
        }
    }
    mesas.emplace_back(numeroMesa, capacidad);
    std::cout << "Mesa #" << numeroMesa << " con capacidad para " << capacidad << " personas ha sido creada.\n";
}

// Manejar login
Usuario* SistemaReservas::login(const std::string& nombreUsuario, const std::string& password) const {
    for (const auto& usuario : usuarios) {
        if (usuario->getNombreUsuario() == nombreUsuario && usuario->getPassword() == password) {
            std::cout << "Bienvenido, " << usuario->getNombreUsuario() << "!\n";
            return usuario;
        }
    }
    std::cout << "Credenciales incorrectas.\n";
    return nullptr;
}
