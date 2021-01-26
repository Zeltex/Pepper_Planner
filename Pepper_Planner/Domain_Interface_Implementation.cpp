#include "Domain_Interface_Implementation.hpp"

namespace del {
	void Domain_Interface_Implementation::new_domain(std::string name) {
		domain.set_name(name);
	}

	void Domain_Interface_Implementation::finish_domain() {
	}

	void Domain_Interface_Implementation::finish_problem() {

		// Perceivability
		for (auto& perceiver_list : perceivability) {
			std::vector<Agent_Id> temp;
			for (auto& agent : perceiver_list.second) {
				temp.emplace_back(domain.get_agent_id(agent));
			}
			initial_state.add_perceivability(domain.get_agent_id(perceiver_list.first), temp);
		}

		// Observability
		for (auto& observer_list : observability) {
			std::vector<Agent_Id> temp;
			for (auto& agent : observer_list.second) {
				temp.emplace_back(domain.get_agent_id(agent));
			}
			initial_state.add_observability(domain.get_agent_id(observer_list.first), temp);
		}

		for (auto& action : actions) {
			library.add_general_action(action, domain);
		}
		domain.set_initial_state(std::move(initial_state));
		initial_state = {};
	}

	void Domain_Interface_Implementation::new_action(std::string name) {
		current_action = General_Action();
		current_action.set_name(name);
	}

	void Domain_Interface_Implementation::finish_action(std::map<Proposition_Instance, Proposition> instance_to_proposition) {
		current_action.set_instance_to_proposition(instance_to_proposition);
		actions.push_back(std::move(current_action));
		current_action = {};
	}


	void Domain_Interface_Implementation::set_action_cost(size_t cost) {
		current_action.set_cost(cost);
	}

	void Domain_Interface_Implementation::set_action_input(std::vector<std::pair<std::string, std::string>>&& inputs) {
		current_action.set_action_input(std::move(inputs));
	}

	void Domain_Interface_Implementation::set_action_owner(std::string type, std::string name, Atom_Id id) {
		current_action.set_owner(type, id);
	}

	void Domain_Interface_Implementation::create_event(std::string name, Formula&& preconditions, std::vector<Proposition> add_list, std::vector<Proposition> delete_list) {
		current_action.create_event(name, std::move(preconditions), add_list, delete_list);
	}


	void Domain_Interface_Implementation::set_designated_events(std::vector<std::string> designated_events) {
		current_action.set_designated_events(designated_events);
	}

	void Domain_Interface_Implementation::set_types(const std::unordered_set<std::string>& types) {
		domain.set_atom_types(types);
	}

	void Domain_Interface_Implementation::add_proposition(std::string name, const std::vector<std::pair<std::string, std::string>>& inputs) {
		general_propositions.emplace_back(name, inputs);
	}

	void Domain_Interface_Implementation::set_objects(std::unordered_map<std::string, std::unordered_set<std::string>>&& objects, const std::unordered_map<std::string, Atom_Id>& atom_to_id) {
		size_t amount_of_agents = objects["agent"].size();
		initial_state.set_amount_of_agents(amount_of_agents);
		library.set_amount_of_agents(amount_of_agents);

		domain.set_objects(objects, atom_to_id);
		for (const auto& entry : objects["agent"]) {
			domain.create_agent(entry);
		}

		std::map<Proposition_Instance, Proposition> instance_to_proposition;
		for (const auto& general_proposition : general_propositions) {
			const auto& inputs = general_proposition.inputs;
			std::vector<size_t> indices(inputs.size());

			// Check at least one atom of each input type exists
			bool empty_input = false;
			for (const auto& input : inputs) {
				if (objects.at(input.first).empty()) {
					empty_input = true;
					break;
				}
			}
			if (empty_input) continue;

			// Convert object set to indicable list
			std::vector<std::vector<std::string>> list_atoms;
			for (const auto& input : inputs) {
				list_atoms.emplace_back();
				for (const auto& object : objects.at(input.first)) {
					list_atoms.back().push_back(object);
				}
			}

			// Produce all grounded propositions
			bool done = false;
			while (!done) {
				std::vector<Atom_Id> grounded_input;
				for (size_t i = 0; i < indices.size(); ++i) {
					const std::string& input_type = inputs.at(i).first;
					const std::string& atom = list_atoms.at(i).at(indices.at(i));
					grounded_input.push_back(domain.get_atom_id(atom));
				}

				instance_to_proposition.insert(std::pair(Proposition_Instance(general_proposition.name, grounded_input), instance_to_proposition.size()));
	
				// Advance indices
				size_t index = 0;
				while (!done && ++indices.at(index) >= list_atoms.at(index).size()) {
					indices.at(index++) = 0;
					done = index >= indices.size();
				}				
			}
		}
		domain.set_proposition_instances(std::move(instance_to_proposition));
	}

	void Domain_Interface_Implementation::set_domain(std::string domain_name) {
		// TODO - problem using domain: domain_name
	}

	void Domain_Interface_Implementation::set_initial_propositions(const std::vector<Proposition_Instance>& proposition_instances) {
		Propositions propositions;
		for (auto& proposition_instance : proposition_instances) {
			propositions.add({ domain.get_proposition(proposition_instance) });
		}
		domain.set_rigid_atoms(propositions);
	}

	void Domain_Interface_Implementation::create_world(std::string name, const std::vector<Proposition_Instance>& proposition_instances) {
		auto& world = initial_state.create_world();
		Propositions propositions;
		for (auto& proposition_instance : proposition_instances) {
			propositions.add({ domain.get_proposition(proposition_instance) });
		}

		world.add_true_propositions(propositions);
		world_name_to_id[name] = world.get_id();
	}

	void Domain_Interface_Implementation::set_designated_worlds(const std::unordered_set<std::string>& designated_worlds) {
		for (auto& entry : designated_worlds) {
			initial_state.add_designated_world(world_name_to_id[entry]);
		}
	}


	void Domain_Interface_Implementation::add_action_relations(Atom_Id agent, const std::vector<std::unordered_set<std::string>>& action_relations) {
		std::vector<std::vector<Event_Id>> event_id_relations;

		for (const auto& indistinguishability_set : action_relations) {
			event_id_relations.emplace_back();
			for (const auto& event_name : indistinguishability_set) {
				event_id_relations.back().push_back(current_action.get_event_id(event_name));
			}
		}

		current_action.add_relations(agent, event_id_relations);
	}

	void Domain_Interface_Implementation::add_problem_relations(Atom_Id agent, const std::vector<std::unordered_set<std::string>>& world_relations) {
		for (const auto& indistinguishability_set : world_relations) {
			std::unordered_set<World_Id> worlds;
			for (const auto& world_name: indistinguishability_set) {
				worlds.insert(world_name_to_id.at(world_name));
			}
			initial_state.add_indistinguishability_set(domain.get_agent_from_atom(agent).get_id(), worlds);
		}
	}

	std::tuple<Domain, Action_Library, Formula> Domain_Interface_Implementation::get_loaded() {
		return { std::move(domain), std::move(library), std::move(goal) };
	}

	void Domain_Interface_Implementation::set_announce_enabled() {
		library.set_announce_enabled();
	}

	void Domain_Interface_Implementation::set_goal(Formula&& goal, const std::map<Proposition_Instance, Proposition>& instance_to_proposition, const std::unordered_map<std::string, Atom_Id>& atom_to_id) {
		auto converter = get_loader_to_formula_converter(instance_to_proposition);

		this->goal = Formula(goal, converter);
	}

	void Domain_Interface_Implementation::add_observability(std::string observer, const std::vector<std::string>& agents) {
		observability[observer] = agents;
	}
	
	void Domain_Interface_Implementation::add_perceivability(std::string perceiver, const std::vector<std::string>& agents) {
		perceivability[perceiver] = agents;
	}

	// Map from domain_loader ids to pepper_planner ids (using Proposition)
	std::unordered_map<Proposition, Proposition> Domain_Interface_Implementation::get_loader_to_formula_converter(const std::map<Proposition_Instance, Proposition>& instance_to_proposition) const {
		std::unordered_map<Proposition, Proposition> converter;
		converter.reserve(instance_to_proposition.size());
		for (const auto& entry : instance_to_proposition) {
			converter[entry.second] = domain.get_proposition(entry.first);
		}
		return converter;
	}
}