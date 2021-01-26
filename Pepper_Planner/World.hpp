#pragma once

#include <iostream>
#include <unordered_set>

#include "Types.hpp"
#include "Formula.hpp"
#include "Propositions.hpp"

namespace del {
	class Domain;
	class World {
	public:

		World(World_Id id);
		World(World_Id id, Propositions true_propositions);

		World_Id get_id() const;
		void add_true_proposition(const Proposition& propositions);
		void add_true_propositions(const Propositions& propositions);
		void remove_true_propositions(const Propositions& propositions);

		World create_copy(World_Id id) const;
		
		const Propositions& get_true_propositions() const;

		std::string to_string(const Domain& domain) const;
		void set_id(World_Id id);
		bool operator!=(const World& other) const;
		std::string to_hash() const;
	private:

		World_Id id;
		Propositions true_propositions;
	};
}