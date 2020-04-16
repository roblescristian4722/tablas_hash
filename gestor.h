#ifndef GESTOR_H
#define GESTOR_H

#if defined(_WIN32) | defined(_WIN64)
    #define CLEAR system("cls")
#else
    #define CLEAR system("clear")
#endif

#define TOTAL_CAMPOS 9

#include <iostream>
#include <cstdio>
#include <fstream>
#include <regex>
#include <string>
#include <cstring>
#include "usuario.h"
#include "hash_map.h"
using namespace std;

class Indice
{
public:
    bool operator<(const Indice &other);
    bool operator>(const Indice &other);
    bool operator==(const Indice &other);
    void operator=(const Indice &other);
    bool operator<(const string &other);
    bool operator>(const string &other);
    bool operator==(const string &other);

private:
    // Atributos
    char codigo[9];
    long referencia;
    Indice() {}
    Indice(const char *cod, long ref);
    ~Indice();
    friend class Gestor;
};

class Gestor
{
public:
    Gestor();
    ~Gestor();

    void menu();
    void capturar(const Usuario& usuario);
    void mostrar();
    void modificar();
    void buscar();
    void eliminar();
    void capturar_datos(Usuario& usuario);
    bool codigo_usado(const string codigo);
    void modificar_datos(Usuario& usuario, char i);
    void actualizar_indices();
    void leer_archivo_datos(Usuario &usuario, long *indice = nullptr);
    int busqueda_binaria(vector<HashMap<string, long>::Pair> &vec, string dato);
    void actualizar_pares();
private:
    vector<HashMap<string, long>::Pair> m_pares;
    HashMap<string, long> m_indices;
    long m_posFinal;

    /// EXPRESIONES REGULARES ///
    regex m_expNombre;
    regex m_expCodigo;
    regex m_expApellido;
    regex m_expGenero;
    regex m_expTipoSangre;

    enum OPC_MENU
    {
        OPC_CAPTURAR = '1',
        OPC_MOSTRAR,
        OPC_BUSCAR,
        OPC_MODIFICAR,
        OPC_ELIMINAR,
        OPC_SALIR
    };

    enum OPC_CAMPOS_MOD
    {
        OPC_CAMPO_NOM = '1',
        OPC_CAMPO_APE,
        OPC_CAMPO_EDAD,
        OPC_CAMPO_SEXO,
        OPC_CAMPO_PESO,
        OPC_CAMPO_MASA,
        OPC_CAMPO_TIPO_SANGRE,
        OPC_CAMPO_ALTURA,
        OPC_CAMPO_CANCELAR
    };

    enum CAMPOS_ACCEDER_ARCHIVO
    {
        CAMPO_COD = 0,
        CAMPO_NOM,
        CAMPO_APE,
        CAMPO_EDAD,
        CAMPO_SEXO,
        CAMPO_PESO,
        CAMPO_MASA,
        CAMPO_TIPO_SANGRE,
        CAMPO_ALTURA
    };
};

#endif