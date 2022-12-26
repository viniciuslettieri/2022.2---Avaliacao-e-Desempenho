#include <bits/stdc++.h> 
#include "declarations.h"
#include "exponential_generator.cpp"

using namespace std;

void validar_seed_aleatoria(int n) {
    printf("\n= Validacao de Geracao Seed Aleatoria =\n");
    printf("Os seguintes valores deverao ser seeds aleatorias, base da geracao de distribuicao.\nCaso os valores sejam iguais sua configuracao de compilador nao garante aleatoriedade dos valores.\n");

    for(int i=0; i<n; i++){
        std::random_device rd;
        printf("Seed Gerada: %u\n", rd());
    }
}

void validar_geracao_numeros(int n) {
    printf("\n= Validacao de Geracao Numeros Aleatorios =\n");

    std::random_device rd;
    std::mt19937 random_engine(rd());
    std::uniform_real_distribution<double> dist(1.0, 10.0);

    for(int i=0; i<n; i++){
        printf("Seed Gerada: %f\n", dist(random_engine));
    }
}

void validar_geracao_exponencial(int n, double lambda){
    ExponentialGenerator number_generator = ExponentialGenerator(lambda);
    vector<double> valores_gerados;
    
    for(int i=0; i<n; i++){
        double v = number_generator.get_random_value();
        valores_gerados.push_back(v);
    }

    double media = ((double) accumulate(valores_gerados.begin(), valores_gerados.end(), 0)) / valores_gerados.size();
    
    double media_esperada = 1.0 / lambda;

    double variancia = 0;
    for(auto v: valores_gerados) variancia += pow(v - media, 2);
    variancia /= n;

    double variancia_esperada = 1.0 / pow(lambda, 2);

    printf("\n= Validacao de Geracao Numeros Aleatorios =\n");
    printf("Quantidade de exemplos: %d\n", n);
    printf("Lambda utilizada: %.2lf\n", lambda);
    printf("Seed Utilizada: %u\n", number_generator.get_current_seed());
    printf("Media Real dos Valores: %.2lf\n", media);
    printf("Media Esperada dos Valores: %.2lf\n", media_esperada);
    printf("Variancia Real dos Valores: %.2lf\n", variancia);
    printf("Variancia Esperada dos Valores: %.2lf\n", variancia_esperada);
    printf("\n");
}

int main(){
    validar_seed_aleatoria(3);

    validar_geracao_numeros(3);
    validar_geracao_numeros(3);

    // validar_geracao_aleatoria(100, 1);
    // validar_geracao_aleatoria(100, 2);
    // validar_geracao_aleatoria(100, 5);
    // validar_geracao_aleatoria(1000, 1);
    // validar_geracao_aleatoria(1000, 2);
    // validar_geracao_aleatoria(1000, 5);

    return 0;
}