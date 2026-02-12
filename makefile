.PHONY: clean 

CC=g++ 
OPTIONS= -g
# Añade aquí la bandera de AddressSanitizer si quieres probarlo:
# OPTIONS= -g -fsanitize=address 

LIBDIR=lib
INCLUDEDIR=include
SRC=src

_OBJ= tokenizador.o 
OBJ = $(patsubst %,$(LIBDIR)/%,$(_OBJ)) 

all: practica1 

# Regla para el ejecutable
practica1: $(SRC)/main.cpp $(OBJ) 
	$(CC) $(OPTIONS) -I$(INCLUDEDIR) $(SRC)/main.cpp $(OBJ) -o practica1 

# Regla para compilar el objeto tokenizador.o
# Cambiamos $(LIBDIR)/%.cpp por $(SRC)/%.cpp
$(LIBDIR)/%.o : $(SRC)/%.cpp $(INCLUDEDIR)/%.h 
	$(CC) $(OPTIONS) -c -I$(INCLUDEDIR) -o $@ $< 

clean: 
	rm -f $(OBJ) practica1