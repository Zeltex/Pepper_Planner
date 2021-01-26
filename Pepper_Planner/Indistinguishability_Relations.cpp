#include <iostream>

#include "Indistinguishability_Relations.hpp"

namespace del {

	Indistinguishability_Relations::Indistinguishability_Relations()
		: index_counter(0) {

	}

	Indistinguishability_Relations::Indistinguishability_Relations(size_t number_of_agents)
		: index_counter(0) {
		set_amount_of_agents(number_of_agents);
	}

	Indistinguishability_Relations::Indistinguishability_Relations(size_t number_of_agents, size_t number_of_worlds)
		: index_counter(0) {
		set_amount_of_agents(number_of_agents);
		set_amount_of_worlds(number_of_worlds);
	}


	Indistinguishability_Relations::Indistinguishability_Relations(std::vector<std::vector<size_t>> relations, size_t index_counter)
		: index_counter(index_counter), relations(relations) {}
	

	void Indistinguishability_Relations::insert(Agent_Id agent, const std::unordered_set<World_Id>& worlds) {
		for (const auto& world : worlds) {
			relations.at(agent.id).at(world.id) = index_counter;
		}
		++index_counter;
	}

	std::vector<World_Id> Indistinguishability_Relations::get_reachables(Agent_Id agent, const std::vector<World_Id>& worlds) const {
		std::vector<World_Id> result;
		std::unordered_set<size_t> valid_indices;
		for (const auto& world : worlds) {
			valid_indices.insert(relations.at(agent.id).at(world.id));
		}
		size_t worlds_size = relations.at(agent.id).size();
		for (size_t i = 0; i < worlds_size; ++i) {
			if (valid_indices.find(relations.at(agent.id).at(i)) != valid_indices.end()) {
				result.push_back({ i });
			}
		}
		return result;
	}

	std::vector<World_Id> Indistinguishability_Relations::get_reachables(const std::vector<World_Id>& worlds) const {
		auto set = get_reachables_set(worlds);
		return std::vector(set.begin(), set.end());
	}

	// TODO - Not very efficient, and should probably make World_Id hashable to use unordered_set
	std::set<World_Id> Indistinguishability_Relations::get_reachables_set(const std::vector<World_Id>& worlds) const {
		std::set<World_Id> result = { worlds.begin(), worlds.end() };
		std::unordered_set<size_t> valid_indices;
		bool changed = true;
		while (changed) {
			changed = false;
			for (const auto& agent_relations : relations) {
				valid_indices.clear();
				for (const auto& world : result) {
					valid_indices.insert(agent_relations.at(world.id));
				}
				size_t worlds_size = agent_relations.size();
				for (size_t i = 0; i < worlds_size; ++i) {
					if (valid_indices.find(agent_relations.at(i)) != valid_indices.end()
						&& result.find({ i }) == result.end()) {
						result.insert({ i });
						changed = true;
					}
				}
			}
		}
		return result;
	}

	bool Indistinguishability_Relations::is_indistinguishable(Agent_Id agent, World_Id world1, World_Id world2) const {
		return relations.at(agent.id).at(world1.id) == relations.at(agent.id).at(world2.id);
	}

	void Indistinguishability_Relations::set_amount_of_agents(size_t number_of_agents) {
		while (relations.size() < number_of_agents) {
			relations.emplace_back();
		}
		set_amount_of_worlds(relations.at(0).size());
	}

	void Indistinguishability_Relations::convert(const std::unordered_map<size_t, size_t>& converter, const size_t new_world_size) {
		std::vector<std::vector<size_t>> new_relations(relations.size(), std::vector<size_t>(new_world_size));

		for (const auto& [from, to] : converter) {
			for (size_t agent = 0; agent < new_relations.size(); ++agent) {
				new_relations.at(agent).at(to) = relations.at(agent).at(from);
			}
		}
		relations = new_relations;
	}

	size_t Indistinguishability_Relations::get_size() const {
		std::unordered_set<size_t> indices;
		for (size_t agent = 0; agent < relations.size(); ++agent) {
			for (const auto& index : relations.at(agent)) {
				indices.insert(index);
			}
		}
		return (relations.size() * relations.at(0).size()) - indices.size();
	}

	size_t Indistinguishability_Relations::get_size(Agent_Id agent) const {
		std::unordered_set<size_t> indices;
		for (const auto& index : relations.at(agent.id)) {
			indices.insert(index);
		}
		return relations.at(0).size() - indices.size();
	}

	bool Indistinguishability_Relations::operator==(const Indistinguishability_Relations& other) const {
		std::unordered_map<size_t, size_t> this_to_other;

		size_t agent_size = relations.size();
		for (size_t i = 0; i < agent_size; ++i) {
			size_t agent_relation_size = relations.at(i).size();
			for (size_t j = 0; j < agent_relation_size; ++j) {
				auto find = this_to_other.find(relations.at(i).at(j));
				if (find != this_to_other.end()) {
					if (find->second != other.relations.at(i).at(j)) {
						return false;
					}
				} else {
					this_to_other.insert({ relations.at(i).at(j), other.relations.at(i).at(j) });
				}
			}
		}
		return true;
	}

	bool Indistinguishability_Relations::operator!=(const Indistinguishability_Relations& other) const {
		return !(*this == other);
	}

	std::string Indistinguishability_Relations::to_hash() const {
		std::string result;
		for (const auto& agent_relations : relations) {
			for (const auto& index : agent_relations) {
				result += std::to_string(index);
			}
		}
		return result;
	}

	std::string Indistinguishability_Relations::to_string() const {
		std::string result;
		size_t agent_size = relations.size();
		for (size_t i = 0; i < agent_size; ++i) {
			result += "(" + std::to_string(i) + ": ";
			for (const auto& index : relations.at(i)) {
				result += std::to_string(index) + " ";
			}
			result += ")\n";
		}
		return result;
	}

	// TODO - This
	std::string Indistinguishability_Relations::to_graph() const {
		//size_t agent_id = 0;
//for (auto& agent_relations : indistinguishability_relation) {
//	for (auto& relation : agent_relations) {
//		result += node_id
//			+ std::to_string(relation.world_from.id) 
//			+ " -> " 
//			+ node_id
//			+ std::to_string(relation.world_to.id) 
//			+ "[label=\"" 
//			+ domain.get_agent(Agent_Id{ agent_id }).get_name()
//			+ "\"];\n";
//	}
//	agent_id++;
//}
		std::cerr << "Indistinguishability relations to graph not implemented\n";
		return "";
	}

	void Indistinguishability_Relations::set_amount_of_worlds(size_t amount_of_worlds) {
		for (size_t i = 0; i < relations.size(); ++i) {
			while (relations.at(i).size() < amount_of_worlds) {
				relations.at(i).emplace_back(index_counter++);
			}
		}
	}

	void Indistinguishability_Relations::set_amount_of_events(size_t amount_of_events) {
		set_amount_of_worlds(amount_of_events);
	}

	// Transforming from low memory indistinguishability classes to high speed sets of indistinguishability worlds
	// NOTE: relations are agent->world->world, but quick_relations are world->agent->world
	Quick_Relations Indistinguishability_Relations::get_quick_relations() const {
		size_t n_agents = relations.size();
		size_t n_worlds = relations.at(0).size();
		std::vector<std::vector<std::set<World_Id>>> quick_relations(n_worlds, std::vector<std::set<World_Id>>(n_agents));

		for (size_t agent = 0; agent < n_agents; ++agent) {
			for (size_t world = 0; world < n_worlds; ++world) {
				size_t index = relations.at(agent).at(world);
				for (size_t other_world = 0; other_world < relations.at(agent).size(); ++other_world) {
					if (relations.at(agent).at(other_world) == index) {
						quick_relations.at(world).at(agent).insert({ other_world });
					}
				}
			}
		}
		return quick_relations;
	}

	size_t Indistinguishability_Relations::get_class(World_Id world, Agent_Id agent) const {
		return relations.at(agent.id).at(world.id);
	}
	size_t Indistinguishability_Relations::get_class(Event_Id world, Agent_Id agent) const {
		return relations.at(agent.id).at(world.id);
	}

	void Indistinguishability_Relations::add_relation(Agent_Id agent, World_Id world_from, World_Id world_to) {
		size_t from_index = relations.at(agent.id).at(world_from.id);
		size_t to_index = relations.at(agent.id).at(world_to.id);
		for (size_t world = 0; world < relations.at(agent.id).size(); ++world) {
			if (relations.at(agent.id).at(world) == from_index) {
				relations.at(agent.id).at(world) = to_index;
			}
		}
	}
	void Indistinguishability_Relations::add_relation(Agent_Id agent, Event_Id world_from, Event_Id world_to) {
		add_relation(agent, World_Id{ world_from.id }, World_Id{ world_to.id });
	}
}