#pragma once

#include <vector>
#include <string>
#include <unordered_set>

#include "Propositions.hpp"
#include "Types.hpp"
#include "Formula.hpp"

namespace del {
	class Action_Event {
	public:
		Action_Event(std::string name, Event_Id id, Formula&& precondition, Propositions proposition_add, Propositions proposition_delete) :
			name(name), id(id), precondition(std::move(precondition)), proposition_add(proposition_add), proposition_delete(proposition_delete) {}
		Action_Event(Event_Id id, Formula&& precondition, Propositions proposition_add, Propositions proposition_delete) :
			name("Unknown"), id(id), precondition(std::move(precondition)), proposition_add(proposition_add), proposition_delete(proposition_delete) {
		}

		Event_Id get_id() const;
		const Formula& get_preconditions() const;
		const Propositions& get_add_list() const;
		const Propositions& get_delete_list() const;
		std::string get_name() const;


		std::string to_string(const Domain& domain) const;

	private:
		std::string name;
		Event_Id id;
		Formula precondition;
		Propositions proposition_add;
		Propositions proposition_delete;

	};
}