#pragma once
namespace del {


#if 0
#define BISIMILAR_USE_HASH_ENABLED 1
#define REMOVE_UNREACHABLE_WORLDS_ENABLED 1
#define BISIM_COMPARISON_ENABLED 1
#define BISIM_CONTRACTION_ENABLED 1
#define DEBUG_PRINT 1
#define PRINT_PARTIAL 1
#endif

// Planner options
#ifndef BISIMILAR_USE_HASH_ENABLED
#define BISIMILAR_USE_HASH_ENABLED 1
#endif
#ifndef REMOVE_UNREACHABLE_WORLDS_ENABLED
#define REMOVE_UNREACHABLE_WORLDS_ENABLED 1
#endif
#ifndef BISIM_COMPARISON_ENABLED 
#define BISIM_COMPARISON_ENABLED 1
#endif
#ifndef BISIM_CONTRACTION_ENABLED
#define BISIM_CONTRACTION_ENABLED 0
#endif
#ifndef DEBUG_PRINT 
#define DEBUG_PRINT 0
#endif
#ifndef PRINT_PARTIAL
#define PRINT_PARTIAL 0
#endif

#define PERCEIVES "perceives"
#define OBSERVES "observes"
#define ANNOUNCE_COST 100
#define ANNOUNCE_NAME "Announce"
#define BISIMILAR_DEPTH 5

#ifndef DEBUG_PRINT_PATH
#define DEBUG_PRINT_PATH "../Debug_Output/"
#endif


#if DEBUG_PRINT == 1

	class Action;
	class Agent;
	class Domain;
	class Graph;
	class Policy;
	class State;

	void print_graph_dot(const Graph& graph, const Domain& domain);
	void print_graph_dot_simple(const Graph& graph, const Domain& domain);
	void print_graph_dot_partial(const Graph& graph, const Domain& domain);
	void print_graph(const Graph& graph, const Domain& domain);
	void print_action(const Action& action, const Domain& domain, size_t counter);
	void print_action_to_console(const Action& action, const Domain& domain);
	void print_state(const State& state, const Domain& domain, size_t counter);
	void clear_action_state_dir();
	void print_policy(const Policy& policy, const Domain& domain);

	#define PRINT_POLICY(policy, domain) print_policy(policy, domain)
	#define PRINT_GRAPH(graph, domain) print_graph(graph,domain)
	#define PRINT_ACTION(action, domain, counter) print_action(action, domain, counter)
	#define PRINT_ACTION_TO_CONSOLE(action, domain) print_action_to_console(action, domain)
	#define PRINT_STATE(state, domain, counter) print_state(state, domain, counter)
	#define CLEAR_ACTION_STATE_DIR() clear_action_state_dir()

	#if PRINT_PARTIAL == 1
		//#define PRINT_GRAPH_DOT(graph, domain) print_graph_dot_simple(graph,domain);print_graph_dot_partial(graph,domain)
		#define PRINT_GRAPH_DOT(graph, domain) print_graph_dot_simple(graph,domain)
	#else
		#define PRINT_GRAPH_DOT(graph, domain) print_graph_dot(graph,domain);print_graph_dot_simple(graph,domain)
	#endif
#else
	#define PRINT_POLICY(policy, domain) 
	#define PRINT_ACTION(action, domain, counter) 
	#define PRINT_ACTION_TO_CONSOLE(action, domain)
	#define PRINT_STATE(state, domain, counter)
	#define PRINT_GRAPH_DOT(graph, domain)
	#define PRINT_GRAPH(graph, domain)
	#define CLEAR_ACTION_STATE_DIR()
#endif
}
