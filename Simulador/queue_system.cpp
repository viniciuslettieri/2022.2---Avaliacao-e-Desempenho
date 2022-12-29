#include <deque>
#include <vector>
#include "statistics.cpp"

class QueueSystem {

	public:

    long double global_time;
    int total_clients;
    int clients_per_round, current_round;
    std::deque<Client> queue1, queue2;       // back = end | front = start
    std::vector<Client> finalized;
    StatisticsHandler statistics_handler;

    bool in_transient_state;
    int debug;

	QueueSystem(int clients_per_round, int debug = false) {
		this->global_time = 0.0;
        this->debug = debug;
        this->statistics_handler = StatisticsHandler(debug);
        this->clients_per_round = clients_per_round;
        this->total_clients = 0;

        this->in_transient_state = false;       // TODO: ADICIONAR ESTADO TRANSIENTE 
        this->current_round = 0;                // TODO: INICIAR NO ROUND 0
	}

    void add_queue1(Client client) {
        client.state = st_queue1;
        client.tm_arrival_queue1 = client.tm_arrival;

        if(!this->in_transient_state) {
            this->total_clients++;

            // Fechou o Round
            if((this->total_clients-1) % this->clients_per_round == 0) {
                this->current_round++;
                this->statistics_handler.set_round_time(client.tm_arrival_queue1);
            }
        } 
        
        this->queue1.push_back(client);   
        this->statistics_handler.add_event(client.tm_arrival_queue1, queue1_arrival);

        if(this->debug == DEBUG_ALL) printf("Client %lld added to Queue 1 at %Lf.\n", client.identifier, client.tm_arrival_queue1);
    }

    void add_queue2(Client client) {
        client.state = st_queue2;
        client.tm_arrival_queue2 = this->global_time;
        this->queue2.push_back(client);
        this->statistics_handler.add_event(client.tm_arrival_queue2, queue2_arrival);

        if(this->debug == DEBUG_ALL) printf("Client %lld added to Queue 2 at %Lf.\n", client.identifier, client.tm_arrival_queue2);
    }

    void readd_queue2(Client client) {
        client.state = st_queue2_retry;
        this->queue2.push_front(client);
        this->statistics_handler.add_event(this->global_time, queue2_arrival);

        if(this->debug == DEBUG_ALL) printf("Client %lld readded to Queue 2 at %Lf.\n", client.identifier, this->global_time);
    }

    Client remove_queue1() {
        Client client = this->queue1.front();
        this->queue1.pop_front();
        this->statistics_handler.add_event(this->global_time, queue1_departure);

        if(this->debug == DEBUG_ALL) printf("Client %lld removed from Queue 1 in %Lf.\n", client.identifier, this->global_time);
        
        return client;
    }

    Client remove_queue2() {
        Client client = this->queue2.front();
        this->queue2.pop_front();
        this->statistics_handler.add_event(this->global_time, queue2_departure);

        if(this->debug == DEBUG_ALL) printf("Client %lld removed from Queue 2 in %Lf.\n", client.identifier, this->global_time);
        
        return client;
    }

    Client execute_service1(Client client) {
        // Inicio do Servico
        client.state = st_service1;
        client.tm_start_service1 = this->global_time;
        this->statistics_handler.add_event(client.tm_start_service1, service1_arrival);

        // Finalizacao do Servico
        this->global_time += client.tm_service1;
        if(this->debug == DEBUG_ALL) printf("\n(gt %Lf)\n", this->global_time);
        client.tm_end_service1 = this->global_time;
        this->statistics_handler.add_event(client.tm_end_service1, service1_departure);

        if(this->debug == DEBUG_ALL) printf("Client %lld executed 1 from %Lf until %Lf.\n", client.identifier, client.tm_start_service1, client.tm_end_service1);

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
                printf("[next %Lf | accum %Lf | remaining %Lf | global %Lf | active %Lf]", 
                    next_arrival, client.tm_accumulated_service2, remaining_service, this->global_time, next_arrival - this->global_time);
            
            long double active_time = next_arrival - this->global_time;
            client.tm_accumulated_service2 += active_time;
            this->global_time += active_time; 
            if(this->debug == DEBUG_ALL) printf("\n(gt %Lf)\n", this->global_time);
            client.state = st_queue2_retry;

            this->statistics_handler.add_event(this->global_time, service2_departure);

            if(this->debug == DEBUG_ALL) printf("Client %lld executed service 2: %Lf/%Lf.\n", client.identifier, client.tm_accumulated_service2, client.tm_service2);
        
        // Se a proxima chegada na fila 1 nao interrompe o servico atual
        } else {
            if(this->debug == DEBUG_ALL)
                printf("[accum %Lf | remaining %Lf | global %Lf]", 
                    client.tm_accumulated_service2, remaining_service, this->global_time);

            client.tm_accumulated_service2 = client.tm_service2;
            client.tm_end_service2 = this->global_time + remaining_service;
            this->global_time += remaining_service; 
            if(this->debug == DEBUG_ALL) printf("\n(gt %Lf)\n", this->global_time);
            client.state = st_finished;

            this->statistics_handler.add_event(this->global_time, service2_departure);
            this->statistics_handler.add_client(client);

            if(this->debug == DEBUG_ALL) printf("Client %lld finished service 2 at %Lf.\n", client.identifier, client.tm_end_service2);
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
            if(client.state == st_finished) {
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
            if(this->debug == DEBUG_ALL) printf("\n(gt %Lf)\n", this->global_time);
        }
    }

    long double get_global_time() {
        return this->global_time;
    }

    void finish() {
        if(this->debug == DEBUG_ALL) printf("\n[...Starting Statistics Consolidation...]\n");

        this->statistics_handler.consolidate_statistics();

        if(this->debug) {
            this->statistics_handler.print_statistics();
        }
    }
};