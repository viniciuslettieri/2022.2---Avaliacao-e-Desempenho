#include <deque>
#include <vector>
#include "client.cpp"

class QueueSystem {

	public:

    std::deque<Client> queue1, queue2;       // back = end | front = start
    long double global_time;
    std::vector<Client> finalized;

    bool debug;

	QueueSystem(bool debug = false) {
		this->global_time = 0.0;
        this->debug = debug;
	}

    void add_queue1(Client client) {
        client.state = st_queue1;
        client.tm_arrival_queue1 = client.tm_arrival;
        this->queue1.push_back(client);

        if(this->debug) printf("Client %lld added to Queue 1 at %Lf.\n", client.identifier, client.tm_arrival_queue1);
    }

    void add_queue2(Client client) {
        client.state = st_queue2;
        client.tm_arrival_queue2 = this->global_time;
        this->queue2.push_back(client);

        if(this->debug) printf("Client %lld added to Queue 2 at %Lf.\n", client.identifier, client.tm_arrival_queue2);
    }

    void readd_queue2(Client client) {
        client.state = st_queue2_retry;
        this->queue2.push_front(client);

        if(this->debug) printf("Client %lld readded to Queue 2 at %Lf.\n", client.identifier, this->global_time);
    }

    Client remove_queue1() {
        Client client = this->queue1.front();
        this->queue1.pop_front();
        return client;

        if(this->debug) printf("Client %lld removed from Queue 1 in %Lf.\n", client.identifier, this->global_time);
    }

    Client remove_queue2() {
        Client client = this->queue2.front();
        this->queue2.pop_front();
        return client;

        if(this->debug) printf("Client %lld removed from Queue 2 in %Lf.\n", client.identifier, this->global_time);
    }

    Client execute_service1(Client client) {
        // Inicio do Servico
        client.state = st_service1;
        client.tm_start_service1 = this->global_time;

        // Finalizacao do Servico
        this->global_time += client.tm_service1;
        if(this->debug) printf("\n(gt %Lf)\n", this->global_time);
        client.tm_end_service1 = this->global_time;

        if(this->debug) printf("Client %lld executed 1 from %Lf until %Lf.\n", client.identifier, client.tm_start_service1, client.tm_end_service1);

        return client;
    }

    Client execute_service2(Client client) {
        // Inicio do Servico
        if(client.state == st_queue2) {      // primeira vez na fila 2
            client.state = st_service2;
            client.tm_start_service2 = this->global_time;
        } else {                            // outra vez na fila 2
            client.state = st_service2_retry;
        }

        long double remaining_service = client.tm_service2 - client.tm_accumulated_service2;

        // Se a proxima chegada ocorrer antes do servico atual terminar
        if(this->queue1.size() > 0 && this->queue1.front().tm_arrival <= this->global_time + remaining_service) {
            long double next_arrival = this->queue1.front().tm_arrival;

            if(this->debug)
                printf("[next %Lf | accum %Lf | remaining %Lf | global %Lf | active %Lf]", 
                    next_arrival, client.tm_accumulated_service2, remaining_service, this->global_time, next_arrival - this->global_time);
            
            long double active_time = next_arrival - this->global_time;
            client.tm_accumulated_service2 += active_time;
            this->global_time += active_time; 
            if(this->debug) printf("\n(gt %Lf)\n", this->global_time);
            client.state = st_queue2_retry;

            if(this->debug) printf("Client %lld executed service 2: %Lf/%Lf.\n", client.identifier, client.tm_accumulated_service2, client.tm_service2);
        
        // Se a proxima chegada na fila 1 nao interrompe o servico atual
        } else {
            if(this->debug)
                printf("[accum %Lf | remaining %Lf | global %Lf]", 
                    client.tm_accumulated_service2, remaining_service, this->global_time);

            client.tm_accumulated_service2 = client.tm_service2;
            client.tm_end_service2 = this->global_time + remaining_service;
            this->global_time += remaining_service; 
            if(this->debug) printf("\n(gt %Lf)\n", this->global_time);
            client.state = st_finished;

            if(this->debug) printf("Client %lld finished service 2 at %Lf.\n", client.identifier, client.tm_end_service2);
        }

        return client;
    }

    void execute() {
        Client client;

        // Se existe alguem na fila 1 que ja chegou de fato
        if(this->queue1.size() > 0 && this->queue1.front().tm_arrival <= this->global_time){
            if(this->debug) printf("\n[primeiro if]\n");
            client = this->remove_queue1();
            client = this->execute_service1(client);
            this->add_queue2(client);

        // Se nao existe ninguem na fila 1 mas existe na 2
        } else if(this->queue2.size() > 0) {
            if(this->debug) printf("\n[segundo if]\n");
            client = this->remove_queue2();
            client = this->execute_service2(client);
            if(client.state == st_finished) {
                if(this->debug) printf("\n[st_finished]\n");
                finalized.push_back(client);
            } else if(client.state == st_queue2_retry) {
                if(this->debug) printf("\n[st_queue2_retry]\n");
                this->readd_queue2(client);
            }

        // Se existe alguem na fila 1 que chega no futuro (ociosidade) - ajuste do tempo
        } else if(this->queue1.size()) {
            if(this->debug) printf("\n[terceiro if]\n");
            this->global_time = this->queue1.front().tm_arrival; 
            if(this->debug) printf("\n(gt %Lf)\n", this->global_time);
        }

        // if(this->debug) {
        //     for(auto fin : this->finalized) {
        //         printf("%lld: %Lf\n", fin.identifier, fin.tm_end_service2);
        //     }
        // }
    }

    long double get_global_time() {
        return this->global_time;
    }
};