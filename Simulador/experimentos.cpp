#include <bits/stdc++.h> 
#include "declarations.h"
#include "exponential_generator.cpp"

using namespace std;

void execucao(){
	// se tiver alguem em servico 1
	// se tiver alguem em servico 2
	// se tiver alguem em fila 1
	// se tiver alguem em fila 2

	// atualiza_chegadas
}

void atualiza_chegadas(){
	// varre a fila de eventos de chegadas e adiciona todos ate o tempo atual
}

void adiciona_fila1(Evento evento){
	// atualiza o tempo do evento com o global
	// adiciona na estrutura de fila 1
}
void adiciona_fila2(Evento evento){
	// atualiza o tempo do evento com o global
	// adiciona na estrutura de fila 2
}
void readiciona_fila2(Evento evento){
	// adiciona na estrutura de fila
}
void adiciona_servico1(Evento evento){
	// atualiza o tempo do evento com o global
	// adiciona na estrutura de servico 1
}
void adiciona_servico2(Evento evento){
	// atualiza o tempo do evento com o global
	// adiciona na estrutura de servico 2
}

void remove_fila1(Evento evento){
	// atualiza o tempo do evento com o global
	// adiciona_fila2
}
void interrompe_fila2(Evento evento){
	// atualiza o tempo do evento com o global
	// readiciona_fila2
}
void remove_fila2(Evento evento){
	// atualiza o tempo do evento com o global
}

void realiza_servico1(Evento evento){
	// obtem tempo de servico
	// atualiza o tempo global
	// remove_fila1
}
void realiza_servico2(Evento evento){
	// obtem tempo de servico: novo ou diferenca do acumulado
	// atualiza o tempo global
	// se o tempo de finalizacao for maior que a chegada no topo da fila:
	// - interrompe_fila2
	// senao
	// - remove_fila2
}

int main(){


    return 0;
}