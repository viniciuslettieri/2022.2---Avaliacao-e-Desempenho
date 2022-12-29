testeIndividual:
	g++ -o simulador.o simulador.cpp
	./simulador.o 0 1 1 0.5 1 1 > ../Simulacoes/teste_individual.csv

testePrioridade:
	g++ -o simulador.o simulador.cpp
	./simulador.o 0 1 7 0.5 3 1 > ../Simulacoes/teste_prioridade.csv

testeRetry:
	g++ -o simulador.o simulador.cpp
	./simulador.o 0 100000 100000 0.5 3 1 > ../Simulacoes/teste_retry.csv

testeLongo:
	g++ -o simulador.o simulador.cpp
	./simulador.o 0 1 1 0.5 10000 10 > ../Simulacoes/teste_longo.csv

testeDebug:
	g++ -o simulador.o simulador.cpp
	./simulador.o 2 42 42 0.5 10000 100

testeDebugPequeno:
	g++ -o simulador.o simulador.cpp
	./simulador.o 3 42 42 0.4 5 3

testeDebug_04_15000_5000:
	g++ -o simulador.o simulador.cpp
	./simulador.o 1 42 42 0.4 15000 3

testeDebug_04_15000_5000_graphic:
	g++ -o simulador.o simulador.cpp
	./simulador.o 0 42 42 0.4 5000 3 > ../Simulacoes/debug_04_15000_5000.csv

testeDebug_05_1000000_10000:
	g++ -o simulador.o simulador.cpp
	./simulador.o 1 42 42 0.5 10000 100

testeDebugAleatorio:
	g++ -o simulador.o simulador.cpp
	./simulador.o 0 -1 -1 0.5 10 5 > ../Simulacoes/debug_aleatorio.csv