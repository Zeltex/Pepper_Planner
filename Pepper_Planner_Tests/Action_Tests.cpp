#include "pch.h"
#include "CppUnitTest.h"

#include <iostream>

#include "Types.hpp"
#include "DEL_Operations.hpp"
#include "Bisimulation_Context.hpp"
#include "Domain.hpp"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace PepperPlannerTests
{
	using namespace del;
	TEST_CLASS(Action_Tests)
	{
	public:

		void test_it(Formula&&  formula, bool pass) {
			Action action({ 0 }, 2);
			

			Action_Event event1 = Action_Event(Event_Id{ 0 }, std::move(formula), std::vector<Proposition>(), std::vector<Proposition>());
			action.add_event(event1);
			for (size_t agent : { 0, 1 }) {
				action.add_reachability({ agent }, { 0 }, { 0 });
			}
			action.add_designated_event(Event_Id{ 0 });



			State state;
			state.set_amount_of_agents(2);
			auto& world1 = state.create_world();
			world1.add_true_proposition({ 0 });
			world1.add_true_proposition({ 1 });
			auto& world2 = state.create_world();
			world2.add_true_proposition({ 1 });
			state.add_indistinguishability_set({ 1 }, { {0}, {1} });
			//state.add_indistinguishability_relation({ 1 }, { 0 }, { 1 });
			//state.add_indistinguishability_relation({ 1 }, { 1 }, { 1 });
			//state.add_indistinguishability_relation({ 0 }, { 0 }, { 0 });
			//state.add_indistinguishability_relation({ 0 }, { 1 }, { 1 });

			state.set_designated_worlds({ {0} });

			Domain domain;
			Assert::AreEqual(pass, is_action_applicable(state, action, domain));
		}
		TEST_METHOD(Test_Action_Applicability_One_Reachable_False) {
			Formula f1;
			f1.f_believes(1, f1.f_prop({ 0 }));
			test_it(std::move(f1), false);
		}

		TEST_METHOD(Test_Action_Applicability_One_Reachable_True) {
			Formula f1;
			f1.f_believes(0, f1.f_prop({ 0 }));
			test_it(std::move(f1), true);
		}
	};
}