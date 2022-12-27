#include <deque>
#include <vector>
#include "event.cpp"

class QueueSystem {

	public:

    std::deque<Event> queue1, queue2;       // back = end | front = start
    long double global_time;
    std::vector<Event> finalized;

	QueueSystem() {
		this->global_time = 0.0;
	}

    void add_queue1(Event event) {
        event.state = st_queue1;
        // atualiza o tempo do evento com o global
        event.tm_arrival_queue1 = event.tm_arrival;
        // adiciona na estrutura de fila 1
        this->queue1.push_back(event);

        printf("Event %lld added to Queue 1 at %Lf.\n", event.identifier, event.tm_arrival_queue1);
    }

    void add_queue2(Event event) {
        event.state = st_queue2;
        // atualiza o tempo do evento com o global
        event.tm_arrival_queue2 = this->global_time;
        // adiciona na estrutura de fila 2
        this->queue2.push_back(event);

        printf("Event %lld added to Queue 2 at %Lf.\n", event.identifier, event.tm_arrival_queue2);
    }

    void readd_queue2(Event event) {
        event.state = st_queue2_retry;
        // readiciona na estrutura de fila
        this->queue2.push_front(event);

        printf("Event %lld readded to Queue 2 at %Lf.\n", event.identifier, this->global_time);
    }

    Event remove_queue1() {
        Event event = this->queue1.front();
        this->queue1.pop_front();
        return event;

        printf("Event %lld removed from Queue 1 in %Lf.\n", event.identifier, this->global_time);
    }

    Event remove_queue2() {
        Event event = this->queue2.front();
        this->queue2.pop_front();
        return event;

        printf("Event %lld removed from Queue 2 in %Lf.\n", event.identifier, this->global_time);
    }

    Event execute_service1(Event event) {
        event.state = st_service1;
        // atualiza o tempo do evento com o global
        event.tm_start_service1 = this->global_time;
        // adiciona na estrutura de servico 1

        // obtem tempo de servico
        // atualiza o tempo global
        this->global_time += event.tm_service1;
        printf("\n(gt %Lf)\n", this->global_time);
        event.tm_end_service1 = this->global_time;
        // remove_queue1

        printf("Event %lld executed 1 from %Lf until %Lf.\n", event.identifier, event.tm_start_service1, event.tm_end_service1);

        return event;
    }

    Event execute_service2(Event event) {
        if(event.state == st_queue2) {
            event.state = st_service2;

            event.tm_start_service2 = this->global_time;
        } else {
            event.state = st_service2_retry;
        }

        long double next_arrival = this->queue1.front().tm_arrival;
        long double remaining_service = event.tm_service2 - event.tm_accumulated_service2;
        
        // se a proxima chegada ocorrer antes do servico atual terminar
        if(next_arrival <= this->global_time + remaining_service) {
            long double active_time = next_arrival - this->global_time;
            printf("[next arrival %Lf | remaining service %Lf | active time %Lf]", next_arrival, remaining_service, active_time);
            event.tm_accumulated_service2 += active_time;
            this->global_time += active_time;
            printf("\n(gt %Lf)\n", this->global_time);
            event.state = st_queue2_retry;

            printf("Event %lld executed service 2: %Lf/%Lf.\n", event.identifier, event.tm_accumulated_service2, event.tm_service2);
        } else {
            event.tm_accumulated_service2 = event.tm_service2;
            event.tm_end_service2 = this->global_time + remaining_service;
            this->global_time += remaining_service;
            printf("\n(gt %Lf)\n", this->global_time);
            event.state = st_finished;

            printf("Event %lld finished service 2 at %Lf.\n", event.identifier, event.tm_end_service2);
        }

        return event;
    }

    void execute() {
        Event event;

        // Se existe alguem na fila 1 que ja chegou de fato
        if(this->queue1.size() > 0 && this->queue1.front().tm_arrival <= this->global_time){
            printf("\n[primeiro if]\n");
            event = this->remove_queue1();
            event = this->execute_service1(event);
            this->add_queue2(event);

        // Se nao existe ninguem na fila 1 mas existe na 2
        } else if(this->queue2.size() > 0) {
            printf("\n[segundo if]\n");
            event = this->remove_queue2();
            event = this->execute_service2(event);
            if(event.state == st_finished) {
                printf("\n[st_finished]\n");
                finalized.push_back(event);
            } else if(event.state == st_queue2_retry) {
                printf("\n[st_queue2_retry]\n");
                this->readd_queue2(event);
            }

        // Se existe alguem na fila 1 que chega no futuro (ociosidade)
        } else if(this->queue1.size()) {
            printf("\n[terceiro if]\n");
            this->global_time = this->queue1.front().tm_arrival;
            printf("\n(gt %Lf)\n", this->global_time);
        }
    }

    long double get_global_time() {
        return this->global_time;
    }
};