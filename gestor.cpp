#include "gestor.h"

Gestor::Gestor()
{
    Gestor::Indice indiceTmp;
    string aux;
    Usuario usuarioTmp;
    fstream archivoDatos("usuarios.txt", ios::out | ios::in);
    fstream archivoIndices("indices.bin", ios::out | ios::in | ios::binary);

    m_expNombre.assign("([a-zA-ZñÑ]{3,})( [a-zA-ZñÑ]{2,})?{1,3}$");
    m_expCodigo.assign("[1-9]{1}[0-9]{8}$");
    m_expApellido.assign("([a-zA-ZñÑ]{3,})( [a-zA-ZñÑ]{3,})$");
    m_expGenero.assign("[MFmf]$");
    m_expTipoSangre.assign("(AB|ab|A|a|B|b|O|o)([+-]{1})$");
    m_posFinal = 0;

    if (!archivoDatos.is_open())
        archivoDatos.open("usuarios.txt", ios::out);

    else if (!archivoIndices.is_open())
    {
        indiceTmp.referencia = m_posFinal;
        archivoIndices.open("indices.bin", ios::out | ios::trunc | ios::binary);
        archivoIndices.write((char *)&indiceTmp, sizeof(indiceTmp));
    }
    else
    {
        archivoIndices.read((char*)&indiceTmp, sizeof(indiceTmp));
        m_posFinal = indiceTmp.referencia;

        while (!archivoIndices.eof())
        {
            archivoIndices.read((char*)&indiceTmp, sizeof(indiceTmp));
            if (archivoIndices.eof())
                break;
            m_indices.push_back(indiceTmp);
        }
    }
    archivoDatos.close();
    archivoIndices.close();
}

Gestor::~Gestor()
{}

void Gestor::menu()
{
    char opc;
    do
    {
        CLEAR;
        cout << " Ingrese la opción a ejecutar: " << endl
             << " " << char(OPC_CAPTURAR) << ") Capturar usuario" << endl
             << " " << char(OPC_MOSTRAR) << ") Mostrar usuarios" << endl
             << " " << char(OPC_BUSCAR) << ") Buscar usuario (por código)" << endl
             << " " << char(OPC_MODIFICAR) << ") Modificar usuario" << endl
             << " " << char(OPC_ELIMINAR) << ") Eliminar usuario" << endl
             << " " << char(OPC_SALIR) << ") Salir" << endl
             << " Opción: ";

        cin >> opc;
        CLEAR;
        cin.ignore();
        switch (opc){
            case OPC_CAPTURAR:{
                Usuario usuarioTmp;
                capturar_datos(usuarioTmp);
                capturar(usuarioTmp);
                cin.ignore();
            }
            break;

            case OPC_MODIFICAR:
                modificar();
            break;

            case OPC_BUSCAR:
                buscar();
            break;

            case OPC_ELIMINAR:
                eliminar();
            break;

            case OPC_MOSTRAR:
                CLEAR;
                mostrar();
            break;

            case OPC_SALIR:
                cout << " Hasta la próxima" << endl;
            return;

            default:
                cout << " Dato inválido, presione ENTER para continuar..." << endl;
            break;
        }
        cin.get();
    } while (opc != OPC_SALIR);
}

void Gestor::buscar()
{
    string codigo;
    Usuario usuarioTmp;
    int res;

    if (!m_indices.size())
        cout << " Aún no se han ingresado usuarios " << endl << endl
             << " Presione ENTER para continuar..." << endl;
    else{
        cout << " Ingrese el código a buscar: ";
        getline(cin, codigo);
        res = busqueda_binaria(m_indices, codigo);
        if (res == -1)
            cout << " Código no encontrado, presione ENTER para continuar..." << endl;
        else{
            leer_archivo_datos(usuarioTmp, res);
            //CLEAR;
            cout << endl
                 << " Usuario #" << res + 1 << endl
                 << " Código: " << usuarioTmp.getCodigo() << endl
                 << " Nombre: " << usuarioTmp.getNombre() << endl
                 << " Apellido: " << usuarioTmp.getApellido() << endl
                 << " Edad: " << usuarioTmp.getEdad() << endl
                 << " Género: " << usuarioTmp.getGenero() << endl
                 << " Peso: " << usuarioTmp.getPeso() << endl
                 << " Masa corporal: " << usuarioTmp.getMasaCorporal() << endl
                 << " Tipo sanguíneo: " << usuarioTmp.getTipoSangre() << endl
                 << " Altura: " << usuarioTmp.getAltura() << endl
                 << "----------------------------------------------"
                 << endl << endl
                 << " Presione ENTER para continuar..." << endl;
        }
    }
}

void Gestor::capturar(const Usuario& usuario)
{
    fstream archivoDatos;
    archivoDatos.open("usuarios.txt", ios::out | ios::in | ios::app);
    Gestor::Indice indiceTmp;

    if (!archivoDatos.is_open()){
        cout << "Error en el archivoDatos de salida" << endl;
        return;
    }
    
    strcpy(indiceTmp.codigo, usuario.getCodigo().c_str());
    indiceTmp.referencia = m_posFinal;
    m_indices.push_back(indiceTmp);

    archivoDatos << usuario.getCodigo() << '|' << usuario.getNombre() << '|'
                 << usuario.getApellido() << '|' << usuario.getEdad() << '|'
                 << usuario.getGenero() << '|' << usuario.getPeso() << '|'
                 << usuario.getMasaCorporal() << '|' << usuario.getTipoSangre() << '|'
                 << usuario.getAltura() << endl;

    m_posFinal += archivoDatos.tellg();

    archivoDatos.close();
    actualizar_indices();
    cout << endl
         << " Usuario añadido exitosamente." << endl
         << " Presione ENTER para continuar..." << endl;
}


void Gestor::eliminar()
{
    unsigned int elim;
    int res;
    long largElim;
    long pos;
    bool found = false;
    fstream archivoDatos("usuarios.txt", ios::in);
    fstream archivoTmp("usuarios.tmp", ios::out);
    string aux;
    Usuario usuarioTmp;

    mostrar();
    if (!m_indices.size()){
        cout << " Aún no hay datos ingresados." << endl
             << " presione ENTER para continuar" << endl;
        return;
    }

    cout << " Ingrese número del usuario a eliminar: ";
    cin >> elim;
    if (elim <= m_indices.size() && elim){
        m_posFinal = 0;
        while (!archivoDatos.eof()){
            // Se lee el código y la posición de este
            pos = archivoDatos.tellg();
            getline(archivoDatos, aux, '|');
            if (archivoDatos.eof())
                break;

            if (aux != m_indices[elim - 1].codigo || found){
                res = busqueda_binaria(m_indices, aux);

                if (!found)
                    m_indices[res].referencia = pos;
                else
                    m_indices[res].referencia = pos - largElim;
                
                // Se escribe el código y su separador en el tmp
                archivoTmp << aux << '|';
                // Se lee el resto del registro y se escribe en el tmp
                getline(archivoDatos, aux);
                archivoTmp << aux << '\n';
                // Se le suma el total de bytes del registro a la posición
                // final del archivo
                m_posFinal += archivoTmp.tellp();
            }
            else
            {
                found = true;
                m_indices.erase(m_indices.begin() + elim - 1);
                getline(archivoDatos, aux);
                largElim = archivoDatos.tellg();
                largElim -= pos;   
            }
        }
        archivoDatos.close();
        archivoTmp.close();
        remove("usuarios.txt");
        rename("usuarios.tmp", "usuarios.txt");
        actualizar_indices();
    }
    else
    {
        cout << endl
            << " Dato inválido, presione ENTER para continuar..."
            << endl;
        cin.get();
    }
}


void Gestor::modificar()
{   
    Usuario usuarioTmp;
    fstream archivoDatos;
    fstream archivoTmp;
    string aux;
    bool found;
    char opc;
    int res;
    long mod;
    long pos;
    long posMod;
    long largMod;

    mostrar();    
    if (m_indices.size())
    {
        cout << " Ingrese número del usuario a modificar: ";
        cin >> mod;

        if (mod <= m_indices.size() && mod){   
            do{
                cout << endl
                     << " Seleccione el campo a modificar:" << endl
                     << char(OPC_CAMPO_NOM) << ") Nombre" << endl
                     << char(OPC_CAMPO_APE) << ") Apellido" << endl
                     << char(OPC_CAMPO_EDAD) << ") Edad" << endl
                     << char(OPC_CAMPO_SEXO) << ") Sexo" << endl
                     << char(OPC_CAMPO_PESO) << ") Peso" << endl
                     << char(OPC_CAMPO_TIPO_SANGRE) << ") Tipo de sanguíneo" << endl
                     << char(OPC_CAMPO_ALTURA) << ") Altura" << endl
                     << char(OPC_CAMPO_CANCELAR) << ") Cancelar" << endl
                     << "Opción: ";
                cin >> opc;
            }while(opc < OPC_CAMPO_NOM || opc > OPC_CAMPO_CANCELAR);
            
            if (opc != OPC_CAMPO_CANCELAR){
                m_posFinal = 0;
                archivoTmp.open("usuarios.tmp", ios::out);
                archivoDatos.open("usuarios.txt", ios::in | ios::binary);
                while (!archivoDatos.eof()){
                    pos = archivoTmp.tellg();
                    getline(archivoDatos, aux, '|');
                    if (archivoDatos.eof())
                        break;

                    res = busqueda_binaria(m_indices, aux);
                    m_indices[res].referencia = pos;

                    if (aux != m_indices[mod - 1].codigo){
                        archivoTmp << aux << '|';
                        getline(archivoDatos, aux);
                        archivoTmp << aux << '\n';
                    }
                    else{
                        usuarioTmp.setCodigo(aux);
                        getline(archivoDatos, aux, '|');
                        usuarioTmp.setNombre(aux);
                        getline(archivoDatos, aux, '|');
                        usuarioTmp.setApellido(aux);
                        getline(archivoDatos, aux, '|');
                        usuarioTmp.setEdad(stoi(aux));
                        getline(archivoDatos, aux, '|');
                        usuarioTmp.setGenero(aux[0]);
                        getline(archivoDatos, aux, '|');
                        usuarioTmp.setPeso(stof(aux));
                        getline(archivoDatos, aux, '|');
                        usuarioTmp.setMasaCorporal(stof(aux));
                        getline(archivoDatos, aux, '|');
                        usuarioTmp.setTipoSangre(aux);
                        getline(archivoDatos, aux);
                        usuarioTmp.setAltura(stof(aux));

                        modificar_datos(usuarioTmp, opc);
                        archivoTmp << usuarioTmp.getCodigo() << '|'
                                   << usuarioTmp.getNombre() << '|'
                                   << usuarioTmp.getApellido() << '|'
                                   << usuarioTmp.getEdad() << '|'
                                   << usuarioTmp.getGenero() << '|'
                                   << usuarioTmp.getPeso() << '|'
                                   << usuarioTmp.getMasaCorporal() << '|'
                                   << usuarioTmp.getTipoSangre() << '|'
                                   << usuarioTmp.getAltura() << '\n';
                    }
                }
                archivoTmp.close();
                archivoDatos.close();
                remove("usuarios.txt");
                rename("usuarios.tmp", "usuarios.txt");
                actualizar_indices();

                cout << endl
                     << " Dato modificado correctamente" << endl
                     << " Presione ENTER para continuar..." << endl;
            }
        }
        else{
            cout << endl
                 << " Dato inválido, presione ENTER para continuar..." << endl;
            cin.get();       
        }
    }
}

void Gestor::mostrar()
{
    fstream archivoDatos("usuarios.txt", ios::in);
    Usuario usuarioTmp;
    string aux;

    if (!m_indices.size()){
        cout << " Aún no se han ingresado usuarios" << endl;
        return;
    }

    for (size_t i = 0; i < m_indices.size(); i++){
        leer_archivo_datos(usuarioTmp, i);
        cout << " Usuario #" << i + 1 << endl
             << " Código: " << usuarioTmp.getCodigo() << endl
             << " Nombre: " << usuarioTmp.getNombre() << endl
             << " Apellido: " << usuarioTmp.getApellido() << endl
             << " Edad: " << usuarioTmp.getEdad() << endl
             << " Género: " << usuarioTmp.getGenero() << endl
             << " Peso: " << usuarioTmp.getPeso() << endl
             << " Masa corporal: " << usuarioTmp.getMasaCorporal() << endl
             << " Tipo sanguíneo: " << usuarioTmp.getTipoSangre() << endl
             << " Altura: " << usuarioTmp.getAltura() << endl
             << "----------------------------------------------"
             << endl;
    }
    cout << " Presione ENTER para continuar" << endl;
}

bool Gestor::codigo_usado(const string codigo)
{
    for (int i = 0; i < m_indices.size(); i++)
        if (busqueda_binaria(m_indices, codigo) != -1)
            return true;
    return false;
}

void Gestor::capturar_datos(Usuario &usuario)
{
    bool continuar = false;
    unsigned int auxUInt;
    float auxFloat;
    string aux;

    cout << " Presione ENTER para continuar e ingrese los siguientes datos"
         << endl << endl;

    // CÓDIGO
    do{
        CLEAR;
        cout << " Código (9 dígitos): ";
        getline(cin, aux);
        if (codigo_usado(aux)){
            cout << endl
                 << " Error, código en uso. Presione ENTER para continuar";
            cin.get();
        }
        else if (!regex_match(aux, m_expCodigo)){
            cout << endl
                 << " Error el código debe de tener 9 dígitos y no debe de"
                 << " empezar con cero" << endl
                 << " Presione ENTER para continuar";
            cin.get();
        }
        else
            continuar = true;
    } while (!continuar);
    usuario.setCodigo(aux);
    continuar = false;

    // NOMBRE
    do{
        CLEAR;
        cout << " Nombre: ";
        getline(cin, aux);
        if (!regex_match(aux, m_expNombre)){
            cout << " Error, introducir mínimo un nombre y máximo tres " << endl
                 << " (mínimo cuatro letras por nombre)" << endl
                 << " Presione ENTER para continuar" << endl;
            cin.get();
        }
        else{
            for (auto &x : aux)
                x = toupper(x);
            continuar = true;
        }
    } while (!continuar);
    usuario.setNombre(aux);
    continuar = false;

    // APELLIDO
    do{
        CLEAR;
        cout << " Apellido: ";
        getline(cin, aux);
        if (!regex_match(aux, m_expApellido)){
            cout << " Error, se deben de introducir dos apellidos"
                 << " separados por espacios"
                 << endl
                 << " Presione ENTER para continuar"
                 << endl;
            cin.get();
        }
        else{
            for (auto &x : aux)
                x = toupper(x);
            continuar = true;
        }
    } while (!continuar);
    usuario.setApellido(aux);

    // EDAD
    do{
        CLEAR;
        cout << " Edad (de 12 a 115 años): ";
        cin >> auxUInt;
    } while (auxUInt > 115 || auxUInt < 12);
    usuario.setEdad(auxUInt);

    // GÉNERO
    do{
        CLEAR;
        cout << " Género (M = masculino | F = femenino): ";
        cin >> aux;
        aux[0] = toupper(aux[0]);
    } while (!regex_match(aux, m_expGenero));
    usuario.setGenero(aux[0]);

    // PESO
    do{
        CLEAR;
        cout << " Peso (de 30 a 300 kg): ";
        cin >> auxFloat;
    } while (auxFloat > 300 || auxFloat < 30);
    usuario.setPeso(auxFloat);
    continuar = false;
    cin.ignore();

    //TIPO DE SANGRE
    do{
        CLEAR;
        cout << " Tipo de sangre (AB, A, B, O)(+, -): ";
        getline(cin, aux);
        if (!regex_match(aux, m_expTipoSangre)){
            cout << endl
                 << " Error, vuelva a intentarlo" << endl;
            cin.get();
        }
        else{
            for (auto &x : aux)
                x = toupper(x);
            continuar = true;
        }
    } while (!continuar);
    usuario.setTipoSangre(aux);

    // ALTURA
    do{
        CLEAR;
        cout << " Altura (de 0.8 a 2.5 mts): ";
        cin >> auxFloat;
    } while (auxFloat < 0.8 || auxFloat > 2.5);
    usuario.setAltura(auxFloat);
    usuario.setMasaCorporal(usuario.getPeso() /
                            (auxFloat * auxFloat));
}

void Gestor::modificar_datos(Usuario &usuario, char i)
{
    bool continuar = false;
    unsigned int auxUInt;
    float auxFloat;
    string aux;

    cin.ignore();
    switch (i){
        case OPC_CAMPO_NOM:{
            do{
                CLEAR;
                cout << " Nombre: ";
                getline(cin, aux);
                if (!regex_match(aux, m_expNombre)){
                    cout << " Error, introducir mínimo un nombre y máximo tres "
                        << endl
                        << " (mínimo cuatro letras por nombre)"
                        << endl
                        << " Presione ENTER para continuar"
                        << endl;
                    cin.get();
                }
                else{
                    for (auto &x : aux)
                        x = toupper(x);
                    continuar = true;
                }
            } while (!continuar);
            usuario.setNombre(aux);
        }
        break;

        case OPC_CAMPO_APE:{
            do{
                CLEAR;
                cout << " Apellido: ";
                getline(cin, aux);
                if (!regex_match(aux, m_expApellido)){
                    cout << " Error, se deben de introducir dos apellidos"
                        << " separados por espacios"
                        << endl
                        << " Presione ENTER para continuar"
                        << endl;
                    cin.get();
                }
                else{
                    for (auto &x : aux)
                        x = toupper(x);
                    continuar = true;
                }
            } while (!continuar);
            usuario.setApellido(aux);
        }
        break;

        case OPC_CAMPO_EDAD:{
            do{
                CLEAR;
                cout << " Edad (de 12 a 115 años): ";
                cin >> auxUInt;
            } while (auxUInt > 115 || auxUInt < 12);
            usuario.setEdad(auxUInt);
        }
        break;

        case OPC_CAMPO_PESO:{
            do{
                CLEAR;
                cout << " Peso (de 30 a 300 kg): ";
                cin >> auxFloat;
            } while (auxFloat > 300 || auxFloat < 30);
            usuario.setPeso(auxFloat);
            usuario.setMasaCorporal(auxFloat /
                                    (usuario.getAltura() * usuario.getAltura()));
        }
        break;

        case OPC_CAMPO_SEXO:{
            do{
                CLEAR;
                cout << " Género (M = masculino | F = femenino): ";
                cin >> aux;
                aux[0] = toupper(aux[0]);
            } while (!regex_match(aux, m_expGenero));
            usuario.setGenero(aux[0]);
        }
        break;

        case OPC_CAMPO_TIPO_SANGRE:{
            do{
                CLEAR;
                cout << " Tipo de sangre (AB, A, B, O)(+, -): ";
                getline(cin, aux);
                if (codigo_usado(aux)){
                    cout << endl
                        << " Error, código en uso. Presione ENTER para continuar";
                    cin.get();
                }
                else if (!regex_match(aux, m_expTipoSangre)){
                    cout << endl
                        << " Error, vuelva a intentarlo" << endl;
                    cin.get();
                }
                else{
                    for (auto &x : aux)
                        x = toupper(x);
                    continuar = true;
                }
            } while (!continuar);
            usuario.setTipoSangre(aux);
        }
        break;

        case OPC_CAMPO_ALTURA:{
            do{
                CLEAR;
                cout << " Altura (de 0.8 a 2.5 mts): ";
                cin >> auxFloat;
            } while (auxFloat < 0.8 || auxFloat > 2.5);
            usuario.setAltura(auxFloat);
            usuario.setMasaCorporal(usuario.getPeso() /
                                    (usuario.getAltura() * usuario.getAltura()));
        }
        break;
    }
}

// INDICES
Gestor::Indice::Indice(const char *cod, long ref)
{
    strcpy(codigo, cod);
    referencia = ref;
}

Gestor::Indice::~Indice(){}

bool Gestor::Indice::operator<(const Gestor::Indice &other)
{ return (strcmp(this->codigo, other.codigo) < 0) ? true : false; }
bool Gestor::Indice::operator>(const Gestor::Indice &other)
{ return (strcmp(this->codigo, other.codigo) > 0) ? true : false; }
bool Gestor::Indice::operator==(const Gestor::Indice &other)
{ return !strcmp(this->codigo, other.codigo); }
bool Gestor::Indice::operator<(const string &other)
{ return string(this->codigo) < other; }
bool Gestor::Indice::operator>(const string &other)
{ return string(this->codigo) > other; }
bool Gestor::Indice::operator==(const string &other)
{ return string(this->codigo) == other; }
void Gestor::Indice::operator=(const Indice &other)
{
    strcpy(this->codigo, other.codigo);
    this->referencia = other.referencia;
}

int Gestor::busqueda_binaria(vector<Gestor::Indice> &vec, string dato)
{
    int l = 0;
    int r = int(vec.size() - 1);
    while (l <= r){
        int m = (l + r) / 2;
        if (vec[m] == dato)
            return m;
        else if (vec[m] > dato)
            r = m - 1;
        else
            l = m + 1;
    }
    return -1;
}

void Gestor::actualizar_indices()
{
    Gestor::Indice indiceTmp;
    fstream archivoDatosIndices("indices.bin", ios::out | ios::binary | ios::trunc);

    sort(m_indices.begin(), m_indices.end());
    indiceTmp.referencia = m_posFinal;
    archivoDatosIndices.write((char *)&indiceTmp, sizeof(indiceTmp));

    for (size_t i = 0; i < m_indices.size(); ++i){
        indiceTmp = m_indices[i];
        archivoDatosIndices.write((char *)&indiceTmp, sizeof(indiceTmp));
    }
    archivoDatosIndices.close();
}

void Gestor::leer_archivo_datos(Usuario& usuario, long pos)
{
    fstream archivo;
    string aux;
    archivo.open("usuarios.txt", ios::in);

    // EN CASO DE QUE SE DESEE LEER UNA POSICIÓN EN ESPECÍFICO
    // EN LUGAR DE TODO EL ARCHIVO DE DATOS
    if (pos != -1){
        archivo.seekg(m_indices[pos].referencia);
    }
    getline(archivo, aux, '|');
    usuario.setCodigo(aux);
    getline(archivo, aux, '|');
    usuario.setNombre(aux);
    getline(archivo, aux, '|');
    usuario.setApellido(aux);
    getline(archivo, aux, '|');
    usuario.setEdad(stoi(aux));
    getline(archivo, aux, '|');
    usuario.setGenero(aux[0]);
    getline(archivo, aux, '|');
    usuario.setPeso(stof(aux));
    getline(archivo, aux, '|');
    usuario.setMasaCorporal(stof(aux));
    getline(archivo, aux, '|');
    usuario.setTipoSangre(aux);
    getline(archivo, aux);
    usuario.setAltura(stof(aux));

    archivo.close();
}