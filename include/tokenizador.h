#ifndef TOKENIZADOR_H
#define TOKENIZADOR_H

#include <string>
#include <list>
#include <ostream>
#include <unordered_map>
#include <unordered_set>

using namespace std;

enum casoEspecial{
    URL,
    DECIMAL,
    EMAIL,
    ACRONIMO,
    MULTIPALABRA,
    NINGUNO
    };

class Tokenizador {
    friend ostream& operator<<(ostream&, const Tokenizador&);	 
    // cout << "DELIMITADORES: " << delimiters << " TRATA CASOS ESPECIALES: " << casosEspeciales << " PASAR A MINUSCULAS Y SIN ACENTOS: " << pasarAminuscSinAcentos;
    // Aunque se modifique el almacenamiento de los delimitadores por temas de eficiencia, el campo delimiters se imprimirá con el string leído en el tokenizador (tras las modificaciones y eliminación de los caracteres repetidos correspondientes)

    public:
        Tokenizador (const string& delimitadoresPalabra, const bool& kcasosEspeciales, const bool& minuscSinAcentos);	
        // Inicializa delimiters a delimitadoresPalabra filtrando que no se introduzcan delimitadores repetidos (de izquierda a derecha, en cuyo caso se eliminarían los que hayan sido repetidos por la derecha); casosEspeciales a kcasosEspeciales; pasarAminuscSinAcentos a minuscSinAcentos

        Tokenizador (const Tokenizador&);

        Tokenizador ();	
        // Inicializa delimiters=",;:.-/+*\\ '\"{}[]()<>¡!¿?&#=\t@"; casosEspeciales a true; pasarAminuscSinAcentos a false

        ~Tokenizador ();	// Pone delimiters=""

        Tokenizador& operator= (const Tokenizador&);

        void Tokenizar (const string& str, list<string>& tokens) const;
        // Tokeniza str devolviendo el resultado en tokens. La lista tokens se vaciará antes de almacenar el resultado de la tokenización. 

        bool Tokenizar (const string& i, const string& f) const; 
        // Tokeniza el fichero i guardando la salida en el fichero f (una palabra en cada línea del fichero). Devolverá true si se realiza la tokenización de forma correcta; false en caso contrario enviando a cerr el mensaje correspondiente (p.ej. que no exista el archivo i)

        bool Tokenizar (const string & i) const;
        // Tokeniza el fichero i guardando la salida en un fichero de nombre i añadiéndole extensión .tk (sin eliminar previamente la extensión de i por ejemplo, del archivo pp.txt se generaría el resultado en pp.txt.tk), y que contendrá una palabra en cada línea del fichero. Devolverá true si se realiza la tokenización de forma correcta; false en caso contrario enviando a cerr el mensaje correspondiente (p.ej. que no exista el archivo i)

        bool TokenizarListaFicheros (const string& i) const; 
        // Tokeniza el fichero i que contiene un nombre de fichero por línea guardando la salida en ficheros (uno por cada línea de i) cuyo nombre será el leído en i añadiéndole extensión .tk, y que contendrá una palabra en cada línea del fichero leído en i. Devolverá true si se realiza la tokenización de forma correcta de todos los archivos que contiene i; devolverá false en caso contrario enviando a cerr el mensaje correspondiente (p.ej. que no exista el archivo i, o que se trate de un directorio, enviando a "cerr" los archivos de i que no existan o que sean directorios; luego no se ha de interrumpir la ejecución si hay algún archivo en i que no exista)

        bool TokenizarDirectorio (const string& i) const; 
        // Tokeniza todos los archivos que contenga el directorio i, incluyendo los de los subdirectorios, guardando la salida en ficheros cuyo nombre será el de entrada añadiéndole extensión .tk, y que contendrá una palabra en cada línea del fichero. Devolverá true si se realiza la tokenización de forma correcta de todos los archivos; devolverá false en caso contrario enviando a cerr el mensaje correspondiente (p.ej. que no exista el directorio i, o los ficheros que no se hayan podido tokenizar)

        void DelimitadoresPalabra(const string& nuevoDelimiters); 
        // Inicializa delimiters a nuevoDelimiters, filtrando que no se introduzcan delimitadores repetidos (de izquierda a derecha, en cuyo caso se eliminarían los que hayan sido repetidos por la derecha)

        void AnyadirDelimitadoresPalabra(const string& nuevoDelimiters); // 
        // Añade al final de "delimiters" los nuevos delimitadores que aparezcan en "nuevoDelimiters" (no se almacenarán caracteres repetidos)

        string DelimitadoresPalabra() const; 
        // Devuelve "delimiters" 

        void CasosEspeciales (const bool& nuevoCasosEspeciales);
        // Cambia la variable privada "casosEspeciales" 

        bool CasosEspeciales () const;
        // Devuelve el contenido de la variable privada "casosEspeciales" 

        void PasarAminuscSinAcentos (const bool& nuevoPasarAminuscSinAcentos);
        // Cambia la variable privada "pasarAminuscSinAcentos". Atención al formato de codificación del corpus (comando "file" de Linux). Para la corrección de la práctica se utilizará el formato actual (ISO-8859). 

        bool PasarAminuscSinAcentos () const;
        // Devuelve el contenido de la variable privada "pasarAminuscSinAcentos"



    private:
        string delimiters;		// Delimitadores de términos. Aunque se modifique la forma de almacenamiento interna para mejorar la eficiencia, este campo debe permanecer para indicar el orden en que se introdujeron los delimitadores

        bool casosEspeciales;
        // Si true detectará palabras compuestas y casos especiales. Sino, trabajará al igual que el algoritmo propuesto en la sección "Versión del tokenizador vista en clase"

        bool pasarAminuscSinAcentos;
        // Si true pasará el token a minúsculas y quitará acentos, antes de realizar la tokenización

        //unordered_map<char,int> dicc_delimitadores; // Almacena los delimitadores para mejorar la eficiencia de la tokenización. El valor del map no se utilizará, solo se usará para comprobar si un caracter es delimitador o no (si el caracter no existe en el map, no es delimitador; si existe, es delimitador)

        unordered_set<char> dicc_delimitadores; // Almacena los delimitadores para mejorar la eficiencia de la tokenización. El valor del set no se utilizará, solo se usará para comprobar si un caracter es delimitador o no (si el caracter no existe en el set, no es delimitador; si existe, es delimitador)

        //unordered_set<char> dicc_noEspeciales; // Almacena los caracteres que no son casos especiales para mejorar la eficiencia de la tokenización. El valor del set no se utilizará, solo se usará para comprobar si un caracter es caso especial o no (si el caracter no existe en el set, es caso especial; si existe, no es caso especial)

        //enum casoEspecial casosEspecialesDetectados; // Almacena los casos especiales detectados en la tokenización. Se utilizará para mejorar la eficiencia de la tokenización, ya que si no se han detectado casos especiales, no se comprobará si un token es caso especial o no, sino que se considerará que no lo es (si el token contiene un caracter que no es delimitador ni caso especial, se considerará que el token es un caso especial)

        string analisisCasosEspeciales (const string& str) const; // Analiza la cadena de entrada para detectar los casos especiales que contiene y almacenarlos en casosEspecialesDetectados. Se utilizará para mejorar la eficiencia de la tokenización, ya que si no se han detectado casos especiales, no se comprobará si un token es caso especial o no, sino que se considerará que no lo es (si el token contiene un caracter que no es delimitador ni caso especial, se considerará que el token es un caso especial)

        string tratar_URL(const string& token) const; // Trata el caso especial de las URLs, eliminando los delimitadores que pueda contener y dejando solo la URL limpia como token. Devuelve 0 si se ha tratado correctamente el caso especial, -1 si no se ha detectado el caso especial (el token no es una URL) y -2 si se ha detectado el caso especial pero no se ha podido tratar correctamente (por ejemplo, porque la URL no tiene un formato correcto)
    
        string minuscSinAcentos(const string& str) const; // Devuelve la cadena de entrada pasada a minúsculas y sin acentos. Atención al formato de codificación del corpus (comando "file" de Linux). Para la corrección de la práctica se utilizará el formato actual (ISO-8859).

        string tratar_DECIMAL(const string& token) const; // Trata el caso especial de los números decimales, eliminando los delimitadores que pueda contener y dejando solo el número decimal limpio como token. Devuelve 0 si se ha tratado correctamente el caso especial, -1 si no se ha detectado el caso especial (el token no es un número decimal) y -2 si se ha detectado el caso especial pero no se ha podido tratar correctamente (por ejemplo, porque el número decimal no tiene un formato correcto)
    };

#endif
