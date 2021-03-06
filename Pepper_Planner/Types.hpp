#pragma once
#include <cstddef>
#include <vector>
#include <optional>
#include "Formula.hpp"

namespace del {
	struct Node_Id {
		size_t id;

		bool operator==(const Node_Id& other) const {
			return this->id == other.id;
		}

        bool operator!=(const Node_Id& other) const {
            return this->id != other.id;
        }
	};

	struct Agent_Id {
		size_t id;

		bool operator==(const Agent_Id& other) const {
			return this->id == other.id;
		}

        bool operator!=(const Agent_Id& other) const {
            return this->id != other.id;
        }
		bool operator<(const Agent_Id& other) const {
			return this->id < other.id;
		}
	};

	struct World_Id {
		size_t id;

		bool operator==(const World_Id& other) const {
			return this->id == other.id;
		}

        bool operator!=(const World_Id& other) const {
            return this->id != other.id;
        }

		bool operator<(const  World_Id& other) const {
			return this->id < other.id;
		}

		bool operator>(const  World_Id& other) const {
			return this->id > other.id;
		}
	};

	struct Event_Id {
		size_t id;

		bool operator==(const Event_Id& other) const {
			return this->id == other.id;
		}

        bool operator!=(const Event_Id& other) const {
            return this->id != other.id;
        }

		Event_Id(size_t id) : id(id) {};
		Event_Id(const Event_Id& other) :id(other.id) {}
	};

	enum class Node_Type {
		Or,
		And
	};

	struct World_Relation {
		World_Relation(World_Id world_from, World_Id world_to) :
			world_from(world_from), world_to(world_to) {}
		World_Id world_from;
		World_Id world_to;
		bool operator!=(const World_Relation& other) const { return (world_from.id != other.world_from.id || world_to.id != other.world_to.id); }
		std::string to_hash() const { return std::to_string(world_from.id) + std::to_string(world_to.id); }
	};

	struct Event_Relation {
		Event_Relation(Event_Id event_from, Event_Id event_to) :
			event_from(event_from), event_to(event_to) {}
		Event_Id event_from;
		Event_Id event_to;
	};

	struct World_Entry {
		World_Entry(World_Id old_world, Event_Id old_event, World_Id new_world) : 
			old_world(old_world), old_event(old_event), new_world(new_world) {}
		World_Id old_world;
		Event_Id old_event;
		World_Id new_world;
	};

	struct Edge_Condition {
		Edge_Condition(std::string event_from, std::string event_to, Formula&& condition) :
			event_from(event_from), event_to(event_to), condition(std::move(condition)) {}
		std::string event_from;
		std::string event_to;
		Formula condition;
	};

	class Domain;
	struct Agent_Edges {
		Agent_Edges(size_t agents) : conditions(agents), current_size(0) {}
		std::unordered_map<size_t, std::unordered_map<size_t, Formula>> conditions;
		void insert(Event_Id event_from, Event_Id event_to, Formula&& condition);
		std::optional<const Formula*> get_condition(Event_Id event_from, Event_Id event_to) const;
		size_t size() const;
		std::string to_graph(const std::string& agent_name, const std::string& base_id, const Domain& domain) const;
	private:
		size_t current_size;
	};

	class Node;
	struct Node_Entry {
		Node_Entry(Node_Id id, size_t cost): id(id), cost(cost){}
		Node_Id id;
		size_t cost;

	};
	struct Node_Entry_Comparator {
		bool operator()(const Node_Entry& lhs, const Node_Entry& rhs);
	};

	class State;
	struct State_Hasher {
		std::size_t operator()(const State& state) const;
	};

}
namespace std {
	template<>
	struct hash<del::World_Id>
	{
		size_t
			operator()(const del::World_Id& obj) const
		{
			return hash<size_t>()(obj.id);
		}
	};
}