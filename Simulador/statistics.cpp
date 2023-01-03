#include <vector>
#include <algorithm>
#include "client.cpp"

#define NO_DEBUG 0
#define DEBUG_STATS 1
#define DEBUG_IMPORTANT 2
#define DEBUG_ALL 3
#define DEBUG_FINAL_STATS 100


// Importante que os eventos com conflito de mesmo tempo tenham a ordem bem definida
// Relevante principalmente no caso de interrupcao e volta pra fila
enum EventType { 
    queue1_arrival, queue1_departure, service1_arrival, service1_departure, 
    service2_departure, queue2_arrival, queue2_departure, service2_arrival
};

template <class T>
T vector_avg(std::vector<T> &v) {
    return accumulate(v.begin(), v.end(), 0.0) / v.size();
}


class StatisticsHandler {

    private:

    std::vector<std::pair<long double, EventType>> event_list;        // time and type of event 
    std::vector<Client> client_list;
    std::vector<long double> round_start_times;
    long double system_start_time;
    int debug;

    public:

    std::vector<long double> rounds_W1, rounds_W2, rounds_T1, rounds_T2, rounds_X1, rounds_X2, rounds_Nq1, rounds_Nq2, rounds_N1, rounds_N2;
    long double AvgW1, AvgW2, AvgT1, AvgT2, AvgX1, AvgX2, AvgNq1, AvgNq2, AvgN1, AvgN2;
    long double VarW1, VarW2, VarT1, VarT2, VarX1, VarX2, VarNq1, VarNq2, VarN1, VarN2;
    long double DevW1, DevW2, DevT1, DevT2, DevX1, DevX2, DevNq1, DevNq2, DevN1, DevN2;
    long double IntConfW1, IntConfW2, IntConfT1, IntConfT2, IntConfX1, IntConfX2, IntConfNq1, IntConfNq2, IntConfN1, IntConfN2;
    long double PrecisionW1, PrecisionW2, PrecisionT1, PrecisionT2, PrecisionX1, PrecisionX2, PrecisionNq1, PrecisionNq2, PrecisionN1, PrecisionN2;

    StatisticsHandler(int debug = 0) {
        this->debug = debug;
        this->system_start_time = 0;
    }

    void add_event(long double event_time, EventType event_type) {
        this->event_list.push_back({event_time, event_type});
    }

    void add_client(Client client) {
        this->client_list.push_back(client);
    }

    void set_round_time(long double time) {
        this->round_start_times.push_back(time);
    }

    void set_system_start(long double time) {
        this->system_start_time = time;
    }

    void consolidate_quantity_metrics_by_round() {
        // Calcula Metricas de Quantidade [Nq1, Nq2, N1, N2]
        int Nq1 = 0, Nq2 = 0, N1 = 0, N2 = 0;                       // Variaveis de estado independente de round
        long double last_time = this->system_start_time;            // Tempo do ultimo evento
        long double AvgNq1 = 0, AvgNq2 = 0, AvgN1 = 0, AvgN2 = 0;   // Variaveis de estado do round atual
        long double total_round_time = 0;                           // Tempo total acumulado do round

        if(this->debug == DEBUG_ALL || this->debug == DEBUG_IMPORTANT) {
            printf("Rounds: ");
            for(auto rounds: round_start_times) {
                printf(" %Lf ", rounds);
            }
            printf("\n");

            printf("\nEvent List Size: %lu\n", event_list.size());
        }

        std::vector<long double>::iterator start_time_round = next(this->round_start_times.begin());    // Iniciamos com o termino do primeiro round
        for(auto itr = event_list.begin(); itr != event_list.end(); itr++) {
            std::pair<long double, EventType> event = *itr;

            // Ignora aqueles da fase transiente
            if(event.first > this->system_start_time) {

                long double delta_time = event.first - last_time;

                // Primeiro fazemos a marcacao ponderada durante o delta_time anterior
                AvgNq1 += Nq1 * delta_time;
                AvgN1 += N1 * delta_time;
                AvgNq2 += Nq2 * delta_time;
                AvgN2 += N2 * delta_time;
                total_round_time += delta_time;

                // Se o evento atual for o ultimo da lista ou 
                // for depois do fim do round [mas que o proximo nao acontece junto]: FINALIZA O ROUND
                std::pair<long double, EventType> next_event = *std::next(itr); 
                if(
                    start_time_round != this->round_start_times.end() && 
                    ((std::next(itr) == event_list.end()) || (event.first >= *start_time_round))   
                ) {
                    AvgNq1 /= total_round_time;
                    AvgN1 /= total_round_time;
                    AvgNq2 /= total_round_time;
                    AvgN2 /= total_round_time;

                    if(this->debug == DEBUG_ALL || this->debug == DEBUG_IMPORTANT) {
                        printf("\nQuantity Metrics: AvgN1 %Lf, AvgN2 %Lf, AvgNq1 %Lf, AvgNq2 %Lf", AvgN1, AvgN2, AvgNq1, AvgNq2);
                    }

                    this->rounds_N1.push_back(AvgN1);
                    this->rounds_N2.push_back(AvgN2);
                    this->rounds_Nq1.push_back(AvgNq1);
                    this->rounds_Nq2.push_back(AvgNq2);

                    // Reseta apenas os acumulados do round
                    AvgNq1 = 0, AvgNq2 = 0, AvgN1 = 0, AvgN2 = 0, total_round_time = 0;

                    start_time_round++;
                }

            }

            // Atualizamos as quantidades nas estruturas
            switch(event.second) {
                case queue1_arrival: Nq1++; N1++; 
                // printf("(queue1_arrival : %Lf)\n", event.first);
                break;

                case queue1_departure: Nq1--; N1--; 
                // printf("(queue1_departure : %Lf)\n", event.first);
                break;

                case queue2_arrival: Nq2++; N2++; 
                // printf("(queue2_arrival : %Lf)\n", event.first);
                break;

                case queue2_departure: Nq2--; N2--; 
                // printf("(queue2_departure : %Lf)\n", event.first);
                break;

                case service1_arrival: N1++; 
                // printf("(service1_arrival : %Lf)\n", event.first);
                break;

                case service1_departure: N1--; 
                // printf("(service1_departure : %Lf)\n", event.first);
                break;

                case service2_arrival: N2++; 
                // printf("(service2_arrival : %Lf)\n", event.first);
                break;

                case service2_departure: N2--; 
                // printf("(service2_departure : %Lf)\n", event.first);
                break;
            }

            last_time = event.first;
        }
    }

    void consolidate_time_metrics_by_round() {
        // Calcula Metricas de Tempo
        // Adiciona metricas no round apenas se a execucao ocorreu dentro de seu intervalo de tempo
        int tot_rounds = round_start_times.size()-1;
        std::vector<std::vector<long double>> W1(tot_rounds), X1(tot_rounds), T1(tot_rounds), W2(tot_rounds), X2(tot_rounds), T2(tot_rounds);
        
        if(this->debug == DEBUG_ALL) {
            printf("\nClient List Size: %lu\n", client_list.size());
        }

        for(auto client: client_list) {
            if(client.round_number < 1) continue;

            // Obtem os tempos de inicio e fim do round desse cliente
            int round_number = client.round_number;
            long double round_start_time = round_start_times[round_number-1];
            long double round_end_time = round_start_times[round_number];

            // Verifica se finalizou a fila 1 antes do tempo do round
            if(client.tm_arrival_queue1 >= round_start_time && client.tm_start_service1 <= round_end_time) {
                W1[round_number-1].push_back(client.tm_start_service1 - client.tm_arrival_queue1);
            }
            // Verifica se finalizou o servico 1 antes do tempo do round
            if(client.tm_start_service1 >= round_start_time && client.tm_end_service1 <= round_end_time) {
                X1[round_number-1].push_back(client.tm_end_service1 - client.tm_start_service1);
                T1[round_number-1].push_back(client.tm_end_service1 - client.tm_arrival_queue1);
            }
            // Verifica se finalizou a fila 2 + servico 2 antes do tempo do round
            if(client.tm_arrival_queue2 >= round_start_time && client.tm_end_service2 <= round_end_time) {
                T2[round_number-1].push_back(client.tm_end_service2 - client.tm_arrival_queue2);
                X2[round_number-1].push_back(client.tm_service2);
                W2[round_number-1].push_back(client.tm_end_service2 - client.tm_arrival_queue2 - client.tm_service2);   // W = T - X
            }
        }

        if(this->debug == DEBUG_ALL) printf("\n[Starting Agreggation]\n");

        // Salva o Agregado do Round
        for(int i=0; i<tot_rounds; i++){
            this->rounds_W1.push_back(W1[i].size() == 0 ? 0.0 : accumulate(W1[i].begin(), W1[i].end(), 0.0) / W1[i].size());
            this->rounds_W2.push_back(W2[i].size() == 0 ? 0.0 : accumulate(W2[i].begin(), W2[i].end(), 0.0) / W2[i].size());
            this->rounds_X1.push_back(X1[i].size() == 0 ? 0.0 : accumulate(X1[i].begin(), X1[i].end(), 0.0) / X1[i].size());
            this->rounds_X2.push_back(X2[i].size() == 0 ? 0.0 : accumulate(X2[i].begin(), X2[i].end(), 0.0) / X2[i].size());
            this->rounds_T1.push_back(T1[i].size() == 0 ? 0.0 : accumulate(T1[i].begin(), T1[i].end(), 0.0) / T1[i].size());
            this->rounds_T2.push_back(T2[i].size() == 0 ? 0.0 : accumulate(T2[i].begin(), T2[i].end(), 0.0) / T2[i].size());
            
            if(this->debug == DEBUG_ALL || this->debug == DEBUG_IMPORTANT){
                printf("\nTime Metrics [round %d]: W1 %Lf, X1 %Lf, T1 %Lf, W2 %Lf, X2 %Lf, T2 %Lf", 
                    i+1, this->rounds_W1.back(), this->rounds_X1.back(), this->rounds_T1.back(),
                    this->rounds_W2.back(), this->rounds_X2.back(), this->rounds_T2.back());
            }
        }
    }

    void consolidate_final_metrics() {
        int nrounds = rounds_N1.size();

        // Media: Sum Xi / n
        
        AvgNq1 = vector_avg(rounds_Nq1);
        AvgNq2 = vector_avg(rounds_Nq2);
        AvgN1 = vector_avg(rounds_N1);
        AvgN2 = vector_avg(rounds_N2);
        AvgW1 = vector_avg(rounds_W1);
        AvgW2 = vector_avg(rounds_W2);
        AvgX1 = vector_avg(rounds_X1);
        AvgX2 = vector_avg(rounds_X2);
        AvgT1 = vector_avg(rounds_T1);
        AvgT2 = vector_avg(rounds_T2);

        // Variancia: Sum (Xi - AvgX)^2 / (n-1)

        VarNq1=0, VarNq2=0, VarN1=0, VarN2=0, VarW1=0, VarW2=0, VarX1=0, VarX2=0, VarT1=0, VarT2=0;
        for(int i=0; i<nrounds; i++) {
            VarNq1 += pow(rounds_Nq1[i] - AvgNq1, 2);
            VarNq2 += pow(rounds_Nq2[i] - AvgNq2, 2);
            VarN1 += pow(rounds_N1[i] - AvgN1, 2);
            VarN2 += pow(rounds_N2[i] - AvgN2, 2);
            VarW1 += pow(rounds_W1[i] - AvgW1, 2);
            VarW2 += pow(rounds_W2[i] - AvgW2, 2);
            VarX1 += pow(rounds_X1[i] - AvgX1, 2);
            VarX2 += pow(rounds_X2[i] - AvgX2, 2);
            VarT1 += pow(rounds_T1[i] - AvgT1, 2);
            VarT2 += pow(rounds_T2[i] - AvgT2, 2);
        }

        VarNq1 /= nrounds-1;
        VarNq2 /= nrounds-1; 
        VarN1 /= nrounds-1;
        VarN2 /= nrounds-1;
        VarW1 /= nrounds-1;
        VarW2 /= nrounds-1;
        VarX1 /= nrounds-1;
        VarX2 /= nrounds-1;
        VarT1 /= nrounds-1;
        VarT2 /= nrounds-1;

        // Desvio Padrao: sqrt(VarX)

        DevNq1 = sqrt(VarNq1);
        DevNq2 = sqrt(VarNq2); 
        DevN1 = sqrt(VarN1);
        DevN2 = sqrt(VarN2);
        DevW1 = sqrt(VarW1);
        DevW2 = sqrt(VarW2);
        DevX1 = sqrt(VarX1);
        DevX2 = sqrt(VarX2);
        DevT1 = sqrt(VarT1);
        DevT2 = sqrt(VarT2);

        // Intervalo de Confianca: 1.96 * DevX / sqrt(n)

        IntConfNq1 = 1.96 * DevNq1 / sqrt(nrounds);
        IntConfNq2 = 1.96 * DevNq2 / sqrt(nrounds); 
        IntConfN1 = 1.96 * DevN1 / sqrt(nrounds);
        IntConfN2 = 1.96 * DevN2 / sqrt(nrounds);
        IntConfW1 = 1.96 * DevW1 / sqrt(nrounds);
        IntConfW2 = 1.96 * DevW2 / sqrt(nrounds);
        IntConfX1 = 1.96 * DevX1 / sqrt(nrounds);
        IntConfX2 = 1.96 * DevX2 / sqrt(nrounds);
        IntConfT1 = 1.96 * DevT1 / sqrt(nrounds);
        IntConfT2 = 1.96 * DevT2 / sqrt(nrounds);

        // Precisao: 

        PrecisionNq1 = IntConfNq1 / AvgNq1;
        PrecisionNq2 = IntConfNq2 / AvgNq2; 
        PrecisionN1 = IntConfN1 / AvgN1;
        PrecisionN2 = IntConfN2 / AvgN2;
        PrecisionW1 = IntConfW1 / AvgW1;
        PrecisionW2 = IntConfW2 / AvgW2;
        PrecisionX1 = IntConfX1 / AvgX1;
        PrecisionX2 = IntConfX2 / AvgX2;
        PrecisionT1 = IntConfT1 / AvgT1;
        PrecisionT2 = IntConfT2 / AvgT2;

    }

    void consolidate_statistics() {
        if(this->debug == DEBUG_ALL) printf("[Sorted Event List]\n");
        std::sort(this->event_list.begin(), this->event_list.end());

        // Sets an infinite round to finish the last one
        this->round_start_times.push_back(std::numeric_limits<long double>::infinity());

        this->consolidate_quantity_metrics_by_round();
        this->consolidate_time_metrics_by_round();
        this->consolidate_final_metrics();
    }

    void print_statistics_by_round() {
        if(this->debug == DEBUG_ALL || this->debug == DEBUG_IMPORTANT) {
            printf(
                "\n\nTotal de Rounds: Nq1 %lu, Nq2 %lu, N1 %lu, N2 %lu, W1 %lu, W2 %lu, X1 %lu, X2 %lu, T1 %lu, T2 %lu\n\n",
                rounds_Nq1.size(), rounds_Nq2.size(), rounds_N1.size(), rounds_N2.size(),
                rounds_W1.size(), rounds_W2.size(), rounds_X1.size(), rounds_X2.size(), rounds_T1.size(), rounds_T2.size()
            );
         
            printf("round,Nq1,Nq2,N1,N2,W1,W2,X1,X2,T1,T2\n");
            for(int i=0; i<rounds_N1.size(); i++) {
                printf(
                    "%d, %Lf, %Lf, %Lf, %Lf, %Lf, %Lf, %Lf, %Lf, %Lf, %Lf\n",
                    i+1, rounds_Nq1[i], rounds_Nq2[i], rounds_N1[i], rounds_N2[i],
                    rounds_W1[i], rounds_W2[i], rounds_X1[i], rounds_X2[i], rounds_T1[i], rounds_T2[i]
                );
            }
        }
    }
};