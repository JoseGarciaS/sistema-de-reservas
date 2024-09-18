#ifndef SISTEMARESERVAS_H
#define SISTEMARESERVAS_H

#include <vector>
#include "Mesa.h"
#include "Usuario.h"

class SistemaReservas {
private:
    std::vector<Mesa> mesas;
    std::vector<Usuario*> usuarios;  
public:
    
    SistemaReservas(const std::vector<Mesa>& listaMesas, const std::vector<Usuario*>& listaUsuarios);

    
    void mostrarMesasDisponibles() const;

   
    bool reservarMesa(int numeroMesa);

    
    void liberarMesa(int numeroMesa);

   
    void crearMesa(int numeroMesa, int capacidad);

    
    Usuario* login(const std::string& nombreUsuario, const std::string& password) const;
};

#endif 
