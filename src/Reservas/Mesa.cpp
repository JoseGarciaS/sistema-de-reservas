#include "Mesa.h"
#include <iostream>

// Constructor: Inicializa la mesa con disponibilidad en todas las horas
Mesa::Mesa(int numero, int capacidad) : numeroMesa(numero), capacidad(capacidad) {
    // Inicializar la disponibilidad de la mesa a true para todas las horas (de 9:00 a 22:00)
    for (int i = 9; i <= 22; ++i) {
        disponibilidadHora[i] = true;
    }
}

int Mesa::getNumeroMesa() const {
    return numeroMesa;
}

int Mesa::getCapacidad() const {
    return capacidad;
}

bool Mesa::estaDisponible(int hora) const {
    if (disponibilidadHora.find(hora) != disponibilidadHora.end()) {
        return disponibilidadHora.at(hora);
    }
    return false;  
}

void Mesa::reservar(int hora) {
    if (estaDisponible(hora)) {
        disponibilidadHora[hora] = false;
        std::cout << "Mesa #" << numeroMesa << " reservada a las " << hora << ":00.\n";
    } else {
        std::cout << "Mesa #" << numeroMesa << " no está disponible a las " << hora << ":00.\n";
    }
}

void Mesa::liberar(int hora) {
    disponibilidadHora[hora] = true;
    std::cout << "Mesa #" << numeroMesa << " ha sido liberada para las " << hora << ":00.\n";
}

void Mesa::mostrarDisponibilidad() const {
    std::cout << "Disponibilidad de la mesa #" << numeroMesa << ":\n";
    for (int hora = 9; hora <= 22; ++hora) {
        std::cout << hora << ":00 - " << (estaDisponible(hora) ? "Disponible" : "Reservada") << "\n";
    }
}


std::string Mesa::getDescripcion() const {
    return descripcion;
}