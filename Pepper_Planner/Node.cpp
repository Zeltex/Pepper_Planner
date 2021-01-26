#include "Node.hpp"
#include "Graph.hpp"

namespace del {
	const State& Node::get_state() const {
		return state;;
	}

	Node_Id Node::get_id() const {
		return id;
	}

	void Node::add_child(Node_Id node) {
		for (auto& child : children) {
			if (child == node) return;
		}
		children.push_back(node);
	}

	void Node::add_parent(Node_Id node, Action action) {
		// TODO - Should maybe prioritise the parent with lowest action cost
		for (auto& parent : parents) {
			if (parent.first == node) return;
		}
		parents.emplace_back(node, action);
	}

	void Node::add_parent(Node_Id node) {
		// TODO - Should maybe prioritise the parent with lowest action cost
		for (auto& parent : parents) {
			if (parent.first == node) return;
		}
		parents.emplace_back(node, Action());
	}

	void Node::calculate_hash() {
		hash = state.to_hash();
	}

	size_t Node::get_hash() const {
		return hash;
	}

	const std::vector<std::pair<Node_Id, Action>>& Node::get_parents() const {
		return parents;
	}

	const std::vector<Node_Id>& Node::get_children() const {
		return children;
	}

	Node_Type Node::get_type() const {
		return type;
	}

	const Action& Node::get_parent_action(Node_Id parent) const {
		for (auto& parent_entry : parents) {
			if (parent_entry.first == parent) return parent_entry.second;
		}
		std::cerr << "Attempted to get parent action of node without parent action" << id.id << std::endl;
		exit(-1);
	}

	bool Node::is_root_node() const {
		return root;
	}

	bool Node::valuate(const Formula& formula, const Domain& domain) const {
		return state.valuate(formula, domain);
	}

	bool Node::is_dead() const {
		return dead;
	}

	bool Node::is_solved() const {
		return solved;
	}

	void Node::set_dead() {
		dead = true;
	}

	void Node::set_solved() {
		solved = true;
	}

	bool Node::check_if_dead(Graph& graph) {
		if (dead) {
			return true;
		}
		else if (solved) {
			return false;
		}

		if (type == Node_Type::And) {
			dead |= check_if_dead_and(graph);
			return dead;
		}
		else if (type == Node_Type::Or) {
			dead |= check_if_dead_or(graph);
			return dead;
		}
		else {
			dead = true;
			return true;
		}
	}
	bool Node::check_if_dead_and(Graph& graph) {
		for (auto child : children) {
			if (graph.get_node(child).is_dead()) {
				dead = true;
				return true;
			}
		}
		return false;
	}
	bool Node::check_if_dead_or(Graph& graph) {
		for (const auto& child : children) {
			if (!graph.get_node(child).is_dead()) {
				return false;
			}
		}
		dead = true;
		return true;
	}

	bool Node::check_if_solved(Graph& graph) {
		if (solved) {
			return true;
		}
		else if (dead) {
			return false;
		}

		if (type == Node_Type::And) {
			solved |= check_if_solved_and(graph);
			return solved;
		}
		else if (type == Node_Type::Or) {
			solved |= check_if_solved_or(graph);
			return solved;
		}
		else {
			solved = false;
			return false;
		}
	}
	bool Node::check_if_solved_or(Graph& graph) {
		for (const auto& child : children) {
			if (graph.get_node(child).is_solved()) {
				solved = true;
				return true;
			}
		}
		return false;
	}
	bool Node::check_if_solved_and(Graph& graph) {
		for (const auto& child : children) {
			if (!graph.get_node(child).is_solved()) {
				return false;
			}
		}
		solved = !children.empty();
		return solved;
	}

	size_t Node::get_cost() const {
		return state.get_cost();
	}

	std::string Node::to_string(const Domain& domain) const {
		std::string type_string = type_to_string();
		std::string parents_string;
		bool first = true;
		for (auto& parent : parents) {
			if (first) {
				first = false;
			} else {
				parents_string += ", ";
			}
			parents_string += std::to_string(parent.first.id);
		}


		std::string result = "---- Node " + std::to_string(id.id)
			+ ": (Type, " + type_string
			+ ") (Parent, " + parents_string
			+ ") (Root, " + (root ? "True" : "False")
			+ ") (Dead, " + (dead ? "True" : "False")
			+ ") (Solved, " + (solved ? "True" : "False")
			+ ") (Children, ";
		first = true;
		for (auto child : children) {
			if (first) {
				first = false;
			} else {
				result += ", ";
			}
			result += std::to_string(child.id);
		}

		result += ")";
		result += "\n--- hash: " + std::to_string(hash);
		result += "\n--- cost: " + std::to_string(state.get_cost());
		if (!this->is_root_node()) {
			for (auto& parent : parents) {
				result += "\n--- Action from parent " + std::to_string(parent.first.id) + "\n" + (type == Node_Type::Or ? "Agent split, no action" : parent.second.to_string(domain)) + " \n";
			}
		}
		result += state.to_string(domain);
		return result;
	}

	std::string Node::to_graph(const std::string node_id, const std::string state_id, const Domain& domain) const {
		std::string result = "subgraph cluster_" + node_id + " {\nlabel=\"(" + type_to_string() + ")Node " + node_id + "\";\n";
		if (solved) {
			result += "pencolor=green;\n";
		} else if (dead) {
			result += "pencolor=red;\n";
		}
		result += state.to_graph(state_id, domain) + "\n}";
		return result;
	}

	std::string Node::type_to_string() const {
		switch (type) {
		case Node_Type::And: return "And"; break;
		case Node_Type::Or: return "Or"; break;
		default: return "Unknown";
		}
	}
}