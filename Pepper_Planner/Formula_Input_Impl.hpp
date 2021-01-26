#pragma once
#include "Formula_Input_Interface.hpp"
#include "Propositions.hpp"

namespace del {
	class State;
	class Domain;
	class Formula_Input_Impl : public Formula_Input_Interface {
	public:
		Formula_Input_Impl(const State* state, const Domain* domain) : state(state), domain(domain) {}
		Formula_Input_Impl(const State* state) : state(state), domain(nullptr) {}
		virtual bool valuate_prop(Proposition prop, const size_t world_id) const override;
		virtual std::vector<size_t> get_reachable_worlds(Proposition agent_id, size_t world_id) const override;

	private:
		const State* state;
		const Domain* domain;
	};
}

