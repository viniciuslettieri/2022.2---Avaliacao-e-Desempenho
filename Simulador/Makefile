testeIndividual:
	g++ -o simulador.o simulador.cpp
	./simulador.o 1 0 1 1 0.5 1 > ../Simulacoes/teste_individual.csv

testePrioridade:
	g++ -o simulador.o simulador.cpp
	./simulador.o 3 0 1 7 0.5 100 > ../Simulacoes/teste_prioridade.csv

testeRetry:
	g++ -o simulador.o simulador.cpp
	./simulador.o 3 0 100000 100000 0.5 100 > ../Simulacoes/teste_retry.csv

testeLongo:
	g++ -o simulador.o simulador.cpp
	./simulador.o 100000 0 1 1 0.5 10 > ../Simulacoes/teste_longo.csv

testeDebug:
	g++ -o simulador.o simulador.cpp
	./simulador.o 1000000 2 100000 100000 0.5 10000

testeDebug_04_15000_5000:
	g++ -o simulador.o simulador.cpp
	./simulador.o 15000 2 42 42 0.4 5000

testeDebug_04_15000_5000_graphic:
	g++ -o simulador.o simulador.cpp
	./simulador.o 15000 0 42 42 0.4 5000 > ../Simulacoes/debug_04_15000_5000.csv

testeDebugAleatorio:
	g++ -o simulador.o simulador.cpp
	./simulador.o 50 0 -1 -1 0.5 10 > ../Simulacoes/debug_aleatorio.csv