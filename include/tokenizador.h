#ifndef TOKENIZADOR_H
#define TOKENIZADOR_H

#include <string>
#include <list>
#include <ostream>
#include <unordered_map>
#include <unordered_set>

using namespace std;

class Tokenizador {

    static inline string URL_DELIMITERS="_:/.?&-=#@"; // Delimitadores especiales de URL que est�n permitidos dentro de la URL

    static inline string DECIMAL_DELIMITERS=".,";

    static inline char EMAIL_DELIMITER='@';

    friend ostream& operator<<(ostream&, const Tokenizador&);	 
    // cout << "DELIMITADORES: " << delimiters << " TRATA CASOS ESPECIALES: " << casosEspeciales << " PASAR A MINUSCULAS Y SIN ACENTOS: " << pasarAminuscSinAcentos;
    // Aunque se modifique el almacenamiento de los delimitadores por temas de eficiencia, el campo delimiters se imprimir? con el string le?do en el tokenizador (tras las modificaciones y eliminaci?n de los caracteres repetidos correspondientes)

    public:
        Tokenizador (const string& delimitadoresPalabra, const bool& kcasosEspeciales, const bool& minuscSinAcentos);	
        // Inicializa delimiters a delimitadoresPalabra filtrando que no se introduzcan delimitadores repetidos (de izquierda a derecha, en cuyo caso se eliminar?an los que hayan sido repetidos por la derecha); casosEspeciales a kcasosEspeciales; pasarAminuscSinAcentos a minuscSinAcentos

        Tokenizador (const Tokenizador&);

        Tokenizador ();	
        // Inicializa delimiters=",;:.-/+*\\ '\"{}[]()<>?!??&#=\t@"; casosEspeciales a true; pasarAminuscSinAcentos a false

        ~Tokenizador ();	// Pone delimiters=""

        Tokenizador& operator= (const Tokenizador&);

        void Tokenizar (const string& str, list<string>& tokens) const;
        // Tokeniza str devolviendo el resultado en tokens. La lista tokens se vaciar? antes de almacenar el resultado de la tokenizaci?n. 

        bool Tokenizar (const string& i, const string& f) const; 
        // Tokeniza el fichero i guardando la salida en el fichero f (una palabra en cada l?nea del fichero). Devolver? true si se realiza la tokenizaci?n de forma correcta; false en caso contrario enviando a cerr el mensaje correspondiente (p.ej. que no exista el archivo i)

        bool Tokenizar (const string & i) const;
        // Tokeniza el fichero i guardando la salida en un fichero de nombre i a?adi?ndole extensi?n .tk (sin eliminar previamente la extensi?n de i por ejemplo, del archivo pp.txt se generar?a el resultado en pp.txt.tk), y que contendr? una palabra en cada l?nea del fichero. Devolver? true si se realiza la tokenizaci?n de forma correcta; false en caso contrario enviando a cerr el mensaje correspondiente (p.ej. que no exista el archivo i)

        bool TokenizarListaFicheros (const string& i) const; 
        // Tokeniza el fichero i que contiene un nombre de fichero por l?nea guardando la salida en ficheros (uno por cada l?nea de i) cuyo nombre ser? el le?do en i a?adi?ndole extensi?n .tk, y que contendr? una palabra en cada l?nea del fichero le?do en i. Devolver? true si se realiza la tokenizaci?n de forma correcta de todos los archivos que contiene i; devolver? false en caso contrario enviando a cerr el mensaje correspondiente (p.ej. que no exista el archivo i, o que se trate de un directorio, enviando a "cerr" los archivos de i que no existan o que sean directorios; luego no se ha de interrumpir la ejecuci?n si hay alg?n archivo en i que no exista)

        bool TokenizarDirectorio (const string& i) const; 
        // Tokeniza todos los archivos que contenga el directorio i, incluyendo los de los subdirectorios, guardando la salida en ficheros cuyo nombre ser? el de entrada a?adi?ndole extensi?n .tk, y que contendr? una palabra en cada l?nea del fichero. Devolver? true si se realiza la tokenizaci?n de forma correcta de todos los archivos; devolver? false en caso contrario enviando a cerr el mensaje correspondiente (p.ej. que no exista el directorio i, o los ficheros que no se hayan podido tokenizar)

        void DelimitadoresPalabra(const string& nuevoDelimiters); 
        // Inicializa delimiters a nuevoDelimiters, filtrando que no se introduzcan delimitadores repetidos (de izquierda a derecha, en cuyo caso se eliminar?an los que hayan sido repetidos por la derecha)

        void AnyadirDelimitadoresPalabra(const string& nuevoDelimiters); // 
        // A?ade al final de "delimiters" los nuevos delimitadores que aparezcan en "nuevoDelimiters" (no se almacenar?n caracteres repetidos)

        string DelimitadoresPalabra() const; 
        // Devuelve "delimiters" 

        void CasosEspeciales (const bool& nuevoCasosEspeciales);
        // Cambia la variable privada "casosEspeciales" 

        bool CasosEspeciales () const;
        // Devuelve el contenido de la variable privada "casosEspeciales" 

        void PasarAminuscSinAcentos (const bool& nuevoPasarAminuscSinAcentos);
        // Cambia la variable privada "pasarAminuscSinAcentos". Atenci?n al formato de codificaci?n del corpus (comando "file" de Linux). Para la correcci?n de la pr?ctica se utilizar? el formato actual (ISO-8859). 

        bool PasarAminuscSinAcentos () const;
        // Devuelve el contenido de la variable privada "pasarAminuscSinAcentos"



    private:
        string delimiters;		// Delimitadores de t?rminos. Aunque se modifique la forma de almacenamiento interna para mejorar la eficiencia, este campo debe permanecer para indicar el orden en que se introdujeron los delimitadores

        bool casosEspeciales;
        // Si true detectar? palabras compuestas y casos especiales. Sino, trabajar? al igual que el algoritmo propuesto en la secci?n "Versi?n del tokenizador vista en clase"

        bool pasarAminuscSinAcentos;
        // Si true pasar? el token a min?sculas y quitar? acentos, antes de realizar la tokenizaci?n

        //unordered_map<char,int> dicc_delimitadores; // Almacena los delimitadores para mejorar la eficiencia de la tokenizaci?n. El valor del map no se utilizar?, solo se usar? para comprobar si un caracter es delimitador o no (si el caracter no existe en el map, no es delimitador; si existe, es delimitador)

        unordered_set<char> dicc_delimitadores; // Almacena los delimitadores para mejorar la eficiencia de la tokenizaci?n. El valor del set no se utilizar?, solo se usar? para comprobar si un caracter es delimitador o no (si el caracter no existe en el set, no es delimitador; si existe, es delimitador)

        //unordered_set<char> dicc_noEspeciales; // Almacena los caracteres que no son casos especiales para mejorar la eficiencia de la tokenizaci?n. El valor del set no se utilizar?, solo se usar? para comprobar si un caracter es caso especial o no (si el caracter no existe en el set, es caso especial; si existe, no es caso especial)

        //enum casoEspecial casosEspecialesDetectados; // Almacena los casos especiales detectados en la tokenizaci?n. Se utilizar? para mejorar la eficiencia de la tokenizaci?n, ya que si no se han detectado casos especiales, no se comprobar? si un token es caso especial o no, sino que se considerar? que no lo es (si el token contiene un caracter que no es delimitador ni caso especial, se considerar? que el token es un caso especial)

        string analisisCasosEspeciales (const string& str) const; // Analiza la cadena de entrada para detectar los casos especiales que contiene y almacenarlos en casosEspecialesDetectados. Se utilizar? para mejorar la eficiencia de la tokenizaci?n, ya que si no se han detectado casos especiales, no se comprobar? si un token es caso especial o no, sino que se considerar? que no lo es (si el token contiene un caracter que no es delimitador ni caso especial, se considerar? que el token es un caso especial)

        string tratar_URL(const string& token) const; // Trata el caso especial de las URLs, eliminando los delimitadores que pueda contener y dejando solo la URL limpia como token. Devuelve 0 si se ha tratado correctamente el caso especial, -1 si no se ha detectado el caso especial (el token no es una URL) y -2 si se ha detectado el caso especial pero no se ha podido tratar correctamente (por ejemplo, porque la URL no tiene un formato correcto)
    
        string minuscSinAcentos(const string& str) const; // Devuelve la cadena de entrada pasada a min?sculas y sin acentos. Atenci?n al formato de codificaci?n del corpus (comando "file" de Linux). Para la correcci?n de la pr?ctica se utilizar? el formato actual (ISO-8859).

        string tratar_DECIMAL(const string& token) const; // Trata el caso especial de los n?meros decimales, eliminando los delimitadores que pueda contener y dejando solo el n?mero decimal limpio como token. Devuelve 0 si se ha tratado correctamente el caso especial, -1 si no se ha detectado el caso especial (el token no es un n?mero decimal) y -2 si se ha detectado el caso especial pero no se ha podido tratar correctamente (por ejemplo, porque el n?mero decimal no tiene un formato correcto)
    
        string tratar_EMAIL(const string& token) const; // Trata el caso especial de los emails, eliminando los delimitadores que pueda contener y dejando solo el email limpio como token. Devuelve 0 si se ha tratado correctamente el caso especial, -1 si no se ha detectado el caso especial (el token no es un email) y -2 si se ha detectado el caso especial pero no se ha podido tratar correctamente (por ejemplo, porque el email no tiene un formato correcto)
    
        string tratar_ACRONIMO(const string& token) const; // Trata el caso especial de los acr?nimos, eliminando los delimitadores que pueda contener y dejando solo el acr?nimo limpio como token. Devuelve 0 si se ha tratado correctamente el caso especial, -1 si no se ha detectado el caso especial (el token no es un acr?nimo) y -2 si se ha detectado el caso especial pero no se ha podido tratar correctamente (por ejemplo, porque el acr?nimo no tiene un formato correcto)
    };

#endif
