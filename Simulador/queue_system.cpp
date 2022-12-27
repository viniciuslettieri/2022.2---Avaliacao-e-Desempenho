#include <deque>
#include <vector>
#include "event.cpp"

class QueueSystem {

	public:

    std::deque<Event> queue1, queue2;       // back = end | front = start
    long double global_time;
    std::vector<Event> finalized;

    bool debug;

	QueueSystem(bool debug = false) {
		this->global_time = 0.0;
        this->debug = debug;
	}

    void add_queue1(Event event) {
        event.state = st_queue1;
        event.tm_arrival_queue1 = event.tm_arrival;
        this->queue1.push_back(event);

        if(this->debug) printf("Event %lld added to Queue 1 at %Lf.\n", event.identifier, event.tm_arrival_queue1);
    }

    void add_queue2(Event event) {
        event.state = st_queue2;
        event.tm_arrival_queue2 = this->global_time;
        this->queue2.push_back(event);

        if(this->debug) printf("Event %lld added to Queue 2 at %Lf.\n", event.identifier, event.tm_arrival_queue2);
    }

    void readd_queue2(Event event) {
        event.state = st_queue2_retry;
        this->queue2.push_front(event);

        if(this->debug) printf("Event %lld readded to Queue 2 at %Lf.\n", event.identifier, this->global_time);
    }

    Event remove_queue1() {
        Event event = this->queue1.front();
        this->queue1.pop_front();
        return event;

        if(this->debug) printf("Event %lld removed from Queue 1 in %Lf.\n", event.identifier, this->global_time);
    }

    Event remove_queue2() {
        Event event = this->queue2.front();
        this->queue2.pop_front();
        return event;

        if(this->debug) printf("Event %lld removed from Queue 2 in %Lf.\n", event.identifier, this->global_time);
    }

    Event execute_service1(Event event) {
        // Inicio do Servico
        event.state = st_service1;
        event.tm_start_service1 = this->global_time;

        // Finalizacao do Servico
        this->global_time += event.tm_service1;
        if(this->debug) printf("\n(gt %Lf)\n", this->global_time);
        event.tm_end_service1 = this->global_time;

        if(this->debug) printf("Event %lld executed 1 from %Lf until %Lf.\n", event.identifier, event.tm_start_service1, event.tm_end_service1);

        return event;
    }

    Event execute_service2(Event event) {
        // Inicio do Servico
        if(event.state == st_queue2) {      // primeira vez na fila 2
            event.state = st_service2;
            event.tm_start_service2 = this->global_time;
        } else {                            // outra vez na fila 2
            event.state = st_service2_retry;
        }

        long double remaining_service = event.tm_service2 - event.tm_accumulated_service2;

        // Se a proxima chegada ocorrer antes do servico atual terminar
        if(this->queue1.size() > 0 && this->queue1.front().tm_arrival <= this->global_time + remaining_service) {
            long double next_arrival = this->queue1.front().tm_arrival;

            printf("[next %Lf | accum %Lf | remaining %Lf | global %Lf | active %Lf]", 
                next_arrival, event.tm_accumulated_service2, remaining_service, this->global_time, next_arrival - this->global_time);
            
            long double active_time = next_arrival - this->global_time;
            event.tm_accumulated_service2 += active_time;
            this->global_time += active_time; 
            if(this->debug) printf("\n(gt %Lf)\n", this->global_time);
            event.state = st_queue2_retry;

            if(this->debug) printf("Event %lld executed service 2: %Lf/%Lf.\n", event.identifier, event.tm_accumulated_service2, event.tm_service2);
        
        // Se a proxima chegada na fila 1 nao interrompe o servico atual
        } else {
            printf("[accum %Lf | remaining %Lf | global %Lf]", 
                event.tm_accumulated_service2, remaining_service, this->global_time);

            event.tm_accumulated_service2 = event.tm_service2;
            event.tm_end_service2 = this->global_time + remaining_service;
            this->global_time += remaining_service; 
            if(this->debug) printf("\n(gt %Lf)\n", this->global_time);
            event.state = st_finished;

            if(this->debug) printf("Event %lld finished service 2 at %Lf.\n", event.identifier, event.tm_end_service2);
        }

        return event;
    }

    void execute() {
        Event event;

        // Se existe alguem na fila 1 que ja chegou de fato
        if(this->queue1.size() > 0 && this->queue1.front().tm_arrival <= this->global_time){
            if(this->debug) printf("\n[primeiro if]\n");
            event = this->remove_queue1();
            event = this->execute_service1(event);
            this->add_queue2(event);

        // Se nao existe ninguem na fila 1 mas existe na 2
        } else if(this->queue2.size() > 0) {
            if(this->debug) printf("\n[segundo if]\n");
            event = this->remove_queue2();
            event = this->execute_service2(event);
            if(event.state == st_finished) {
                if(this->debug) printf("\n[st_finished]\n");
                finalized.push_back(event);
            } else if(event.state == st_queue2_retry) {
                if(this->debug) printf("\n[st_queue2_retry]\n");
                this->readd_queue2(event);
            }

        // Se existe alguem na fila 1 que chega no futuro (ociosidade) - ajuste do tempo
        } else if(this->queue1.size()) {
            if(this->debug) printf("\n[terceiro if]\n");
            this->global_time = this->queue1.front().tm_arrival; 
            if(this->debug) printf("\n(gt %Lf)\n", this->global_time);
        }

        if(this->debug) {
            for(auto fin : this->finalized) {
                printf("%lld: %Lf\n", fin.identifier, fin.tm_end_service2);
            }
        }
    }

    long double get_global_time() {
        return this->global_time;
    }
};