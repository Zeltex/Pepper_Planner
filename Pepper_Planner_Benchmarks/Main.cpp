#include <string>
#include <math.h>
#include <unordered_set>
#include <iostream>
#include <sstream>
#include <fstream>
#include <chrono>
#include <ctime>    
#include <numeric>

//#include "../Domain_Loader/Loader.hpp"
#include "Loader.hpp"
#include "Domain_Interface_Implementation.hpp"
#include "Domain.hpp"
#include "Action_Library.hpp"
#include "Run_Planner.hpp"
#include "MAPF_Benchmark.hpp"


using namespace del;




std::string get_date_stamp() {
	struct tm ltm;
	time_t now = time(0);
	auto temp = localtime_s(&ltm ,&now);
	std::string output;
	bool first = true;
	for (const auto& entry : { ltm.tm_year - 100, ltm.tm_mon + 1, ltm.tm_mday, ltm.tm_hour, ltm.tm_min, ltm.tm_sec }) {
		if (first) {
			first = false;
		} else {
			output += "-";
		}
		if (entry < 10) output += "0";
		output += std::to_string(entry);
	}
	return output;
}
std::string get_benchmark_file_name(std::string date_stamp) {
	return "../Benchmarks/Benchmarks_" + date_stamp + ".csv";
}
std::string get_benchmark_file_name() {
	return get_benchmark_file_name(get_date_stamp());
}

class Simp_Timer {
public:
	Simp_Timer() : time_start(std::chrono::high_resolution_clock::now()) {}
	long get_time() {
		auto time_end = std::chrono::high_resolution_clock::now();
		return std::chrono::duration_cast<std::chrono::milliseconds>(time_end - time_start).count();
	}
private:
	std::chrono::steady_clock::time_point time_start;
};

struct Log_Entry {
	std::string descr;
	long size;
	long time;
	Log_Entry(const std::string& descr, long size, long time) : descr(descr), size(size), time(time) { };

	const std::string to_string() const {
		return descr + delim + std::to_string(size) + delim + std::to_string(time);
	}
private:
	char delim = ';';
};

class Logger {
public:

	Logger(const std::string& title) : title(title), date_stamp(get_date_stamp()), entries() {}
	
	void add_entry(const std::string& descr, long size, long time) {
		entries.emplace_back(descr, size, time);
	}

	std::string to_string() {
		std::ostringstream output;
		for (const auto& entry : entries) {
			output << entry.to_string() << "\n";
		}
		return output.str();
	}

	void save() {
		std::ofstream output;
		output.open("../Benchmarks/Benchmarks_" + date_stamp + ".csv");
		output << title << "\n" << "Description;Size;Time(ms)\n\n";
		output << to_string();
		output.close();
	}


private:
	std::string title;
	std::vector<Log_Entry> entries;
	std::string date_stamp;
};

std::vector<State> get_states(const std::string file_path, const int action_depth, Logger& logger) {

	Loader loader;
	Domain_Interface_Implementation domain_interface;
	loader.parse(&domain_interface, file_path);

	auto [domain, action_library, goal] = domain_interface.get_loaded();

	const size_t action_count = action_library.get_actions().size();

	std::vector<State> states = { domain.get_current_state() };
	states.reserve(std::pow(action_count, 2) * action_depth); // Estimate size
	std::unordered_set<size_t> hashes;

	Simp_Timer timer;
	for (size_t i = 0; i < action_depth; i++) {
		const size_t temp_state_size = states.size();
		for (size_t k = 0; k < temp_state_size; k++) {
			const auto current_state = states.at(k);
			action_library.load_actions(current_state, domain);
			while (action_library.has_action()) {
				const Action& action = action_library.get_next_action();
				State state_perspective_shift = perform_perspective_shift(current_state, action.get_owner());
				if (!is_action_applicable(state_perspective_shift, action, domain)) {
					continue;
				}
				State state_product_update = perform_product_update(state_perspective_shift, action, domain.get_agents(), domain);
				size_t hash = state_product_update.to_hash();
				if (hashes.find(hash) == hashes.end()) {
					hashes.insert(hash);
					states.push_back(state_product_update);
				}

			}
		}
	}
	logger.add_entry("Initial states size", states.size(), timer.get_time());
	return states;
}

std::tuple<Loader, Domain, Action_Library, std::vector<State>> get_stuff(const std::string& file_path) {
	Loader loader;
	Domain_Interface_Implementation domain_interface;
	loader.parse(&domain_interface, file_path);
	auto [domain, action_library, goal] = domain_interface.get_loaded();
	const size_t action_count = action_library.get_actions().size();
	Agent_Id dummy_agent = { 0 };
	std::vector<State> states = split_into_global_states(domain.get_current_state(), dummy_agent);
	return { loader, domain, action_library, states };
}

//std::vector<State> get_states_using_globals(const std::string file_path, const int action_depth, Logger& logger) {
std::vector<State> get_states_using_globals_phase_times(const std::vector<State>& states, Action_Library& action_library, Domain& domain, Logger& logger, bool use_contraction) {

	Agent_Id dummy_agent = { 0 };
	Simp_Timer timer;
	std::unordered_set<size_t> hashes;
	std::vector<State> result;
	result.reserve(states.size() * action_library.get_actions().size() / 2);
	long total_times[5] = { 0,0,0,0,0 };
	long time_new_state = 0;
	long time_old_state = 0;
	//for (size_t i = 0; i < action_depth; i++) {
	const size_t temp_state_size = states.size();
	for (size_t k = 0; k < temp_state_size; k++) {
		const auto current_state = states.at(k);
		action_library.load_actions(current_state, domain);
		while (action_library.has_action()) {
			const Action& action = action_library.get_next_action();
			std::chrono::steady_clock::time_point temp_times[6];
			temp_times[0] = std::chrono::high_resolution_clock::now();
			State state_perspective_shift = perform_perspective_shift(current_state, action.get_owner());
			temp_times[1] = std::chrono::high_resolution_clock::now();
			if (!is_action_applicable(state_perspective_shift, action, domain)) {
				continue;
			}
			temp_times[2] = std::chrono::high_resolution_clock::now();
			State state_product_update = perform_product_update(state_perspective_shift, action, domain.get_agents(), domain);
			temp_times[3] = std::chrono::high_resolution_clock::now();
			auto globals = split_into_global_states(state_product_update, dummy_agent);
			temp_times[4] = std::chrono::high_resolution_clock::now();
			bool found_new_state = false;
			for (auto& state : globals) {
				if (use_contraction) {
					state.remove_unreachable_worlds();
					state = perform_k_bisimilar_contraction(std::move(state), BISIMILAR_DEPTH);
				}
				size_t hash = state.to_hash();
				if (hashes.find(hash) == hashes.end()) {
					hashes.insert(hash);
					result.push_back(state);
					found_new_state = true;
				}
			}
			temp_times[5] = std::chrono::high_resolution_clock::now();
			for (size_t i = 1; i < 6; ++i) {
				total_times[i - 1] += std::chrono::duration_cast<std::chrono::microseconds>(temp_times[i] - temp_times[i - 1]).count();
			}
			auto temp_time = std::chrono::duration_cast<std::chrono::microseconds>(temp_times[5] - temp_times[0]).count();
			if (found_new_state) {
				time_new_state += temp_time;
			} else {
				time_old_state += temp_time;
			}
		}
	}
	//}
	long total_time = 0;
	total_time = std::accumulate(std::begin(total_times), std::end(total_times), total_time);
	for (size_t i = 0; i < 5; ++i) {
		std::cout << "Phase " << i << " took 'us: " << total_times[i] << " = " << (100.0 * total_times[i] / total_time) << "%\n";
	}
	std::cout << "Spent 'us on new states: " << time_new_state << std::endl;
	std::cout << "Spent 'us on old states: " << time_old_state << std::endl;
	std::cout << "Spent " << (100 * (0.0 + time_old_state) / (time_new_state + time_old_state)) << "% time on old states" << "\n";

	logger.add_entry("Initial states size", states.size(), timer.get_time());
	return result;
}


//std::vector<State> get_states_using_globals(const std::string file_path, const int action_depth, Logger& logger) {
std::tuple<std::vector<State>, long> get_states_using_globals(const std::vector<State>& states, Action_Library& action_library, Domain& domain, Logger& logger, bool use_contraction) {

	Agent_Id dummy_agent = { 0 };
	Simp_Timer timer;
	std::unordered_set<size_t> hashes;
	std::vector<State> result;
	result.reserve(states.size() * action_library.get_actions().size() / 2);
	const size_t temp_state_size = states.size();
	for (size_t k = 0; k < temp_state_size; k++) {
		const auto& current_state = states.at(k);
		action_library.load_actions(current_state, domain);
		while (action_library.has_action()) {
			const Action& action = action_library.get_next_action();
			State state_perspective_shift = perform_perspective_shift(current_state, action.get_owner());
			if (!is_action_applicable(state_perspective_shift, action, domain)) {
				continue;
			}
			State state_product_update = perform_product_update(state_perspective_shift, action, domain.get_agents(), domain);
			auto globals = split_into_global_states(state_product_update, dummy_agent);
			for (State& state : globals) {
				if (use_contraction) {
					state.remove_unreachable_worlds();
					state = perform_k_bisimilar_contraction(std::move(state), BISIMILAR_DEPTH);
				}

				size_t hash = state.to_hash();
				if (hashes.find(hash) == hashes.end()) {
					hashes.insert(hash);
					result.push_back(state);
				}
			}
		}
	}
	logger.add_entry((std::string("Initial states size") + (use_contraction ? " with contraction" : "")), states.size(), 0);
	logger.add_entry((std::string("Hashes") + (use_contraction ? " with contraction" : "")), result.size(), timer.get_time());
	return { result, timer.get_time() };
}

void benchmark1_calculations(std::vector<State>& states, Logger& logger) {
	bool test_raw = false, test_raw_con = true, test_glob = true, test_glob_con = true;
	// After bisimilar contraction
	if (test_raw_con) {
		std::vector<State> contracted_states;
		contracted_states.reserve(states.size());
		Simp_Timer timer_con;
		std::transform(states.begin(), states.end(), std::back_inserter(contracted_states),
			[](State state) -> State {return perform_k_bisimilar_contraction(state, BISIMILAR_DEPTH); });
		logger.add_entry("Perform bisimilar contraction", contracted_states.size(), timer_con.get_time());


		std::unordered_set<size_t> contracted_hashes;
		contracted_hashes.reserve(states.size());
		Simp_Timer timer_hash;
		std::transform(contracted_states.begin(), contracted_states.end(), std::inserter(contracted_hashes, contracted_hashes.begin()),
			[](State state) -> size_t { return state.to_hash(); });
		//std::cout << "States after contraction: " << contracted_states.size() << ", hashes of contracted: " << contracted_hashes.size() << "\n";
		logger.add_entry("Hashes of contracted states", contracted_hashes.size(), timer_hash.get_time());

	}

	// Raw globals
	std::vector<State> global_states;
	if (test_glob) {
		Agent_Id dummy_agent = { 0 };
		std::unordered_set<size_t> global_hashes;
		global_states.reserve(states.size() * states[0].get_designated_worlds_count());
		Simp_Timer timer_globals;
		for (const auto& state : states) {
			for (const auto& entry : split_into_global_states(state, dummy_agent)) {
				auto temp_hash = entry.to_hash();
				if (global_hashes.find(temp_hash) == global_hashes.end()) {
					global_states.push_back(entry);
					global_hashes.insert(temp_hash);
				}

			}
		}
		logger.add_entry("Getting globals and hashing", global_hashes.size(), timer_globals.get_time());
		//std::cout << "States after globals: " << global_states.size() << ", hashes: " << global_hashes.size() << "\n";

		//std::transform(states.begin(), states.end(), std::back_inserter(global_states),
			//[](State state) -> State {return perform_k_bisimilar_contraction(state, BISIMILAR_DEPTH); });
	}

	if (test_glob_con) {
		std::unordered_set<size_t> glob_con_hashes;
		glob_con_hashes.reserve(global_states.size());
		Simp_Timer timer_glob_con;
		std::transform(global_states.begin(), global_states.end(), std::inserter(glob_con_hashes, glob_con_hashes.begin()),
			[](State state) -> size_t { return state.to_hash(); });
		logger.add_entry("Hashes of contracted global states", glob_con_hashes.size(), timer_glob_con.get_time());
		//std::cout << "States after contraction: " << global_states.size() << ", hashes of contracted: " << glob_con_hashes.size() << "\n";
		// TODO - These are not contracted yet
	}

	__debugbreak;
}

void benchmark1(const std::string& file_path, const size_t action_depth) {
	auto [loader, domain, action_library, states] = get_stuff(file_path);

	Logger logger(file_path);
	for (size_t i = 1; i <= action_depth; ++i) {
		std::cout << "Starting " << i << "\n";
		logger.add_entry("Action depth " + std::to_string(i), 0, 0);
		auto [temp_states, time] = get_states_using_globals(states, action_library, domain, logger,false);
		states = std::move(temp_states);
		if (states.empty()) {
			__debugbreak;
		}
		benchmark1_calculations(states, logger);
		logger.save();
	}
	logger.save();
}

void benchmark2(const std::string& file_path, const size_t action_depth) {
	auto [loader, domain, action_library, states] = get_stuff(file_path);
	auto normal_states = states;
	auto contracted_states = states;

	Logger logger(file_path);
	for (size_t i = 1; i <= action_depth; ++i) {
		std::cout << "Starting " << i << "\n";
		logger.add_entry("Action depth " + std::to_string(i), 0, 0);
		normal_states = get_states_using_globals_phase_times(normal_states, action_library, domain, logger, false);
		contracted_states = get_states_using_globals_phase_times(contracted_states, action_library, domain, logger, true);
		logger.save();
	}
	logger.save();
}

void log_data(const std::vector<std::vector<long>>& data, const std::vector<std::string>& data_descriptions, const std::string& output_file_name, const std::string& title) {
	std::string output;
	for (size_t j = 0; j < data.size(); ++j) {
		output += data_descriptions[j];
		for (auto& entry : data[j]) {
			output += ", " + std::to_string(entry);
		}
		output += '\n';
	}
	std::ofstream stream;
	stream.open(output_file_name);
	stream << title << '\n';
	stream << output;
	stream.close();
}

/**
Generating all possible states to specific action depth and recording amount of hashes with and without contraction along with timing
*/
void benchmark3(const std::string& file_path, const size_t action_depth) {
	auto [loader, domain, action_library, states] = get_stuff(file_path);
	auto normal_states = states;
	auto contracted_states = states;
	const std::string title = file_path + " Hashes with and without contraction, and calculation time";
	std::vector<std::vector<long>> data(4);
	std::vector<std::string> data_descriptions = { "Baseline hashes", "Contracted hashes", "Baseline time", "Contracted time" };
	auto output_file_name = get_benchmark_file_name();

	Logger logger(file_path);
	for (size_t i = 1; i <= action_depth; ++i) {
		std::cout << "Starting " << i << "\n";
		logger.add_entry("Action depth " + std::to_string(i), 0, 0);
		auto [temp_normal_states, normal_time] = get_states_using_globals(normal_states, action_library, domain, logger, false);
		auto [temp_contracted_states, contracted_time] = get_states_using_globals(contracted_states, action_library, domain, logger, true);

		std::cout << "\nPrinting contracted states at depth " << i << std::endl;
		for (auto& entry : temp_contracted_states) {
			std::cout << entry.to_string(domain) << std::endl;
		}

		normal_states = std::move(temp_normal_states);
		contracted_states = std::move(temp_contracted_states);
		data[0].emplace_back(normal_states.size());
		data[1].emplace_back(contracted_states.size());
		data[2].emplace_back(normal_time);
		data[3].emplace_back(contracted_time);
		log_data(data, data_descriptions, output_file_name, title);
		//logger.save();
	}
	//logger.save();
}



int main(int argc, char* argv[]) {

	//auto file_path = "../Examples/Block_Search.maepl";
	//auto file_path = "../Examples/Block_Search3.maepl"; 
	//auto file_path = "../Examples/Second_Order.maepl";
	//auto file_path = "../Examples/Sally_Anne.maepl";
	//auto file_path = "../Examples/False_Belief_Synthesis.maepl";
	//auto file_path = "../Examples/Dice5-3.maepl";
	//auto file_path = "../Examples/MAPF/p12.maepl";
	//auto file_path = "../Examples/Coin_Flip.maepl";
	//benchmark1(file_path, 3);
	//benchmark3(file_path, 100);



	//execute_second_order();
//execute_test_case();


//find_and_execute("Simple.maepl", "Pepper");
//find_and_execute("Stack.maepl", "P");
//find_and_execute("False_Belief_Synthesis.maepl", "P");
//find_and_execute("Block_Search.maepl", "R");
	//find_and_execute("MAPFDU.maepl", "R");
	//find_and_execute("Block_Search_Single.maepl", "R");

	//find_and_execute("MAPF/p7.maepl", "a0");
	//find_and_execute("Thorsten_Domains/p1.maepl", "a0");

	run_mapf_benchmark();
	//run_mapf_and_solve();

	__debugbreak;
	return 0;
}