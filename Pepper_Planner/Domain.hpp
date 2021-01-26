#pragma once

#include <vector>
#include <string>
#include <map>

#include "Types.hpp"
#include "State.hpp"
#include "Formula.hpp"
#include "Action_Event.hpp"
#include "Action.hpp"
#include "DEL_Operations.hpp"
#include "Core.hpp"

namespace del {
	class Domain {
	public:

		Domain() : amount_of_agents(0), states(), name(""), agents(), atom_types() {}
		Domain(State initial_state) : states({ initial_state }), name(), agents(), atom_types() {}

		//void perform_do(const Agent_Id i, const std::vector<Proposition_Instance>& add, const std::vector<Proposition_Instance>& del);
		//void perform_oc(const Agent_Id i, std::vector<Proposition_Instance>&& add_list, std::vector<Proposition_Instance>&& delete_list, std::string perceivability_proposition = "perceives", std::string observability_proposition = "obeserves");
		void perform_action(Action action);
		State			get_current_state() const;
		void			set_name(const std::string name);
		std::string		get_name() const;

		Agent_Id									create_agent(std::string name);
		const Agent&								get_agent(const std::string& name) const;
		const std::vector<Agent>&					get_agents() const;
		const Agent&								get_agent_from_atom(const Atom_Id& id) const;
		Agent_Id									get_agent_id(Atom_Id atom_id) const;
		Agent_Id									get_agent_id(const std::string& name) const;
		std::optional<Agent_Id>						get_agent_id_optional(Atom_Id atom_id) const;
		const std::vector<Atom_Id>&					get_all_atoms_of_type(const std::string& type) const;
		Atom_Id										get_atom_id(const std::string& atom_name) const;
		std::string									get_atom_name(Atom_Id atom_id) const;
		const std::unordered_set<std::string>&		get_atom_types() const;
		const Propositions&							get_atom_rigids() const;
		size_t										get_amount_of_agents() const;
		const Proposition&							get_proposition(const Proposition_Instance& proposition_instance) const;
		const Proposition_Instance&					get_proposition_instance(const Proposition& proposition) const;

		void set_atom_types(std::unordered_set<std::string> types);
		void set_initial_state(State&& state);
		void set_objects(const std::unordered_map<std::string, std::unordered_set<std::string>>& objects, const std::unordered_map<std::string, Atom_Id>& atom_to_id);
		void set_rigid_atoms(Propositions rigid_atoms);
		void set_proposition_instances(std::map<Proposition_Instance, Proposition>&& instance_to_proposition);

		std::unordered_map<std::string, size_t>			get_agent_to_id() const;
		const std::unordered_map<std::string, Atom_Id>& get_atom_to_id() const;
		const std::unordered_map<size_t, std::string>&	get_id_to_atom() const;

	private:
		//std::unordered_set<size_t> get_obs_set(const Agent_Id& owner, const std::vector<Proposition_Instance>& add_list, const std::vector<Proposition_Instance>& delete_list);

		size_t amount_of_agents;
		std::vector<State> states;
		std::string name;
		std::vector<Agent> agents;
		std::unordered_set<std::string> atom_types;

		Propositions rigid_atoms;
		std::map<Proposition_Instance, Proposition> instance_to_proposition;
		std::unordered_map<Proposition, Proposition_Instance> proposition_to_instance;
		// Objects and atoms are same thing, one is just sorted on type (need to rename objects to atoms)
		std::unordered_map<size_t, Agent_Id> atom_to_agent;
		std::unordered_map<std::string, std::vector<Atom_Id>> objects;
		std::unordered_map<std::string, Atom_Id> atom_to_id;
		std::unordered_map<size_t, std::string> id_to_atom;
#if DEBUG_PRINT == 1
		size_t debug_counter = 0;
#endif
	};
}