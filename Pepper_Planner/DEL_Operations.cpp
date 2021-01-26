#include "DEL_Operations.hpp"
#include "Core.hpp"
#include "Formula_Input_Impl.hpp"
#include "Domain.hpp"

namespace del {

	State perform_product_update(const State& state, const Action& action, const std::vector<Agent>& agents, const Domain& domain) {
		State result(state.get_number_of_agents());
		result.set_cost(state.get_cost() + action.get_cost());
		Formula_Input_Impl input = { &state, &domain };

		size_t events_size = action.get_events().size();
		std::vector<std::vector<size_t>> new_relations(domain.get_amount_of_agents());
		const auto& state_relations = state.get_relations();
		const auto& action_relations = action.get_relations();
		size_t index_counter = 0;

		for (auto& world : state.get_worlds()) {
			for (const auto& event : action.get_events()) {
				if (event.get_preconditions().valuate(world.get_id().id, &input)) {

					bool designated = state.is_world_designated(world.get_id()) && action.is_event_designated(event.get_id());
					result.create_world(world, event, designated);

					for (size_t agent = 0; agent < state.get_number_of_agents(); ++agent) {
						size_t index = state_relations.get_class(world.get_id(), { agent }) * events_size + action_relations.get_class(event.get_id(), { agent });
						new_relations.at(agent).push_back(index);
						index_counter = std::max(index_counter, index);
					}
					result.set_relations({ new_relations, index_counter });
				}
			}
		}

		result.copy_perceivability_and_observability(state);
#if REMOVE_UNREACHABLE_WORLDS_ENABLED == 1
		result.remove_unreachable_worlds();
#endif
		return std::move(result);
	}

	State perform_perspective_shift(const State& state, Agent_Id agent_id) {
		State result = State(state);
		result.set_designated_worlds(state.get_designated_world_reachables(agent_id));
		return std::move(result);
	}

	std::vector<State> split_into_global_states(const State& state, const Agent_Id agent) {
		std::vector<State> result;
		for (const auto& designated_world : state.get_designated_worlds()) {
			State new_state = State(state);
			new_state.set_single_designated_world(designated_world);
#if REMOVE_UNREACHABLE_WORLDS_ENABLED == 1
			new_state.remove_unreachable_worlds();
#endif
			result.push_back(std::move(new_state));
		}
		return result;
	}


	bool is_action_applicable(const State& state, const Action& action, const Domain& domain) {
		auto worlds = state.get_designated_world_reachables(action.get_owner());
		Formula_Input_Impl input = { &state, &domain };

		for (auto& world_id : worlds) {
			bool found_applicable_event = false;
			for (const auto& event : action.get_events()) {
				if (action.is_event_designated(event.get_id().id) && event.get_preconditions().valuate(world_id.id, &input)) {
					found_applicable_event = true;
					break;
				}
			}
			if (!found_applicable_event) {
				return false;
			}
		}
		return !worlds.empty();
	}

	State perform_k_bisimilar_contraction(const State& state, size_t depth) {
		State temp1;
		State temp2;
		Bisimulation_Context bisimulation_context(temp1, temp2);
		return bisimulation_context.to_bisimulation_contraction(state, depth);
	}
}