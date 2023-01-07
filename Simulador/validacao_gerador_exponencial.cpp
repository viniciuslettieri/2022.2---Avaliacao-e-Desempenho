#include <bits/stdc++.h> 
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

void validar_geracao_deterministica(int n, long double lambda, unsigned int seed) {
    printf("\n= Validacao de Geracao Numeros Deterministicamente =\n");

    std::mt19937 random_engine(seed);
    std::exponential_distribution<long double> dist(lambda);

    for(int i=0; i<n; i++){
        printf("%.3Lf ", dist(random_engine));
    }
    printf("\n");
}

void validar_geracao_numeros(int n, long double lambda) {
    printf("\n= Validacao de Geracao Numeros Aleatorios =\n");

    std::random_device rd;
    std::mt19937 random_engine(rd());
    std::exponential_distribution<long double> dist(lambda);

    for(int i=0; i<n; i++){
        printf("%.3Lf ", dist(random_engine));
    }
    printf("\n");
}

void validar_geracao_exponencial(int n, long double lambda){
    ExponentialGenerator number_generator = ExponentialGenerator(lambda);
    vector<long double> valores_gerados;
    
    for(int i=0; i<n; i++){
        long double v = number_generator.get_random_value();
        valores_gerados.push_back(v);
    }

    long double media = 0.0;
    for(auto v: valores_gerados) media += v;
    media /= valores_gerados.size();
    
    long double media_esperada = 1.0 / lambda;

    long double variancia = 0.0;
    for(auto v: valores_gerados) variancia += pow(v - media, 2);
    variancia /= valores_gerados.size();

    long double variancia_esperada = 1.0 / pow(lambda, 2);

    printf("\n= Validacao de Geracao Numeros Aleatorios =\n");
    printf("Quantidade de exemplos: %d\n", n);
    printf("Lambda utilizada: %.2Lf\n", lambda);
    printf("Seed Utilizada: %u\n", number_generator.get_current_seed());
    printf("Media Real dos Valores: %.2Lf\n", media);
    printf("Media Esperada dos Valores: %.2Lf\n", media_esperada);
    printf("Variancia Real dos Valores: %.2Lf\n", variancia);
    printf("Variancia Esperada dos Valores: %.2Lf\n", variancia_esperada);
}

int main(){
    validar_seed_aleatoria(5);

    validar_geracao_deterministica(5, 1.0, 42);
    validar_geracao_deterministica(5, 1.0, 42);

    validar_geracao_numeros(5, 1.0);
    validar_geracao_numeros(5, 1.0);
    validar_geracao_numeros(5, 1.0);

    validar_geracao_exponencial(1000, 1);
    validar_geracao_exponencial(1000, 2);
    validar_geracao_exponencial(1000, 5);
    validar_geracao_exponencial(100000, 1);
    validar_geracao_exponencial(100000, 2);
    validar_geracao_exponencial(100000, 5);

    return 0;
}