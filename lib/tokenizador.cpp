#include <iostream>
#include <list>
#include <cctype>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <locale>
#include <algorithm>
#include "../include/tokenizador.h"

using namespace std;

ostream& operator<<(ostream& os, const Tokenizador& t){
    os << "DELIMITADORES: " << t.DelimitadoresPalabra() << " TRATA CASOS ESPECIALES: " << t.CasosEspeciales() << " PASAR A MINUSCULAS Y SIN ACENTOS: " << t.PasarAminuscSinAcentos();
    return os;
}

Tokenizador::Tokenizador (const string& delimitadoresPalabra, const bool& kcasosEspeciales, const bool& minuscSinAcentos){	
    // Inicializa delimiters a delimitadoresPalabra filtrando que no se introduzcan delimitadores repetidos (de izquierda a derecha, en cuyo caso se eliminar?an los que hayan sido repetidos por la derecha); casosEspeciales a kcasosEspeciales; pasarAminuscSinAcentos a minuscSinAcentos
    this->delimiters = "";
    DelimitadoresPalabra(delimitadoresPalabra);
    this->casosEspeciales = kcasosEspeciales;
    PasarAminuscSinAcentos(minuscSinAcentos);
}

Tokenizador::Tokenizador (const Tokenizador& token){
    this->delimiters = "";
    this->DelimitadoresPalabra(token.DelimitadoresPalabra());
    this->casosEspeciales = token.casosEspeciales;
    this->pasarAminuscSinAcentos = token.pasarAminuscSinAcentos;
}

Tokenizador::Tokenizador (){	
    // Inicializa delimiters=",;:.-/+*\\ '\"{}[]()<>?!??&#=\t@"; casosEspeciales a true; pasarAminuscSinAcentos a false
    this->delimiters = "";
    this->DelimitadoresPalabra(",;:.-/+*\\ '\"{}[]()<>?!??&#=\t@");
    this->casosEspeciales = true;
    this->pasarAminuscSinAcentos = false;
}

Tokenizador::~Tokenizador (){
	// Pone delimiters=""
    this->delimiters = "";
    this->casosEspeciales = false;
    this->pasarAminuscSinAcentos = false;
}

Tokenizador& Tokenizador::operator= (const Tokenizador& token){
    this->delimiters = token.delimiters;
    this->casosEspeciales = token.casosEspeciales;
    this->pasarAminuscSinAcentos = token.pasarAminuscSinAcentos;
    return *this;
}

string Tokenizador::tratar_URL(const string& token) const{

    //cout << "Tratando URL: " << token << endl;

    size_t prefix_len = 0;
    if(token.find("https:") == 0) prefix_len = 6;
    else if(token.find("http:") == 0) prefix_len = 5;
    else if(token.find("ftp:") == 0) prefix_len = 4;
    
    if (prefix_len == 0 || token.size() <= prefix_len){
        //cout << "No es una URL válida: " << token << endl;
        return "";
    }
    
    //cout << "Prefijo: " << prefix_len << endl;
    // Trata el caso especial de las URLs, detectando los l?mites correctamente.
    // Las URLs terminan cuando se encuentran:
    // - Un delimitador que NO sea de los especiales "_:/.?&-=#@"
    // - Un espacio en blanco
    // Los delimitadores especiales de URL est?n permitidos dentro de la URL
    //cout << "Tratando URL: " << token << endl;
    unordered_set<char> url_special_delimiters = {'_', ':', '/', '.', '?', '&', '-', '=', '#', '@'};
    
    size_t pos_fin = string::npos;
    
    // Buscar el final de la URL
    for (size_t i = 0; i < token.size(); i++){
        char c = token[i];
        
        // Los espacios siempre terminan la URL
        /*if (c == ' ' || c == '\n' || c == '\r' || c == '\t'){
            pos_fin = i;
            break;
        }*/
        
        // Si es un delimitador que no es especial de URL, termina la URL
        if (this->dicc_delimitadores.find(c) != this->dicc_delimitadores.end()){
            // Si es un delimitador especial de URL, continuamos
            if (url_special_delimiters.find(c) == url_special_delimiters.end()){ // NO es ?_:/.?&-=#@?
                pos_fin = i;
                break;
            }
        }
    }
    
    if (pos_fin == string::npos){
        pos_fin = token.size();
    }
    //cout << "Tratado URL: " << " -> " << string(token.data(), pos_fin) << endl;
    return string(token.data(), pos_fin); // Se devuelve la URL limpia, sin los delimitadores que pueda contener al final
}

string Tokenizador::tratar_DECIMAL(const string& token) const{
    // Trata el caso especial de los n?meros decimales, detectando los l?mites correctamente.
    // Los n?meros decimales terminan cuando se encuentran:
    // - Un delimitador que NO sea un punto o una coma
    // - Un espacio en blanco
    // Los puntos y comas est?n permitidos como separador decimal (solo uno)
    
    size_t pos_dec = token.find_first_of(this->DECIMAL_DELIMITERS);

    if(pos_dec != string::npos){
        // Si el punto decimal est? al final de la cadena o no est? precedido por un d?gito, no es un decimal
        if (pos_dec > 0 && pos_dec < token.size() - 1){
            if (!isdigit(token[pos_dec - 1]) || !isdigit(token[pos_dec + 1])){
                return "";
            }
        }
    }

    size_t pos_fin = string::npos;
    bool has_decimal_sep = false; // Indica si ya hemos encontrado un separador decimal
    
    // Iterar a trav?s del token para encontrar el final del n?mero decimal
    for (size_t i = 0; i < token.size(); i++){
        char c = token[i];
        
        // Si es un d?gito, continuamos
        if (isdigit(c)){
            continue;
        }
        
        // Si es un punto o coma (separador decimal)
        if ((c == '.' || c == ',') && !has_decimal_sep){
            has_decimal_sep = true;
            continue; // Permitimos un separador decimal
        }
        
        // Si es un delimitador del sistema
        if (this->dicc_delimitadores.find(c) != this->dicc_delimitadores.end()){
            // Si es otro delimitador, termina aqu?
            pos_fin = i;
            break;
        }
        else{
            // Si es otro car?cter que no es d?gito ni delimitador ni separador decimal, termina aqu?
            pos_fin = i;
            break;
        }
    }
    //cout << "Tratado DECIMAL: " << token << " -> " << string(token.data(), pos_fin) << endl;
    return string(token.data(), pos_fin); // Se devuelve el n?mero decimal limpio
}

string Tokenizador::tratar_EMAIL(const string& token) const{
    // Trata el caso especial de los emails, detectando los l?mites correctamente.
    // Los emails terminan cuando se encuentran:
    // - Un delimitador que NO sea un punto, guion o guion bajo
    // - Un espacio en blanco
    // Los puntos, guiones y guiones bajos est?n permitidos dentro del email
    size_t pos_espacio = token.find_first_of(" ");
    size_t pos_at = token.find("@");
    if(pos_at != string::npos){ // Si hay un '@' y est? antes de cualquier espacio (o no hay espacios)
        // Verificar que el indicador "@" est? rodeado de caracteres que no sean delimitadores
        if (pos_espacio != string::npos && pos_espacio < pos_at){
            return "";
        }
        if (pos_at == 0 || pos_at >= token.size() - 1){
            return "";
        }
    }

    cout << "Tratando EMAIL: " << token << endl;
    size_t pos_fin = token.size();
    bool has_a = false; // Indica si ya hemos encontrado el caracter '@'
    
    // Iterar a trav?s del token para encontrar el final del email
    for (size_t i = 0; i < token.size(); i++){
        char c = token[i];
        
        // Si es un d?gito o letra, continuamos
        
        // Si es un punto, guion o guion bajo, continuamos
        if (c == '.' || c == '-' || c == '_' ){
            continue;
        }

        if (c == '@' && !has_a){
            has_a = true;
            continue; // Permitimos un solo '@' en el email
        }
        
        // Si es un delimitador del sistema, termina aqu?
        if (this->dicc_delimitadores.find(c) != this->dicc_delimitadores.end()){
            cout << "Delimitador encontrado en email: '" << c << "' en la posicin " << i << endl;
            pos_fin = i;
            break;
        }
        /*else{
            // Si es otro car?cter que no es d?gito ni letra ni delimitador ni punto/guion/guion bajo, termina aqu?
            cout << "Car?cter no permitido encontrado en email: '" << c << "' en la posici?n " << i << endl;
            pos_fin = i;
            break;
        }*/
    }
    
    //cout << "Tratado EMAIL: " << token << " -> " << string(token.data(), pos_fin) << endl;
    cout << "Tratado EMAIL: " << token << " -> " << string(token.data(), pos_fin) << endl;
    return string(token.data(), pos_fin); // Se devuelve el email limpio
}

string Tokenizador::tratar_ACRONIMO(const string& token) const{
    // Trata el caso especial de los acr?nimos, detectando los l?mites correctamente.
    // Los acr?nimos terminan cuando se encuentran:
    // - Un delimitador que NO sea un punto
    // - Un espacio en blanco
    // Los puntos est?n permitidos dentro del acr?nimo (por ejemplo, "U.S.A.")
    
    size_t pos_fin = token.size();
    size_t pos_punto = 0;
    
    // Iterar a trav?s del token para encontrar el final del acr?nimo
    for (size_t i = 0; i < token.size(); i++){
        char c = token[i];
        
        // Si es un d?gito o letra, continuamos
        
        // Si es un punto, continuamos
        if (c == '.' && i > pos_punto){
            pos_punto = i;
            continue;
        }
        
        // Si es un delimitador del sistema, termina aqu?
        if (this->dicc_delimitadores.find(c) != this->dicc_delimitadores.end()){
            cout << "Delimitador encontrado en acr?nimo: '" << c << "' en la posicin " << i << "  "<< pos_punto << endl;
            pos_fin = i;
            break;
        }
        else{
            // Si es otro car?cter que no es d?gito ni letra ni delimitador ni punto, termina aqu?
            cout << "Delimitador encontrado en acr?nimo: '" << c << "' en la posicin " << i << "  "<< pos_punto << endl;
            pos_fin = i;
            break;
        }
    }
    
    return string(token.data(), pos_fin); // Se devuelve el acr?nimo limpio
}

string Tokenizador::minuscSinAcentos(const string& str) const{
    string s = str; // Se crea una copia de la cadena de entrada para modificarla, ya que la cadena de entrada es const
    for (size_t i = 0; i < s.length(); ++i) {
        unsigned char& c = (unsigned char&)s[i];

        // 1. Convertir CUALQUIER letra may?scula a min?scula
        // Rango A-Z est?ndar (ASCII)
        if (c >= 0x41 && c <= 0x5A) {
            c += 32;
        }
        // Rango extendido ISO-8859-1 (May?sculas acentuadas y especiales) 
        // De 0xC0 (?) a 0xDE (?), excluyendo 0xD7 (signo de multiplicaci?n) 
        else if (c >= 0xC0 && c <= 0xDE && c != 0xD7) {
            c += 32;
        }

        // 2. Mapear variantes de vocales (ya en min?scula) a su versi?n simple
        // Los valores Hex corresponden a las tablas del documento 
        switch (c) {
            // Variantes de 'a' (?, ?, ?, ?, ?, ?) 
            case 0xE0: case 0xE1: case 0xE2: case 0xE3: case 0xE4: case 0xE5:
                c = 'a'; break;
            
            // Variantes de 'e' (?, ?, ?, ?) 
            case 0xE8: case 0xE9: case 0xEA: case 0xEB:
                c = 'e'; break;
            
            // Variantes de 'i' (?, ?, ?, ?) [cite: 29]
            case 0xEC: case 0xED: case 0xEE: case 0xEF:
                c = 'i'; break;
            
            // Variantes de 'o' (?, ?, ?, ?, ?, ?) [cite: 29]
            case 0xF2: case 0xF3: case 0xF4: case 0xF5: case 0xF6: case 0xF8:
                c = 'o'; break;
            
            // Variantes de 'u' (?, ?, ?, ?) [cite: 29]
            case 0xF9: case 0xFA: case 0xFB: case 0xFC:
                c = 'u'; break;
                
            // Otros casos comunes (opcional seg?n tu tokenizador)
            case 0xF1: c = 'n'; break; // ? -> n [cite: 5, 29]
            default: break;
        }
    }
    return s;
}

string Tokenizador::analisisCasosEspeciales (const string& str_analisis) const{
    
    size_t pos_espacio = str_analisis.find_first_of(" ");

    // Detectar URL
    if((str_analisis.find("http:") == 0 || str_analisis.find("https:") == 0 || str_analisis.find("ftp:") == 0)){
        // Verificar que el indicador sea seguido de algo que no sea delimitador
        size_t prefix_len = 0;
        if(str_analisis.find("https:") == 0) prefix_len = 6;
        else if(str_analisis.find("http:") == 0) prefix_len = 5;
        else if(str_analisis.find("ftp:") == 0) prefix_len = 4;

        // La URL debe tener al menos un car?cter adicional despu?s del prefijo
        if(str_analisis.size() > prefix_len){
            //string token = this->tratar_URL(str_analisis);
            //return token;
            return this->tratar_URL(str_analisis);
        }
    }
    
    // Detectar decimal
    string delimit_decimal = ",.";
    size_t pos_dec = str_analisis.find_first_of(delimit_decimal);

    if(pos_dec != string::npos && (pos_espacio == string::npos || pos_dec < pos_espacio)){
        // Si el punto decimal est? al final de la cadena o no est? precedido por un d?gito, no es un decimal
        if (pos_dec > 0 && pos_dec < str_analisis.size() - 1){
            if (isdigit(str_analisis[pos_dec - 1]) && isdigit(str_analisis[pos_dec + 1])){
                return this->tratar_DECIMAL(str_analisis);
            }
        }
    }

    // Detectar email
    size_t pos_at = str_analisis.find("@");
    if(pos_at != string::npos && (pos_espacio == string::npos || pos_at < pos_espacio)){ // Si hay un '@' y est? antes de cualquier espacio (o no hay espacios)
        // Verificar que el indicador "@" est? rodeado de caracteres que no sean delimitadores
        if (pos_at > 0 && pos_at < str_analisis.size() - 1){
            return this->tratar_EMAIL(str_analisis);
        }
    }
    
    // Detectar acronimo
    size_t pos_punto = str_analisis.find_first_of(".");
    if(pos_punto != string::npos && (pos_espacio == string::npos || pos_punto < pos_espacio)){
        if (pos_punto > 0 && pos_punto < str_analisis.size() - 1){
            // Verificar que los caracteres antes y despus del punto sean letras o dgitos
            if (isalnum(str_analisis[pos_punto - 1]) && isalnum(str_analisis[pos_punto + 1])){
                return this->tratar_ACRONIMO(str_analisis);
            }
        }
    }
    return "";
    
}

void Tokenizador::Tokenizar (const string& str, list<string>& tokens) const{

    tokens.clear();
    size_t inicio = 0;
    if (str.empty()){ // Si la cadena de entrada es vac?a, no se a?aden tokens a la lista y se devuelve
        return;
    }

    string str_analisis;
    if(this->pasarAminuscSinAcentos){
        str_analisis = this->minuscSinAcentos(str);
    }
    else{
        str_analisis = str;
    }

    for(size_t i = 0; i < str_analisis.size(); i++){
        //cout << "Analizando el caracter '" << str[i] << "' en la posici?n " << i << endl;
        /*if (this->dicc_delimitadores.find(str_analisis[i]) != this->dicc_delimitadores.end()){ // Si el caracter es un delimitador, se a?ade a la lista sin modificarlo
            if (inicio == i){ // Si el delimitador est? al inicio de la cadena o hay delimitadores consecutivos, se ignora el delimitador y no se a?ade un token vac?o a la lista
                inicio += 1;
                continue;
            }
            if (this->casosEspeciales){
                string token = string(str_analisis.data() + inicio, str_analisis.size() - inicio); // Se crea el token con el resto de la cadena a partir del delimitador, ya que si es un caso especial, el token puede contener delimitadores en su interior (por ejemplo, una URL puede contener puntos, guiones, etc.) y no se puede considerar que el token termina en el delimitador actual
                //cout << "Token analizado para casos especiales: " << token << endl;
                token = this->analisisCasosEspeciales(token);
                //cout << "Token analizado para casos especiales: " << token << endl;
                if (!token.empty()){
                    //cout << "Token detectado como caso especial: " << token << endl;
                    tokens.push_back(token);
                    i = inicio + token.size() - 1; // Se actualiza i para que apunte al ?ltimo caracter del caso especial, ya que el bucle incrementar? i en 1, por lo que el siguiente caracter a analizar ser? el siguiente al ?ltimo caracter del caso especial
                    inicio = i + 1;
                    continue; // Si el token es un caso especial, se a?ade a la lista sin modificar
                }

            }
            //cout << "PALABRA: " << string(str.data() + inicio, i - inicio) << endl;
            string token = string(str_analisis.data() + inicio, i - inicio);
            tokens.push_back(token);
            inicio = i + 1;
        }
        */
        char c = str_analisis[i];
        string token;
        //cout << "Analisis: "<< string(str_analisis.data() + inicio, str_analisis.size() - inicio) << endl;
        if (this->URL_DELIMITERS.find(c) != string::npos){
            token = this->tratar_URL(string(str_analisis.data() + inicio, str_analisis.size() - inicio + 1));
            //cout << "Token analizado para URL: " << token << endl;
        }

        if (this->DECIMAL_DELIMITERS.find(c) != string::npos){
            token = this->tratar_DECIMAL(string(str_analisis.data() + inicio, str_analisis.size() - inicio));
        }

        else if (this->EMAIL_DELIMITER==c){
            token = this->tratar_EMAIL(string(str_analisis.data() + inicio, str_analisis.size() - inicio));
        }

        else if (token.empty() && this->dicc_delimitadores.find(c) != this->dicc_delimitadores.end()){ // 
            //cout << "Delimitador encontrado: '" << c << "' en la posici?n " << i << endl;
            if (inicio == i){ // Si el delimitador est? al inicio de la cadena o hay delimitadores consecutivos, se ignora el delimitador y no se a?ade un token vac?o a la lista
                inicio += 1;
                continue;
            }
            token = string(str_analisis.data() + inicio, i - inicio); 
            tokens.push_back(token);
            i= inicio + token.size();
            inicio = i + 1;
        } 

        else if(!token.empty()){
            //cout << "Push de "<< token<<endl;
            if (inicio == i){ // Si el delimitador est? al inicio de la cadena o hay delimitadores consecutivos, se ignora el delimitador y no se a?ade un token vac?o a la lista
                inicio += 1;
                continue;
            }
            tokens.push_back(token);
            i = inicio + token.size();
            inicio = i + 1;

        }

        

    }
    if (inicio < str.size()) { // Si elltimo token no termina con un delimitador, se aade elltimo token a la lista
        string token = string(str.data() + inicio, str.size() - inicio);
        tokens.push_back(token);
    }

    for (const string& token : tokens){
        cout << token << endl;
    }
    cout << endl << endl << endl;
}

bool Tokenizador::Tokenizar (const string& i, const string& f) const{
    // Tokeniza el fichero i guardando la salida en el fichero f (una palabra en cada l?nea del fichero). Devolver? true si se realiza la tokenizaci?n de forma correcta; false en caso contrario enviando a cerr el mensaje correspondiente (p.ej. que no exista el archivo i)
}

bool Tokenizador::Tokenizar (const string & i) const{
    // Tokeniza el fichero i guardando la salida en un fichero de nombre i a?adi?ndole extensi?n .tk (sin eliminar previamente la extensi?n de i por ejemplo, del archivo pp.txt se generar?a el resultado en pp.txt.tk), y que contendr? una palabra en cada l?nea del fichero. Devolver? true si se realiza la tokenizaci?n de forma correcta; false en caso contrario enviando a cerr el mensaje correspondiente (p.ej. que no exista el archivo i)
}

bool Tokenizador::TokenizarListaFicheros (const string& i) const{
    // Tokeniza el fichero i que contiene un nombre de fichero por l?nea guardando la salida en ficheros (uno por cada l?nea de i) cuyo nombre ser? el le?do en i a?adi?ndole extensi?n .tk, y que contendr? una palabra en cada l?nea del fichero le?do en i. Devolver? true si se realiza la tokenizaci?n de forma correcta de todos los archivos que contiene i; devolver? false en caso contrario enviando a cerr el mensaje correspondiente (p.ej. que no exista el archivo i, o que se trate de un directorio, enviando a "cerr" los archivos de i que no existan o que sean directorios; luego no se ha de interrumpir la ejecuci?n si hay alg?n archivo en i que no exista)
}

bool Tokenizador::TokenizarDirectorio (const string& i) const{
    // Tokeniza todos los archivos que contenga el directorio i, incluyendo los de los subdirectorios, guardando la salida en ficheros cuyo nombre ser? el de entrada a?adi?ndole extensi?n .tk, y que contendr? una palabra en cada l?nea del fichero. Devolver? true si se realiza la tokenizaci?n de forma correcta de todos los archivos; devolver? false en caso contrario enviando a cerr el mensaje correspondiente (p.ej. que no exista el directorio i, o los ficheros que no se hayan podido tokenizar)
}

void Tokenizador::DelimitadoresPalabra(const string& nuevoDelimiters){
    // Inicializa delimiters a nuevoDelimiters, filtrando que no se introduzcan delimitadores repetidos (de izquierda a derecha, en cuyo caso se eliminar?an los que hayan sido repetidos por la derecha)
    /*
    this->delimiters = " ";

    for(char letra : nuevoDelimiters){
        if (!this->dicc_delimitadores.count(letra)){
            this->dicc_delimitadores[letra] = 1;
            this->delimiters += letra; 
        }
    }
    */
    //cout << "Nuevo delimitadores: " << nuevoDelimiters << endl;

    //this->delimiters = "";
    //this->dicc_delimitadores.clear();
    

    if (this->dicc_delimitadores.empty()){
        this->dicc_delimitadores.insert(' '); // El espacio se a?adir? siempre como delimitador, aunque no se introduzca en nuevoDelimiters
        this->dicc_delimitadores.insert('\n'); // El salto de l?nea se a?adir? siempre como delimitador, aunque no se introduzca en nuevoDelimiters
        this->dicc_delimitadores.insert('\r'); // El retorno de carro se a?adir? siempre como delimitador, aunque no se introduzca en nuevoDelimiters
    }

    for(char letra : nuevoDelimiters) {
        if (this->dicc_delimitadores.find(letra) == this->dicc_delimitadores.end()) {
            this->dicc_delimitadores.insert(letra);
            this->delimiters += letra;
        }
    }

    //cout << "Delimitadores finales: " << this->delimiters << endl;
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
    // A?ade al final de "delimiters" los nuevos delimitadores que aparezcan en "nuevoDelimiters" (no se almacenar?n caracteres repetidos)
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
    // Cambia la variable privada "pasarAminuscSinAcentos". Atenci?n al formato de codificaci?n del corpus (comando "file" de Linux). Para la correcci?n de la pr?ctica se utilizar? el formato actual (ISO-8859). 
    this->pasarAminuscSinAcentos = nuevoPasarAminuscSinAcentos;
}

bool Tokenizador::PasarAminuscSinAcentos () const{
    // Devuelve el contenido de la variable privada "pasarAminuscSinAcentos"
    return this->pasarAminuscSinAcentos;
}