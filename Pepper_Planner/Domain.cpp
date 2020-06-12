#include "Domain.hpp"

namespace del {
	bool Domain::find_policy() {
		Graph graph;
		while (true) {
			if (graph.is_frontier_empty()) {
				return false;
			}

			Node current_node = graph.get_next_from_frontier();
			if (is_goal_node(current_node)) {
				if (is_root_node(current_node)) {
					extract_policy();
					return true;
				}
				else {
					continue;
				}
			}

			bool found_applicable_action = false;
			for (Action action : get_all_actions()) {
				State temp = perform_product_update(current_node.get_state(), action);
				if (!is_valid_state(temp)) {
					continue;
				}
				else {
					found_applicable_action = true;
				}
				Node action_node(temp);
				current_node.add_child(action_node);

				for (Agent agent : get_all_agents()) {
					State perspective_shifted = perform_perspective_shift(action_node.get_state(), agent);
					std::vector<State> global_states = split_into_global_states(perspective_shifted);

					for (State state : global_states) {
						Node global_agent_node(state);
						action_node.add_child(global_agent_node);
						graph.add_to_frontier(action_node);
					}
				}
			}
			if (!found_applicable_action) {
				propogate_dead_end_node(current_node);
			}
		}
		return false;
	}

	Node Domain::get_next_from_frontier(std::vector<Node> frontier) {
		throw;
	}

	void Domain::extract_policy() {
		throw;
	}

	void Domain::propogate_dead_end_node(Node& node) {
		throw;
	}

	bool Domain::is_goal_node(Node& node) {
		throw;
	}

	bool Domain::is_valid_state(State state) {
		throw;
	}

	bool Domain::is_root_node(Node& node) {
		throw;
	}

	std::vector<Action> Domain::get_all_actions() {
		throw;
	}

	std::vector<Agent> Domain::get_all_agents() {
		throw;
	}


}