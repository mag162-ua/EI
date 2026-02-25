#include <iostream>
#include <list>
#include <cctype>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <locale>
#include <algorithm>
#include <fstream>
#include <sys/stat.h>
#include "../include/tokenizador.h"

using namespace std;

ostream& operator<<(ostream& os, const Tokenizador& t){
    os << "DELIMITADORES: " << t.DelimitadoresPalabra() << " TRATA CASOS ESPECIALES: " << t.CasosEspeciales() << " PASAR A MINUSCULAS Y SIN ACENTOS: " << t.PasarAminuscSinAcentos();
    return os;
}

Tokenizador::Tokenizador (const string& delimitadoresPalabra, const bool& kcasosEspeciales, const bool& minuscSinAcentos){	
    // Inicializa delimiters a delimitadoresPalabra filtrando que no se introduzcan delimitadores repetidos (de izquierda a derecha, en cuyo caso se eliminar?an los que hayan sido repetidos por la derecha); casosEspeciales a kcasosEspeciales; pasarAminuscSinAcentos a minuscSinAcentos
    this->delimiters = "";
    this->DelimitadoresPalabra(delimitadoresPalabra);
    this->CasosEspeciales(kcasosEspeciales);
    this->PasarAminuscSinAcentos(minuscSinAcentos);
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
    // Trata el caso especial de las URLs, detectando los l?mites correctamente.
    // Las URLs terminan cuando se encuentran:
    // - Un delimitador que NO sea de los especiales "_:/.?&-=#@"
    // - Un espacio en blanco
    // Los delimitadores especiales de URL est?n permitidos dentro de la URL

    size_t prefix_len = 0;
    if(token.find("https:") == 0) prefix_len = 5;
    else if(token.find("http:") == 0) prefix_len = 4;
    else if(token.find("ftp:") == 0) prefix_len = 3;
    if (prefix_len == 0 || token.size() <= prefix_len){
        return "";
    }
    
    size_t pos_fin = string::npos;
    
    // Buscar el final de la URL
    for (size_t i = prefix_len; i < token.size(); i++){
        char c = token[i];

        if (this->URL_DELIMITERS.find(c) != string::npos){
            continue;
        }
        // Si es un delimitador que no es especial de URL, termina la URL
        if (this->dicc_delimitadores.find(c) != this->dicc_delimitadores.end()){
            // Si es un delimitador especial de URL, continuamos
            if (i == prefix_len+1){
                pos_fin = prefix_len;
                break;
            }
            pos_fin = i;
            break;
        }
    }
    
    if (pos_fin == string::npos){
        pos_fin = token.size();
    }
    return string(token.data(), pos_fin); // Se devuelve la URL limpia, sin los delimitadores que pueda contener al final
}

string Tokenizador::tratar_DECIMAL(const string& token) const{
    // Trata el caso especial de los n?meros decimales, detectando los l?mites correctamente.
    // Los n?meros decimales terminan cuando se encuentran:
    // - Un delimitador que NO sea un punto o una coma
    // - Un espacio en blanco
    // Los puntos y comas est?n permitidos como separador decimal (solo uno)

    size_t pos_dec = token.find_first_of(this->DECIMAL_DELIMITERS);

    if (pos_dec == string::npos){
        return "";
    }
    else if (pos_dec == token.size() - 1 || !isdigit(token[pos_dec+1])){
        return "";
    }

    size_t pos_fin = string::npos;
    size_t pos_ult_dec = 0;
    
    // Iterar a través del token para encontrar el final del número decimal
    for (size_t i = 0; i < token.size(); i++){
        char c = token[i];
        
        // Si es un dígito, punto o coma, continuamos
        if (isdigit(c) || c == '.' || c == ','){
            if (c=='.' || c == ','){
                if (i == token.size() - 1){
                    pos_fin = token.size() - 1;
                    break;
                }
                if (i == pos_ult_dec + 1){
                    return "";
                }
                pos_ult_dec = i;
            }
            continue;
        }

        if (c == '%' || c == '$'){
            if (i == pos_dec + 1){ // Si el símbolo de porcentaje o dólar está justo después del separador decimal, no es un número válido
                return "";
            }
            if (i == token.size() - 1){
                pos_fin = i;
                break;
            } 
            if (i == token.size() - 1 || (token[i+1] == ' ' || token[i+1] == '\t' || token[i+1] == '\n' || token[i+1] == '\r')){ // Si el símbolo de porcentaje o dólar no está al final del token, no es un número válido
                pos_fin = i+1;
                break;
            }
            else{
                return "";
            }
        }

        if (this->dicc_delimitadores.find(c) != this->dicc_delimitadores.end()){
            // Si es un delimitador que no es punto o coma, termina el número decimal
            if (i == pos_ult_dec + 1){
                pos_fin = pos_ult_dec;
                break;
            }
            else{
                pos_fin = i;
                break;
            }
        }
        if (!isdigit(c)){
            return "";
        }
    }
    
    if (pos_fin == string::npos){
        pos_fin = token.size();
    }
    if (pos_dec == 0){ // Si el número decimal empieza por un punto o coma, se añade un cero al principio
        return "0" + string(token.data(), pos_fin); // Se devuelve el número decimal limpio con un cero al principio
    }
    
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

    size_t pos_fin = token.size();
    size_t pos_sig = token.size();
    bool has_a = false; // Indica si ya hemos encontrado el caracter '@'

    for (size_t i = 0; i < token.size(); i++){
        char c = token[i];

        // Si es un punto, guion o guion bajo, continuamos
        if (c == '.' || c == '-' || c == '_' ){
            if (i == pos_sig + 1){
                pos_fin = i-1; // Si el punto/guion/guion bajo est? justo despu?s de un '@', se actualiza el final del email para permitir casos como "a@b.c"
                break;
            }
            pos_sig = i;
            continue;
        }

        if (c == '@' && !has_a){
            pos_sig = i;
            has_a = true;
            pos_fin = i;
            continue; // Permitimos un solo '@' en el email
        }
        if (c == '@' && has_a){
            return ""; // Si hay un segundo '@', no es un email válido
        }
        
        // Si es un delimitador del sistema, termina aqu?
        if (this->dicc_delimitadores.find(c) != this->dicc_delimitadores.end()){
            if (i == pos_sig + 1){
                pos_fin = i-1; // Si el punto/guion/guion bajo est? justo despu?s de un '@', se actualiza el final del email para permitir casos como "a@b.c"
                break;
            }
            pos_fin = i;
            break;
        }
    }

    return string(token.data(), pos_fin); // Se devuelve el email limpio
}

string Tokenizador::tratar_ACRONIMO(const string& token) const{
    // Trata el caso especial de los acr?nimos, detectando los l?mites correctamente.
    // Los acr?nimos terminan cuando se encuentran:
    // - Un delimitador que NO sea un punto
    // - Un espacio en blanco
    // Los puntos est?n permitidos dentro del acr?nimo
    
    size_t pos_punto = string::npos;
    size_t pos_fin = token.size();
    
    // Iterar a través del token para encontrar el final del acrónimo
    for (size_t i = 0; i < token.size(); i++){
        char c = token[i];

        if (c == '.'){

            if (i == 0 || i == token.size() -1){ // Punto primera o ultima posición
                return "";
            }
            if (pos_punto != string::npos && i == pos_punto+1){ // Dospuntos seguidos
                return "";
            }

            pos_punto = i;
            continue;
        }
        
        // Si es un delimitador del sistema, termina aquí
        if (this->dicc_delimitadores.find(c) != this->dicc_delimitadores.end()){
            if (i == pos_punto+1){
                pos_fin = pos_punto;
                break;
            }
            pos_fin = i;
            break;
        }
    }
    
    return string(token.data(), pos_fin); // Se devuelve el acr?nimo limpio
}

string Tokenizador::tratar_MULTIPALABRA(const string& token) const{
    size_t pos_guion = string::npos;
    size_t pos_fin = token.size();

    for (size_t i = 0; i < token.size(); i++){
        char c = token[i];
       
        if (c == '-'){
            if (i == 0 || i == token.size() - 1){
                return "";
            }
            if (pos_guion != string::npos && i == pos_guion + 1){
                pos_fin = pos_guion; // Si el guion está justo después de otro guion, se actualiza el final para permitir casos como "MS-DOS"
                break;
            }
            pos_guion = i;
        }

        if (this->dicc_delimitadores.find(c) != this->dicc_delimitadores.end()){
            if (c == '-'){
                continue;
            }
            if (pos_guion != string::npos && i == pos_guion + 1){
                pos_fin = pos_guion; // Si el guion está justo después de un delimitador, se actualiza el final para permitir casos como "MS-DOS"
                break;
            }
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

        // Convertir CUALQUIER letra may?scula a min?scula
        // Rango A-Z est?ndar (ASCII)
        if (c >= 0x41 && c <= 0x5A) {

            c += 32;
        }
        // Rango extendido ISO-8859-1 (May?sculas acentuadas y especiales) 
        // De 0xC0 (?) a 0xDE (?), excluyendo 0xD7 (signo de multiplicaci?n) 
        else if (c >= 0xC0 && c <= 0xDE && c != 0xD7) {
            c += 32;
        }

        // Mapear variantes de vocales (ya en min?scula) a su versi?n simple
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
                
            default: break;
        }
    }
    return s;
}

void Tokenizador::Tokenizar (const string& str, list<string>& tokens) const{

    tokens.clear();
    size_t inicio = 0;
    if (str.empty()){ // Si la cadena de entrada es vac?a, no se a?aden tokens a la lista y se devuelve
        return;
    }

    string str_analisis;
    if(this->pasarAminuscSinAcentos){
        str_analisis = this->minuscSinAcentos(str) + '\n';
    }
    else{
        str_analisis = str + '\n';
    }

    for(size_t i = 0; i < str_analisis.size(); i++){
        char c = str_analisis[i];
        string token;
        if(this->dicc_delimitadores.find(c) != this->dicc_delimitadores.end()){
            
            if (this->casosEspeciales){
                if (this->URL_DELIMITERS.find(c) != string::npos){
                    token = this->tratar_URL(string(str_analisis.data() + inicio, str_analisis.size() - inicio + 1));
                }

                if (token.empty() && this->DECIMAL_DELIMITERS.find(c) != string::npos){
                    token = this->tratar_DECIMAL(string(str_analisis.data() + inicio, str_analisis.size() - inicio));
                }

                if (token.empty() && this->EMAIL_DELIMITER==c){
                    token = this->tratar_EMAIL(string(str_analisis.data() + inicio, str_analisis.size() - inicio));
                }

                if (token.empty() && this->ACRONIMO_DELIMITER==c){
                    token = this->tratar_ACRONIMO(string(str_analisis.data() + inicio, str_analisis.size() - inicio));
                }

                if (token.empty() && this->MULTIPALABRA_DELIMITER==c){
                    token = this->tratar_MULTIPALABRA(string(str_analisis.data() + inicio, str_analisis.size() - inicio));
                }

                if (token.empty()){
                    if (inicio == i){
                        inicio += 1;
                        continue;
                    }
                    token = string(str_analisis.data() + inicio, i - inicio); 
                    tokens.emplace_back(token);
                    i= inicio + token.size();
                    inicio = i + 1;
                    continue;
                }
                else{
                    tokens.emplace_back(token);
                    i = inicio + token.size() - 1; // Se actualiza i para que apunte al ?ltimo caracter del caso especial, ya que el bucle incrementar? i en 1, por lo que el siguiente caracter a analizar ser? el siguiente al ?ltimo caracter del caso especial
                    inicio = i + 1;
                    continue; // Si el token es un caso especial, se a?ade a la lista sin modificar
                }
            }
            if (inicio == i){
                inicio += 1;
                continue;
            }
            token = string(str_analisis.data() + inicio, i - inicio); 
            tokens.emplace_back(token);
            i= inicio + token.size();
            inicio = i + 1;
            continue;
            
        }

    }
    if (inicio < str.size()) { // Si elltimo token no termina con un delimitador, se aade elltimo token a la lista
        string token = string(str.data() + inicio, str.size() - inicio);
        tokens.push_back(token);
    }
}

bool Tokenizador::Tokenizar (const string& i, const string& f) const{
    // Tokeniza el fichero i guardando la salida en el fichero f (una palabra en cada l?nea del fichero). Devolver? true si se realiza la tokenizaci?n de forma correcta; false en caso contrario enviando a cerr el mensaje correspondiente (p.ej. que no exista el archivo i)

    // 1. Apertura directa de archivos usando los constructores
    ifstream entrada(i);
    if (!entrada) {
        cerr << "ERROR: No existe el archivo: " << i << '\n';
        return false;
    }

    ofstream salida(f);
    if (!salida) {
        cerr << "ERROR: No se pudo crear/escribir el archivo de salida: " << f << '\n';
        return false;
    }

    string cadena;
    list<string> tokens;

    // 2. Bucle de lectura seguro y eficiente
    while (getline(entrada, cadena)) {
        if (!cadena.empty()) {
            // 3. Tokenizamos la línea actual. Según el enunciado, esto vacía la lista 'tokens' previamente
            Tokenizar(cadena, tokens);
            
            // 4. Escribimos los tokens inmediatamente en el fichero de salida
            for (const string& token : tokens) {
                salida << token << '\n'; // 5. Uso de '\n' en lugar de endl
            }
        }
    }

    // Los destructores de ifstream y ofstream cierran los ficheros automáticamente
    return true;
}

bool Tokenizador::Tokenizar (const string & i) const{
    // Tokeniza el fichero i guardando la salida en un fichero de nombre i a?adi?ndole extensi?n .tk (sin eliminar previamente la extensi?n de i por ejemplo, del archivo pp.txt se generar?a el resultado en pp.txt.tk), y que contendr? una palabra en cada l?nea del fichero. Devolver? true si se realiza la tokenizaci?n de forma correcta; false en caso contrario enviando a cerr el mensaje correspondiente (p.ej. que no exista el archivo i)
    return this->Tokenizar(i,i+".tk");
}

bool Tokenizador::TokenizarListaFicheros (const string& i) const{
    // Tokeniza el fichero i que contiene un nombre de fichero por l?nea guardando la salida en ficheros (uno por cada l?nea de i) cuyo nombre ser? el le?do en i a?adi?ndole extensi?n .tk, y que contendr? una palabra en cada l?nea del fichero le?do en i. Devolver? true si se realiza la tokenizaci?n de forma correcta de todos los archivos que contiene i; devolver? false en caso contrario enviando a cerr el mensaje correspondiente (p.ej. que no exista el archivo i, o que se trate de un directorio, enviando a "cerr" los archivos de i que no existan o que sean directorios; luego no se ha de interrumpir la ejecuci?n si hay alg?n archivo en i que no exista)
    
    ifstream lista_ficheros(i);
    if (!lista_ficheros.is_open()) {
        cerr << "ERROR: No existe o no se pudo abrir el archivo de lista: " << i << '\n';
        return false;
    }

    string archivo_actual;
    bool todoCorrecto = true; // Se volverá false si falla la apertura de la lista o algún archivo
    struct stat info_archivo; // Estructura para almacenar información de la ruta

    // Leemos el archivo línea a línea
    while (getline(lista_ficheros, archivo_actual)) {
        // Optimización/Robustez: Limpiar posible '\r' si el archivo txt fue creado en Windows
        if (!archivo_actual.empty() && archivo_actual.back() == '\r') {
            archivo_actual.pop_back();
        }

        // Saltamos líneas vacías para evitar falsos errores
        if (archivo_actual.empty()) {
            continue;
        }

        // stat() devuelve 0 si tiene éxito obteniendo la info del archivo
        if (stat(archivo_actual.c_str(), &info_archivo) != 0) {
            cerr << "ERROR: El archivo listado no existe o no es accesible: " << archivo_actual << '\n';
            todoCorrecto = false;
            continue; // NO interrumpimos la ejecución, pasamos al siguiente
        }

        // Comprobamos si la ruta pertenece a un directorio utilizando la macro S_ISDIR
        if (S_ISDIR(info_archivo.st_mode)) {
            cerr << "ERROR: La ruta es un directorio, no un archivo: " << archivo_actual << '\n';
            todoCorrecto = false;
            continue; // Pasamos al siguiente
        }

        // Generamos el nombre del archivo de salida añadiendo ".tk"
        string archivo_salida = archivo_actual + ".tk";

        // Reutilizamos el método de tokenizar archivos que ya estaba optimizado.
        // Si este devuelve false, marcamos todoCorrecto a false, pero seguimos procesando.
        if (!Tokenizar(archivo_actual, archivo_salida)) {
            todoCorrecto = false;
        }
    }

    // El destructor de ifstream cierra la lista automáticamente
    return todoCorrecto;
}

bool Tokenizador::TokenizarDirectorio (const string& i) const{
    // Tokeniza todos los archivos que contenga el directorio i, incluyendo los de los subdirectorios, guardando la salida en ficheros cuyo nombre ser? el de entrada a?adi?ndole extensi?n .tk, y que contendr? una palabra en cada l?nea del fichero. Devolver? true si se realiza la tokenizaci?n de forma correcta de todos los archivos; devolver? false en caso contrario enviando a cerr el mensaje correspondiente (p.ej. que no exista el directorio i, o los ficheros que no se hayan podido tokenizar)
}

void Tokenizador::DelimitadoresPalabra(const string& nuevoDelimiters){
    this->delimiters = "";
    this->dicc_delimitadores.clear();

    this->dicc_delimitadores.reserve(nuevoDelimiters.size() + 3); // +3 para \n, \r, espacio
    

    if (this->dicc_delimitadores.empty()){
        if (this->casosEspeciales){
            this->dicc_delimitadores.insert(' ');
        }
        if (nuevoDelimiters.find(' ') != string::npos){
            this->dicc_delimitadores.insert(' ');
            this->delimiters+=' ';
        }
        this->dicc_delimitadores.insert('\n'); // El salto de l?nea se a?adir? siempre como delimitador, aunque no se introduzca en nuevoDelimiters
        this->dicc_delimitadores.insert('\r'); // El retorno de carro se a?adir? siempre como delimitador, aunque no se introduzca en nuevoDelimiters
    }

    for(char letra : nuevoDelimiters) {
        if (this->dicc_delimitadores.find(letra) == this->dicc_delimitadores.end()) {
            this->dicc_delimitadores.insert(letra);
            this->delimiters += letra;
        }
    }
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
    if (!nuevoCasosEspeciales && this->delimiters.find(" ") == string::npos){
        this->dicc_delimitadores.erase(' ');
    }
    if (nuevoCasosEspeciales){
        this->dicc_delimitadores.insert(' ');
    }
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