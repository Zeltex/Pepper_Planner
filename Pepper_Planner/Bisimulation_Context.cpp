#include "Bisimulation_Context.hpp"
#include <set>

#include "Propositions.hpp"

namespace del {

	State Bisimulation_Context::to_bisimulation_contraction(const State& state, size_t k) {

		auto relations = state.get_quick_relations();
		std::unordered_map<std::string, size_t> valuation_to_block;

		initialize_blocks(state, valuation_to_block);
		partition_into_relations_blocks_contraction(relations);
		return create_new_state(state, relations);
	}

	State Bisimulation_Context::create_new_state(const State& state, const Quick_Relations& relations) {
		State result(state.get_number_of_agents());
		result.set_cost(state.get_cost());
		std::unordered_map<size_t, World_Id> block_to_new_world;
		std::vector<std::vector<std::set<size_t>>> block_relations(state.get_number_of_agents(), std::vector<std::set<size_t>>(state.get_worlds_count()));


		// Create new worlds
		for (size_t block = 0; block < this->blocks.size(); ++block) {
			auto& world = result.create_world();
			block_to_new_world[block] = world.get_id();
			auto& props = state.get_world(*blocks[block].begin()).get_true_propositions();
			world.add_true_propositions(props);
		}

		auto[new_relations, relation_index] = relations.convert_to_block_relations(world_to_block, block_to_new_world, state.get_number_of_agents(), blocks.size());
		result.set_relations(Indistinguishability_Relations(new_relations, relation_index));

		// Set designated worlds
		std::unordered_set<size_t> visited;
		visited.reserve(state.get_designated_worlds_count());
		for (auto& designated_world : state.get_designated_worlds()) {
			auto& block = world_to_block[designated_world.id];
			auto& world = block_to_new_world[block];
			if (visited.find(world.id) == visited.end()) {
				visited.insert(world.id);
				result.add_designated_world(world);
			}
		}

		return result;
	}
	
	void Bisimulation_Context::initialize_blocks(const State& state, std::unordered_map<std::string, size_t>& valuation_to_block) {
		std::set<std::string> valuations;

		// Get sorted valuations
		for (auto& world : state.get_worlds()) {
			valuations.insert(world.get_true_propositions().to_compact_string());
		}

		// Initialise blocks
		blocks = std::vector<std::set<World_Id>>(valuations.size());
		size_t block_counter = 0;
		for (auto& valuation : valuations) {
			valuation_to_block[valuation] = block_counter++;
		}

		// Set block/world mapping
		for (const auto& world : state.get_worlds()) {
			std::string valuation = world.get_true_propositions().to_compact_string();
			blocks[valuation_to_block[valuation]].insert(world.get_id());
			world_to_block[world.get_id().id] = valuation_to_block[valuation];
		}
	}

	void Bisimulation_Context::partition_into_relations_blocks_contraction(const Quick_Relations& relations) {
		size_t blocks_size = blocks.size();
		bool changed = false;

		for (size_t i = 0; i < blocks_size; ++i) {
			std::set<World_Id> new_block;
			size_t new_block_index = blocks.size();
			auto block_signature = relations.get_signature(*blocks.at(i).begin(), world_to_block);

			for (auto iter = blocks.at(i).begin(); iter != blocks.at(i).end();) {
				auto signature = relations.get_signature(*iter, world_to_block);

				if (!signature.equals(block_signature)) {
					new_block.insert(*iter);
					world_to_block[iter->id] = new_block_index;
					iter = blocks.at(i).erase(iter);
				} else {
					++iter;
				}
			}

			if (!new_block.empty()) {
				blocks.push_back(new_block);
				changed = true;
			}
		}
		if (changed) {
			partition_into_relations_blocks_contraction(relations);
		}
	}	
}