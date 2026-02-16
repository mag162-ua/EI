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

Tokenizador a("", true, false);
list<string> tokens;
 
a.DelimitadoresPalabra("@.&"); 
 
a.Tokenizar("catedraTelefonicaUA@iuii.ua.es p1 p2", tokens); 
// La lista de tokens a devolver debería contener: "catedraTelefonicaUA@iuii.ua.es, p1, p2" 
 
a.Tokenizar("catedraTelefonicaUA@@iuii.ua.es p1 p2", tokens); 

// La lista de tokens a devolver debería contener (el @@ hace que no se detecte como email, y para ?iuii.ua.es? lo detecta como acrónimo): "catedraTelefonicaUA, iuii.ua.es, p1, p2" 
 
a.Tokenizar("pal1 @iuii.ua.es p1 p2", tokens); 
// La lista de tokens a devolver debería contener (se ha quitado el @ porque es delimitador, no se ha detectado un email): "pal1, iuii.ua.es, p1, p2" 
 
a.Tokenizar("pal1 cat@iuii.ua.es@cd p1 p2", tokens); 
// La lista de tokens a devolver debería contener (se separa por el primer @ porque al detectar el segundo @ comprueba que no es un email, y al ser el @ un delimitador, detectaría un acrónimo en el segundo ya que entraría en ese caso por aparecer el punto, el cual finalizaría al encontrar el @ ya que es delimitador): "pal1, cat, iuii.ua.es, cd, p1, p2" 
 
a.Tokenizar("pal1 cat@iuii@cd.ua.es p1 p2", tokens); 
// La lista de tokens a devolver debería contener (se separa por el primer @ porque al detectar el segundo @ comprueba que no es un email, y al ser el @ un delimitador, detectaría un email en el segundo @): "pal1, cat, iuii@cd.ua.es, p1, p2" 
 
a.DelimitadoresPalabra("&."); 
a.Tokenizar("catedraTelefonicaUA@iuii.ua.es p1 p2", tokens); 
// La lista de tokens a devolver debería contener (no detecta e-mail sino acrónimo ya que el @ no es delimitador y el punto sí): "catedraTelefonicaUA@iuii.ua.es, p1, p2" 
 
a.Tokenizar("pal1 @iuii.ua.es p1 p2", tokens); 
// La lista de tokens a devolver debería contener (no se quita el @ porque no es delimitador, pero no se ha detectado un email, sino un acrónimo): "pal1, @iuii.ua.es, p1, p2" 
 
a.Tokenizar("pal1&@iuii.ua.es p1 p2", tokens); 
// La lista de tokens a devolver debería contener (separa por el & al ser delimitador; no detecta email ya que @ no es delimitador, lo mete en el token, el cual se detecta como acrónimo): "pal1, @iuii.ua.es, p1, p2" 
 
a.Tokenizar("pal1&catedra@iuii.ua.es p1 p2", tokens); 
// La lista de tokens a devolver debería contener (no lo detecta como email ya que el @ no es delimitador, sino como acrónimo): "pal1, catedra@iuii.ua.es, p1, p2" 
 
a.Tokenizar("pal1 cat@iuii.ua.es@cd p1 p2", tokens); 
// La lista de tokens a devolver debería contener (no lo detecta como email ya que el @ no es delimitador, sino como acrónimo): "pal1, cat@iuii.ua.es@cd, p1, p2" 
 
a.DelimitadoresPalabra("@.-_"); 
a.Tokenizar("-catedraTelefonicaUA@iuii.ua.es @p1 p2 ", tokens); 
// La lista de tokens a devolver debería contener: "catedraTelefonicaUA@iuii.ua.es, p1, p2" 
 
a.Tokenizar("@p2@@dot.com@p1-p2", tokens); 
// La lista de tokens a devolver debería contener (?dot.com? lo detecta como acrónimo y ?p1-p2? como multipalabra): "p2, dot.com, p1-p2" 
 
a.Tokenizar("a@a- b@- c@c--c d@d-- e@-e f@--", tokens); 
// La lista de tokens a devolver debería contener: "a@a, b, c@c, c, d@d, e, e, f"
 
cout << "Ha tardado " << getcputime() - aa << " segundos" << endl; 
return 0;
} 
 
