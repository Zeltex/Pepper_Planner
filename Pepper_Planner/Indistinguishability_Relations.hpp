#pragma once
#include <vector>
#include <set>
#include <unordered_map>
#include <assert.h>

#include "Types.hpp"


namespace del {
	class Signature {
	public:
		Signature(std::vector<size_t> signature) : signature(signature) {}
		bool equals(const Signature& other) const {
			if (signature.size() != other.signature.size()) return false;
			for (size_t i = 0; i < signature.size(); ++i) {
				if (signature.at(i) != other.signature.at(i)) return false;
			}
			return true;
		}
	private:
		std::vector<size_t> signature;
	};
	class Indistinguishability_Relations;
	class Quick_Relations {
	public:
		Quick_Relations(std::vector<std::vector<std::set<World_Id>>> relations) :relations(relations) {}
		Signature get_signature(World_Id world, std::unordered_map<size_t, size_t> converter) const {
			size_t n_worlds = relations.size();
			size_t n_agents = relations.at(0).size();
			std::vector<size_t> signature;
			for (size_t agent = 0; agent < n_agents; ++agent) {
				for (const auto& world_to : relations.at(world.id).at(agent)) {
					signature.push_back(agent * n_worlds + converter.at(world_to.id));
				}
			}
			return Signature(signature);
		}
		std::pair<std::vector<std::vector<size_t>>, size_t> convert_to_block_relations(const std::unordered_map<size_t, size_t>& world_to_block,
			const std::unordered_map<size_t, World_Id>& block_to_new_world, size_t n_agents, size_t n_worlds) const {

			size_t relation_index = 0;
			std::vector<std::vector<size_t>> new_relations(n_agents);
			for (size_t i = 0; i < new_relations.size(); ++i) {
				for (size_t j = 0; j < n_worlds; ++j) {
					new_relations.at(i).emplace_back(relation_index++);
				}
			}
			const size_t initial_index = relation_index;

			for (size_t world_from = 0; world_from < relations.size(); ++world_from) {
				for (size_t agent = 0; agent < relations.at(world_from).size(); ++agent) {
					std::unordered_set<size_t> seen_indices;
					for (const auto& world_to : relations.at(world_from).at(agent)) {
						auto world = block_to_new_world.at(world_to_block.at(world_to.id));
						if (new_relations.at(agent).at(world.id) >= initial_index) {
							seen_indices.insert(new_relations.at(agent).at(world.id));
						}
						new_relations.at(agent).at(world.id) = relation_index;
					}
					++relation_index;
					assert(seen_indices.size() < 2);
				}
			}
			return { new_relations, relation_index };
		}
	private:
		// world -> agent -> world
		std::vector<std::vector<std::set<World_Id>>> relations;
	};

	class Indistinguishability_Relations {
	public:
		Indistinguishability_Relations();
		Indistinguishability_Relations(size_t number_of_agents);
		Indistinguishability_Relations(size_t number_of_agents, size_t number_of_worlds);
		Indistinguishability_Relations(std::vector<std::vector<size_t>> relations, size_t index_counter);
		void insert(Agent_Id agent, const std::unordered_set<World_Id>& worlds);
		std::vector<World_Id> get_reachables(Agent_Id agent, const std::vector<World_Id>& worlds) const;
		std::vector<World_Id> get_reachables(const std::vector<World_Id>& worlds) const;
		std::set<World_Id> get_reachables_set(const std::vector<World_Id>& worlds) const;
		bool is_indistinguishable(Agent_Id agent, World_Id world1, World_Id world2) const;
		void set_amount_of_agents(size_t number_of_agents);
		void convert(const std::unordered_map<size_t, size_t>& converter, const size_t new_world_size);
		size_t get_size() const;
		size_t get_size(Agent_Id agent) const;
		bool operator==(const Indistinguishability_Relations& other) const;
		bool operator!=(const Indistinguishability_Relations& other) const;
		std::string to_hash() const;
		std::string to_string() const;
		std::string to_graph() const;
		void set_amount_of_worlds(size_t amount_of_worlds);
		void set_amount_of_events(size_t amount_of_events);
		size_t get_class(World_Id world, Agent_Id agent) const;
		size_t get_class(Event_Id world, Agent_Id agent) const;
		void add_relation(Agent_Id agent, World_Id world_from, World_Id world_to);
		void add_relation(Agent_Id agent, Event_Id world_from, Event_Id world_to);

		Quick_Relations get_quick_relations() const;
	private:
		size_t index_counter;
		std::vector<std::vector<size_t>> relations;
	};
}