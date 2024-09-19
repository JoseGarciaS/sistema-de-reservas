#ifndef SISTEMARESERVAS_H
#define SISTEMARESERVAS_H

#include <vector>
#include <string>

#include "mongoDBHandler.h"  // Incluir el handler de MongoDB
#include <bsoncxx/document/value.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include "envParser.h"
class SistemaReservas {
private:
    mongoDBHandler *dbHandler;  // Para manejar la base de datos
    // Lista de mesas
    int contadorReservas;       // Contador para generar códigos únicos

public:
    SistemaReservas();

    
    void mostrarMesasDisponibles(const std::string& fecha, int hora) const;

   void reservarMesa(int numeroMesa, const std::string& fecha, int horaInicio, int horaFin, 
    const std::string& nombreCliente, const std::string& apellidoCliente, const std::string& telefono, 
    const std::string& email);

    
   bool liberarMesaPorCodigo(const std::string& codigoReserva);

    void crearMesa(int numeroMesa, int capacidadMax, const std::string& descripcion);

   
    std::string generarCodigoReserva(int numeroMesa, const std::string& fecha, int hora);

   
    void guardarReservaEnBaseDeDatos(int numeroMesa, const std::string& fecha, int horaInicio, int horaFin, 
    const std::string& codigoReserva, const std::string& nombreCliente, const std::string& apellidoCliente, 
    const std::string& telefono, const std::string& email);

    void guardarAccionEnLog(const std::string& accion, const std::string& detalles);
    bool existeReserva(const std::string& codigoReserva);
    bool existeReservaEnFechaHora(const std::string& fecha, int horaInicio, int horaFin);
};

#endif // SISTEMARESERVAS_H
