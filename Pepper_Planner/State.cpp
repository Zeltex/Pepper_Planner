#include "State.hpp"
#include "Domain.hpp"
#include "Formula_Input_Impl.hpp"
#include "Action_Event.hpp"

namespace del {

	State::State():
		cost(0), worlds(), designated_worlds(), relations(){
	}

	State::State(size_t number_of_agents) :
		cost(0), worlds(), designated_worlds(), relations(number_of_agents) {
	}

	const std::vector<Proposition>& State::get_true_propositions(size_t world_id) const {
		return worlds[world_id].get_true_propositions();
	}

	std::vector<size_t> State::get_reachable_worlds(size_t agent_id, size_t world_id) const {
		auto worlds = relations.get_reachables({ agent_id }, { {world_id} });
		std::vector<size_t> result;
		for (const auto& world : worlds) {
			result.push_back(world.id);
		}
		return result;
	}
	size_t State::get_number_of_agents() const {
		return relations.get_number_of_agents();
	}

	void State::set_relations(Indistinguishability_Relations relations) {
		this->relations = relations;
	}

	bool State::valuate(const Formula& formula, const Domain& domain) const {
		Formula_Input_Impl input = { this, &domain };
		for (const auto& world : designated_worlds) {
			if (!formula.valuate(world.id, &input)) {
				return false;
			}
		}
		return true;
	}

	std::vector<World_Id> State::get_reachables(Agent_Id agent, World_Id world) const {
		return relations.get_reachables(agent, { world });
	}

	const std::vector<World>& State::get_worlds() const {
		return worlds;
	}

	bool State::is_one_reachable(Agent_Id agent, World_Id world1, World_Id world2) const {
		return relations.is_indistinguishable(agent, world1, world2);
	}


	std::vector<World_Id> State::get_designated_world_reachables(Agent_Id agent) const {
		return relations.get_reachables(agent, designated_worlds);
	}

	void State::add_indistinguishability_set(Agent_Id agent, std::unordered_set<World_Id> worlds) {
		relations.insert(agent, worlds);
		//indistinguishability_relation[agent.id].emplace_back(world_from, world_to);
	}

	void State::add_indistinguishability_relation(Agent_Id agent, World_Id world_from, World_Id world_to) {
		relations.add_relation(agent, world_from, world_to);
	}

	void State::add_true_propositions(World_Id world, const std::vector<Proposition>& propositions) {
		worlds[world.id].add_true_propositions(propositions);
	}

	void State::remove_true_propositions(World_Id world, const std::vector<Proposition>& propositions) {
		worlds[world.id].remove_true_propositions(propositions);
	}

	World& State::create_world() {
		World_Id new_world = World_Id{ worlds.size() };
		worlds.emplace_back(new_world);
		relations.set_amount_of_worlds(worlds.size());
		return worlds[new_world.id];
	}

	World& State::create_world(const World& world) {
		World_Id new_world = World_Id{ worlds.size() };
		worlds.emplace_back(new_world, world.get_true_propositions());
		relations.set_amount_of_worlds(worlds.size());
		return worlds[new_world.id];
	}

	void State::create_worlds(size_t amount_to_create) {
		for (size_t i = 0; i < amount_to_create; i++) {
			World_Id new_world = World_Id{ worlds.size() };
			worlds.emplace_back(new_world);
		}
		relations.set_amount_of_worlds(worlds.size());
	}

	void State::create_world(const World& world, const Action_Event& event, bool designated) {
		auto& new_world = create_world();
		new_world.add_true_propositions(world.get_true_propositions());
		new_world.remove_true_propositions(event.get_delete_list());
		new_world.add_true_propositions(event.get_add_list());
		if (designated) {
			add_designated_world(new_world.get_id());
		}
	}

	bool State::is_world_designated(World_Id world) const {
		return find(designated_worlds.begin(), designated_worlds.end(), world) != designated_worlds.end();
	}

	void State::add_designated_world(World_Id world) {
		if (find(designated_worlds.begin(), designated_worlds.end(), world) != designated_worlds.end()) {
			return;
		}
		for (size_t i = 0; i < designated_worlds.size(); ++i) {
			if (designated_worlds.at(i) > world) {
				designated_worlds.insert(designated_worlds.begin() + i, world);
				return;
			}
		}
		designated_worlds.push_back(world);
	}

	size_t State::get_worlds_count() const {
		return worlds.size();
	}

	size_t State::get_designated_worlds_count() const {
		return designated_worlds.size();
	}

	const std::vector<World_Id>& State::get_designated_worlds() const {
		return designated_worlds;
	}

	const World& State::get_world(World_Id world) const {
		return worlds[world.id];
	}

	size_t State::get_cost() const {
		return cost;
	}

	void State::set_cost(size_t cost) {
		this->cost = cost;
	}

	void State::set_single_designated_world(World_Id world) {
		designated_worlds.clear();
		designated_worlds.push_back(world);
	}

	void State::set_designated_worlds(const std::vector<World_Id>& worlds) {
		designated_worlds = worlds;
	}

	void State::remove_unreachable_worlds() {
		auto reachable_worlds = relations.get_reachables_set(designated_worlds);


		std::unordered_map<size_t, size_t> world_old_to_new;
		world_old_to_new.reserve(worlds.size());

		// Delete worlds
		std::vector<World> new_worlds;
		new_worlds.reserve(reachable_worlds.size());
		size_t world_counter = 0;
		{
			for (auto& world : worlds) {
				if (reachable_worlds.find(world.get_id()) != reachable_worlds.end()) {
					world_old_to_new[world.get_id().id] = world_counter;
					world.set_id({ world_counter });
					new_worlds.emplace_back(std::move(world));
					world_counter++;
				}
			}
			worlds = std::move(new_worlds);
		}

		relations.convert(world_old_to_new, world_counter);

		std::vector<World_Id> new_designated_worlds;
		new_designated_worlds.reserve(designated_worlds.size());
		for (auto& world : designated_worlds) {
			new_designated_worlds.push_back({ world_old_to_new[world.id] });
		}
		designated_worlds = std::move(new_designated_worlds);
	}

	bool State::operator==(const State& other) const {
		if (worlds.size() != other.worlds.size()
			|| designated_worlds.size() != other.designated_worlds.size()
			|| relations.get_size() != other.relations.get_size()) return false;
		for (size_t i = 0; i < worlds.size(); i++) {
			if (worlds[i] != other.worlds[i]) return false;
		}
		for (size_t i = 0; i < designated_worlds.size(); i++) {
			if (designated_worlds[i].id != other.designated_worlds[i].id) return false;
		}
		if (relations != other.relations) return false;

		return true;
	}

	size_t State::to_hash() const {
		std::vector<std::string> hashes;
		//size_t relations_size = 0;
		//for (auto& relations : indistinguishability_relation) relations_size += relations.size();
		//hashes.reserve(worlds.size() + designated_worlds.size() + relations_size);
		hashes.reserve(worlds.size() + designated_worlds.size());

		hashes.push_back(std::to_string(worlds.size()));
		hashes.push_back(std::to_string(designated_worlds.size()));
		hashes.push_back(std::to_string(relations.get_size()));

		for (auto& world : worlds) {
			hashes.emplace_back(std::move(world.to_hash()));
		}
		for (auto& designated_world : designated_worlds) {
			hashes.emplace_back(std::to_string(designated_world.id));
		}

		std::string hash;
		hash.reserve(hashes.size() * sizeof(size_t));
		for (const auto& entry : hashes) hash += entry;
		hash += relations.to_hash();

		return std::hash<std::string>()(hash);
	}

	std::string State::to_string(const Domain& domain) const {
		return to_string(3, domain);
	}


	std::string State::to_string(size_t indentation, const Domain& domain) const {

		size_t relations_size = relations.get_size();

		std::string result = get_indentation(indentation) + " State\n" + get_indentation(indentation - 1) + " Sizes: " +
			") (worlds, " + std::to_string(worlds.size()) +
			") (designated worlds, " + std::to_string(designated_worlds.size()) +
			") (cost, " + std::to_string(cost) +
			") (relations, " + std::to_string(relations_size) + ")\n";
		result += get_indentation(indentation - 1) + " Designated worlds: ";
		bool first = true;
		for (const auto& designated_world : designated_worlds) {
			if (first) {
				first = false;
			} else {
				result += ", ";
			}
			result += std::to_string(designated_world.id);
		}
		result += "\n" + get_indentation(indentation - 1) + " ({Agent}, {World from}, {World to}) Relations";
		size_t current_agent = 0;
		result += relations.to_string();

		result += "\n" + get_indentation(indentation - 1) + " World {id}: {propositions}";
		for (const auto& world : worlds) {
			result += "\n" + world.to_string(domain);
		}
		return result;
	}

	std::string State::to_graph(const std::string node_id, const Domain& domain) const {
		std::string result;
		for (auto& world : worlds) {
			std::string propositions;
			bool first = true;
			for (auto proposition : world.get_true_propositions()) {
				if (first) {
					first = false;
				} else {
					propositions += ", \n";
				}
				propositions += domain.get_proposition_instance(proposition).to_string(domain.get_id_to_atom());
			}
			result += node_id + std::to_string(world.get_id().id) + " [label=\"" + std::to_string(world.get_id().id) + "\n" + propositions + "\"";
			if (std::find(designated_worlds.begin(), designated_worlds.end(), world.get_id()) != designated_worlds.end()) {
				result += ", shape=doublecircle];\n";
			} else {
				result += "];\n";
			}
		}
		result += relations.to_graph();

		return result;
	}
	Quick_Relations State::get_quick_relations() const {
		return relations.get_quick_relations();
	}

	const Indistinguishability_Relations& State::get_relations() const {
		return relations;
	}

}