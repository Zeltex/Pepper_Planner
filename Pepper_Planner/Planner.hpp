#pragma once

#include <vector>
//#include <iostream>
//#include <fstream>

#include "Node.hpp"
#include "Action.hpp"
#include "State.hpp"
#include "DEL_Operations.hpp"
#include "Agent.hpp"
#include "Graph.hpp"
#include "Types.hpp"
#include "Formula.hpp"
#include "Action_Library.hpp"
#include "Policy.hpp"


namespace del {

	class Planner {
	public:
		Policy find_policy(const Formula& formula, const Action_Library& action_library, const State& initial_state) const;
	private:
		Policy extract_policy(Graph& graph) const;
		void propogate_dead_end_node(Graph& graph, Node_Id node) const;
		void propogate_solved_node(Graph& graph, Node_Id node_id) const;
		bool is_goal_node(const Node& node, const Formula& goal_formula) const;
		bool is_root_node(const Node& node) const;
		bool is_valid_state(const State& state) const;
		const std::vector<Agent>& get_all_agents(const Action_Library& action_library) const;
		void print_graph(const Graph& graph) const;
	};
}