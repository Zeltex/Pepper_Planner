#pragma once

#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <deque>

#include "State.hpp"
#include "Types.hpp"

namespace del {

	//struct Signature {
	//	Signature(const size_t& index_of_block, const size_t& index_in_block, const World_Id& world, const std::vector<std::vector<size_t>>& relations, size_t agents_size, size_t worlds_size):
	//		index_of_block(index_of_block), world(world), data(agents_size* worlds_size, 0), index_in_block(index_in_block) {
	//		if (relations.empty()) return;
	//		for (size_t i = 0; i < relations.size(); ++i) {
	//			for (auto& world_entry : relations.at(i)) {
	//				data.at(i * worlds_size + world_entry) = true;
	//			}
	//		}
	//	}

	//	bool operator==(const Signature& other) {
	//		if (this->data.size() != other.data.size()) return false;
	//		for (size_t i = 0; i < this->data.size(); ++i) {
	//			if (this->data.at(i) != other.data.at(i)) return false;
	//		}
	//		return true;
	//	}

	//	bool operator<(const Signature& other) {
	//		if (this->data.size() < other.data.size()) return true;
	//		else if (this->data.size() > other.data.size()) return false;

	//		for (size_t i = 0; i < this->data.size(); ++i) {
	//			if (this->data.at(i) && !other.data.at(i)) return false;
	//			if (!this->data.at(i) && other.data.at(i)) return true;
	//		}
	//		return false;
	//	}
	
	//	size_t index_of_block;
	//	size_t index_in_block;
	//	World_Id world;
	//	std::vector<bool> data;
	//};

	class Domain;
	class Bisimulation_Context {
	public:
		Bisimulation_Context(const State& state1, const State& state2) : state1(state1), state2(state2) {}

		void partition_into_relations_blocks_contraction(const Quick_Relations& relations);
		State to_bisimulation_contraction(const State& state, size_t k = 0);
	private:
		void initialize_blocks(const State& state, std::unordered_map<std::string, size_t>& valuation_to_block);
		State create_new_state(const State& state, const Quick_Relations& relations);

		State state1;
		State state2;
		std::unordered_map<size_t, size_t> old_to_new_world1;
		std::unordered_map<size_t, size_t> old_to_new_world2;
		std::unordered_map<size_t, bool> is_world_from_state1;
		std::unordered_map<size_t, bool> is_world_designated;
		std::vector<World> worlds;

		// TODO - Store relations more elegantly
		std::vector<std::unordered_map<size_t, std::vector<size_t>>> indistinguishability_relation;
		std::unordered_map<size_t, size_t> world_to_block;
		std::vector<std::set<World_Id>> blocks;
	};
}