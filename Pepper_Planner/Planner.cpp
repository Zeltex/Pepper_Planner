#include "Planner.hpp"

namespace del {

	// TODO - Add option to specify for what person the goal must be fulfilled
	Policy Planner::find_policy(const Formula& goal_formula, const Action_Library& action_library, const State& initial_state) const {
		Graph graph;
		Node_Id root_node = graph.create_root_node(initial_state);
		graph.add_to_frontier(root_node);
		while (true) {
			print_graph(graph);
			if (graph.is_frontier_empty()) {
				return Policy(false);
			}

			Node_Id current_node = graph.get_next_from_frontier();
			if (is_goal_node(graph.get_node(current_node), goal_formula)) {
				propogate_solved_node(graph, current_node);
				if (graph.get_root_node().is_solved()) {
//#ifdef _DEBUG
//					std::ofstream myfile;
//					myfile.open("../Graph.dot");
//					myfile << graph.to_graph({ Agent({0}, "Pepper"), Agent({1}, "L") });
//					myfile.close();
//#endif


					return extract_policy(graph);
				} else {
					continue;
				}
			}

			bool found_applicable_action = false;
			for (Action action : action_library.get_actions()) {
				State temp_perspective_shift = perform_perspective_shift(graph.get_node(current_node).get_state(), action.get_owner());
				if (!is_action_applicable(temp_perspective_shift, action)) {
					continue;
				}

				State temp_product_update = perform_product_update(temp_perspective_shift, action);
				if (!is_valid_state(temp_product_update)) {
					continue;
				}
				else {
					found_applicable_action = true;
				}
				Node_Id action_node = graph.create_and_node(temp_product_update, current_node, action);

				std::vector<State> global_states = split_into_global_states(temp_product_update, action.get_owner());

				for (State state : global_states) {
					Node_Id global_agent_node = graph.create_or_node(state, action_node);
					graph.add_to_frontier(global_agent_node);
				}

			}
			if (!found_applicable_action) {
				propogate_dead_end_node(graph, current_node);
				if (graph.get_root_node().is_dead()) {
					return Policy(false);
				}
			}
		}
		return Policy(false);
	}


	Policy Planner::extract_policy(Graph& graph) const {
		print_graph(graph);
		Policy policy(true);

		std::deque<Node_Id> frontier;
		frontier.push_back(graph.get_root_node().get_id());
		while (!frontier.empty()) {
			auto current_node_id = frontier.back();
			frontier.pop_back();
			auto& current_node = graph.get_node(current_node_id);

			if (current_node.get_type() == Node_Type::And) {
				auto& parent = graph.get_node(current_node.get_parent());
				policy.add_policy_entry(parent.get_state(), current_node.get_action());
			}
			for (auto child_id : current_node.get_children()) {

				auto& child = graph.get_node(child_id);
				if (child.is_solved()) {
					frontier.push_back(child_id);
					if (current_node.get_type() == Node_Type::Or) {
						break;
					}
				}
			}
		}

		return policy;
	}

	void Planner::propogate_dead_end_node(Graph& graph, Node_Id node_id) const {
		auto& node = graph.get_node(node_id);
		node.set_dead();
		if (node.is_root_node()) {
			return;
		}
		Node_Id current_node_id = node.get_parent();
		while (true) {
			auto& current_node = graph.get_node(current_node_id);
			if (current_node.check_if_dead(graph)) {
				if (current_node.is_root_node()) {
					return;
				}
				current_node_id = current_node.get_parent();
			} else {
				break;
			}
		}
	}



	void Planner::propogate_solved_node(Graph& graph, Node_Id node_id) const {
		auto& node = graph.get_node(node_id);
		node.set_solved();
		if (node.is_root_node()) {
			return;
		}
		Node_Id current_node_id = node.get_parent();
		while (true) {
			auto& current_node = graph.get_node(current_node_id);
			if (current_node.check_if_solved(graph)) {
				if (current_node.is_root_node()) {
					return;
				}
				current_node_id = current_node.get_parent();
			}
			else {
				break;
			}
		}
	}

	bool Planner::is_goal_node(const Node& node, const Formula& goal_formula) const {
		return node.valuate(goal_formula);
	}

	bool Planner::is_valid_state(const State& state) const {
		return state.get_worlds_count() > 0 && state.get_designated_worlds_count() > 0;
	}

	bool Planner::is_root_node(const Node& node) const {
		return node.is_root_node();
	}

	const std::vector<Agent>& Planner::get_all_agents(const Action_Library& action_library) const {
		return action_library.get_agents();
	}

	void Planner::print_graph(const Graph& graph) const {
		std::cout << graph.to_string() << "\n\n\n\n\n" << std::endl;;
	}
}