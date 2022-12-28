testeIndividual:
	g++ -o simulador.o simulador.cpp
	./simulador.o 1 0 1 1 0.5 > ../Simulacoes/teste_individual.csv

testePrioridade:
	g++ -o simulador.o simulador.cpp
	./simulador.o 3 0 1 7 0.5 > ../Simulacoes/teste_prioridade.csv

testeRetry:
	g++ -o simulador.o simulador.cpp
	./simulador.o 3 0 100000 100000 0.5 > ../Simulacoes/teste_retry.csv

testeLongo:
	g++ -o simulador.o simulador.cpp
	./simulador.o 100000 0 1 1 0.5 > ../Simulacoes/teste_longo.csv

testeDebug:
	g++ -o simulador.o simulador.cpp
	./simulador.o 5 1 100000 100000 0.5

testeDebugAleatorio:
	g++ -o simulador.o simulador.cpp
	./simulador.o 1000 1 -1 -1 0.5