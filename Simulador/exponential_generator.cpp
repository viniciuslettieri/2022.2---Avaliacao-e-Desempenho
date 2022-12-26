#include <random>

class ExponentialGenerator {

	private:

	double lambda;
	unsigned int seed;
	std::mt19937 random_engine;
	std::exponential_distribution<double> distribution;

	void set_distribution_properties(double lambda) {
		this->distribution = std::exponential_distribution<double>(lambda);
	}

	void set_random_seed() {
		std::random_device rd;
		this->seed = rd();
		this->random_engine = std::mt19937(this->seed);
	}

	public:

	ExponentialGenerator(double lambda) {
		this->lambda = lambda;
		this->set_distribution_properties(lambda);
		this->set_random_seed();
	}

	void set_deterministic_seed(unsigned int seed) {
		this->random_engine = std::mt19937(seed);
	}

	double get_random_value() {
		return this->distribution(this->random_engine);
	}

	unsigned int get_current_seed() {
		return this->seed;
	}

};