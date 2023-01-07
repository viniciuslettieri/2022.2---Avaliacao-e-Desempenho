#include <vector>
#include <sstream>
#include "queue_system.cpp"

using namespace std;


bool valida_ic(QueueSystem &queue_system) {
    return (
        2*queue_system.statistics_handler.IntConfNq1 < 0.10 * queue_system.statistics_handler.AvgNq1 &&
        2*queue_system.statistics_handler.IntConfNq2 < 0.10 * queue_system.statistics_handler.AvgNq2 &&
        2*queue_system.statistics_handler.IntConfN1 < 0.10 * queue_system.statistics_handler.AvgN1 &&
        2*queue_system.statistics_handler.IntConfN2 < 0.10 * queue_system.statistics_handler.AvgN2 &&
        2*queue_system.statistics_handler.IntConfW1 < 0.10 * queue_system.statistics_handler.AvgW1 &&
        2*queue_system.statistics_handler.IntConfW2 < 0.10 * queue_system.statistics_handler.AvgW2 &&
        2*queue_system.statistics_handler.IntConfX1 < 0.10 * queue_system.statistics_handler.AvgX1 &&
        2*queue_system.statistics_handler.IntConfX2 < 0.10 * queue_system.statistics_handler.AvgX2 &&
        2*queue_system.statistics_handler.IntConfT1 < 0.10 * queue_system.statistics_handler.AvgT1 &&
        2*queue_system.statistics_handler.IntConfT2 < 0.10 * queue_system.statistics_handler.AvgT2 &&
        queue_system.statistics_handler.PrecisionNq1 < 0.05 &&
        queue_system.statistics_handler.PrecisionNq2 < 0.05 &&
        queue_system.statistics_handler.PrecisionN1 < 0.05 &&
        queue_system.statistics_handler.PrecisionN2 < 0.05 &&
        queue_system.statistics_handler.PrecisionW1 < 0.05 &&
        queue_system.statistics_handler.PrecisionW2 < 0.05 &&
        queue_system.statistics_handler.PrecisionX1 < 0.05 &&
        queue_system.statistics_handler.PrecisionX2 < 0.05 &&
        queue_system.statistics_handler.PrecisionT1 < 0.05 &&
        queue_system.statistics_handler.PrecisionT2 < 0.05
    );
}


int main(int argc, char* argv[]){
	int debug = DEBUG_IMPORTANT;    
    // int debug = NO_DEBUG;    

    // vector<double> rho_list = {0.2, 0.4, 0.6, 0.8, 0.9};
    vector<double> rho_list = {0.1};
    
    int clients_per_round = 1;
    int transient_clients = 100;

    for(auto rho: rho_list) {
        for(int nrounds = 30000; nrounds <= 30000; nrounds += 1000) {
        // for(int nrounds = 1000; nrounds <= 100000; nrounds += 1000) {

            // A forma que simulamos a fase transiente é atraves de uma simulacao com 1 cliente para K rounds
            // onde K é a quantidade de 'clientes transientes'.

            QueueSystem queue_system(clients_per_round, transient_clients, debug, false);

            long double last_arrival = 0.0;
            double lambda = rho / 2.0;
            
            ExponentialGenerator arrival_generator(lambda);
            ExponentialGenerator service_generator(1.0);

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

            // if(valida_ic(queue_system)) {
            //     printf("Rho %f, Total Transient Clients Simulated %10d: ok\n", rho, clients);
            //     break;
            // }
            // else{
            //     printf("Rho %f, Total Transient Clients Simulated %10d\n", rho, clients);
            // }
        }
    }

    return 0;
}