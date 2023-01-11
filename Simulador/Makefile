testeIndividual:
	g++ -o simulador.o simulador.cpp
	./simulador.o 101 1 1 0.5 1 1 0 > ../Simulacoes/teste_individual.csv

testePrioridade:
	g++ -o simulador.o simulador.cpp
	./simulador.o 101 1 7 0.5 3 1 0 > ../Simulacoes/teste_prioridade.csv

testeRetry:
	g++ -o simulador.o simulador.cpp
	./simulador.o 101 100000 100000 0.5 3 1 0 > ../Simulacoes/teste_retry.csv

testeLongo:
	g++ -o simulador.o simulador.cpp
	./simulador.o 2 42 42 0.5 10000 10 0

testeLongoGrafico:
	g++ -o simulador.o simulador.cpp
	./simulador.o 101 42 42 0.5 10000 10 0 > ../Simulacoes/teste_longo.csv

testeDebug:
	g++ -o simulador.o simulador.cpp
	./simulador.o 2 42 42 0.5 10000 100 -1

testeDebugPequeno:
	g++ -o simulador.o simulador.cpp
	./simulador.o 3 42 42 0.4 5 3 -1

testeDebug_04_15000_5000:
	g++ -o simulador.o simulador.cpp
	./simulador.o 2 42 42 0.4 15000 3 1000

testeDebug_04_15000_5000_graphic:
	g++ -o simulador.o simulador.cpp
	./simulador.o 101 42 42 0.4 5000 3 -1 > ../Simulacoes/debug_04_15000_5000.csv

testeDebug_05_1000000_10000:
	g++ -o simulador.o simulador.cpp
	./simulador.o 100 42 42 0.5 10000 100 -1

testeAleatorio_05_1000_10_1000:
	g++ -o simulador.o simulador.cpp
	./simulador.o 100 -1 -1 0.5 1000 10 1000

testeAleatorio_05_10000_10_1000:
	g++ -o simulador.o simulador.cpp
	./simulador.o 100 -1 -1 0.5 10000 10 1000

testeDebugAleatorio:
	g++ -o simulador.o simulador.cpp
	./simulador.o 101 -1 -1 0.5 10 5 -1 > ../Simulacoes/debug_aleatorio.csv

validacaoMetricasTransiente:
	g++ -o simulador.o simulador.cpp
	./simulador.o 100 -1 -1 0.5 10 10000 0

validacaoOtimizacaoMelhor:
	g++ -o simulador.o simulador.cpp
	./simulador.o 100 -1 -1 0.6 100 1000 0

validacaoOtimizacao:
	g++ -o simulador_otimizacao.o simulador_otimizacao.cpp
	./simulador_otimizacao.o > ../Simulacoes/Otimizacao/otimizacao.csv

validacaoDeterministica:
	g++ -o simulador_deterministico.o simulador_deterministico.cpp
	./simulador_deterministico.o

validacaoDeterministicaSave:
	g++ -o simulador_deterministico.o simulador_deterministico.cpp
	./simulador_deterministico.o > ../Simulacoes/validacao_deterministica.csv

validacaoFaseTransiente:
	g++ -o simulador_validacao_transiente.o simulador_validacao_transiente.cpp
	./simulador_validacao_transiente.o

validacaoGeradorExponencial:
	g++ -o validacao_gerador_exponencial.o validacao_gerador_exponencial.cpp
	./validacao_gerador_exponencial.o

validacaoHandlerEstatisticas:
	g++ -o validacao_handler_estatisticas.o validacao_handler_estatisticas.cpp
	./validacao_handler_estatisticas.o

simulacao1a:
	g++ -o simulador.o simulador.cpp
	./simulador.o 100 42 42 0.2 100 100 -1

simulacao1b:
	g++ -o simulador.o simulador.cpp
	./simulador.o 100 42 42 0.2 500 100 -1

simulacao1c:
	g++ -o simulador.o simulador.cpp
	./simulador.o 100 42 42 0.2 600 100 -1

simulacao3a:
	g++ -o simulador.o simulador.cpp
	./simulador.o 100 42 42 0.2 1000 100 -1