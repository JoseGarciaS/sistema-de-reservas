#ifndef USUARIO_H
#define USUARIO_H

#include <string>

class Usuario {
protected:
    std::string nombreUsuario;
    std::string password;
    std::string rol;

public:
    Usuario(const std::string& nombre, const std::string& pass, const std::string& rol);
    virtual ~Usuario() = default;


    std::string getNombreUsuario() const;
    std::string getPassword() const;
    std::string getRol() const;

    virtual void mostrarMenu() const = 0;
};

class Admin : public Usuario {
public:
    Admin(const std::string& nombre, const std::string& pass);
    void mostrarMenu() const override;
};

class Cliente : public Usuario {
public:
    Cliente(const std::string& nombre, const std::string& pass);
    void mostrarMenu() const override;
};

#endif 
