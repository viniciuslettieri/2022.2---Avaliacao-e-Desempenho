#include <vector>
#include <sstream>
#include "queue_system.cpp"

using namespace std;

// Sequencia de criacao: tm_arrival, tm_service1, tm_service2, round_number

void set_flow_test(std::deque<Client> &deterministic_queue1, int &clients_per_round, int &nrounds, int &transient_clients, int &debug){
	// Queremos simular apenas um cliente para verificar se as metricas estao sendo coletadas

	deterministic_queue1.push_back(generate_deterministic_arrival(0.1, 1.0, 1.0, 1));

	clients_per_round = deterministic_queue1.size();
	nrounds = 1;
	transient_clients = 0;
	debug = NO_DEBUG;
}


void set_priority_test(std::deque<Client> &deterministic_queue1, int &clients_per_round, int &nrounds, int &transient_clients, int &debug){
	// Queremos simular a prioridade da fila 1 em relacao a fila 2

	deterministic_queue1.push_back(generate_deterministic_arrival(0.1, 1.0, 2.0, 1));
	deterministic_queue1.push_back(generate_deterministic_arrival(0.8, 1.2, 1.0, 1));
	deterministic_queue1.push_back(generate_deterministic_arrival(1.3, 0.9, 1.0, 1));
	deterministic_queue1.push_back(generate_deterministic_arrival(1.8, 1.1, 1.0, 1));

	clients_per_round = deterministic_queue1.size();
	nrounds = 1;
	transient_clients = 0;
	debug = NO_DEBUG;
}


void set_interruption_test(std::deque<Client> &deterministic_queue1, int &clients_per_round, int &nrounds, int &transient_clients, int &debug){
	// Queremos simular a interrupcao que a fila 1 faz em cima da fila 2

	deterministic_queue1.push_back(generate_deterministic_arrival(0.1, 1.0, 2.0, 1));
	deterministic_queue1.push_back(generate_deterministic_arrival(1.3, 1.2, 1.0, 1));
	deterministic_queue1.push_back(generate_deterministic_arrival(2.9, 0.9, 1.0, 1));
	deterministic_queue1.push_back(generate_deterministic_arrival(3.0, 1.1, 1.0, 1));

	clients_per_round = deterministic_queue1.size();
	nrounds = 1;
	transient_clients = 0;
	debug = NO_DEBUG;
}


int main(){
	int clients_per_round, nrounds, transient_clients, debug;

	std::deque<Client> deterministic_queue1;

	// set_flow_test(deterministic_queue1, clients_per_round, nrounds, transient_clients, debug);
	// set_priority_test(deterministic_queue1, clients_per_round, nrounds, transient_clients, debug);
	set_interruption_test(deterministic_queue1, clients_per_round, nrounds, transient_clients, debug);

	QueueSystem queue_system(clients_per_round, transient_clients, debug);
	queue_system.set_deterministic_arrivals(deterministic_queue1);
	
	// Executamos o simulador
	long long int total_arrivals = 0;
	int clients = nrounds * clients_per_round;
	while(queue_system.finalized.size() < clients) {
		queue_system.execute();
	}

	queue_system.finish();

	if(debug == NO_DEBUG || debug == DEBUG_ALL) {
		printf("identifier,tm_arrival,tm_service1,tm_service2,tm_arrival_queue1,tm_start_service1,tm_end_service1,tm_arrival_queue2,tm_start_service2,tm_end_service2\n");
		for(auto client: queue_system.finalized) {
			print_client(client);
		}
	}

    return 0;
}