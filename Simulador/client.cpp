#include "exponential_generator.cpp"

enum State {
	st_waiting, 
	st_queue1, st_service1, 
	st_queue2, st_queue2_retry, st_service2, st_service2_retry,
	st_finished
};

long long int identifier = 1;

typedef struct {
    long long int identifier;
    int round_number;

	State state = st_waiting;

	long double tm_arrival = -1;                     // random generation
	long double tm_service1 = -1;                    // random generation
	long double tm_service2 = -1;                    // random generation

	long double tm_arrival_queue1 = -1;              // time of first arrival in queue 1
	long double tm_arrival_queue2 = -1;              // time of first arrival in queue 2
	long double tm_start_service1 = -1;              // start time for service 1
    long double tm_start_service2 = -1;              // start time for service 1 [first try]
	long double tm_accumulated_service2 = 0.0;       // total accumulated time for service 2
	long double tm_end_service1 = -1;                // finish time for service 1
	long double tm_end_service2 = -1;                // finish time for service 2
} Client;

Client generate_arrival(long double current_time, ExponentialGenerator &arrival_generator, ExponentialGenerator &service_generator) {
    Client client;
    client.tm_arrival = current_time + arrival_generator.get_random_value();
    client.tm_service1 = service_generator.get_random_value();
    client.tm_service2 = service_generator.get_random_value();
    client.identifier = identifier++;
    client.round_number = 0;
    return client;
}

void print_client(Client client) {
    // printf("\nClient %lld:\n", client.identifier);
    // printf("Arrival: %Lf\nService 1: %Lf\nService 2: %Lf\n", client.tm_arrival, client.tm_service1, client.tm_service2);
    // printf("Arrival in Queue 1: %Lf\nArrival in Service 1: %Lf\nEnd of Service 1: %Lf\n", client.tm_arrival_queue1, client.tm_start_service1, client.tm_end_service1);
    // printf("Arrival in Queue 2: %Lf\nEnd of Service 2: %Lf\n", client.tm_arrival_queue2, client.tm_end_service2);
    printf("%lld, %Lf, %Lf, %Lf, %Lf, %Lf, %Lf, %Lf, %Lf, %Lf\n", 
            client.identifier, client.tm_arrival, client.tm_service1, client.tm_service2,
            client.tm_arrival_queue1, client.tm_start_service1, client.tm_end_service1, 
            client.tm_arrival_queue2, client.tm_start_service2, client.tm_end_service2);
}