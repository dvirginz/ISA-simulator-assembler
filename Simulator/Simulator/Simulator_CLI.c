#include "Simulator.h"
int main(int argc, char* argv[])
{
	if (argc < 6) return 1;
	simulator_context* sim_ctx = create_simulator_context(argv[1], argv[2], argv[3], argv[4], argv[5]);
	if (!sim_ctx) {
		return 1;
	}
	if (start_simulator(sim_ctx) != SIM_SUCCESS) return 1;
	destroy_simulator_context(sim_ctx);
	return 0;
}

