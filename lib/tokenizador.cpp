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

unsigned char Tokenizador::mapa_minusculas[256];

void Tokenizador::inicializar_delimitadores_especiales(){

    DELIMITADOR_URL[(unsigned char)':'] = true;
    DELIMITADOR_URL[(unsigned char)'/'] = true;
    DELIMITADOR_URL[(unsigned char)'.'] = true;
    DELIMITADOR_URL[(unsigned char)'?'] = true;
    DELIMITADOR_URL[(unsigned char)'&'] = true;
    DELIMITADOR_URL[(unsigned char)'='] = true;
    DELIMITADOR_URL[(unsigned char)'-'] = true;
    DELIMITADOR_URL[(unsigned char)'#'] = true;
    DELIMITADOR_URL[(unsigned char)'@'] = true;
    DELIMITADOR_URL[(unsigned char)'_'] = true;

    DELIMITADOR_DECIMAL[(unsigned char)'.'] = true;
    DELIMITADOR_DECIMAL[(unsigned char)','] = true;

    /*
    // 1. IDENTIDAD: Inicialmente cada carácter se mapea a sí mismo (0->0, 1->1, etc.)
    for (int i = 0; i < 256; ++i) {
        mapa_minusculas[i] = (unsigned char)i;
    }

    // 2. ASCII ESTÁNDAR: Transformar A-Z (0x41-0x5A) a a-z (0x61-0x7A)
    for (int i = 0x41; i <= 0x5A; ++i) {
        mapa_minusculas[i] = i + 32;
    }

    // 3. ISO-8859-1 EXTENDIDO: Transformar mayúsculas acentuadas a sus minúsculas 
    // Rango: À (0xC0) hasta Þ (0xDE), excluyendo el símbolo de multiplicación × (0xD7)
    for (int i = 0xC0; i <= 0xDE; ++i) {
        if (i != 0xD7) {
            mapa_minusculas[i] = i + 32; // Ejemplo: À (0xC0) -> à (0xE0)
        }
    }

    // 4. NORMALIZACIÓN DE ACENTOS (Mapeo a letra base)
    // Ya que todo está en minúsculas en la tabla, mapeamos las variantes a la base
    
    // Variantes de 'a': à, á, â, ã, ä, å  ->  a
    for (unsigned char c : {0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5}) mapa_minusculas[c] = 'a';
    
    // Variantes de 'e': è, é, ê, ë  ->  e
    for (unsigned char c : {0xE8, 0xE9, 0xEA, 0xEB}) mapa_minusculas[c] = 'e';

    // Variantes de 'i': ì, í, î, ï  ->  i
    for (unsigned char c : {0xEC, 0xED, 0xEE, 0xEF}) mapa_minusculas[c] = 'i';

    // Variantes de 'o': ò, ó, ô, õ, ö, ø  ->  o
    for (unsigned char c : {0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF8}) mapa_minusculas[c] = 'o';

    // Variantes de 'u': ù, ú, û, ü  ->  u
    for (unsigned char c : {0xF9, 0xFA, 0xFB, 0xFC}) mapa_minusculas[c] = 'u';

    // Variantes de 'c': ç -> c
    mapa_minusculas[0xE7] = 'c'; 

    // Variante de 'y': ý, ÿ -> y 
    for (unsigned char c : {0xFD, 0xFF}) mapa_minusculas[c] = 'y';
    
    // NOTA: No hace falta mapear las mayúsculas acentuadas (Á, É...) a base directamente
    // porque el paso 3 ya las convirtió en minúsculas acentuadas (á, é...)
    // y el paso 4 las convierte de minúscula acentuada a base.
    */
    // ... (tus inicializaciones de DELIMITADOR_URL, etc.)

    // 1. Identidad total
    for (int i = 0; i < 256; ++i) mapa_minusculas[i] = (unsigned char)i;

    // 2. A-Z estándar (ASCII)
    for (int i = 0x41; i <= 0x5A; ++i) mapa_minusculas[i] = i + 32;

    // 3. MAPEO DIRECTO A BASE (ISO-8859-1)
    // Esto ignora si es mayúscula o minúscula y va directo al grano:
    
    // Variantes de 'a' (À Á Â Ã Ä Å à á â ã ä å) -> 'a'
    for (unsigned char c : {0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5}) 
        mapa_minusculas[c] = 'a';

    // Variantes de 'e' (È É Ê Ë è é ê ë) -> 'e'
    for (unsigned char c : {0xC8, 0xC9, 0xCA, 0xCB, 0xE8, 0xE9, 0xEA, 0xEB}) 
        mapa_minusculas[c] = 'e';

    // Variantes de 'i' (Ì Í Î Ï ì í î ï) -> 'i'
    for (unsigned char c : {0xCC, 0xCD, 0xCE, 0xCF, 0xEC, 0xED, 0xEE, 0xEF}) 
        mapa_minusculas[c] = 'i';

    // Variantes de 'o' (Ò Ó Ô Õ Ö Ø ò ó ô õ ö ø) -> 'o'
    for (unsigned char c : {0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD8, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF8}) 
        mapa_minusculas[c] = 'o';

    // Variantes de 'u' (Ù Ú Û Ü ù ú û ü) -> 'u'
    for (unsigned char c : {0xD9, 0xDA, 0xDB, 0xDC, 0xF9, 0xFA, 0xFB, 0xFC}) 
        mapa_minusculas[c] = 'u';

    // Caso de la 'ñ': Ñ (0xD1) pasa a ñ (0xF1), pero la ñ se queda como ñ
    mapa_minusculas[0xD1] = 0xF1; 

    // Otros casos: Ç/ç -> c, Ý/ý/ÿ -> y
    mapa_minusculas[0xC7] = mapa_minusculas[0xE7] = 'c';
    mapa_minusculas[0xDD] = mapa_minusculas[0xFD] = mapa_minusculas[0xFF] = 'y';
}

ostream& operator<<(ostream& os, const Tokenizador& t){
    os << "DELIMITADORES: " << t.DelimitadoresPalabra() << " TRATA CASOS ESPECIALES: " << t.CasosEspeciales() << " PASAR A MINUSCULAS Y SIN ACENTOS: " << t.PasarAminuscSinAcentos();
    return os;
}

Tokenizador::Tokenizador (const string& delimitadoresPalabra, const bool& kcasosEspeciales, const bool& minuscSinAcentos){	
    // Inicializa delimiters a delimitadoresPalabra filtrando que no se introduzcan delimitadores repetidos (de izquierda a derecha, en cuyo caso se eliminar?an los que hayan sido repetidos por la derecha); casosEspeciales a kcasosEspeciales; pasarAminuscSinAcentos a minuscSinAcentos
    inicializar_delimitadores_especiales();
    this->delimiters = "";
    this->CasosEspeciales(kcasosEspeciales);
    this->DelimitadoresPalabra(delimitadoresPalabra);
    this->PasarAminuscSinAcentos(minuscSinAcentos);
}

Tokenizador::Tokenizador (const Tokenizador& token){
    inicializar_delimitadores_especiales();
    this->delimiters = "";
    this->CasosEspeciales(token.CasosEspeciales());
    this->DelimitadoresPalabra(token.DelimitadoresPalabra());
    this->pasarAminuscSinAcentos = token.pasarAminuscSinAcentos;
}

Tokenizador::Tokenizador (){	
    // Inicializa delimiters=",;:.-/+*\\ '\"{}[]()<>?!??&#=\t@"; casosEspeciales a true; pasarAminuscSinAcentos a false
    inicializar_delimitadores_especiales();
    this->delimiters = "";
    this->CasosEspeciales(true);
    this->DelimitadoresPalabra(",;:.-/+*\\ '\"{}[]()<>?!??&#=\t@");
    this->pasarAminuscSinAcentos = false;
}

Tokenizador::~Tokenizador (){
	// Pone delimiters=""
    this->delimiters = "";
    this->CasosEspeciales(false);
    this->DelimitadoresPalabra("");
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
    
    // Iterar a trav?s del token para encontrar el final del n?mero decimal
    for (size_t i = 0; i < token.size(); i++){
        char c = token[i];
        
        // Si es un d?gito, punto o coma, continuamos
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
            if (i == pos_dec + 1){ // Si el s?mbolo de porcentaje o d?lar est? justo despu?s del separador decimal, no es un n?mero v?lido
                return "";
            }
            if (i == token.size() - 1){
                pos_fin = i;
                break;
            } 
            if (i == token.size() - 1 || (token[i+1] == ' ' || token[i+1] == '\t' || token[i+1] == '\n' || token[i+1] == '\r')){ // Si el s?mbolo de porcentaje o d?lar no est? al final del token, no es un n?mero v?lido
                pos_fin = i+1;
                break;
            }
            else{
                return "";
            }
        }

        if (this->dicc_delimitadores.find(c) != this->dicc_delimitadores.end()){
            // Si es un delimitador que no es punto o coma, termina el n?mero decimal
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
    if (pos_dec == 0){ // Si el n?mero decimal empieza por un punto o coma, se a?ade un cero al principio
        return "0" + string(token.data(), pos_fin); // Se devuelve el n?mero decimal limpio con un cero al principio
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
            return ""; // Si hay un segundo '@', no es un email v?lido
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
    
    // Iterar a trav?s del token para encontrar el final del acr?nimo
    for (size_t i = 0; i < token.size(); i++){
        char c = token[i];

        if (c == '.'){

            if (i == 0 || i == token.size() -1){ // Punto primera o ultima posici?n
                return "";
            }
            if (pos_punto != string::npos && i == pos_punto+1){ // Dospuntos seguidos
                return "";
            }

            pos_punto = i;
            continue;
        }
        
        // Si es un delimitador del sistema, termina aqu?
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
                pos_fin = pos_guion; // Si el guion est? justo despu?s de otro guion, se actualiza el final para permitir casos como "MS-DOS"
                break;
            }
            pos_guion = i;
        }

        if (this->dicc_delimitadores.find(c) != this->dicc_delimitadores.end()){
            if (c == '-'){
                continue;
            }
            if (pos_guion != string::npos && i == pos_guion + 1){
                pos_fin = pos_guion; // Si el guion est? justo despu?s de un delimitador, se actualiza el final para permitir casos como "MS-DOS"
                break;
            }
            pos_fin = i;
            break;
        }
            
    }

    return string(token.data(), pos_fin); // Se devuelve el acr?nimo limpio
}

void Tokenizador::minuscSinAcentos(string& str) const{
    /*string s = str; // Se crea una copia de la cadena de entrada para modificarla, ya que la cadena de entrada es const

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
    return s;*/ 
    size_t len = str.length();

    for (size_t i = 0; i < len; ++i) {
        // Accedemos a la tabla usando el carácter actual como índice
        // y guardamos el resultado (que ya es la minúscula sin acento)
        str[i] = mapa_minusculas[(unsigned char)str[i]];
    }

}

size_t Tokenizador::tratar_url(const string& str,const int& inicio) const{

    //cout << "URL: "<< str.substr(inicio, str.size() - inicio)<<endl;

    size_t prefix_len = 0;
    size_t str_len = str.length();
    
    // Usamos compare para no copiar nada. 
    // Comparamos desde 'inicio' los caracteres necesarios.
    if (str.compare(inicio, 6, "https:") == 0) prefix_len = 6;
    else if (str.compare(inicio, 5, "http:") == 0) prefix_len = 5;
    else if (str.compare(inicio, 4, "ftp:") == 0) prefix_len = 4;

    if (prefix_len == 0 || (size_t)inicio + prefix_len > str.size()) {
        //return "";
        return string::npos;
    }

    size_t i = inicio + prefix_len;

    for (; i < str_len; ++i) {
        unsigned char c = (unsigned char)str[i];

        // 1. Si es un delimitador de URL (.:/?&=#@...), seguimos adelante
        if (this->DELIMITADOR_URL[c]) {
            continue;
        }

        // 2. Si es un delimitador general del tokenizador, la URL termina
        if (this->delimitadores[c]) {
            if(i == prefix_len){
                i--;
            }
            break;
        }

        // 3. Si no es ninguna de las anteriores, es un carácter normal (letra/número)
    }
    // Devolvemos la URL desde la 'h' hasta donde hayamos llegado
    //return str.substr(inicio, i - inicio);
    return i;
}

size_t Tokenizador::tratar_decimal(const string& str,const int& inicio,const size_t& primer_sig) const{
    
    size_t i = inicio;
    size_t str_len = str.length();

    for(;i < primer_sig;i++){
        if (!isdigit((unsigned char) str[i])){
            //cout<<"NO ES DECIMAL1"<<endl;
            //return "";
            return string::npos;
        }
    }

    i = primer_sig+1;

    if (i >= str_len || !isdigit(str[i])){ // Si el primer signo se encuentra en el final del string o el siguiente carac ter no es un número
        //return "";
        return string::npos;
    }

    size_t pos_signo = primer_sig;

    for(; i < str_len; i++){
        //unsigned char c = (unsigned char)str[i];

        if (isdigit(str[i])){ // Si no es un número NO es un decimal
            
            continue;
        }
        else{
            if (this->DELIMITADOR_DECIMAL[str[i]]) {
                if (i == pos_signo + 1){ // Dos signos juntos
                    //cout << str[pos_signo]<< " "<< str[i] << endl;
                    //cout<<"NO ES DECIMAL2"<<endl;
                    //return "";
                    return string::npos;
                }
                //cout << c << "pos: "<< i<<endl;
                pos_signo = i;
                continue;
            }

            if (str[i] == '%' || str[i] == '$'){
                if (i == pos_signo + 1){ // Si el s?mbolo de porcentaje o d?lar est? justo despu?s del separador decimal, no es un n?mero v?lido
                    //return "";
                    return string::npos;
                }
                if (i == str_len - 1 || (str[i+1] == ' ' || str[i+1] == '\t' || str[i+1] == '\n' || str[i+1] == '\r')){ // Si el s?mbolo de porcentaje o d?lar no est? al final del token, no es un n?mero v?lido
                    i = i+1;
                    break;
                }
                else{
                    //return "";
                    return string::npos;
                }
            }

            if(this->delimitadores[str[i]]){
                if (i == pos_signo + 1){ // Si './,' seguido de Delimitador, se elimina el .
                    //cout<<"NO ES DECIMAL5"<<endl;
                    i--;
                    break;
                }
                //cout<<"NO ES DECIMAL6"<< "Signo :"<<c<<" pos:"<<i<<endl;
                break;
            }

            //return ""; // No es un número, es dinstinto de ,.%$ y no es un delimitador.
            return string::npos;
        }

        

        

        
    }
    
    //return str.substr(inicio, i - inicio);
    return i;
}

size_t Tokenizador::tratar_email(const string& str,const int& inicio,const size_t& primer_arroba) const{
    if (primer_arroba == (size_t)inicio){ // @ no es el primer caracter
        //return "";
        return string::npos;
    }

    size_t str_len = str.length();

    if (primer_arroba + 1 >= str_len || this->delimitadores[(unsigned char)str[primer_arroba + 1]]) { // @ esta al final de str o está seguido de un delimitador
        //return "";
        return string::npos;
    }

    size_t i = primer_arroba + 1;
    size_t ult_sig = primer_arroba;

    for(;i < str_len; i++){
        //unsigned char c = (unsigned char)str[i];

        if (str[i] == '.' || str[i] == '-' || str[i] == '_'){
            if (i == ult_sig + 1){
                i--;
                break;
            }
            ult_sig = i;
            continue;
        }

        if (str[i] == '@'){
            //return "";
            return string::npos;
        }

        if(this->delimitadores[str[i]]){
            if (i == ult_sig + 1){
                i--;
                break;
            }
            break;
        }
    }
    //return str.substr(inicio, i - inicio);
    return i;
}

size_t Tokenizador::tratar_acronimo(const string& str,const int& inicio,const size_t& primer_punto) const{

    if (primer_punto == (size_t)inicio){ //punto en la primera posición
        //return "";
        return string::npos;
    }

    size_t i = primer_punto + 1;
    size_t str_len = str.length();
    size_t ultimo_punto = primer_punto;

    for(;i < str_len; i++){
        //unsigned char c = (unsigned char)str[i];

        if(str[i] == '.'){
            if(i == ultimo_punto + 1){
                i--;
                break;
            }
            ultimo_punto = i;
            continue;
        }

        if(this->delimitadores[str[i]]){
            if(i == ultimo_punto + 1){
                i--;
                break;
            }
            break;
        }
    }

    //return str.substr(inicio, i - inicio);
    return i;
}

size_t Tokenizador::tratar_multipalabra(const string& str,const int& inicio,const size_t& primer_guion) const{
    if(inicio == (size_t)primer_guion){
        //return "";
        return string::npos;
    }

    size_t i = primer_guion + 1;
    size_t str_len = str.length();
    size_t ultimo_guion = primer_guion;

    for(; i < str_len; i++){
        //unsigned char c = (unsigned char)str[i];
        
        if (str[i] == '-'){
            if (i == ultimo_guion + 1){
                i--;
                break;
            }
            ultimo_guion = i;
            continue;
        }

        if(this->delimitadores[str[i]]){
            if (i == ultimo_guion + 1){
                i--;
                break;
            }
            break;
        }
    }

    //return str.substr(inicio, i - inicio);
    return i;
}

void Tokenizador::Tokenizar (const string& str, list<string>& tokens) const{
    /*
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
    */

    tokens.clear();
    if (str.empty()){
        return;
    }

    string str_analisis;
    str_analisis.reserve(str.size() + 1); // Pre-reservamos memoria para evitar copias
    str_analisis = str;
    if(this->pasarAminuscSinAcentos){
        this->minuscSinAcentos(str_analisis);
    } 
    str_analisis.push_back('\n'); // Añadimos el salto de línea al final (instantáneo)

    size_t inicio = 0;
    size_t longitud = str_analisis.size();
    bool caracter_plus = false;

    const char* data = str_analisis.data();

    //cout << str_analisis<<endl;

    for(size_t i = 0; i < longitud; ++i){
        //unsigned char c = (unsigned char)str_analisis[i];
        size_t token = string::npos;
        

        if (this->delimitadores[(unsigned char)data[i]]){

            //cout << "DELIMITADOR" <<data[i]<<endl;

            if (this->CasosEspeciales()){

                if (this->DELIMITADOR_URL[data[i]]){
                    //cout << "URL" <<endl;
                    token = tratar_url(str_analisis, inicio);
                }

                if (token == string::npos && this->DELIMITADOR_DECIMAL[data[i]]){
                    //cout << "DECIMAL" <<endl;
                    token = tratar_decimal(str_analisis, inicio, i);
                    if (token != string::npos && this->DELIMITADOR_DECIMAL[data[inicio]]){
                        //token = '0' + token;
                        caracter_plus = true;
                    }
                }

                if (token == string::npos && data[i] == '@'){
                    //cout << "EMAIL" <<endl;
                    token = tratar_email(str_analisis, inicio, i);
                }

                if (token == string::npos && data[i] == '.'){
                    //cout << "ACRONIMO" <<endl;
                    token = tratar_acronimo(str_analisis, inicio, i);
                }

                if (token == string::npos && data[i] == '-'){
                    //cout << "MUTIPALABRA" <<endl;
                    token = tratar_multipalabra(str_analisis, inicio, i);
                }

                if (token == string::npos){
                    /*cout << "NO CASO" <<endl;
                    if (inicio == i){
                        cout << "AAAAAA: "<< str.substr(inicio, str.size() - inicio)<<endl;
                        inicio ++;
                        continue;
                    }
                    //cout << str_analisis.substr(inicio, i - inicio)<<endl;
                    //token = str_analisis.substr(inicio, i - inicio); 
                    tokens.emplace_back(str_analisis.data() + inicio, i - inicio);
                    cout << "PUSH: "<< str.substr(inicio, str.size() - inicio)<<endl;
                    //i= token;
                    inicio = i + 1;
                    continue;*/
                    if (i > inicio) {
                        tokens.emplace_back(str_analisis.data() + inicio, i - inicio);
                    }
                    inicio = i + 1;
                    continue;
                }
                else{
                    
                    if (caracter_plus){
                        //i = inicio + token - 1;
                        string carater_plus_c = "0";
                        tokens.emplace_back(carater_plus_c.append(str_analisis.data() + inicio, token - inicio));
                        caracter_plus = false;
                    }
                    else{
                        //i = inicio + token.size();
                        tokens.emplace_back(str_analisis.data() + inicio, token - inicio);
                    }
                    i= token;
                    inicio = i + 1;
                    continue;
                }
            }
            else{
                if (inicio == i){
                    inicio += 1;
                    continue;
                }
                //token = str_analisis.substr(inicio, i - inicio); 
                tokens.emplace_back(str_analisis.data() + inicio, i - inicio);
                //i= inicio + token;
                inicio = i + 1;
                continue;
            }
        }
    }

    /*for(const auto& token : tokens){
        cout<<token<<endl;
    }
    cout<<endl;*/

    /*for(size_t i = 0; i < str_analisis.size(); i++){
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
    */
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
            // 3. Tokenizamos la l?nea actual. Seg?n el enunciado, esto vac?a la lista 'tokens' previamente
            Tokenizar(cadena, tokens);
            
            // 4. Escribimos los tokens inmediatamente en el fichero de salida
            for (const string& token : tokens) {
                salida << token << '\n'; // 5. Uso de '\n' en lugar de endl
            }
        }
    }

    // Los destructores de ifstream y ofstream cierran los ficheros autom?ticamente
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
    bool todoCorrecto = true; // Se volver? false si falla la apertura de la lista o alg?n archivo
    struct stat info_archivo; // Estructura para almacenar informaci?n de la ruta

    // Leemos el archivo l?nea a l?nea
    while (getline(lista_ficheros, archivo_actual)) {
        // Optimizaci?n/Robustez: Limpiar posible '\r' si el archivo txt fue creado en Windows
        if (!archivo_actual.empty() && archivo_actual.back() == '\r') {
            archivo_actual.pop_back();
        }

        // Saltamos l?neas vac?as para evitar falsos errores
        if (archivo_actual.empty()) {
            continue;
        }

        // stat() devuelve 0 si tiene ?xito obteniendo la info del archivo
        if (stat(archivo_actual.c_str(), &info_archivo) != 0) {
            cerr << "ERROR: El archivo listado no existe o no es accesible: " << archivo_actual << '\n';
            todoCorrecto = false;
            continue; // NO interrumpimos la ejecuci?n, pasamos al siguiente
        }

        // Comprobamos si la ruta pertenece a un directorio utilizando la macro S_ISDIR
        if (S_ISDIR(info_archivo.st_mode)) {
            cerr << "ERROR: La ruta es un directorio, no un archivo: " << archivo_actual << '\n';
            todoCorrecto = false;
            continue; // Pasamos al siguiente
        }

        // Generamos el nombre del archivo de salida a?adiendo ".tk"
        string archivo_salida = archivo_actual + ".tk";

        // Reutilizamos el m?todo de tokenizar archivos que ya estaba optimizado.
        // Si este devuelve false, marcamos todoCorrecto a false, pero seguimos procesando.
        if (!Tokenizar(archivo_actual, archivo_salida)) {
            todoCorrecto = false;
        }
    }

    // El destructor de ifstream cierra la lista autom?ticamente
    return todoCorrecto;
}

bool Tokenizador::TokenizarDirectorio (const string& i) const{
    // Tokeniza todos los archivos que contenga el directorio i, incluyendo los de los subdirectorios, guardando la salida en ficheros cuyo nombre ser? el de entrada a?adi?ndole extensi?n .tk, y que contendr? una palabra en cada l?nea del fichero. Devolver? true si se realiza la tokenizaci?n de forma correcta de todos los archivos; devolver? false en caso contrario enviando a cerr el mensaje correspondiente (p.ej. que no exista el directorio i, o los ficheros que no se hayan podido tokenizar)
}

void Tokenizador::DelimitadoresPalabra(const string& nuevoDelimiters){
    /*
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
    */
    
    /*
    // 1. Evitamos reasignaciones de memoria dinámicas
    this->delimiters = "";
    this->delimiters.reserve(nuevoDelimiters.size()); // Pre-reservamos el string de salida
    
    this->dicc_delimitadores.clear();
    this->dicc_delimitadores.reserve(nuevoDelimiters.size() + 3);

    // 2. Quitamos el if(.empty()) inútil y añadimos los obligatorios
    this->dicc_delimitadores.insert('\n');
    this->dicc_delimitadores.insert('\r');

    if (this->casosEspeciales) {
        this->dicc_delimitadores.insert(' ');
    }

    // 4. Bucle de una sola pasada (O(N))
    for (char letra : nuevoDelimiters) {
        // insert() inserta la letra. ".second" será 'true' si es una letra nueva, 
        // o 'false' si ya estaba en el diccionario (como \n, \r, o repetidas)
        bool insertado_nuevo = this->dicc_delimitadores.insert(letra).second;

        if (insertado_nuevo) {
            // Si es totalmente nueva, la añadimos al string visual de delimitadores
            this->delimiters += letra;
        } 
        else if (letra == ' ' && this->delimiters.find(' ') == string::npos) {
            // Si casosEspeciales es true, no se habrá añadido la letra, pero esta es necesaria que sea añadida
            this->delimiters += ' ';
        }
    }

    */
    std::fill(std::begin(this->delimitadores), std::end(this->delimitadores), false); //Todos los valores del array a false

    this->delimiters = "";
    this->delimiters.reserve(nuevoDelimiters.size()); // Pre-reservamos el string de salida

    this->delimitadores[(unsigned char)'\n'] = true;
    this->delimitadores[(unsigned char)'\r'] = true;

    if (this->casosEspeciales) {
        this->delimitadores[(unsigned char)' '] = true;
    }

    for (unsigned char letra : nuevoDelimiters) {
        // Hacemos cast a unsigned char para evitar índices negativos con caracteres raros/acentos
        if (!this->delimitadores[(unsigned char)letra]){
            this->delimitadores[(unsigned char)letra] = true;
            this->delimiters += letra;
        }
        else if (letra == ' ' && this->delimiters.find(' ') == string::npos){
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
    /*if (!nuevoCasosEspeciales && this->delimiters.find(" ") == string::npos){
        this->dicc_delimitadores.erase(' ');
    }
    if (nuevoCasosEspeciales){
        this->dicc_delimitadores.insert(' ');
    }
    this->casosEspeciales = nuevoCasosEspeciales;
    */
    this->casosEspeciales = nuevoCasosEspeciales;

    if (nuevoCasosEspeciales) {
        // Si activamos casos especiales, el espacio SIEMPRE es delimitador
        this->delimitadores[(unsigned char)' '] = true;
    } 
    else {
        // Si desactivamos, el espacio solo es delimitador si está en la cadena 'delimiters'
        if (this->delimiters.find(' ') == string::npos) {
            this->delimitadores[(unsigned char)' '] = false;
        }
    }
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