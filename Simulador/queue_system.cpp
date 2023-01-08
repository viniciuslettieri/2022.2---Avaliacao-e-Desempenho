#include <deque>
#include <vector>
#include "statistics.cpp"

class QueueSystem {

	public:

    double lambda_arrival, lambda_service;

    long double global_time;
    int total_clients;
    int clients_per_round, current_round;
    std::deque<Client> queue1, queue2;       // back = end | front = start
    std::vector<Client> finalized;
    StatisticsHandler statistics_handler;

    bool in_transient_state;
    int transient_clients_left;
    int debug;

	QueueSystem(int clients_per_round, int transient_clients, double lambda_arrival, double lambda_service, int debug = false, bool transient_mode = false) {
		this->global_time = 0.0;
        this->debug = debug;
        this->statistics_handler = StatisticsHandler(debug, transient_mode);
        this->clients_per_round = clients_per_round;
        this->total_clients = 0;
        this->current_round = 0;

        this->lambda_arrival = lambda_arrival;
        this->lambda_service = lambda_service;

        if(transient_clients == 0)
            this->in_transient_state = false;
        else
            this->in_transient_state = true;

        this->transient_clients_left = transient_clients;
	}

    void set_deterministic_arrivals(std::deque<Client> &deterministic_queue1) {
        this->queue1.insert(this->queue1.end(), deterministic_queue1.begin(), deterministic_queue1.end());
    }

    void add_queue1(Client client) {
        client.state = st_queue1;
        client.tm_arrival_queue1 = client.tm_arrival;

        if(!this->in_transient_state) {
            this->total_clients++;

            // Inicia o Round
            if((this->total_clients-1) % this->clients_per_round == 0) {
                this->current_round++;
                this->statistics_handler.set_round_time(client.tm_arrival_queue1);
            }
        } else {
            if(this->debug == DEBUG_ALL) printf("[transient]");
            
            this->transient_clients_left--;
            if(this->transient_clients_left == 0) {
                if(this->debug == DEBUG_ALL) printf("[transiente finalizado!]");
                this->statistics_handler.set_system_start(client.tm_arrival);
                this->in_transient_state = false;
            }
        }
        
        this->queue1.push_back(client);   
        this->statistics_handler.add_event(client.tm_arrival_queue1, queue1_arrival);

        if(this->debug == DEBUG_ALL) printf("Client %lld added to Queue 1 at %.3Lf.\n", client.identifier, client.tm_arrival_queue1);
    }

    void add_queue2(Client client) {
        client.state = st_queue2;
        client.tm_arrival_queue2 = this->global_time;
        this->queue2.push_back(client);
        this->statistics_handler.add_event(client.tm_arrival_queue2, queue2_arrival);

        if(this->debug == DEBUG_ALL) printf("Client %lld added to Queue 2 at %.3Lf.\n", client.identifier, client.tm_arrival_queue2);
    }

    void readd_queue2(Client client) {
        client.state = st_queue2_retry;
        this->queue2.push_front(client);
        this->statistics_handler.add_event(this->global_time, queue2_arrival);

        if(this->debug == DEBUG_ALL) printf("Client %lld readded to Queue 2 at %.3Lf.\n", client.identifier, this->global_time);
    }

    Client remove_queue1() {
        Client client = this->queue1.front();
        this->queue1.pop_front();
        this->statistics_handler.add_event(this->global_time, queue1_departure);

        if(this->debug == DEBUG_ALL) printf("Client %lld removed from Queue 1 in %.3Lf.\n", client.identifier, this->global_time);
        
        return client;
    }

    Client remove_queue2() {
        Client client = this->queue2.front();
        this->queue2.pop_front();
        this->statistics_handler.add_event(this->global_time, queue2_departure);

        if(this->debug == DEBUG_ALL) printf("Client %lld removed from Queue 2 in %.3Lf.\n", client.identifier, this->global_time);
        
        return client;
    }

    Client execute_service1(Client client) {
        // Inicio do Servico
        client.state = st_service1;
        client.tm_start_service1 = this->global_time;
        this->statistics_handler.add_event(client.tm_start_service1, service1_arrival);

        // Finalizacao do Servico
        this->global_time += client.tm_service1;
        if(this->debug == DEBUG_ALL) printf("\n(gt %.3Lf)\n", this->global_time);
        client.tm_end_service1 = this->global_time;
        this->statistics_handler.add_event(client.tm_end_service1, service1_departure);

        if(this->debug == DEBUG_ALL) printf("Client %lld executed 1 from %.3Lf until %.3Lf.\n", client.identifier, client.tm_start_service1, client.tm_end_service1);

        return client;
    }

    Client execute_service2(Client client) {
        // Inicio do Servico
        if(client.state == st_queue2) {         // primeira vez na fila 2
            client.state = st_service2;
            client.tm_start_service2 = this->global_time;
        } else {                                // outra vez na fila 2
            client.state = st_service2_retry;
        }
        this->statistics_handler.add_event(this->global_time, service2_arrival);

        long double remaining_service = client.tm_service2 - client.tm_accumulated_service2;

        // Se a proxima chegada ocorrer antes do servico atual terminar
        if(this->queue1.size() > 0 && this->queue1.front().tm_arrival <= this->global_time + remaining_service) {
            long double next_arrival = this->queue1.front().tm_arrival;

            if(this->debug == DEBUG_ALL)
                printf("[next %.3Lf | accum %.3Lf | remaining %.3Lf | global %.3Lf | active %.3Lf]", 
                    next_arrival, client.tm_accumulated_service2, remaining_service, this->global_time, next_arrival - this->global_time);
            
            long double active_time = next_arrival - this->global_time;
            client.tm_accumulated_service2 += active_time;
            this->global_time += active_time; 
            if(this->debug == DEBUG_ALL) printf("\n(gt %.3Lf)\n", this->global_time);
            client.state = st_queue2_retry;

            this->statistics_handler.add_event(this->global_time, service2_departure);

            if(this->debug == DEBUG_ALL) printf("Client %lld executed service 2: %.3Lf/%.3Lf.\n", client.identifier, client.tm_accumulated_service2, client.tm_service2);
        
        // Se a proxima chegada na fila 1 nao interrompe o servico atual
        } else {
            if(this->debug == DEBUG_ALL)
                printf("[accum %.3Lf | remaining %.3Lf | global %.3Lf]", 
                    client.tm_accumulated_service2, remaining_service, this->global_time);

            client.tm_accumulated_service2 = client.tm_service2;
            client.tm_end_service2 = this->global_time + remaining_service;
            this->global_time += remaining_service; 
            if(this->debug == DEBUG_ALL) printf("\n(gt %.3Lf)\n", this->global_time);
            client.state = st_finished;

            this->statistics_handler.add_event(this->global_time, service2_departure);
            this->statistics_handler.add_client(client);

            if(this->debug == DEBUG_ALL) printf("Client %lld finished service 2 at %.3Lf.\n", client.identifier, client.tm_end_service2);
        }

        return client;
    }

    void execute() {
        Client client;

        // Se existe alguem na fila 1 que ja chegou de fato
        if(this->queue1.size() > 0 && this->queue1.front().tm_arrival <= this->global_time){
            if(this->debug == DEBUG_ALL) printf("\n[primeiro if]\n");
            client = this->remove_queue1();
            client = this->execute_service1(client);
            this->add_queue2(client);

        // Se nao existe ninguem na fila 1 mas existe na 2
        } else if(this->queue2.size() > 0) {
            if(this->debug == DEBUG_ALL) printf("\n[segundo if]\n");
            client = this->remove_queue2();
            client = this->execute_service2(client);
            if(client.state == st_finished && client.round_number != 0) {
                if(this->debug == DEBUG_ALL) printf("\n[st_finished]\n");
                finalized.push_back(client);
            } else if(client.state == st_queue2_retry) {
                if(this->debug == DEBUG_ALL) printf("\n[st_queue2_retry]\n");
                this->readd_queue2(client);
            }

        // Se existe alguem na fila 1 que chega no futuro (ociosidade) - ajuste do tempo
        } else if(this->queue1.size()) {
            if(this->debug == DEBUG_ALL) printf("\n[terceiro if]\n");
            this->global_time = this->queue1.front().tm_arrival; 
            if(this->debug == DEBUG_ALL) printf("\n(gt %.3Lf)\n", this->global_time);
        }
    }

    long double get_global_time() {
        return this->global_time;
    }

    void finish() {
        if(this->debug == DEBUG_ALL) printf("\n[...Starting Statistics Consolidation...]\n");

        this->statistics_handler.consolidate_statistics();

        if(this->debug < 100) {
            this->statistics_handler.print_statistics_by_round();
        }

        // Obtencao das medidas analiticas para comparacao
        long double ExpectedAvgX1 = 1/lambda_service;
        long double ExpectedAvgX2 = 1/lambda_service;
        long double rho = 2 * lambda_arrival;
        long double rho1 = lambda_arrival * ExpectedAvgX1;
        long double rho2 = lambda_arrival * ExpectedAvgX2;
        long double rhoXr = rho1 * ExpectedAvgX1 + rho2 * ExpectedAvgX2 + lambda_arrival * ExpectedAvgX1 * ExpectedAvgX2;
        
        long double ExpectedAvgW1 = (rho1 * ExpectedAvgX1) / (1 - rho1);
        long double ExpectedAvgT1 = ExpectedAvgW1 + ExpectedAvgX1;
        
        long double ExpectedAvgT = (rhoXr + (1-rho)*(ExpectedAvgX1 + ExpectedAvgX2))/((1-rho) * (1-rho1));
        long double ExpectedAvgT2 = ExpectedAvgT - ExpectedAvgT1;
        long double ExpectedAvgW2 = ExpectedAvgT2 - ExpectedAvgX2;
        long double ExpectedAvgN1 = lambda_arrival * ExpectedAvgT1;
        long double ExpectedAvgN2 = lambda_arrival * ExpectedAvgT2;
        long double ExpectedAvgNq1 = lambda_arrival * ExpectedAvgW1;
        long double ExpectedAvgNq2 = lambda_arrival * ExpectedAvgW2;
        
        if(this->debug == DEBUG_ALL || this->debug == DEBUG_IMPORTANT || this->debug == DEBUG_FINAL_STATS){
            printf("\n\nEstatisticas Finais: [lambda arrival %.3f, lambda service %.3f, rho1 = %.3Lf, rho2 = %.3Lf]\n", 
                lambda_arrival, lambda_service, rho1, rho2);

            print_metric(
                "Nq1", ExpectedAvgNq1,
                statistics_handler.AvgNq1 - statistics_handler.IntConfNq1,
                statistics_handler.AvgNq1 + statistics_handler.IntConfNq1,
                statistics_handler.PrecisionNq1
            );
            print_metric(
                "Nq2", ExpectedAvgNq2,
                statistics_handler.AvgNq2 - statistics_handler.IntConfNq2,
                statistics_handler.AvgNq2 + statistics_handler.IntConfNq2,
                statistics_handler.PrecisionNq2
            );
            print_metric(
                "N1", ExpectedAvgN1,
                statistics_handler.AvgN1 - statistics_handler.IntConfN1,
                statistics_handler.AvgN1 + statistics_handler.IntConfN1,
                statistics_handler.PrecisionN1
            );
            print_metric(
                "N2", ExpectedAvgN2,
                statistics_handler.AvgN2 - statistics_handler.IntConfN2,
                statistics_handler.AvgN2 + statistics_handler.IntConfN2,
                statistics_handler.PrecisionN2
            );
            print_metric(
                "W1", ExpectedAvgW1,
                statistics_handler.AvgW1 - statistics_handler.IntConfW1,
                statistics_handler.AvgW1 + statistics_handler.IntConfW1,
                statistics_handler.PrecisionW1
            );
            print_metric(
                "W2", ExpectedAvgW2,
                statistics_handler.AvgW2 - statistics_handler.IntConfW2,
                statistics_handler.AvgW2 + statistics_handler.IntConfW2,
                statistics_handler.PrecisionW2
            );
            print_metric(
                "VarW1", -10000,
                statistics_handler.AvgVarW1 - statistics_handler.IntConfVarW1,
                statistics_handler.AvgVarW1 + statistics_handler.IntConfVarW1,
                statistics_handler.PrecisionVarW1
            );
            print_metric(
                "VarW2", -10000,
                statistics_handler.AvgVarW2 - statistics_handler.IntConfVarW2,
                statistics_handler.AvgVarW2 + statistics_handler.IntConfVarW2,
                statistics_handler.PrecisionVarW2
            );
            print_metric(
                "X1", ExpectedAvgX1,
                statistics_handler.AvgX1 - statistics_handler.IntConfX1,
                statistics_handler.AvgX1 + statistics_handler.IntConfX1,
                statistics_handler.PrecisionX1
            );
            print_metric(
                "X2", ExpectedAvgX2,
                statistics_handler.AvgX2 - statistics_handler.IntConfX2,
                statistics_handler.AvgX2 + statistics_handler.IntConfX2,
                statistics_handler.PrecisionX2
            );
            print_metric(
                "T1", ExpectedAvgT1,
                statistics_handler.AvgT1 - statistics_handler.IntConfT1,
                statistics_handler.AvgT1 + statistics_handler.IntConfT1,
                statistics_handler.PrecisionT1
            );
            print_metric(
                "T2", ExpectedAvgT2,
                statistics_handler.AvgT2 - statistics_handler.IntConfT2,
                statistics_handler.AvgT2 + statistics_handler.IntConfT2,
                statistics_handler.PrecisionT2
            );
        }
    }
};