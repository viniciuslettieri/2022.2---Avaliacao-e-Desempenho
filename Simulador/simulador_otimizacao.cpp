#include <vector>
#include <sstream>
#include <time.h>
#include "queue_system.cpp"

using namespace std;


bool valida_precision(QueueSystem &queue_system) {
    return (
        queue_system.statistics_handler.PrecisionNq1 < 0.05 &&
        queue_system.statistics_handler.PrecisionNq2 < 0.05 &&
        queue_system.statistics_handler.PrecisionN1 < 0.05 &&
        queue_system.statistics_handler.PrecisionN2 < 0.05 &&
        queue_system.statistics_handler.PrecisionW1 < 0.05 &&
        queue_system.statistics_handler.PrecisionW2 < 0.05 &&
        queue_system.statistics_handler.PrecisionX1 < 0.05 &&
        queue_system.statistics_handler.PrecisionX2 < 0.05 &&
        queue_system.statistics_handler.PrecisionT1 < 0.05 &&
        queue_system.statistics_handler.PrecisionT2 < 0.05 &&
        queue_system.statistics_handler.PrecisionVarW1 < 0.05 &&
        queue_system.statistics_handler.PrecisionVarW2 < 0.05
    );
}


int main(int argc, char* argv[]){
	// int debug = DEBUG_IMPORTANT;    
    int debug = NO_DEBUG;    

    double rho = 0.6;    
    int transient_clients = 20000; // pela tabela, a partir do rho = 0.6

    printf("clients,rounds,precision_ok,factor,tempo\n");

    for(int clients_per_round = 100; clients_per_round <= 10000; clients_per_round += 100) {
        for(int nrounds = 100; nrounds <= 10000; nrounds += 100) {

            if(clients_per_round * nrounds >= 300000) continue;

            long double last_arrival = 0.0;
            double lambda_arrival = rho / 2.0;
            double lambda_service = 1.0;

            // A forma que simulamos a fase transiente é atraves de uma simulacao com 1 cliente para K rounds
            // onde K é a quantidade de 'clientes transientes'.

            QueueSystem queue_system(clients_per_round, transient_clients, lambda_arrival, lambda_service, debug, false);

            ExponentialGenerator arrival_generator(lambda_arrival);
            ExponentialGenerator service_generator(lambda_service);

            clock_t start, end;
            double cpu_time_used;
            start = clock();

            // Executamos o simulador ate que todos os clientes tenham sido criados e finalizados
            long long int total_arrivals = 0;
            int clients = nrounds * clients_per_round;
            while(queue_system.finalized.size() < clients) {
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
            
            string precisao_ok = valida_precision(queue_system) ? "Ok" : "Nao Ok";

            printf("%d,%d,%s,%d,%.4f\n",clients_per_round, nrounds, precisao_ok.c_str(), clients_per_round*nrounds+transient_clients, cpu_time_used);
        }
    }

    return 0;
}