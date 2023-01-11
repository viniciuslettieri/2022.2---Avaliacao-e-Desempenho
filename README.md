# Simulador de Filas - Avaliação e Desempenho 2022.2

Este repositório é destinado ao trabalho final da disciplina de Avaliação e Desempenho

Seu objetivo é projetar um simulador de filas e avaliar sua execução para comprovar corretude e estudar o funcionamento. 

No sistema proposto, existem duas filas, tal que a primeira recebe chegadas exógenas por um Processo Poisson com taxa $\lambda$, e a segunda fila recebe as saídas da primeira execução de serviço. A prioridade é da primeira fila, que também possui interrupção sobre a segunda. A respeito dos clientes interrompidos no segundo serviço, continuam a partir de onde estavam, sem precisar reiniciar o serviço. Ambos os serviços são definidos por uma distribuição exponencial com taxa $\mu=1$. Ambas as filas são servidas por First Come First Serve.

## Uso do Simulador

O simulador todo foi feito em C++ e as análises de dados feitas em Python.

A execução padrão do simulador ocorre pelo arquivo `Simulador/simulador.cpp`, que possui os seguintes argumentos de linha de comando:

1. **modo debug**: int
2. **arrival seed**: int
3. **service seed**: int
4. **utilização (rho)**: double
5. **clientes/round**: int
6. **total de rounds**: int
7. **total de clientes transientes**: int

O modo de debug é importante para o output que é emitido, de acordo com nosso objetivo. Os valores do modo de debug podem ser encontrados em `Simulador/statistics.cpp`.

## Estrutura do Projeto

Existem duas grandes pastas:

### Simulador

Possui todos os arquivos para executar de fato o simulador em C++.

Arquivos de estrutura do simulador:

* client.cpp: implementa as estruturas do cliente, que é o objeto passado na fila.
* exponential_generator.cpp: implementa o gerador de numeros aleatorios em distribuicao exponencial.
* queue_system.cpp: implementa o sistema de filas, responsavel por manter cada um dos clientes em sua devida estrutura na fila e executar a lógica de quem deve executar antes.
* statistics.cpp: implementa o gerenciador das estatisticas, que realiza a consolidação das mesmas ao fim da execução. 

Arquivos de execução do simulador:

* simulador.cpp: nosso principal arquivo, que é o Main da simulação, em que os parâmetros podem ser passados para gerar uma simulação.
* simulador_deterministico.cpp: simulador auxiliar pra fazer testes no sistema de filas, relevante para o relatório.
* validacao_gerador_exponencial.cpp: simulador auxiliar para fazer testes de geracao de numeros aleatorios, relevante para o relatório.
* validacao_handler_estatisticas.cpp: simulador auxiliar para fazer testes do handler de estatisticas, relevante para o relatório.
* simulador_validacao_transiente.cpp: simulador auxiliar para verificar a quantidade minima de clientes transientes.
* simulador_otimizacao.cpp: simulador auxiliar para fazer testes e encontrar o fator minimo de quantidade de clientes e rounds que permitem uma precisao de IC menor que 5%.

Makefile: Possui alguns testes de execução que podem ser usados

### Simulacoes

Possui todos os testes de simulacoes e dados relacionados, para validações e gráficos.

No arquivo `analise_simulacoes.ipynb` fazemos algumas análises gráficas dos dados.