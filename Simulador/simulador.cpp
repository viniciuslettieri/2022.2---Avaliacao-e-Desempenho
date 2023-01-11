#include <vector>
#include <sstream>
#include <time.h>
#include "queue_system.cpp"

using namespace std;


int transient_table(double rho) {
	if(rho <= 0.2) {
		return 45000;
	} else if(rho <= 0.4) {
		return 20000;
	} else if(rho <= 0.6) {
		return 20000;
	} else if(rho <= 0.8) {
		return 35000;
	} else {
		return 60000;
	}
}


int main(int argc, char* argv[]){
	if(argc-1 != 7) {
		printf("Foram passados %d argumentos pelo cmd.\n", argc-1);
		printf("Esperado [modo debug: bool] [arrival seed: int] [service seed: int] [utilizacao: double] [clientes/round: int] [rounds: int] [clientes transiente: int]\n");
		return 1;
	}

	// Interpretacao dos argumentos de linha
	int debug = atoi(argv[1]);
	unsigned int arrival_seed;
	unsigned int service_seed;

	std::stringstream ss;
	ss << argv[2];
	ss >> arrival_seed;
	ss.clear();
	ss << argv[3];
	ss >> service_seed;

	char *ptr;
	double rho = strtod(argv[4], &ptr);

	int clients_per_round = atoi(argv[5]);

	int nrounds = atoi(argv[6]);
	int transient_clients = atoi(argv[7]);

	if(debug == DEBUG_ALL || debug == DEBUG_IMPORTANT) {
		printf("\n[Raw Arguments: debug %s, arrival_seed %s, service_seed %s, rho %s, per_round %s, rounds %s, transient_clients %s]\n", 
			argv[1], argv[2], argv[3], argv[4], argv[5], argv[6], argv[7]);
		printf("[Inputs: debug %d, arrival_seed %u, service_seed %u, rho %f, per_round %d, rounds %d, transient_clients %d]\n\n", 
			debug, arrival_seed, service_seed, rho, clients_per_round, nrounds, transient_clients);
	}

	// quantidade de transientes padrao da tabela
	if(transient_clients == -1) {
		transient_clients = transient_table(rho);
	}

	long double last_arrival = 0.0;
	double lambda_arrival = rho / 2.0;
	double lambda_service = 1.0;

	if(debug == DEBUG_ALL || debug == DEBUG_IMPORTANT) {
		printf("\nTransient Clients %d\n", transient_clients);
	}

	QueueSystem queue_system(clients_per_round, transient_clients, lambda_arrival, lambda_service, debug);

	ExponentialGenerator arrival_generator(lambda_arrival);
	ExponentialGenerator service_generator(lambda_service);

	if(arrival_seed != -1)
		arrival_generator.set_deterministic_seed(arrival_seed);
	
	if(service_seed != -1)
		service_generator.set_deterministic_seed(service_seed);	
	
	clock_t start, end;
    double cpu_time_used;
	start = clock();

	// Executamos o simulador ate que todos os clientes tenham sido criados e finalizados
	long long int total_arrivals = 0;
	int clients = nrounds * clients_per_round;
	while(queue_system.finalized.size() < clients) {
		if(debug == DEBUG_ALL) printf("\n[Execute]");
		
		// Criamos apenas a proxima chegada a cada instante
		if(queue_system.queue1.size() <= 1 && queue_system.total_clients < clients){
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

	end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;

	if(debug == DEBUG_CLIENTS || debug == DEBUG_ALL) {
		printf("identifier,tm_arrival,tm_service1,tm_service2,tm_arrival_queue1,tm_start_service1,tm_end_service1,tm_arrival_queue2,tm_start_service2,tm_end_service2\n");
		for(auto client: queue_system.finalized) {
			print_client(client);
		}
	}

	printf("\nExecutado em %f segundos!\n", cpu_time_used);

    return 0;
}