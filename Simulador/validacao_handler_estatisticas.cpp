#include <vector>
#include <sstream>
#include "queue_system.cpp"

using namespace std;

// Sequencia de criacao: tm_arrival, tm_service1, tm_service2, round_number

QueueSystem two_round(int &clients_per_round, int &nrounds, int &transient_clients, int &debug){
	clients_per_round = 2;
	nrounds = 2;
	transient_clients = 0;
	debug = DEBUG_ALL;

	QueueSystem queue_system(clients_per_round, transient_clients, 0, 0, debug);

	queue_system.add_queue1(generate_deterministic_arrival(0.5, 1.0, 0.5, 1));
	queue_system.add_queue1(generate_deterministic_arrival(1.0, 1.5, 1.0, 1));
	queue_system.add_queue1(generate_deterministic_arrival(5.5, 0.5, 0.5, 2));
	queue_system.add_queue1(generate_deterministic_arrival(6.0, 2.0, 1.0, 2));

	return queue_system;
}


QueueSystem two_round_overlay(int &clients_per_round, int &nrounds, int &transient_clients, int &debug){
	clients_per_round = 2;
	nrounds = 2;
	transient_clients = 0;
	debug = DEBUG_ALL;

	QueueSystem queue_system(clients_per_round, transient_clients, 0, 0, debug);

	queue_system.add_queue1(generate_deterministic_arrival(0.5, 1.0, 0.5, 1));
	queue_system.add_queue1(generate_deterministic_arrival(1.0, 1.5, 1.0, 1));
	queue_system.add_queue1(generate_deterministic_arrival(4.0, 0.5, 0.5, 2));
	queue_system.add_queue1(generate_deterministic_arrival(4.5, 2.0, 1.0, 2));

	return queue_system;
}


int main(){
	int clients_per_round, nrounds, transient_clients, debug;

	// Escolher qual teste usar:
	QueueSystem queue_system = two_round(clients_per_round, nrounds, transient_clients, debug);
    // QueueSystem queue_system = two_round_overlay(clients_per_round, nrounds, transient_clients, debug);

	// Executamos o simulador
	long long int total_arrivals = 0;
	int clients = nrounds * clients_per_round;
	while(queue_system.finalized.size() < clients) {
		queue_system.execute();
	}

	if(debug == DEBUG_CLIENTS || debug == DEBUG_ALL || debug == DEBUG_IMPORTANT) {
		printf("identifier,tm_arrival,tm_service1,tm_service2,tm_arrival_queue1,tm_start_service1,tm_end_service1,tm_arrival_queue2,tm_start_service2,tm_end_service2\n");
		for(auto client: queue_system.finalized) {
			print_client(client);
		}
	}

	queue_system.finish();

    return 0;
}