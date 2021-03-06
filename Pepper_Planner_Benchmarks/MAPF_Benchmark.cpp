#include "Run_Planner.hpp"
#include "DEL_Interface.hpp"
#include "Utils.hpp"

#include <chrono>
#include <vector>
#include <iostream>
#include <fstream>



namespace del {
	void run_mapf_benchmark(const std::string& folder) {
		std::string planning_agent = "a0";
		std::vector<long> times;
		for (size_t i = 1; i <= 18; ++i) {
			std::string file_name = folder + "p" + std::to_string(i) + ".maepl";
			DEL_Interface del_interface(file_name);
			auto time_start = std::chrono::high_resolution_clock::now();
			del_interface.create_policy(planning_agent, true);
			auto time_end = std::chrono::high_resolution_clock::now();

			times.push_back(std::chrono::duration_cast<std::chrono::milliseconds>(time_end - time_start).count());
			std::cout << "P" << i << " took ms " << times.back() << "\n" << std::endl;

			std::ofstream output;
			output.open("../Benchmarks/Benchmarks_mapfdu.csv");
			for (size_t j = 0; j < times.size(); ++j) {
				output << "P" << (j + 1) << ": " << times[j] << " ms" << std::endl;
			}
			output.close();
		}
	}

	void run_coin_flip_benchmark(const std::string& folder) {
		std::string planning_agent = "a0";
		std::vector<long> times;
		std::string base_file_name = "Coin_Flip_";
		for (size_t i = 1; i <= 18; ++i) {
			std::string file_name = folder + base_file_name + std::to_string(i) + ".maepl";
			DEL_Interface del_interface(file_name);
			auto time_start = std::chrono::high_resolution_clock::now();
			del_interface.create_policy(planning_agent, true);
			auto time_end = std::chrono::high_resolution_clock::now();

			times.push_back(std::chrono::duration_cast<std::chrono::milliseconds>(time_end - time_start).count());
			std::cout << base_file_name << i << " took ms " << times.back() << "\n" << std::endl;

			std::ofstream output;
			output.open("../Benchmarks/Benchmarks_Coin_Flip.csv");
			for (size_t j = 0; j < times.size(); ++j) {
				output << base_file_name << (j + 1) << ": " << times[j] << " ms" << std::endl;
			}
			output.close();
		}
	}

	void run_mapf_and_solve(const std::string& folder) {
		std::string planning_agent = "a0";
		std::vector<long> times;
		for (size_t i = 1; i <= 18; ++i) {
			std::string file_name = folder + "p" + std::to_string(i) + ".maepl";
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
	}

	void run_benchmark(const std::string& folder, const std::string& base_file_name, size_t start_index, size_t end_index) {
		std::string planning_agent = "a0";
		std::vector<long> times;
		std::string benchmark_file_name = get_benchmark_file_name();
		for (size_t i = start_index; i <= end_index; ++i) {
			std::string file_name = folder + base_file_name + std::to_string(i) + ".maepl";
			DEL_Interface del_interface(file_name);
			auto time_start = std::chrono::high_resolution_clock::now();
			del_interface.create_policy(planning_agent, true);
			auto time_end = std::chrono::high_resolution_clock::now();

			times.push_back(std::chrono::duration_cast<std::chrono::milliseconds>(time_end - time_start).count());
			std::cout << base_file_name << i << " took ms " << times.back() << "\n\n";

			std::ofstream output;
			output.open(benchmark_file_name);
			for (size_t j = 0; j < times.size(); ++j) {
				output << base_file_name << (j + 1) << ": " << times[j] << " ms\n";
			}
			output.close();
		}
	}
}
