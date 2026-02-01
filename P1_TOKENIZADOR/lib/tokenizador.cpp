#include <iostream>
#include <list>
#include <unordered_map>
#include <string>
#include "../include/tokenizador.h"

using namespace std;

ostream& operator<<(ostream&, const Tokenizador&){

}

Tokenizador::Tokenizador (const string& delimitadoresPalabra, const bool& kcasosEspeciales, const bool& minuscSinAcentos){	
    // Inicializa delimiters a delimitadoresPalabra filtrando que no se introduzcan delimitadores repetidos (de izquierda a derecha, en cuyo caso se eliminarían los que hayan sido repetidos por la derecha); casosEspeciales a kcasosEspeciales; pasarAminuscSinAcentos a minuscSinAcentos
    DelimitadoresPalabra(this->delimiters);
    this->casosEspeciales = kcasosEspeciales;
    this->pasarAminuscSinAcentos = ""; //Inicializar a "", a�adir valor posteriormente
    PasarAminuscSinAcentos(this->pasarAminuscSinAcentos);
}

Tokenizador::Tokenizador (const Tokenizador&){

}

Tokenizador::Tokenizador (){	
    // Inicializa delimiters=",;:.-/+*\\ '\"{}[]()<>¡!¿?&#=\t@"; casosEspeciales a true; pasarAminuscSinAcentos a false
}

Tokenizador::~Tokenizador (){
	// Pone delimiters=""
}

Tokenizador& Tokenizador::operator= (const Tokenizador&){

}

void Tokenizador::Tokenizar (const string& str, list<string>& tokens) const{
    // Tokeniza str devolviendo el resultado en tokens. La lista tokens se vaciará antes de almacenar el resultado de la tokenización. 
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
    unordered_map<char,int> dicc;

    for(char letra : nuevoDelimiters){
        if (!dicc.count(letra)){
            dicc[letra] = 1;
            this->pasarAminuscSinAcentos += letra; 
        }
    }
    */
    
    bool visto[256] = {false}; //rellena el array a valor False

    for(unsigned char letra : nuevoDelimiters){
        if (!visto[letra]){
            visto[letra]=true;
            this->pasarAminuscSinAcentos+=letra;
        }
    }
    
}

void Tokenizador::AnyadirDelimitadoresPalabra(const string& nuevoDelimiters){ 
    // Añade al final de "delimiters" los nuevos delimitadores que aparezcan en "nuevoDelimiters" (no se almacenarán caracteres repetidos)
}

string Tokenizador::DelimitadoresPalabra() const{
    // Devuelve "delimiters" 
}

void Tokenizador::CasosEspeciales (const bool& nuevoCasosEspeciales){
    // Cambia la variable privada "casosEspeciales" 
}

bool Tokenizador::CasosEspeciales (){
    // Devuelve el contenido de la variable privada "casosEspeciales" 
}

void Tokenizador::PasarAminuscSinAcentos (const bool& nuevoPasarAminuscSinAcentos){
    // Cambia la variable privada "pasarAminuscSinAcentos". Atención al formato de codificación del corpus (comando "file" de Linux). Para la corrección de la práctica se utilizará el formato actual (ISO-8859). 
}

bool Tokenizador::PasarAminuscSinAcentos (){
    // Devuelve el contenido de la variable privada "pasarAminuscSinAcentos"
}
