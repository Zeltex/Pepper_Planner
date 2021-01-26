#include "pch.h"
#include "CppUnitTest.h"

#include <iostream>

#include "Formula.hpp"
#include "Formula_Component.hpp"
#include "State.hpp"
#include "Action.hpp"
#include "Action_Event.hpp"
#include "DEL_Operations.hpp"
#include "Types.hpp"
#include "DEL_Operations.hpp"
#include "Bisimulation_Context.hpp"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace PepperPlannerTests
{
	using namespace del;
	TEST_CLASS(DEL_Operations_Bisimilar_Contraction_Tests)
	{
	public:

		TEST_METHOD(Bisimilar_Contraction_Merge_Worlds) {
			//State state(1);
			//state.create_worlds(4);
			//state.add_designated_world({ 0 });
			//state.add_indistinguishability_relation({ 0 }, { 0 }, { 1 });
			//state.add_indistinguishability_relation({ 0 }, { 0 }, { 2 });
			//state.add_indistinguishability_relation({ 0 }, { 1 }, { 3 });
			//state.add_indistinguishability_relation({ 0 }, { 2 }, { 3 });

			//state.add_true_propositions({ 0 }, { {"p"} });
			//state.add_true_propositions({ 1 }, { {"q"} });
			//state.add_true_propositions({ 2 }, { {"q"} });
			//state.add_true_propositions({ 3 }, { {"p"} });

			//Bisimulation_Context context = { state, state };

			//State bisim = context.to_bisimulation_contraction(state, 2);

			//Assert::AreEqual({ 2 }, bisim.get_worlds_count());
			//Assert::AreEqual({ 2 }, bisim.get_indistinguishability_relations()[0].size());

			//auto h1 = state.to_hash();
			//auto h2 = bisim.to_hash();

			////auto hash1 = std::hash<std::string>()(h1);
			////auto hash2 = std::hash<std::string>()(h2);

			//size_t debug = 0;
		}

		TEST_METHOD(Contraction_Makes_Hash_Match) {
			State state(1);
			state.create_worlds(2);
			state.add_designated_world({ 0 });
			state.add_indistinguishability_relation({ 0 }, { 0 }, { 1 });
			state.add_indistinguishability_relation({ 0 }, { 1 }, { 0 });
			state.add_indistinguishability_relation({ 0 }, { 0 }, { 0 });
			state.add_indistinguishability_relation({ 0 }, { 1 }, { 1 });
			state.add_true_proposition({ 0 }, { 0 });
			state.add_true_proposition({ 1 }, { 1 });


			State state2(1);
			state2.create_worlds(2);
			state2.add_designated_world({ 1 });
			state2.add_indistinguishability_relation({ 0 }, { 0 }, { 1 });
			state2.add_indistinguishability_relation({ 0 }, { 1 }, { 0 });
			state2.add_indistinguishability_relation({ 0 }, { 0 }, { 0 });
			state2.add_indistinguishability_relation({ 0 }, { 1 }, { 1 });
			state2.add_true_proposition({ 0 }, { 1 });
			state2.add_true_proposition({ 1 }, { 0 });

			std::cout << state.to_hash() << std::endl;
			std::cout << state2.to_hash() << std::endl;


			Assert::AreNotEqual(state.to_hash(), state2.to_hash());


			Bisimulation_Context context = { state, state };
			State bisim = context.to_bisimulation_contraction(state, 2);

			Bisimulation_Context context2 = { state2, state2 };
			State bisim2 = context2.to_bisimulation_contraction(state2, 2);

			Assert::AreEqual(bisim.to_hash(), bisim2.to_hash());

			//Assert::AreEqual({ 2 }, bisim.get_worlds_count());
			//Assert::AreEqual({ 2 }, bisim.get_indistinguishability_relations()[0].size());

			//auto h1 = state.to_hash();
			//auto h2 = bisim.to_hash();

			////auto hash1 = std::hash<std::string>()(h1);
			////auto hash2 = std::hash<std::string>()(h2);

			//size_t debug = 0;
		}
	};
}