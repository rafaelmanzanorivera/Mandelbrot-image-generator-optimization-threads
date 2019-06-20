#include "ListaEnlazada.h"
#include "cassert"
using namespace std;

Nodo * ListaEnlazada::getNodo (int posicion) {
	assert(n > 0);
	assert(posicion >= 0 && posicion <= n-1);

	Nodo *resultado = NULL;
	resultado = lista;

	for(int i = 0; i < posicion; i++)
	{
        resultado = resultado->siguienteNodo;
	}
	
	return (resultado);
}

ListaEnlazada::ListaEnlazada() {
	lista = NULL; 
	n=0;
	
}

work_batch* ListaEnlazada::getValue(int posicion) {
    if (n <= 0) return NULL;
    
	assert (posicion >= 0 && posicion <= n-1);
	
	Nodo *nodoResultado = getNodo (posicion);
    
	return &(nodoResultado->elemento);
}


int ListaEnlazada::getN() {
	return (n);
}

void ListaEnlazada::insert (int posicion, work_batch nuevoValor) {
	assert (posicion >= 0 && posicion <= n);

	// Creamos el nuevo nodo
	Nodo *nuevoNodo = new Nodo;
	nuevoNodo->elemento = nuevoValor;

	// Lista vacia
	if (n==0)
    {
        lista = nuevoNodo;
        nuevoNodo->siguienteNodo = NULL;

    }
    
	// Si la lista no esta vacía
	else {
		
        if(posicion == 0)
        {
            nuevoNodo->siguienteNodo = lista;
            lista = nuevoNodo;
        }
        else
        {
            Nodo *nodoSiguiente;
            Nodo *nodoAnterior;
            
            nodoAnterior = getNodo(posicion-1);
            nodoSiguiente = nodoAnterior->siguienteNodo;
            
            nodoAnterior->siguienteNodo = nuevoNodo;
            nuevoNodo->siguienteNodo = nodoSiguiente;
        }
		
	} 
	n++;

}


void ListaEnlazada::remove (int posicion) {
	//assert(n > 0);
	//assert (posicion >= 0 && posicion <= n-1);

    if(n <= 0)return;
    
    Nodo *nodoAEliminar = NULL;;

	if (n == 1)
    {
        nodoAEliminar = getNodo(posicion);
        lista = NULL;
    }
    
	else {
        
        if(posicion == 0)
        {
            Nodo *nodoSiguiente = lista->siguienteNodo;
            nodoAEliminar = lista;
            lista = nodoSiguiente;
        }
	
        else
        {
            Nodo *nodoAnterior = getNodo(posicion-1);
            Nodo *nodoAEliminar = nodoAnterior->siguienteNodo;
            
            nodoAnterior->siguienteNodo=nodoAEliminar->siguienteNodo;
        }
		
	}
    
	delete(nodoAEliminar);

	n--;

}


ListaEnlazada::~ListaEnlazada() {
	while (n>0) remove(0);
}



