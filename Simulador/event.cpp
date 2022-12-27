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
} Event;

double rho = 0.5;
double lambda = rho / 2.0;
ExponentialGenerator arrival_generator(lambda);

ExponentialGenerator service_generator(1.0);

Event generate_arrival(long double current_time) {
    Event event;
    event.tm_arrival = current_time + arrival_generator.get_random_value();
    event.tm_service1 = service_generator.get_random_value();
    event.tm_service2 = service_generator.get_random_value();
    event.identifier = identifier++;
    return event;
}

void print_event(Event event) {
    // printf("\nEvent %lld:\n", event.identifier);
    // printf("Arrival: %Lf\nService 1: %Lf\nService 2: %Lf\n", event.tm_arrival, event.tm_service1, event.tm_service2);
    // printf("Arrival in Queue 1: %Lf\nArrival in Service 1: %Lf\nEnd of Service 1: %Lf\n", event.tm_arrival_queue1, event.tm_start_service1, event.tm_end_service1);
    // printf("Arrival in Queue 2: %Lf\nEnd of Service 2: %Lf\n", event.tm_arrival_queue2, event.tm_end_service2);
    printf("%lld, %Lf, %Lf, %Lf, %Lf, %Lf, %Lf, %Lf, %Lf, %Lf\n", 
            event.identifier, event.tm_arrival, event.tm_service1, event.tm_service2,
            event.tm_arrival_queue1, event.tm_start_service1, event.tm_end_service1, 
            event.tm_arrival_queue2, event.tm_start_service2, event.tm_end_service2);
}