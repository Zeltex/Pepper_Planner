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

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace PepperPlannerTests
{
	using namespace del;
	TEST_CLASS(DEL_Operations_Tests)
	{
	public:

		State get_state() {
			std::unordered_map<std::string, Atom_Id> atom_to_id;
			atom_to_id["red"] = 0;
			atom_to_id["Box0"] = 1;
			atom_to_id["Box1"] = 2;
			atom_to_id["Box2"] = 3;

			//Agents; 0:Pepper, 1:L
			State state(2);
			state.create_world();
			state.create_world();
			state.create_world();
			state.create_world();
			state.add_true_proposition(World_Id{ 0 }, { 0 });
			state.add_true_proposition(World_Id{ 1 }, { 1 });
			state.add_true_proposition(World_Id{ 2 }, { 1 });
			state.add_true_proposition(World_Id{ 3 }, { 2 });

			for (size_t i = 0; i < 4; i++) {
				state.add_indistinguishability_relation(Agent_Id{ 0 }, World_Id{ i }, World_Id{ i });
			}

			state.add_indistinguishability_relation(Agent_Id{ 1 }, World_Id{ 1 }, World_Id{ 0 });
			state.add_indistinguishability_relation(Agent_Id{ 1 }, World_Id{ 1 }, World_Id{ 1 });
			state.add_indistinguishability_relation(Agent_Id{ 1 }, World_Id{ 1 }, World_Id{ 2 });
			state.add_indistinguishability_relation(Agent_Id{ 1 }, World_Id{ 1 }, World_Id{ 3 });

			for (size_t i : {0, 2, 3}) {
				for (size_t j : {0, 2, 3}) {
					state.add_indistinguishability_relation(Agent_Id{ 1 }, World_Id{ i }, World_Id{ j });
				}
			}


			state.add_designated_world(World_Id{ 1 });
			return state;
		}

		TEST_METHOD(Perspective_Shift_Agent_P) {
			auto state = get_state();

			

			Agent agent(Agent_Id{ 0 }, { 0 }, "Pepper");
			auto new_state = perform_perspective_shift(state, agent.get_id());


			Assert::AreEqual(size_t{ 1 }, new_state.get_designated_worlds().size());
			Assert::AreEqual(size_t{ 4 }, new_state.get_relations().get_size(Agent_Id{ 0 }));
			Assert::AreEqual(size_t{ 13 }, new_state.get_relations().get_size(Agent_Id{ 1 }));

			Assert::IsTrue(new_state.is_one_reachable(Agent_Id{ 1 }, World_Id{ 0 }, World_Id{ 0 }));
			Assert::IsTrue(new_state.is_one_reachable(Agent_Id{ 0 }, World_Id{ 0 }, World_Id{ 0 }));
		}

		TEST_METHOD(Perspective_Shift_Agent_L) {


			std::unordered_map<std::string, Atom_Id> atom_to_id;
			atom_to_id["red"] = 0;
			atom_to_id["Box0"] = 1;
			atom_to_id["Box1"] = 2;
			atom_to_id["Box2"] = 3;

			//Agents; 0:Pepper, 1:L
			State state(2);
			state.create_world();
			state.create_world();
			state.create_world();
			state.create_world();
			state.add_true_proposition(World_Id{ 0 }, {0});
			state.add_true_proposition(World_Id{ 1 }, {1});
			state.add_true_proposition(World_Id{ 2 }, {1});
			state.add_true_proposition(World_Id{ 3 }, {2});

			state.add_indistinguishability_relation(Agent_Id{ 0 }, World_Id{ 1 }, World_Id{ 1 });

			state.add_indistinguishability_relation(Agent_Id{ 1 }, World_Id{ 1 }, World_Id{ 0 });
			state.add_indistinguishability_relation(Agent_Id{ 1 }, World_Id{ 1 }, World_Id{ 1 });
			state.add_indistinguishability_relation(Agent_Id{ 1 }, World_Id{ 1 }, World_Id{ 2 });
			state.add_indistinguishability_relation(Agent_Id{ 1 }, World_Id{ 1 }, World_Id{ 3 });

			for (size_t i : {0, 2, 3}) {
				for (size_t j : {0, 2, 3}) {
					state.add_indistinguishability_relation(Agent_Id{ 1 }, World_Id{ i }, World_Id{ j });
					state.add_indistinguishability_relation(Agent_Id{ 0 }, World_Id{ i }, World_Id{ j });
				}
			}


			state.add_designated_world(World_Id{ 1 });

			Agent agent(Agent_Id{ 1 }, { atom_to_id.size() }, "L");
			auto new_state = perform_perspective_shift(state, agent.get_id());


			auto& worlds = new_state.get_worlds();
			Assert::AreEqual(size_t{ 4 }, worlds.size());
			auto temp_size = new_state.get_relations().get_size(Agent_Id{ 0 });
			Assert::AreEqual(size_t{ 10 }, new_state.get_relations().get_size(Agent_Id{ 0 }));
			Assert::AreEqual(size_t{ 13 }, new_state.get_relations().get_size(Agent_Id{ 1 }));

			Assert::IsTrue(new_state.is_one_reachable(Agent_Id{ 1 }, World_Id{ 1 }, World_Id{ 1 }));
			Assert::IsTrue(new_state.is_one_reachable(Agent_Id{ 0 }, World_Id{ 1 }, World_Id{ 1 }));

			Assert::AreEqual(size_t{ 4 }, new_state.get_designated_worlds_count());
		}
	};
}