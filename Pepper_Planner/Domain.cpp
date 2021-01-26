#include "Domain.hpp"
#include "Core.hpp"

namespace del {
	void Domain::perform_action(Action action) {
		const State& current_state = states.back();
		auto product_update = perform_product_update(current_state, action, agents, *this);
#if BISIM_CONTRACTION_ENABLED == 1
		product_update = perform_k_bisimilar_contraction(std::move(product_update), BISIMILAR_DEPTH);
#endif
		states.push_back(std::move(product_update));
		PRINT_ACTION_TO_CONSOLE(action, *(this));
		PRINT_ACTION(action, *(this), debug_counter++);
		PRINT_STATE(get_current_state(), *(this), debug_counter);
	}

	State Domain::get_current_state() const {
		return states.back();
	}

	void Domain::set_name(const std::string name) {
		this->name = name;
	}

	std::string Domain::get_name() const {
		return name;
	}
	const Agent& Domain::get_agent(const std::string& name) const {
		for (const auto& agent : agents) {
			if (agent.get_name() == name) {
				return agent;
			}
		}
		std::cerr << "No agent with name: " << name << "\n";
		exit(-1);
	}

	const std::vector<Agent>& Domain::get_agents() const {
		return agents;
	}

	const Agent& Domain::get_agent_from_atom(const Atom_Id& id) const {
		return get_agent(get_atom_name(id));
	}

	std::optional<Agent_Id> Domain::get_agent_id_optional(Atom_Id atom_id) const {
		auto it = atom_to_agent.find(atom_id.id);
		if (it != atom_to_agent.end()) {
			return { it->second };
		}
		return {};
	}

	Agent_Id Domain::get_agent_id(const std::string& name) const {
		for (auto& entry : agents) {
			if (entry.get_name() == name) {
				return entry.get_id();
			}
		}
		std::cerr << "No agent with name: " << name << "\n";
		exit(-1);
	}

	Agent_Id Domain::get_agent_id(Atom_Id atom_id) const {
		auto name = get_atom_name(atom_id);
		for (auto& entry : agents) {
			if (entry.get_name() == name) {
				return entry.get_id();
			}
		}
		std::cerr << "No agent with name: " << name << "\n";
		exit(-1);
	}

	Agent_Id Domain::create_agent(std::string name) {
		Agent_Id agent_id = Agent_Id{ agents.size() };
		Atom_Id atom_id = get_atom_id(name);
		agents.push_back(Agent(agent_id, atom_id, name));
		atom_to_agent[atom_id.id] = agent_id;
		amount_of_agents = agents.size();
		return agent_id;
	}

	void Domain::set_atom_types(std::unordered_set<std::string> types) {
		atom_types = types;
	}

	const std::unordered_set<std::string>& Domain::get_atom_types() const {
		return atom_types;
	}

	const std::vector<Atom_Id>& Domain::get_all_atoms_of_type(const std::string& type) const {
		if (objects.find(type) == objects.end()) {
			std::cerr << "No objects of type: " << type << "\n";
			exit(-1);
		}
		return objects.at(type);
	}

	void Domain::set_objects(const std::unordered_map<std::string, std::unordered_set<std::string>>& objects, const std::unordered_map<std::string, Atom_Id>& atom_to_id) {
		this->objects.clear();
		this->objects.reserve(objects.size());
		this->atom_to_id = atom_to_id;
		this->id_to_atom = {};

		for (auto& [atom, id] : atom_to_id) {
			id_to_atom.insert(std::pair(id.id, atom));
		}

		// Recording all atoms of types
		for (auto& type : objects) {
			this->objects[type.first].reserve(type.second.size());
			for (auto& atom : type.second) {
				this->objects[type.first].push_back(atom_to_id.at(atom).id);
			}
		}
	}

	void Domain::set_initial_state(State&& state) {
		states = {};
		states.push_back(state);
	}


	std::string Domain::get_atom_name(Atom_Id atom_id) const {
		return id_to_atom.at(atom_id.id);
	}

	Atom_Id Domain::get_atom_id(const std::string& atom_name) const {
		return atom_to_id.at(atom_name);
	}

	const std::unordered_map<size_t, std::string>& Domain::get_id_to_atom() const {
		return id_to_atom;
	}

	const std::unordered_map<std::string, Atom_Id>& Domain::get_atom_to_id() const {
		return atom_to_id;
	}
	
	std::unordered_map<std::string, size_t> Domain::get_agent_to_id() const {
		std::unordered_map<std::string, size_t> result;
		for (auto& agent : agents) {
			result[agent.get_name()] = agent.get_id().id;
		}
		return result;
	}

	void Domain::set_rigid_atoms(Propositions rigid_atoms) {
		this->rigid_atoms = std::move(rigid_atoms);
	}

	const Propositions& Domain::get_atom_rigids() const {
		return rigid_atoms;
	}

	void Domain::set_proposition_instances(std::map<Proposition_Instance, Proposition>&& instance_to_proposition) {
		proposition_to_instance.clear();
		for (const auto& [prop_instance, prop] : instance_to_proposition) {
			proposition_to_instance[prop] = prop_instance;
		}
		this->instance_to_proposition = std::move(instance_to_proposition);
	}

	const Proposition& Domain::get_proposition(const Proposition_Instance& proposition_instance) const {
		return instance_to_proposition.at(proposition_instance);
	}

	const Proposition_Instance& Domain::get_proposition_instance(const Proposition& proposition) const {
		return proposition_to_instance.at(proposition);
	}

	size_t Domain::get_amount_of_agents() const {
		return amount_of_agents;
	}


}