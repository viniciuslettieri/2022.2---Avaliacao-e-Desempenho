#include <vector>
#include <sstream>
#include "queue_system.cpp"

using namespace std;

int main(int argc, char* argv[]){
	if(argc-1 != 6) {
		printf("Foram passados %d argumentos pelo cmd.\n", argc-1);
		printf("Esperado [total clientes: int] [modo debug: bool] [arrival seed: int] [service seed: int] [utilizacao: double] [clientes/round: int]\n");
		return 1;
	}

	// Interpretacao dos argumentos de linha
	int clients = atoi(argv[1]);
	int debug = atoi(argv[2]);
	unsigned int arrival_seed;
	unsigned int service_seed;

	std::stringstream ss;
	ss << argv[3];
	ss >> arrival_seed;
	ss.clear();
	ss << argv[4];
	ss >> service_seed;

	char *ptr;
	double rho = strtod(argv[5], &ptr);

	int clients_per_round = atoi(argv[6]);

	if(debug == 1) {
		printf("\n[Raw Arguments: clients %s, debug %s, arrival_seed %s, service_seed %s, rho %s, per_round %s]\n", 
			argv[1], argv[2], argv[3], argv[4], argv[5], argv[6]);
		printf("[Inputs: clients %d, debug %d, arrival_seed %u, service_seed %u, rho %f, per_round %d]\n\n", 
			clients, debug, arrival_seed, service_seed, rho, clients_per_round);
	}

	QueueSystem queue_system(clients_per_round, debug);

	long double last_arrival = 0.0;
	
	double lambda = rho / 2.0;
	ExponentialGenerator arrival_generator(lambda);
	ExponentialGenerator service_generator(1.0);

	if(arrival_seed != -1)
		arrival_generator.set_deterministic_seed(arrival_seed);
	
	if(service_seed != -1)
		service_generator.set_deterministic_seed(service_seed);	
	
	// Executamos o simulador ate que todos os clientes tenham sido criados e finalizados
	long long int total_arrivals = 0;
	while(queue_system.finalized.size() < clients) {
		// Criamos apenas a proxima chegada a cada instante
		if(queue_system.queue1.size() <= 1 && total_arrivals <= clients){
			Client next_client = generate_arrival(last_arrival, arrival_generator, service_generator);

			if(!queue_system.in_transient_state) {
				total_arrivals++;
				next_client.round_number = (total_arrivals-1) / clients_per_round + 1; 
			}

			queue_system.add_queue1(next_client);
			last_arrival = next_client.tm_arrival;
		}

		// Execucao do sistema: fila ou servico
		queue_system.execute();
	}

	queue_system.finish();

	if(debug == 0 || debug == 1) {
		printf("identifier,tm_arrival,tm_service1,tm_service2,tm_arrival_queue1,tm_start_service1,tm_end_service1,tm_arrival_queue2,tm_start_service2,tm_end_service2\n");
		for(auto client: queue_system.finalized) {
			print_client(client);
		}
	}

    return 0;
}