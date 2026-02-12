#include <iostream>  
#include <string> 
#include <list>  
#include <sys/resource.h> 
#include "../include/tokenizador.h" 
 
using namespace std; 
 
double getcputime(void) {  
struct timeval tim;         
struct rusage ru;         
getrusage(RUSAGE_SELF, &ru);         
tim=ru.ru_utime;         
double t=(double)tim.tv_sec + (double)tim.tv_usec / 1000000.0;         
tim=ru.ru_stime;         
t+=(double)tim.tv_sec + (double)tim.tv_usec / 1000000.0;         
return t;  
} 
 
int main() { 
long double aa=getcputime(); 
 
Tokenizador a("/&.", true, true); 
string frase = "1.2 u1.2a p0 Http://intime.dlsi.ua.es:8080/dossierct/index.jsp?lang=es&status=probable&date=22-01-2013&newspaper=catedraTelefonicaUA@iuii.ua.es p1 p2";
list<string> tokens;
a.Tokenizar(frase, tokens);
for (const string& token : tokens) {
    cout << token << endl;
}
//a.TokenizarListaFicheros("listaFicheros.txt");  // TODO EL CORPUS 
 
cout << "Ha tardado " << getcputime() - aa << " segundos" << endl; 
return 0;
} 
 
