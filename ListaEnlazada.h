#ifndef ListaEnlazada_h
#define ListaEnlazada_h

#include "mandelbrotCalculators.h"

struct Nodo
{
    work_batch elemento; // El elemento que guarda el nodo
    Nodo *siguienteNodo; // Puntero al siguiente nodo de la lista enlazada
};

class ListaEnlazada
{
	Nodo *lista;

	int n;

	Nodo * getNodo (int posicion);

public:

	ListaEnlazada();

	work_batch* getValue(int posicion);

	int getN(); 

	void insert (int posicion, work_batch nuevoValor);

	void remove (int posicion);

	~ListaEnlazada();
};



#endif





