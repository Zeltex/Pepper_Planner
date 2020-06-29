#pragma once

#include <vector>
#include <string>

#include "Domain.hpp"
#include "Action.hpp"
#include "Planner.hpp"
#include "Interface_DTO.hpp"
#include "Action_Library.hpp"
#include "Environment_Loader.hpp"
#include "Domain_Interface_Implementation.h"
#include "Loader.hpp"
#include "Types.hpp"

namespace del {
	class DEL_Interface {
	public:
		DEL_Interface(std::string file_path);
		DEL_Interface(size_t domain_to_load);
		DEL_Interface(State initial_state, Action_Library library);
		Interface_DTO get_next_action();
		void perform_do(const Agent_Id i, const std::vector<Proposition_Instance>& add, const std::vector<Proposition_Instance>& del);
		void perform_oc(const Agent_Id i, std::vector<Proposition_Instance>&& add, std::vector<Proposition_Instance>&& del);
		void perform_oc(const std::string owner_name, std::vector<Proposition_Instance>&& add, std::vector<Proposition_Instance>&& del);

		void perform_action(Action action);
		void perform_action(std::string name, std::string owner, std::vector<std::string> arguments);
		bool create_policy(Formula goal);
		bool create_policy();
		bool is_solved();
	private:
		Domain domain;
		Planner planner;
		bool has_policy;
		Policy policy;
		Action_Library action_library;
		Agent_Id pepper_id;
		Formula goal;
	};
}