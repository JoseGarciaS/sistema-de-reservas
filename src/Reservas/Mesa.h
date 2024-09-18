#ifndef MESA_H
#define MESA_H

class Mesa {
private:
    int numeroMesa;
    int capacidad;
    bool disponible;

public:
    
    Mesa(int numero, int cap);


    int getNumeroMesa() const;

   
    int getCapacidad() const;

    
    bool estaDisponible() const;

    
    void reservar();

    
    void liberar();
};

#endif 
