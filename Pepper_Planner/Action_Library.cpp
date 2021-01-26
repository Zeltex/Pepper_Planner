#include "Action_Library.hpp"

namespace del {

	Action_Library::Action_Library(): amount_of_agents(0), action_counter(0), announce_enabled(false), actions() {
	}

	Action_Library::Action_Library(size_t amount_of_agents) : action_counter(0), announce_enabled(false), actions() {
		set_amount_of_agents(amount_of_agents);
	}


	void Action_Library::load_actions(const State& state, const Domain& domain) {
		action_counter = 0;
	}

	bool Action_Library::has_action() const {
		return action_counter < actions.size() + announce_actions.size();
	}

	const Action& Action_Library::get_next_action() {
		size_t temp_counter = action_counter++;
		if (temp_counter < actions.size()) {
			return actions.at(temp_counter);
		} else {
			return announce_actions.at(temp_counter - actions.size());
		}
	}

	void Action_Library::set_amount_of_agents(size_t amount_of_agents) {
		this->amount_of_agents = amount_of_agents;
	}

	const std::vector<Action>& Action_Library::get_actions() const{
		return actions;
	}

	const General_Action& Action_Library::get_general_action(const std::string& name) const {
		return general_actions.at(general_action_name_to_id.at(name));
	}

	void Action_Library::add_action(const Action& action) {
		actions.push_back(action);
	}

	void Action_Library::set_announce_enabled() {
		announce_enabled = true;
	}

	void Action_Library::add_general_action(const General_Action& general_action, const Domain& domain) {

		general_actions.push_back(general_action);
		general_action_name_to_id[general_action.get_name()] = general_actions.size() - 1;

		std::vector<std::vector<Atom_Id>> atoms;
		std::vector<size_t> counters;

		// Load all combinations of input
		for (auto& entry : general_action.get_inputs()) {
			auto& temp = domain.get_all_atoms_of_type(entry.first);
			atoms.emplace_back(temp.begin(), temp.end());
			counters.emplace_back(0);
		}

		// Generate all possible valid inputs
		bool done = false;
		while (!done) {

			std::vector<Atom_Id> arguments;
			for (size_t i = 0; i < counters.size(); i++) {
				arguments.emplace_back(atoms[i][counters[i]]);
			}
			actions.emplace_back(general_action, domain, arguments);

			// Advance indices
			size_t index = 0;
			while (!done && ++counters.at(index) >= atoms.at(index).size()) {
				counters.at(index++) = 0;
				done = index >= counters.size();
			}
		}
	}
}