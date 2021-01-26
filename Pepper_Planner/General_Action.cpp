#include "General_Action.hpp"
#include "Action.hpp"
#include "Domain.hpp"
#include "Formula_Core.hpp"

namespace del {

	void General_Action::set_cost(size_t cost) {
		this->cost = cost;
	}

	void General_Action::set_owner(std::string type, Atom_Id name) {
		owner = { type, name };
	}

	void General_Action::set_name(std::string name) {
		this->name = name;
	}

	void General_Action::set_designated_events(std::vector<std::string> designated_events) {
		this->designated_events = designated_events;
	}

	void General_Action::set_action_input(std::vector<std::pair<std::string, std::string>>&& inputs) {
		this->inputs = inputs;
	}

	void General_Action::create_event(std::string name, Formula&& preconditions, const Propositions& add_list, const Propositions& delete_list) {
		Event_Id id = { events.size() };
		event_to_id.insert({ name, id });
		events.emplace_back(name, id, std::move(preconditions), add_list, delete_list);
	}

	void General_Action::add_edge_condition(Atom_Id agent, std::vector<Edge_Condition>&& edge_conditions_input) {
		edge_conditions.insert({ agent.id, std::move(edge_conditions_input) });
	} 

	size_t General_Action::get_cost() const {
		return cost;
	}


	const std::vector<std::pair<std::string, std::string>>& General_Action::get_inputs() const {
		return inputs;
	}

	std::pair<std::string, Atom_Id> General_Action::get_owner() const{
		return owner;
	}

	std::string General_Action::get_name() const {
		return name;
	}

	const std::vector<std::string>& General_Action::get_designated_events() const {
		return designated_events;
	}

	const std::vector<Action_Event>& General_Action::get_events() const {
		return events;
	}

	const std::unordered_map<size_t, std::vector<Edge_Condition>>& General_Action::get_edge_conditions() const {
		return edge_conditions;
	}

	std::unordered_map<size_t, std::vector<Agent>> General_Action::get_condition_owner_to_agent(const Domain& domain, const std::vector<Atom_Id>& arguments) const{
		std::unordered_map<size_t, std::vector<Agent>> condition_owner_to_agent;
		std::unordered_set<std::string> seen_agents;

		for (auto& edge : edge_conditions) {
			if (edge.first == REST_INDEX) {
				condition_owner_to_agent.insert({ REST_INDEX, { } });
			}
			else {
				auto& agent = domain.get_agent_from_atom(arguments.at(edge.first));
				seen_agents.insert(agent.get_name());
				condition_owner_to_agent.insert({ edge.first, { agent } });
			}
		}

		for (auto& agent : domain.get_agents()) {
			if (seen_agents.find(agent.get_name()) == seen_agents.end()) {
				condition_owner_to_agent[REST_INDEX].push_back(agent);
			}
		}

		return std::move(condition_owner_to_agent);
	}
	void General_Action::set_instance_to_proposition(std::map<Proposition_Instance, Proposition> instance_to_proposition) {
		input_to_formula = instance_to_proposition;
	}
	std::unordered_map<Proposition, Proposition> General_Action::create_converter(const Domain& domain, const std::unordered_map<size_t, Atom_Id>& arguments) const {
		std::unordered_map<Proposition, Proposition> formula_to_domain;
		formula_to_domain.reserve(input_to_formula.size());
		for (auto& [prop_instance, prop] : input_to_formula) {
			auto grounded_prop_instance = Proposition_Instance(prop_instance, arguments);
			formula_to_domain[prop] = domain.get_proposition(grounded_prop_instance);
		}
		return formula_to_domain;
	}
	std::unordered_map<Proposition, Proposition> General_Action::create_converter(const Domain& domain, const std::vector<Atom_Id>& arguments) const{
		std::unordered_map<Proposition, Proposition> formula_to_domain;
		formula_to_domain.reserve(input_to_formula.size());
		for (auto& [prop_instance, prop] : input_to_formula) {
			if (!prop_instance.contains_non_atom_entry()) {
				auto grounded_prop_instance = Proposition_Instance(prop_instance, arguments);
				formula_to_domain[prop] = domain.get_proposition(grounded_prop_instance);
			}
		}
		return formula_to_domain;
	}


	const std::unordered_map<Proposition, Proposition>& General_Action::get_converter() const {
		return formula_to_domain;
	}


	const std::map<Atom_Id, std::vector<std::vector<Event_Id>>>& General_Action::get_relations() const {
		return relations;
	}

	void General_Action::add_relations(Atom_Id agent, const std::vector<std::vector<Event_Id>>& agent_relations) {
		this->relations.insert({ agent, agent_relations });
	}

	Event_Id General_Action::get_event_id(const std::string& name) const {
		return event_to_id.at(name);
	}
}
