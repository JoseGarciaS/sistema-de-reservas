#include "SistemaReservas.h"
#include <iostream>
#include <sstream>
#include <ctime>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>
using namespace bsoncxx::builder::stream;


SistemaReservas::SistemaReservas() {
   

}


void SistemaReservas::mostrarMesasDisponibles(const std::string& fecha, int hora) const {
    std::cout << "Mesas disponibles el " << fecha << " a las " << hora << ":00:\n";
    
    auto documentos = dbHandler->findDocuments("mesas", document{} << finalize);
    
    for (const auto& doc : documentos) {
        int numeroMesa = doc["numeroMesa"].get_int32();
        int capacidad = doc["capacidad"].get_int32();
        
        std::cout << "Mesa #" << numeroMesa << " - Capacidad: " << capacidad << "\n";
    }
}

bool SistemaReservas::existeReserva(const std::string& codigoReserva) {
    document filtro{};
    filtro << "reservation_code" << codigoReserva;

    auto resultado = dbHandler->findDocument("reservations", filtro << finalize);
    return resultado.has_value();
}

bool SistemaReservas::existeReservaEnFechaHora(const std::string& fecha, int horaInicio, int horaFin) {

    time_t ahora = time(0);
    tm *tiempoActual = localtime(&ahora);


    std::stringstream ss;
    ss << (1900 + tiempoActual->tm_year) << "-"
       << (1 + tiempoActual->tm_mon) << "-"
       << tiempoActual->tm_mday;
    std::string fechaActual = ss.str();

    
    if (fecha < fechaActual) {
        std::cout << "Error: La fecha de la reservación debe ser mayor o igual a la fecha actual.\n";
        return true;  
    }

 
    if (horaInicio >= horaFin) {
        std::cout << "Error: La hora de inicio debe ser menor que la hora de fin.\n";
        return true;  
    }

    bsoncxx::builder::stream::document filtro{};
    filtro << "reservation_date" << fecha
           << "start_time" << bsoncxx::builder::stream::open_document
           << "$lt" << horaFin << bsoncxx::builder::stream::close_document  
           << "end_time" << bsoncxx::builder::stream::open_document
           << "$gt" << horaInicio << bsoncxx::builder::stream::close_document;

    auto reserva = dbHandler->findDocument("reservations", filtro << bsoncxx::builder::stream::finalize);

    return reserva.has_value();
}



int convertirHoraAMinutos(const std::string &hora) {
    int horas, minutos;
    char separador;

    std::stringstream ss(hora);
    ss >> horas >> separador >> minutos;  // Separador debe ser ':'

    return horas * 60 + minutos;
}


void SistemaReservas::reservarMesa(int numeroMesa, const std::string& fecha, int horaInicio, int horaFin, 
    const std::string& nombreCliente, const std::string& apellidoCliente, const std::string& telefono, 
    const std::string& email) {

    // Obtener la fecha y hora actual
    time_t ahora = time(0);
    tm *tiempoActual = localtime(&ahora);

  
    std::stringstream ss;
    ss << (1900 + tiempoActual->tm_year) << "-"
       << (1 + tiempoActual->tm_mon) << "-"
       << tiempoActual->tm_mday;
    std::string fechaActual = ss.str();

  
    if (fecha <= fechaActual) {
        std::cout << "Error: La fecha de la reservación debe ser mayor que la fecha actual.\n";
        return;
    }

    
    if (horaFin <= horaInicio) {
        std::cout << "Error: La hora final debe ser mayor que la hora inicial.\n";
        return;
    }

    
    if (existeReservaEnFechaHora(fecha, horaInicio, horaFin)) {
        std::cout << "Ya existe una reserva en esa fecha y hora.\n";
        return;
    }

  
    std::string codigoReserva = generarCodigoReserva(numeroMesa, fecha, horaInicio);

 
    guardarReservaEnBaseDeDatos(numeroMesa, fecha, horaInicio, horaFin, codigoReserva, nombreCliente, apellidoCliente, telefono, email);
    
    
    guardarAccionEnLog("Reserva creada", "Mesa " + std::to_string(numeroMesa) + " reservada por " + nombreCliente + " " + apellidoCliente);
    
   
    std::cout << "Reserva creada con código: " << codigoReserva << std::endl;
}


bool SistemaReservas::liberarMesaPorCodigo(const std::string& codigoReserva) {
    document filtro{};
    filtro << "reservation_code" << codigoReserva;

    bool eliminado = dbHandler->deleteDocument("reservations", filtro << finalize);

    if (eliminado) {
        std::cout << "Reserva liberada exitosamente." << std::endl;
        return true;
    } else {
        std::cout << "No se encontró ninguna reserva con el código " << codigoReserva << "." << std::endl;
        return false;
    }
}

void SistemaReservas::crearMesa(int numeroMesa, int capacidadMax, const std::string& descripcion) {
    document nuevaMesa{};
    nuevaMesa << "table_number" << numeroMesa
              << "max_capacity" << capacidadMax
              << "description" << descripcion;

    dbHandler->createDocument("tables", nuevaMesa << finalize);

    std::cout << "Mesa creada exitosamente con número: " << numeroMesa << std::endl;
}

std::string SistemaReservas::generarCodigoReserva(int numeroMesa, const std::string& fecha, int hora) {
    std::ostringstream codigo;
    codigo << "M" << numeroMesa << "_F" << fecha << "_H" << hora << "_" << contadorReservas;
    return codigo.str();
}

void SistemaReservas::guardarReservaEnBaseDeDatos(int numeroMesa, const std::string& fecha, int horaInicio, int horaFin, 
    const std::string& codigoReserva, const std::string& nombreCliente, const std::string& apellidoCliente, 
    const std::string& telefono, const std::string& email) {

    document doc{};
    doc << "reservation_code" << codigoReserva
        << "table_number" << numeroMesa
        << "first_name" << nombreCliente
        << "last_name" << apellidoCliente
        << "reservation_date" << fecha
        << "start_time" << horaInicio
        << "end_time" << horaFin
        << "phone_number" << telefono
        << "email_address" << email;

    dbHandler->createDocument("reservations", doc << finalize);
}


void SistemaReservas::guardarAccionEnLog(const std::string& accion, const std::string& detalles) {
    auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

    document doc{};
    doc << "action" << accion
        << "action_details" << detalles
        << "time_logged" << bsoncxx::types::b_date{std::chrono::system_clock::now()};
    
    dbHandler->createDocument("logs", doc << finalize);
}
