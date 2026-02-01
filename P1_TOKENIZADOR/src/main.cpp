#include <iostream>
#include "../include/tokenizador.h"
#include <chrono>

using namespace std;

int main(){
    auto inicio = std::chrono::steady_clock::now();
    string delimiter = "@/@:.__";
    Tokenizador a(delimiter,true,"hola muy buenas");
    auto fin = std::chrono::steady_clock::now();

    auto duracion = std::chrono::duration_cast<std::chrono::microseconds>(fin - inicio);

    std::cout << "La funcion tardo: " << duracion.count() << " microsegundos." << std::endl;
}