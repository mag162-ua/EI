#include <iostream>
#include <list>
#include <cctype>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include "../include/tokenizador.h"

using namespace std;

ostream& operator<<(ostream& os, const Tokenizador& t){
    os << "DELIMITADORES: " << t.DelimitadoresPalabra() << " TRATA CASOS ESPECIALES: " << t.CasosEspeciales() << " PASAR A MINUSCULAS Y SIN ACENTOS: " << t.PasarAminuscSinAcentos();
    return os;
}

Tokenizador::Tokenizador (const string& delimitadoresPalabra, const bool& kcasosEspeciales, const bool& minuscSinAcentos){	
    // Inicializa delimiters a delimitadoresPalabra filtrando que no se introduzcan delimitadores repetidos (de izquierda a derecha, en cuyo caso se eliminarían los que hayan sido repetidos por la derecha); casosEspeciales a kcasosEspeciales; pasarAminuscSinAcentos a minuscSinAcentos
    DelimitadoresPalabra(delimitadoresPalabra);
    this->casosEspeciales = kcasosEspeciales;
    PasarAminuscSinAcentos(minuscSinAcentos);
}

Tokenizador::Tokenizador (const Tokenizador& token){
    this->delimiters = token.delimiters;
    this->casosEspeciales = token.casosEspeciales;
    this->pasarAminuscSinAcentos = token.pasarAminuscSinAcentos;
}

Tokenizador::Tokenizador (){	
    // Inicializa delimiters=",;:.-/+*\\ '\"{}[]()<>¡!¿?&#=\t@"; casosEspeciales a true; pasarAminuscSinAcentos a false
    this->delimiters = ",;:.-/+*\\ '\"{}[]()<>¡!¿?&#=\t@";
    this->casosEspeciales = true;
    this->pasarAminuscSinAcentos = false;
}

Tokenizador::~Tokenizador (){
	// Pone delimiters=""
    this->delimiters = " ";
    this->casosEspeciales = false;
    this->pasarAminuscSinAcentos = false;
}

Tokenizador& Tokenizador::operator= (const Tokenizador& token){
    this->delimiters = token.delimiters;
    this->casosEspeciales = token.casosEspeciales;
    this->pasarAminuscSinAcentos = token.pasarAminuscSinAcentos;
    return *this;
}

enum casoEspecial Tokenizador::analisisCasosEspeciales (const string& str) const{
    // Analiza la cadena de entrada para detectar los casos especiales que contiene y almacenarlos en casosEspecialesDetectados. Se utilizará para mejorar la eficiencia de la tokenización, ya que si no se han detectado casos especiales, no se comprobará si un token es caso especial o no, sino que se considerará que no lo es (si el token contiene un caracter que no es delimitador ni caso especial, se considerará que el token es un caso especial)
    //cout << "Analizando casos especiales para la cadena: " << str << endl;

    if(str.find("http:") == 0 || str.find("https:") == 0 || str.find("ftp:") == 0){
        return URL;
    }
    // Detectar decimal
    int pos_dec = str.find('.');
    pos_dec = (pos_dec != string::npos) ? pos_dec : str.find(',');
    if(pos_dec != string::npos){
        if(pos_dec > 0 && isdigit(str[pos_dec-1])){
            return DECIMAL;
        }
    }
    return NINGUNO; // No se han detectado casos especiales
    /*else if(str.find("@") != string::npos){
        return EMAIL;
    }
    else if(str.find(".") != string::npos && str.find_first_not_of("0123456789.") == string::npos){
        return DECIMAL;
    }
    else if(str.find("-") != string::npos && str.find_first_not_of("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-") == string::npos){
        return ACRONIMO;
    }
    else if(str.find(" ") != string::npos){
        return MULTIPALABRA;
    }
    else{
        return (casoEspecial) -1; // No se han detectado casos especiales
    }*/
}

void Tokenizador::Tokenizar (const string& str, list<string>& tokens) const{
    // Tokeniza str devolviendo el resultado en tokens. La lista tokens se vaciará antes de almacenar el resultado de la tokenización. 
    /*tokens.clear();
    size_t inicio = 0;
    if (str.empty()){ // Si la cadena de entrada es vacía, no se añaden tokens a la lista y se devuelve
        return;
    } 
    enum casoEspecial casosEspecialesDetectados = NINGUNO; // Analiza la cadena de entrada para detectar los casos especiales que contiene y almacenarlos en casosEspecialesDetectados. Se utilizará para mejorar la eficiencia de la tokenización, ya que si no se han detectado casos especiales, no se comprobará si un token es caso especial o no, sino que se considerará que no lo es (si el token contiene un caracter que no es delimitador ni caso especial, se considerará que el token es un caso especial)
    //enum casoEspecial casosEspecialesDetectados = this->analisisCasosEspeciales(str); // Analiza la cadena de entrada para detectar los casos especiales que contiene y almacenarlos en casosEspecialesDetectados. Se utilizará para mejorar la eficiencia de la tokenización, ya que si no se han detectado casos especiales, no se comprobará si un token es caso especial o no, sino que se considerará que no lo es (si el token contiene un caracter que no es delimitador ni caso especial, se considerará que el token es un caso especial)
    
    for (size_t i = 0; i < str.size(); i++) {
        cout << "Analizando el caracter '" << str[i] << "' en la posición " << i << endl;
        if (this->dicc_delimitadores.find(str[i]) == this->dicc_delimitadores.end()) { // Si el caracter no es un delimitador o es un caso especial
            continue;
        }
        else{
            if (inicio == i){ // Si el delimitador está al inicio de la cadena o hay delimitadores consecutivos, se ignora el delimitador y no se añade un token vacío a la lista
                inicio += 1;
                continue;
            }
            if (this->dicc_noEspeciales.find(str[i]) != this->dicc_noEspeciales.end()) { // Si el caracter es un caso especial, se añade a la lista sin modificarlo
                string token = string(str.data() + inicio, i - inicio);
                tokens.push_back(token);
                inicio = i + 1;
                casosEspecialesDetectados = NINGUNO; // Si se ha detectado un caso especial, se resetea casosEspecialesDetectados para que no se considere que los siguientes tokens son casos especiales aunque contengan caracteres que no son delimitadores ni casos especiales, ya que solo se considerará que un token es un caso especial si contiene un caracter que no es delimitador ni caso especial
                continue;
            }
            string token = string(str.data() + inicio, i - inicio + 1); // +1 para incluir el delimitador como parte del token
            if (this->casosEspeciales){
                cout << "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAaa" << endl;
                if (this->analisisCasosEspeciales(token) == URL){
                    cout << "BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB" << endl;
                    casosEspecialesDetectados = URL;
                    continue; // Si el token es un caso especial, se añade a la lista sin modificarlo
                }
            }
            cout << "CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC" << endl;
            tokens.push_back(token);
            inicio = i + 1;
        }
    }
    if (inicio < str.size()) { // Si el último token no termina con un delimitador, se añade el último token a la lista
        string token = string(str.data() + inicio, str.size() - inicio);
        tokens.push_back(token);
    }*/

    tokens.clear();
    size_t inicio = 0;
    if (str.empty()){ // Si la cadena de entrada es vacía, no se añaden tokens a la lista y se devuelve
        return;
    }
    unordered_set<char> dicc_AllDelimitadores;
    dicc_AllDelimitadores.insert(' '); // El espacio se añadirá siempre como delimitador, aunque no se introduzca en nuevoDelimiters
    dicc_AllDelimitadores.insert('\n'); // El salto de línea se añadirá siempre como delimitador, aunque no se introduzca en nuevoDelimiters
    dicc_AllDelimitadores.insert('\r'); // El retorno de carro se añadirá siempre como delimitador, aunque no
    if (!this->casosEspeciales){
        dicc_AllDelimitadores.insert(this->dicc_delimitadores.begin(), this->dicc_delimitadores.end()); // Se añaden los delimitadores a dicc_AllDelimitadores
    }
    
    enum casoEspecial casoDetectado = NINGUNO;

    for(size_t i = 0; i < str.size(); i++){
        if (!this->casosEspeciales){ // No hay casos especiales, delimitadores normales
            if (dicc_AllDelimitadores.find(str[i]) == dicc_AllDelimitadores.end()){ // Si el caracter no es un delimitador ni un caso especial, se añade a la lista sin modificarlo
                continue;
            }

            if (inicio == i){ // Si el delimitador está al inicio de la cadena o hay delimitadores consecutivos, se ignora el delimitador y no se añade un token vacío a la lista
                inicio += 1;
                continue;
            }
            string token = string(str.data() + inicio, i - inicio);
            tokens.push_back(token);
            inicio = i + 1;
            continue;
        }
        else{
            if (dicc_AllDelimitadores.find(str[i]) != dicc_AllDelimitadores.end()) { // Si el caracter es un caso especial, se añade a la lista sin modificarlo
                if (inicio == i){ // Si el delimitador está al inicio de la cadena o hay delimitadores consecutivos, se ignora el delimitador y no se añade un token vacío a la lista
                    inicio += 1;
                    continue;
                }
                string token = string(str.data() + inicio, i - inicio);
                tokens.push_back(token);
                inicio = i + 1;
                casoDetectado = NINGUNO; // Si se ha detectado un caso especial, se resetea casoDetectado para que no se considere que los siguientes tokens son casos especiales aunque contengan caracteres que no son delimitadores ni casos especiales, ya que solo se considerará que un token es un caso especial si contiene un caracter que no es delimitador ni caso especial
                continue;
            }
            else{
                if (casoDetectado != NINGUNO){ // Si se ha detectado un caso especial, se añade a la lista sin modificarlo
                    continue;
                }
                if (this->dicc_delimitadores.find(str[i]) != this->dicc_delimitadores.end()){ // Si el caracter es un delimitador, se añade a la lista sin modificarlo
                    //cout << "Especiales: " << str[i] << endl;
                    if (inicio == i){ // Si el delimitador está al inicio de la cadena o hay delimitadores consecutivos, se ignora el delimitador y no se añade un token vacío a la lista
                        inicio += 1;
                        continue;
                    }
                    string token = string(str.data() + inicio, i - inicio + 1); // +1 para incluir el delimitador como parte del token
                    casoDetectado = this->analisisCasosEspeciales(token);
                    if (casoDetectado == URL){
                        cout << "Token detectado como URL: " << token << endl;
                        continue; // Si el token es un caso especial, se añade a la lista sin modificarlo
                    }
                    if (casoDetectado == DECIMAL){
                        cout << "Token detectado como DECIMAL: " << token << endl;
                        continue; // Si el token es un caso especial, se añade a la lista sin modificarlo
                    }
                    cout << "PALABRA: " << token << endl;
                    token.pop_back(); // Se elimina el delimitador del token, ya que el delimitador se añadirá como un token separado en la siguiente iteración del bucle
                    tokens.push_back(token); // Se añade el token sin el delimitador, ya que el delimitador se añadirá como un token separado en la siguiente iteración del bucle
                    inicio = i + 1;
                    casoDetectado = NINGUNO; // Si se ha detectado un caso especial, se resetea casoDetectado para que no se considere que los siguientes tokens son casos especiales aunque contengan caracteres que no son delimitadores ni casos especiales, ya que solo se considerará que un token es un caso especial si contiene un caracter que no es delimitador ni caso especial
                }
            }
        }
        //cout << i << endl;
    }
}

bool Tokenizador::Tokenizar (const string& i, const string& f) const{
    // Tokeniza el fichero i guardando la salida en el fichero f (una palabra en cada línea del fichero). Devolverá true si se realiza la tokenización de forma correcta; false en caso contrario enviando a cerr el mensaje correspondiente (p.ej. que no exista el archivo i)
}

bool Tokenizador::Tokenizar (const string & i) const{
    // Tokeniza el fichero i guardando la salida en un fichero de nombre i añadiéndole extensión .tk (sin eliminar previamente la extensión de i por ejemplo, del archivo pp.txt se generaría el resultado en pp.txt.tk), y que contendrá una palabra en cada línea del fichero. Devolverá true si se realiza la tokenización de forma correcta; false en caso contrario enviando a cerr el mensaje correspondiente (p.ej. que no exista el archivo i)
}

bool Tokenizador::TokenizarListaFicheros (const string& i) const{
    // Tokeniza el fichero i que contiene un nombre de fichero por línea guardando la salida en ficheros (uno por cada línea de i) cuyo nombre será el leído en i añadiéndole extensión .tk, y que contendrá una palabra en cada línea del fichero leído en i. Devolverá true si se realiza la tokenización de forma correcta de todos los archivos que contiene i; devolverá false en caso contrario enviando a cerr el mensaje correspondiente (p.ej. que no exista el archivo i, o que se trate de un directorio, enviando a "cerr" los archivos de i que no existan o que sean directorios; luego no se ha de interrumpir la ejecución si hay algún archivo en i que no exista)
}

bool Tokenizador::TokenizarDirectorio (const string& i) const{
    // Tokeniza todos los archivos que contenga el directorio i, incluyendo los de los subdirectorios, guardando la salida en ficheros cuyo nombre será el de entrada añadiéndole extensión .tk, y que contendrá una palabra en cada línea del fichero. Devolverá true si se realiza la tokenización de forma correcta de todos los archivos; devolverá false en caso contrario enviando a cerr el mensaje correspondiente (p.ej. que no exista el directorio i, o los ficheros que no se hayan podido tokenizar)
}

void Tokenizador::DelimitadoresPalabra(const string& nuevoDelimiters){
    // Inicializa delimiters a nuevoDelimiters, filtrando que no se introduzcan delimitadores repetidos (de izquierda a derecha, en cuyo caso se eliminarían los que hayan sido repetidos por la derecha)
    /*
    this->delimiters = " ";

    for(char letra : nuevoDelimiters){
        if (!this->dicc_delimitadores.count(letra)){
            this->dicc_delimitadores[letra] = 1;
            this->delimiters += letra; 
        }
    }
    */

    this->delimiters = " ";
    //this->dicc_noEspeciales.insert(' '); // El espacio se añadirá siempre como delimitador, aunque no se introduzca en nuevoDelimiters
    //this->dicc_noEspeciales.insert('\n'); // El salto de línea se añadirá siempre como delimitador, aunque no se introduzca en nuevoDelimiters
    //this->dicc_noEspeciales.insert('\r'); // El retorno de carro se añadirá siempre como delimitador, aunque no se introduzca en nuevoDelimiters

    for(char letra : nuevoDelimiters) {
        if (this->dicc_delimitadores.find(letra) == this->dicc_delimitadores.end()) {
            this->dicc_delimitadores.insert(letra);
            this->delimiters += letra;
        }
    }
    /*
    bool visto[256] = {false}; //rellena el array a valor False

    this->delimiters = " ";

    for(unsigned char letra : nuevoDelimiters){
        if (!visto[letra]){
            visto[letra]=true;
            this->delimiters += letra;
        }
    }
    */
}

void Tokenizador::AnyadirDelimitadoresPalabra(const string& nuevoDelimiters){ 
    // Añade al final de "delimiters" los nuevos delimitadores que aparezcan en "nuevoDelimiters" (no se almacenarán caracteres repetidos)
    DelimitadoresPalabra(this->delimiters + nuevoDelimiters);
}

string Tokenizador::DelimitadoresPalabra() const{
    // Devuelve "delimiters" 
    return this->delimiters;
}

void Tokenizador::CasosEspeciales (const bool& nuevoCasosEspeciales){
    // Cambia la variable privada "casosEspeciales" 
    this->casosEspeciales = nuevoCasosEspeciales;
}

bool Tokenizador::CasosEspeciales () const{
    // Devuelve el contenido de la variable privada "casosEspeciales" 
    return this->casosEspeciales;
}

void Tokenizador::PasarAminuscSinAcentos (const bool& nuevoPasarAminuscSinAcentos){
    // Cambia la variable privada "pasarAminuscSinAcentos". Atención al formato de codificación del corpus (comando "file" de Linux). Para la corrección de la práctica se utilizará el formato actual (ISO-8859). 
    this->pasarAminuscSinAcentos = nuevoPasarAminuscSinAcentos;
}

bool Tokenizador::PasarAminuscSinAcentos () const{
    // Devuelve el contenido de la variable privada "pasarAminuscSinAcentos"
    return this->pasarAminuscSinAcentos;
}