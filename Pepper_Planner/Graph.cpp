#include "Graph.hpp"
#include "Node_Comparator.hpp"
#include "DEL_Operations.hpp"
#include "Core.hpp"

namespace del {


	Graph::Graph(size_t node_size, const State& state, Node_Comparator& history, Agent_Id planning_agent) {
		nodes.reserve(node_size); 
		auto root = create_root_node(state);
		history.insert(nodes.at(root.id));

		for (auto& global : split_into_global_states(state, planning_agent)) {
			auto node = create_or_node(std::move(global), root);
			history.insert(nodes.at(node.id));
			add_to_frontier(node);
		}
	}

	Node_Id Graph::get_next_from_frontier() {
		Node_Id node_id = frontier.top().id;
		frontier.pop();
		return node_id;
	}

	bool Graph::is_frontier_empty() const {
		return frontier.size() == 0;
	}

	Node_Id Graph::create_or_node(State state, Node_Id parent) {
		Node_Id node_id = Node_Id{ nodes.size() };
		nodes.emplace_back(state, node_id, parent, false);
		nodes.back().calculate_hash();
		nodes[parent.id].add_child(node_id);
		return node_id;
	}

	Node_Id Graph::create_and_node(State state, Node_Id parent, Action action_from_parent) {
		Node_Id node_id = Node_Id{ nodes.size() };
		nodes.emplace_back(state, node_id, parent, action_from_parent, false);
		nodes.back().calculate_hash();
		nodes[parent.id].add_child(node_id);
		return node_id;
	}

	Node_Id Graph::create_root_node(State state) {
		Node_Id node_id = Node_Id{ nodes.size() };
		// The dummy action makes sure root is and-node
		nodes.emplace_back(state, node_id, Node_Id{ 0 }, Action{ }, true);
		nodes.back().calculate_hash();
		root = node_id;
		return node_id;
	}

	void Graph::add_to_frontier(Node_Id node_id) {
		if (node_id.id < size_t{ 0 } || node_id.id >= size_t{ nodes.size() }) {
			throw;
		}
		frontier.emplace(node_id, nodes.at(node_id.id).get_cost());
	}

	void Graph::set_parent_child(Node_Id parent_id, Node_Id child_id, const Action& action) {
		get_node(child_id).add_parent(parent_id, action);
		get_node(parent_id).add_child(child_id);
	}

	void Graph::set_parent_child(Node_Id parent_id, Node_Id child_id) {
		get_node(child_id).add_parent(parent_id);
		get_node(parent_id).add_child(child_id);
	}

	const std::vector<Node>& Graph::get_const_nodes() const {
		return nodes;
	}

	std::vector<Node>& Graph::get_nodes() {
		return nodes;
	}

	const Node& Graph::get_const_node(Node_Id node_id) const {
		return nodes.at(node_id.id);
	}

	Node& Graph::get_node(Node_Id node_id) {
		return nodes.at(node_id.id);
	}

	Node& Graph::get_root_node() {
		return nodes.at(root.id);
	}

	std::string Graph::to_string(const Domain& domain) const {
		std::string result = "Graph: (root, " + std::to_string(root.id) + ") (frontier";
		result += ")";
		for (const auto& node : nodes) {
			result += "\n\n\n" + node.to_string(domain);
		}
		return result;
	}

	std::string Graph::to_graph(const Domain& domain) const {
		std::string result = "digraph G {\ncompound=true;";
		std::string connections;

		size_t counter = 0;
		for (auto& node : nodes) {
			result += "\n" + node.to_graph(std::to_string(counter), "s" + std::to_string(counter), domain);
			
			for (auto& child : node.get_children()) {
				std::string left = "s" + std::to_string(counter) + "0";
				std::string right = "s" + std::to_string(child.id) + "0";

				connections += "\n" + left + " -> " + right + "[ltail=cluster_" + std::to_string(counter) + ", lhead=cluster_" + std::to_string(child.id);

				if (nodes[child.id].get_type() == Node_Type::And) {
					connections += ",label=\"" + nodes[child.id].get_parent_action(node.get_id()).to_compact_string(domain) + "\"";
				}
					
				connections += "];";
			}
			counter++;
		}
		result += connections + "\n}";


		return result;
	}

	std::string Graph::to_graph_simple(const Domain& domain) const {
		std::string result = "digraph G {\ncompound=true;";
		std::string connections;
		size_t counter = 0;

		std::deque<size_t> frontier;
		std::unordered_set<size_t> visited_and;
		std::unordered_set<size_t> visited_or;

		frontier.push_back(root.id);
		visited_or.insert(nodes[root.id].get_state().to_hash());

		while (!frontier.empty()) {
			auto node_id = frontier.front();
			frontier.pop_front();
			auto& node = nodes[node_id];
			auto type_string = node.get_type() == Node_Type::And ? "And\n" : "Or\n";
			result += "\n n" + std::to_string(node.get_id().id) + " [label=\"" + type_string + std::to_string(node.get_id().id) + "\"];";

			for (auto& child : node.get_children()) {
				if (!nodes[child.id].is_solved()) continue;
				connections += "\n n" + std::to_string(node.get_id().id) + " -> n" + std::to_string(child.id) + ";";
				auto hash = nodes[child.id].get_state().to_hash();
				if (nodes[child.id].get_type() == Node_Type::And) {
					if (visited_and.find(hash) == visited_and.end()) {
						visited_and.insert(hash);
						frontier.push_back(child.id);
					}
				} else {
					if (visited_or.find(hash) == visited_or.end()) {
						visited_or.insert(hash);
						frontier.push_back(child.id);
					}
				}
			}
		}
		result += connections + "\n}";


		return result;
	}

	std::string Graph::to_partial_graph(const Domain& domain) const {
		std::string result = "digraph G {\ncompound=true;";
		std::string connections;

		size_t counter = 0;
		std::deque<size_t> frontier = { root.id };

		while (!frontier.empty()) {
			auto node_id = frontier.front();
			auto& node = nodes.at(node_id);
			counter = node_id;
			frontier.pop_front();
			result += "\n" + node.to_graph(std::to_string(counter), "s" + std::to_string(counter), domain);

			if (node.is_solved()) {
				for (auto& child : node.get_children()) {

					frontier.push_back(child.id);

					std::string left = "s" + std::to_string(counter) + "0";
					std::string right = "s" + std::to_string(child.id) + "0";

					connections += "\n" + left + " -> " + right + "[ltail=cluster_" + std::to_string(counter) + ", lhead=cluster_" + std::to_string(child.id);

					if (nodes[child.id].get_type() == Node_Type::And) {
						connections += ",label=\"" + nodes[child.id].get_parent_action(node.get_id()).to_compact_string(domain) + "\"";
					}

					connections += "];";
				}
			}
			counter++;
		}
		result += connections + "\n}";


		return result;
	}
}
