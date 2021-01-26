#pragma once

#include <unordered_set>
#include <vector>

#include "Formula_Types.hpp"
namespace del {
	class Domain;
	class Propositions {
	public:
		Propositions() : propositions() {}
		Propositions(const Propositions& propositions) : propositions(propositions.propositions) {}
		Propositions(const Propositions& propositions, const std::unordered_map<Proposition, Proposition>& converter);
		Propositions(const std::unordered_set<Proposition>& propositions) : propositions(propositions) {}
		Propositions(const Proposition& proposition) : propositions({ proposition }) {}
		Propositions(const std::vector<Proposition>& propositions) : propositions(propositions.begin(), propositions.end()) {}

		void add(const Propositions& propositions_add);
		void add(const Proposition& proposition_add);
		void remove(const Propositions& propositions_remove);

		std::string to_compact_string() const;
		std::string to_hash() const;
		std::string to_instance_string(const Domain& domain) const;
		std::string to_string(const Domain& domain) const;

		bool valuate(const Proposition& proposition) const;

		bool operator==(const Propositions& other) const;
		size_t size() const;
	private:
		std::unordered_set<Proposition> propositions;
	};
}