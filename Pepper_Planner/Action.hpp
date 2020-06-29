#pragma once

#include <algorithm>
#include <vector>
#include <string>
#include <unordered_map>

#include "Formula.hpp"
#include "Formula_Types.hpp"
#include "Action_Event.hpp"
#include "Misc.hpp"
#include "Types.hpp"
#include "General_Action.hpp"
#include "Agent.hpp"


namespace del {
	class World;
	class Action {
	public:
		Action() {}
		Action(Agent_Id owner, size_t number_of_agents);
		Action(	General_Action general_action, 
				Agent_Id owner, 
				const std::unordered_map<std::string, std::string>& input_to_atom, 
				const std::unordered_map<std::string, std::vector<Agent>>& condition_owner_to_agent);
		void add_event(const Action_Event& event);
		void add_event(std::string name, Event_Id id, Formula&& precondition, std::vector<Proposition_Instance>&& proposition_add, std::vector<Proposition_Instance>&& proposition_delete);
		void add_reachability(Agent_Id owner, Event_Id event_from, Event_Id event_to, Formula&& condition);

		const std::vector<Action_Event>& get_events() const;
		Agent_Id get_owner() const;
		std::string get_name() const;
		bool is_event_designated(Event_Id event) const;

		bool is_condition_fulfilled(Agent_Id agent, Event_Id event_from, Event_Id event_to, const World& world) const;

		void add_designated_event(Event_Id event);

		std::string to_string() const;
		std::string to_string(size_t indentation) const;
		std::string to_compact_string() const;
		std::string to_graph(const std::vector<Agent>& agents) const;

	private:
		void copy_and_instantiate_edge_conditions(const General_Action& general_action, const std::unordered_map<std::string, std::vector<Agent>>& condition_owner_to_id, const std::unordered_map<std::string, Event_Id>& event_name_to_id, std::unordered_map<std::string, std::string> input_to_atom);
		void copy_and_instantiate_designated_events(const General_Action& general_action, const std::unordered_map<std::string, Event_Id>& event_name_to_id);
		std::unordered_map<std::string, Event_Id> copy_and_instantiate_events(const General_Action& general_action, const std::unordered_map<std::string, std::string>& input_to_atom);

		std::string get_string(const std::vector<Proposition_Instance>& propositions) const;

		std::vector<Action_Event> events;
		std::vector<Event_Id> designated_events;
		Agent_Id owner;
		std::string name;
		std::vector<Agent_Edges> edge_conditions;

	};
}