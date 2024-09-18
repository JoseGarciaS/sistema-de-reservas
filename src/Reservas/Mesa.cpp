#include "Mesa.h"


Mesa::Mesa(int numero, int cap) : numeroMesa(numero), capacidad(cap), disponible(true) {}


int Mesa::getNumeroMesa() const {
    return numeroMesa;
}

int Mesa::getCapacidad() const {
    return capacidad;
}

bool Mesa::estaDisponible() const {
    return disponible;
}

void Mesa::reservar() {
    disponible = false;
}

void Mesa::liberar() {
    disponible = true;
}
