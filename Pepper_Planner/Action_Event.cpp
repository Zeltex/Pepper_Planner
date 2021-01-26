#include "Action_Event.hpp"
#include "Domain.hpp"

namespace del {

	Event_Id Action_Event::get_id() const {
		return id;
	}

	const Propositions& Action_Event::get_add_list() const {
		return proposition_add;
	}

	const Propositions& Action_Event::get_delete_list() const {
		return proposition_delete;
	}

	const Formula& Action_Event::get_preconditions() const {
		return precondition;
	}

	std::string Action_Event::get_name() const {
		return name;
	}

	std::string Action_Event::to_string(const Domain& domain) const {
		return "Event " 
			+ std::to_string(id.id) 
			+ ": (Preconditions: " 
			+ precondition.to_string(domain.get_id_to_atom()) 
			+ ") (Add list"
			+ proposition_add.to_string(domain)
			+ ") (Delete list, "
			+ proposition_delete.to_string(domain)
			+ ")";
	}
}