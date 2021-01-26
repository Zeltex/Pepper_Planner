
#include "Domain.hpp"
#include "World.hpp"


namespace del {

	World::World(World_Id id) : id(id), true_propositions() {}
	World::World(World_Id id, Propositions true_propositions) :
		id(id), true_propositions(true_propositions) {}

	World_Id World::get_id() const {
		return id;
	}

	World World::create_copy(World_Id id) const {
		return World(id, true_propositions);
	}

	void World::add_true_proposition(const Proposition& propositions) {
		this->true_propositions.add(propositions);
	}

	void World::add_true_propositions(const Propositions& propositions) {
		this->true_propositions.add(propositions);
	}

	void World::remove_true_propositions(const Propositions& propositions) {
		this->true_propositions.remove(propositions);
	}

	const Propositions& World::get_true_propositions() const {
		return true_propositions;
	}

	std::string World::to_string(const Domain& domain) const{
		return "World " + std::to_string(id.id) + ": " + true_propositions.to_instance_string(domain);
	}
	void World::set_id(World_Id id) {
		this->id = std::move(id);
	}

	bool World::operator!=(const World& other) const {
		return !(true_propositions == other.true_propositions);
	}
	std::string World::to_hash() const {
		return true_propositions.to_hash();
	}
}