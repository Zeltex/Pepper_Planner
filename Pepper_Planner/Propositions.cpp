#include "Domain.hpp"
#include "Propositions.hpp"

namespace del {

	Propositions::Propositions(const Propositions& propositions, const std::unordered_map<Proposition, Proposition>& converter)
		: propositions() {
		for (const auto& proposition : propositions.propositions) {
			this->propositions.insert(converter.at(proposition));
		}
	}

	std::string Propositions::to_string(const Domain& domain) const {
		std::set<Proposition> sorted_propositions;
		for (const auto& proposition : propositions) {
			sorted_propositions.insert(proposition);
		}

		std::string result;
		bool first = true;
		for (auto& proposition : sorted_propositions) {
			if (first) {
				first = false;
			} else {
				result += "; ";
			}
			result += domain.get_proposition_instance(proposition).to_string(domain.get_id_to_atom());
		}
		return result;
	}

	std::string Propositions::to_compact_string() const {
		std::set<Proposition> sorted_propositions;
		for (const auto& proposition : propositions) {
			sorted_propositions.insert(proposition);
		}
		std::string result;
		for (auto& proposition : sorted_propositions) {
			result += proposition.to_string();
		}
		return result;
	}

	bool Propositions::valuate(const Proposition& proposition) const {
		return propositions.find(proposition) != propositions.end();
	}

	std::string Propositions::to_instance_string(const Domain& domain) const {
		std::set<Proposition> sorted_propositions;
		for (const auto& proposition : propositions) {
			sorted_propositions.insert(proposition);
		}
		std::string result;
		bool first = true;
		for (const auto& proposition : sorted_propositions) {
			if (first) {
				first = false;
			} else {
				result += ", ";
			}
			result += domain.get_proposition_instance(proposition).to_string(domain.get_id_to_atom());
		}
		return result;
	}

	std::string Propositions::to_hash() const {
		std::set<Proposition> sorted_propositions;
		for (const auto& proposition : propositions) {
			sorted_propositions.insert(proposition);
		}
		std::string hash;
		for (const auto& prop : sorted_propositions) {
			hash += prop.to_hash();
		}
		return hash;
	}

	void Propositions::add(const Propositions& propositions_add) {
		for (const auto& proposition : propositions_add.propositions) {
			propositions.insert(proposition);
		}
	}

	void Propositions::add(const Proposition& proposition_add) {
		propositions.insert(proposition_add);
	}

	void Propositions::remove(const Propositions& propositions_remove) {
		for (const auto& proposition : propositions_remove.propositions) {
			auto it = propositions.find(proposition);
			if (it != propositions.end()) {
				propositions.erase(it);
			}
		}
	}

	bool Propositions::operator==(const Propositions& other) const {
		if (propositions.size() != other.propositions.size()) return false;
		for (const auto& proposition : propositions) {
			if (other.propositions.find(proposition) == other.propositions.end()) {
				return false;
			}
		}
		return true;
	}

	size_t Propositions::size() const {
		return propositions.size();
	}
}