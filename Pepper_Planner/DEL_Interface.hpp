#pragma once

#include <vector>
#include <string>
#include <optional>

#include "Domain.hpp"
#include "Action.hpp"
#include "Planner.hpp"
#include "Interface_DTO.hpp"
#include "Action_Library.hpp"
#include "Domain_Interface_Implementation.hpp"
#include "Loader.hpp"
#include "Types.hpp"

namespace del {
	class DEL_Interface {
	public:
		DEL_Interface(std::string file_path);
		DEL_Interface(size_t domain_to_load);
		DEL_Interface(State initial_state, Action_Library library);
		Interface_DTO get_next_action();

		bool query(const Formula& query);
		void perform_do(const Agent_Id i, const std::vector<Proposition_Instance>& add, const std::vector<Proposition_Instance>& del);
		void perform_oc(const Agent_Id i, std::vector<std::vector<std::string>>&& add, std::vector<std::vector<std::string>>&& del);
		void perform_oc(const std::string owner_name, std::vector<std::vector<std::string>>&& add, std::vector<std::vector<std::string>>&& del);

		void perform_action(Action action);
		void perform_action(const std::string& name, const std::string& owner, const std::vector<std::string>& arguments);
		bool create_policy(Formula goal, const std::string& planning_agent, const bool is_benchmark=false);
		bool create_policy(const std::string& planning_agent, const bool is_benchmark=false);
		bool is_solved();

		const std::unordered_map<std::string, Atom_Id>& get_proposition_context();
		std::unordered_map<std::string, size_t> get_belief_context();

        //TODO Temporary access to domain
        Domain* get_domain() { return &domain; }
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
