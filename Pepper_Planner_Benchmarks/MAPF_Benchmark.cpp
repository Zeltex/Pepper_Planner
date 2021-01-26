#include "Run_Planner.hpp"
#include "DEL_Interface.hpp"

#include <chrono>
#include <vector>
#include <iostream>
#include <fstream>



namespace del {
	void run_mapf_benchmark() {
		std::string planning_agent = "a0";
		std::vector<long> times;
		for (size_t i = 1; i <= 1; ++i) {
			std::string file_name = "../Examples/MAPF/p" + std::to_string(i) + ".maepl";
			DEL_Interface del_interface(file_name);
			auto time_start = std::chrono::high_resolution_clock::now();
			del_interface.create_policy(planning_agent, true);
			auto time_end = std::chrono::high_resolution_clock::now();

			times.push_back(std::chrono::duration_cast<std::chrono::milliseconds>(time_end - time_start).count());
			std::cout << "P" << i << " took ms " << times.back() << "\n\n";

			std::ofstream output;
			output.open("../Benchmarks/Benchmarks_mapfdu.csv");
			for (size_t j = 0; j < times.size(); ++j) {
				output << "P" << (j + 1) << ": " << times[j] << " ms\n";
			}
			output.close();
		}
		__debugbreak;
	}

	void run_mapf_and_solve() {
		std::string planning_agent = "a0";
		std::vector<long> times;
		for (size_t i = 1; i <= 15; ++i) {
			std::string file_name = "../Examples/MAPF/p" + std::to_string(i) + ".maepl";
			DEL_Interface del_interface(file_name);
			auto time_start = std::chrono::high_resolution_clock::now();
			del_interface.create_policy(planning_agent, false);
			auto time_end = std::chrono::high_resolution_clock::now();

			times.push_back(std::chrono::duration_cast<std::chrono::milliseconds>(time_end - time_start).count());
			std::cout << "P" << i << " took ms " << times.back() << "\n\n";

			execute(del_interface);

			std::ofstream output;
			output.open("../Benchmarks/Benchmarks_mapfdu.csv");
			for (size_t j = 0; j < times.size(); ++j) {
				output << "P" << (j + 1) << ": " << times[j] << " ms\n";
			}
			output.close();
		}
		__debugbreak;
	}
}