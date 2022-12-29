#include <vector>
#include <algorithm>
#include "client.cpp"

#define NO_DEBUG 0
#define DEBUG_STATS 1
#define DEBUG_IMPORTANT 2
#define DEBUG_ALL 3


// Importante que os eventos com conflito de mesmo tempo tenham a ordem bem definida
// Relevante principalmente no caso de interrupcao e volta pra fila
enum EventType { 
    queue1_arrival, queue1_departure, service1_arrival, service1_departure, 
    service2_departure, queue2_arrival, queue2_departure, service2_arrival
};

class StatisticsHandler {

    private:

    std::vector<std::pair<long double, EventType>> event_list;        // time and type of event 
    std::vector<Client> client_list;
    std::vector<long double> round_start_times;

    int debug;

    public:

    std::vector<long double> rounds_W1, rounds_W2, rounds_T1, rounds_T2, rounds_X1, rounds_X2, rounds_Nq1, rounds_Nq2, rounds_N1, rounds_N2;

    StatisticsHandler(int debug = 0) {
        this->debug = debug;
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

    void consolidate_statistics() {
        if(this->debug == DEBUG_ALL) printf("[Sorted Event List]\n");
        std::sort(this->event_list.begin(), this->event_list.end());

        // Sets an infinite round to finish the last one
        this->round_start_times.push_back(std::numeric_limits<long double>::infinity());

        // Calcula Metricas de Tempo [Nq1, Nq2, N1, N2]
        int Nq1 = 0, Nq2 = 0, N1 = 0, N2 = 0;           // Variaveis de estado independente de round
        long double last_time = 0.0;                    // Tempo do ultimo evento
        long double AvgNq1 = 0, AvgNq2 = 0, AvgN1 = 0, AvgN2 = 0;   // Variaveis de estado do round atual
        long double total_round_time = 0;                           // Tempo total acumulado do round

        if(this->debug == DEBUG_ALL || this->debug == DEBUG_IMPORTANT) {
            printf("Rounds: ");
            for(auto rounds: round_start_times) {
                printf(" %Lf ", rounds);
            }
            printf("\n");
        }

        std::vector<long double>::iterator start_time_round = next(this->round_start_times.begin());    // Iniciamos com o termino do primeiro round
        for(auto itr = event_list.begin(); itr != event_list.end(); itr++) {
            std::pair<long double, EventType> event = *itr;

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
            // if(start_time_round != this->round_start_times.end() && next_event.first >= *start_time_round) {
            if(
                start_time_round != this->round_start_times.end() && 
                (   
                    (std::next(itr) == event_list.end()) || (event.first >= *start_time_round) 
                )   
            ) {
                // TODO: Sem um evento final finalizador todas as metricas da ultima rodada sao ignoradas!!!!!
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

        // Calcula Metricas de Quantidade
        // Adiciona metricas no round apenas se a execucao ocorreu dentro de seu intervalo de tempo
        int tot_rounds = round_start_times.size()-1;
        std::vector<std::vector<long double>> W1(tot_rounds), X1(tot_rounds), T1(tot_rounds), W2(tot_rounds), X2(tot_rounds), T2(tot_rounds);
        
        for(auto client: client_list) {
            // Obtem os tempos de inicio e fim do round desse cliente
            int round_number = client.round_number;
            long double round_start_time = round_start_times[round_number-1];
            long double round_end_time = round_start_times[round_number];

            // Verifica se finalizou a fila 1 antes do tempo do round
            if(client.tm_start_service1 <= round_end_time) {
                W1[round_number-1].push_back(client.tm_start_service1 - client.tm_arrival_queue1);
            }
            // Verifica se finalizou o servico 1 antes do tempo do round
            if(client.tm_end_service1 <= round_end_time) {
                X1[round_number-1].push_back(client.tm_end_service1 - client.tm_start_service1);
                T1[round_number-1].push_back(client.tm_end_service1 - client.tm_arrival_queue1);
            }
            // Verifica se finalizou a fila 2 + servico 2 antes do tempo do round
            if(client.tm_end_service2 <= round_end_time) {
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

        this->clear_all();
    }

    void clear_all() {
        this->event_list.clear();
        this->client_list.clear();
    }

    void print_statistics() {
        if(this->debug == DEBUG_ALL || this->debug == DEBUG_IMPORTANT) {
            printf(
                "\n\nTotal de Rounds: NQ1 %lu, NQ2 %lu, N1 %lu, N2 %lu, W1 %lu, W2 %lu, X1 %lu, X2 %lu, T1 %lu, T2 %lu\n\n",
                rounds_Nq1.size(), rounds_Nq2.size(), rounds_N1.size(), rounds_N2.size(),
                rounds_W1.size(), rounds_W2.size(), rounds_X1.size(), rounds_X2.size(), rounds_T1.size(), rounds_T2.size()
            );
        }

        printf("round,NQ1,NQ2,N1,N2,W1,W2,X1,X2,T1,T2\n");
        for(int i=0; i<rounds_N1.size(); i++) {
            printf(
                "%d, %Lf, %Lf, %Lf, %Lf, %Lf, %Lf, %Lf, %Lf, %Lf, %Lf\n",
                i+1, rounds_Nq1[i], rounds_Nq2[i], rounds_N1[i], rounds_N2[i],
                rounds_W1[i], rounds_W2[i], rounds_X1[i], rounds_X2[i], rounds_T1[i], rounds_T2[i]
            );
        }
    }
};