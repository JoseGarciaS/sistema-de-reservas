#ifndef MESA_H
#define MESA_H

#include <vector>
#include <map>
#include <iostream>
#include <string>
class Mesa {
private:
    int numeroMesa;
    int capacidad;
    std::string descripcion;
    std::map<int, bool> disponibilidadHora;

public:
   
    Mesa(int numero, int capacidad);

  
    int getNumeroMesa() const;

    
    int getCapacidad() const;

   
    bool estaDisponible(int hora) const;

    
    void reservar(int hora);

    
    void liberar(int hora);

   
    void mostrarDisponibilidad() const;

    void setDescripcion(const std::string& desc);

    std::string getDescripcion() const;
    
};

#endif // MESA_H
