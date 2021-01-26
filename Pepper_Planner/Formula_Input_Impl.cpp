#include "Formula_Input_Impl.hpp"
#include "State.hpp"
#include "Domain.hpp"

namespace del {

	bool Formula_Input_Impl::valuate_prop(Proposition prop, const size_t world_id) const {
		auto& propositions_state = state->get_true_propositions(world_id);
		if (propositions_state.valuate(prop)) return true;
		if (domain != nullptr) {
			auto& propositions_domain = domain->get_atom_rigids();
			if (propositions_domain.valuate(prop)) return true;
		}
		return false;
	}
	
	std::vector<size_t> Formula_Input_Impl::get_reachable_worlds(Proposition agent_id, size_t world_id) const {
		return state->get_reachable_worlds(agent_id.to_hash(), world_id);
	}

}