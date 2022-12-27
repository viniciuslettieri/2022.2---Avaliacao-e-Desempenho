#include <vector>
#include "queue_system.cpp"

using namespace std;

int main(){
	QueueSystem queue_system;

	long double last_arrival = 0.0;

	// sera retirado - deixa determinada a aleatoriedade
	// arrival_generator.set_deterministic_seed(100000);	// possui prioridade
	// arrival_generator.set_deterministic_seed(1);	// possui retry
	arrival_generator.set_deterministic_seed(1);	// sequencia longa
	service_generator.set_deterministic_seed(1);	


	int t = 200;
	while(t--) {
		if(queue_system.queue1.size() <= 1){
			Event next_event = generate_arrival(last_arrival);
			queue_system.add_queue1(next_event);
			last_arrival = next_event.tm_arrival;
		}

		queue_system.execute();
	}

	printf("identifier,tm_arrival,tm_service1,tm_service2,tm_arrival_queue1,tm_start_service1,tm_end_service1,tm_arrival_queue2,tm_start_service2,tm_end_service2\n");
	for(auto event: queue_system.finalized) {
		print_event(event);
	}

    return 0;
}