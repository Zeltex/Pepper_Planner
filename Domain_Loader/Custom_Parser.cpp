#include "Custom_Parser.hpp"

namespace del {
    Custom_Parser::Custom_Parser(Domain_Interface* domain_input, Domain_Buffer* buffer_input, Custom_Lexer* lexer)
            : domain(domain_input), buffer(buffer_input), lexer(lexer), pointer(0), value_pointer(0), tokens_size(lexer->tokens.size()) {
        maepl();
    }

    bool Custom_Parser::try_match(const std::vector<Token>& pattern) {
        if (pointer + pattern.size() > tokens_size) {
            return false;
        }
        for (size_t i = 0; i < pattern.size(); i++) {
            if (pattern[i] != lexer->tokens[pointer + i]) {
                return false;
            }
        }
        value_pointer = pointer;
        pointer += pattern.size();
        return true;
    }

    bool Custom_Parser::must_match(const std::vector<Token>& pattern) {
        if (!try_match(pattern)) {
            std::cerr << "Syntax error line: " << lexer->line_numbers[pointer] << ". Expected token" << (pattern.size() > 1 ? "s " : " ");
            for (auto& entry : pattern) {
                std::cerr << token_to_string(entry) << " ";
            }
            if (pointer < lexer->tokens.size()) {
                std::cerr << " Found " << token_to_string(lexer->tokens[pointer]);
            }
            std::cerr << std::endl;
            // TODO - Throw custom error
            exit(-1);
        }
        else {
            return true;
        }
    }

    size_t Custom_Parser::get_ivalue(size_t argument_index) {
        return lexer->values.at(value_pointer + argument_index).ival;
    }

    const std::string& Custom_Parser::get_svalue(size_t argument_index) {
        return lexer->values.at(value_pointer + argument_index).sval;
    }

    bool Custom_Parser::get_bvalue(size_t argument_index) {
        return lexer->values.at(value_pointer + argument_index).bval;
    }

    std::string Custom_Parser::token_to_string(Token token) {
        switch (token) {
            case Token::ACTION_DEF				: return "_action";
            case Token::ANNOUNCE_DEF			: return "_announce";
            case Token::COST_DEF				: return "_announce";
            case Token::DESIGNATED_EVENTS_DEF	: return "_designated_events";
            case Token::DESIGNATED_WORLDS_DEF	: return "_designated_worlds";
            case Token::DOMAIN_DEF				: return "_domain";
            case Token::EFFECT_ADD_DEF			: return "_effect_add";
            case Token::EFFECT_DELETE_DEF		: return "_effect_delete";
            case Token::EVENT_DEF				: return "_event";
            case Token::GOAL_DEF				: return "_goal";
            case Token::INIT_DEF				: return "initial_state";
            case Token::OBJECTS_DEF				: return "_objects";
            case Token::OWNER_DEF				: return "_owner";
            case Token::PRECONDITIONS_DEF		: return "_preconditions";
            case Token::PROBLEM_DEF				: return "_problem";
            case Token::PROPOSITIONS_DEF		: return "_propositions";
            case Token::REACHABILITY_DEF		: return "_reachability";
            case Token::REFLEXIVITY_DEF			: return "_reflexivity";
            case Token::REST_DEF				: return "_rest";
            case Token::TYPES_DEF				: return "_types";
            case Token::WORLD_DEF				: return "_world";
            case Token::LBRACK					: return "(|[|{";
            case Token::RBRACK					: return ")|]|}";
            case Token::BELIEVES				: return "BELIEF";
            case Token::AND						: return "AND";
            case Token::OR						: return "OR";
            case Token::NOT						: return "NOT";
            case Token::TOP						: return "TOP";
            case Token::NAME					: return "NAME";
            case Token::INTEGER					: return "INTEGER";
            case Token::TRUTH					: return "true|false";
            case Token::EQUALS					: return "=";
        }
    }

    void Custom_Parser::maepl() {
        if (try_match({ Token::DOMAIN_DEF, Token::NAME, Token::LBRACK})) {
            domain->new_domain(get_svalue(1));
            domain_body();
            if (!must_match({ Token::RBRACK })) return;
            domain->finish_domain();
            return maepl();
        }
        if (try_match({ Token::PROBLEM_DEF, Token::NAME, Token::LBRACK })) {
            buffer->clear_seen_atoms();
            problem_body();
            if (!must_match({ Token::RBRACK })) return;
            domain->finish_problem();
            return maepl();
        }
        return;
    }

    void Custom_Parser::domain_body() {

        if (try_match({ Token::ANNOUNCE_DEF, Token::EQUALS, Token::TRUTH })) {
            if (get_bvalue(2)) domain->set_announce_enabled();
            return domain_body();
        }

        if (try_match({ Token::TYPES_DEF, Token::EQUALS, Token::LBRACK })) {
            variables();
            if (!must_match({ Token::RBRACK })) return;
            buffer->push_types();
            domain->set_types(buffer->get_types());

            if (!must_match({ Token::PROPOSITIONS_DEF, Token::EQUALS, Token::LBRACK })) return;
            propositions();
            buffer->get_proposition_instances();
            if (!must_match({ Token::RBRACK })) return;

            return actions();
        }
        std::cerr << "Syntax error line: " << lexer->line_numbers[pointer] << std::endl;
        exit(-1);
    }

    // Object def must come before world def and designated worlds def
    // Need to implement restrictions and proper error messages
    void Custom_Parser::problem_body() {
        if (try_match({Token::DOMAIN_DEF, Token:: EQUALS, Token:: NAME})) {
            domain->set_domain(get_svalue(2));
            return problem_body();
        }

        if (try_match({ Token:: OBJECTS_DEF, Token::EQUALS, Token:: LBRACK })) {
            objects();
            domain->set_objects(buffer->get_objects(), buffer->get_atom_to_id());
            if (!must_match({ Token::RBRACK })) return;
            return problem_body();
        }

        if (try_match({ Token:: INIT_DEF, Token:: EQUALS, Token:: LBRACK})) {
            buffer->clear_proposition_instances();
            proposition_instances();
            domain->set_initial_propositions(buffer->get_proposition_instances());
            if (!must_match({ Token::RBRACK })) return;
            return problem_body();
        }

        if (try_match({ Token::WORLD_DEF, Token::NAME, Token::LBRACK })) {
            auto world_name = get_svalue(1);
            buffer->clear_proposition_instances();
            proposition_instances();
            domain->create_world(world_name, buffer->get_proposition_instances());
            if (!must_match({ Token::RBRACK })) return;
            return problem_body();
        }

        if (try_match({ Token::GOAL_DEF, Token::EQUALS, Token::LBRACK })) {
            formula();
            domain->set_goal(buffer->get_formula(), buffer->get_instance_to_proposition(), buffer->get_atom_to_id());
            if (!must_match({ Token::RBRACK })) return;
            return problem_body();
        }

        if (try_match({ Token::DESIGNATED_WORLDS_DEF, Token::EQUALS, Token::LBRACK })) {
            variables();
            domain->set_designated_worlds(buffer->get_variables());
            if (!must_match({ Token::RBRACK })) return;
            return problem_body();
        }

        if (try_match({ Token::REACHABILITY_DEF, Token::EQUALS, Token::LBRACK })) {
            reachability_body();
            if (!must_match({ Token::RBRACK })) return;
            return problem_body();
        }

        if (try_match({ Token::REFLEXIVITY_DEF, Token::EQUALS, Token::TRUTH })) {
            buffer->set_state_reflexivity(get_bvalue(2));
            return problem_body();
        }
    }

    void Custom_Parser::reachability_body() {
        if (try_match({ Token::NAME, Token::EQUALS, Token::LBRACK })) {
            auto& agent_name = get_svalue(0);
            buffer->clear_relations();
            reachability_body_recurse();
            domain->add_problem_relations(buffer->translate_atom_to_id(agent_name), buffer->get_relations());
            if (!must_match({ Token::RBRACK })) return;
            return reachability_body();
        }
        return;
    }

    void Custom_Parser::reachability_body_recurse() {
        if (try_match({ Token::LBRACK })) {
            buffer->clear_variable_list();
            variables();
            buffer->push_relations();;
            if (!must_match({ Token::RBRACK })) return;
            reachability_body_recurse();
        }
        return;
    }

    //if (try_match({ Token::REST_DEF, Token::EQUALS, Token::LBRACK })) {
    //    buffer->clear_action_relations();
    //    action_agent_reachability();
    //    domain->add_action_relations(REST_INDEX, buffer->get_action_relations());
    //    if (!must_match({ Token::RBRACK })) return;
    //    return action_reachability();
    //}
    //return;
    //}

    //void Custom_Parser::action_agent_reachability() {

    //    if (try_match({ Token::LBRACK })) {
    //        buffer->clear_variable_list();
    //        variables();
    //        buffer->push_action_relations();
    //        //domain->add_action_reachability_set(name, buffer->get_variables());
    //        if (!must_match({ Token::RBRACK })) return;
    //        action_agent_reachability();
    //    }
    //    return;
    //}

    void Custom_Parser::objects() {
        if (try_match({ Token::NAME, Token::EQUALS, Token::LBRACK })) {
            auto name = get_svalue(0);
            variables();
            buffer->set_object_type(name);
            buffer->push_objects();
            if (!must_match({ Token::RBRACK })) return;
            return objects();
        }
        return;
    }

    bool Custom_Parser::proposition_instance() {
        if (try_match({ Token::NAME, Token::LBRACK })) {
            auto name = get_svalue(0);
            ordered_variables();
            buffer->push_proposition_instance(name);
            if (!must_match({ Token::RBRACK })) return false;
            return true;
        }
        return false;
    }

    void Custom_Parser::bracketed_input() {
        if (try_match({ Token::LBRACK, Token::NAME, Token::NAME, Token::RBRACK })) {
            buffer->add_input(get_svalue(1), get_svalue(2));
            return bracketed_input();
        }
        return;
    }

    void Custom_Parser::input() {
        if (try_match({ Token::NAME, Token::NAME })) {
            buffer->add_input(get_svalue(0), get_svalue(1));
            return input();
        }
        return;
    }

    void Custom_Parser::action_body() {
        if (try_match({ Token::OWNER_DEF, Token::EQUALS, Token::NAME, Token::NAME })) {
            domain->set_action_owner(get_svalue(2), get_svalue(3), buffer->get_owner_input_index(get_svalue(2), get_svalue(3)));
            return action_body();
        }

        if (try_match({ Token::EVENT_DEF, Token::NAME, Token::LBRACK })) {
            buffer->set_event_name(get_svalue(1));
            event_body();
            domain->create_event(
                    buffer->get_event_name(),
                    buffer->get_formula(),
                    buffer->get_event_add_list(),
                    buffer->get_event_delete_list());
            if (!must_match({ Token::RBRACK })) return;
            return action_body();
        }

        if (try_match({ Token::DESIGNATED_EVENTS_DEF, Token::EQUALS, Token::LBRACK })) {
            buffer->clear_designated_events();
            designated_events_body();
            domain->set_designated_events(buffer->get_designated_events());
            if (!must_match({ Token::RBRACK })) return;
            return action_body();
        }

        if (try_match({ Token::REACHABILITY_DEF, Token::EQUALS, Token::LBRACK })) {
            action_reachability();

            // TODO -- If _rest not defined, add empty entry, important for agent size later on
            if (!must_match({ Token::RBRACK })) return;
            return action_body();
        }

        if (try_match({ Token::COST_DEF, Token::EQUALS, Token::INTEGER })) {
            domain->set_action_cost(get_ivalue(2));
            return action_body();
        }
    }

    void Custom_Parser::action_reachability() {
        if (try_match({ Token::NAME, Token::EQUALS, Token::LBRACK })) {
            std::string agent_name = get_svalue(0);
            buffer->clear_relations();
            action_agent_reachability();
            domain->add_action_relations(buffer->translate_atom_to_id(agent_name), buffer->get_relations());
            if (!must_match({ Token::RBRACK })) return;
            return action_reachability();
        }

        if (try_match({ Token::REST_DEF, Token::EQUALS, Token::LBRACK })) {
            buffer->clear_relations();
            action_agent_reachability();
            domain->add_action_relations(REST_INDEX, buffer->get_relations());
            if (!must_match({ Token::RBRACK })) return;
            return action_reachability();
        }
        return;
    }

    void Custom_Parser::action_agent_reachability() {

        if (try_match({ Token::LBRACK })) {
            buffer->clear_variable_list();
            variables();
            buffer->push_relations();
            //domain->add_action_reachability_set(name, buffer->get_variables());
            if (!must_match({ Token::RBRACK })) return;
            action_agent_reachability();
        }
        return;
    }

    //void Custom_Parser::reachability_body() {
    //    if (try_match({ Token::NAME, Token::EQUALS, Token::LBRACK })) {
    //        auto& name = get_svalue(0);
    //        buffer->clear_variable_list();
    //        variables();

    //        domain->add_reachability_set(name, buffer->get_variables());
    //        if (!must_match({ Token::RBRACK })) return;
    //        return reachability_body();
    //    }
    //    return;
    //}

    void Custom_Parser::designated_events_body() {
        if (try_match({ Token::NAME })) {
            buffer->add_designated_event(get_svalue(0));
            return designated_events_body();
        }
        return;
    }

    void Custom_Parser::event_body() {
        if (try_match({ Token::PRECONDITIONS_DEF, Token::EQUALS, Token::LBRACK })) {
            formula_single();
            if (!must_match({ Token::RBRACK })) return;
            return event_body();
        }

        if (try_match({ Token::EFFECT_DELETE_DEF, Token::EQUALS, Token::LBRACK })) {
            buffer->clear_variable_list();
            proposition_instances();
            buffer->push_event_delete_list();
            if (!must_match({ Token::RBRACK })) return;
            return event_body();
        }

        if (try_match({ Token::EFFECT_ADD_DEF, Token::EQUALS, Token::LBRACK })) {
            buffer->clear_variable_list();
            proposition_instances();
            buffer->push_event_add_list();
            if (!must_match({ Token::RBRACK })) return;
            return event_body();
        }
    }

    void Custom_Parser::formula() {
        if (proposition_instance()) {
            buffer->push_pop_formula("Prop");
            return formula();
        }

        if (try_match({ Token::AND, Token::LBRACK })) {
            buffer->push_formula("And");
            formula();
            if (!must_match({ Token::RBRACK })) return;
            buffer->pop_formula();
            return formula();
        }

        if (try_match({ Token::OR, Token::LBRACK })) {
            buffer->push_formula("Or");
            formula();
            if (!must_match({ Token::RBRACK })) return;
            buffer->pop_formula();
            return formula();
        }

        if (try_match({ Token::NOT, Token::LBRACK })) {
            buffer->push_formula("Not");
            formula_single();
            if (!must_match({ Token::RBRACK })) return;
            buffer->pop_formula();
            return formula();
        }

        if (try_match({ Token::TOP })) {
            buffer->push_pop_formula("TOP");
            return formula();
        }

        if (try_match({ Token::BELIEVES, Token::LBRACK, Token::NAME })) {
            auto agent = get_svalue(2);
            buffer->push_formula("Believes");
            formula_single();
            if (!must_match({ Token::RBRACK })) return;
            buffer->pop_formula(agent);
            return formula();
        }

        return;
    }

    void Custom_Parser::formula_single() {
        if (proposition_instance()) {
            buffer->push_pop_formula("Prop");
            return;
        }

        if (try_match({ Token::AND, Token::LBRACK })) {
            buffer->push_formula("And");
            formula();
            if (!must_match({ Token::RBRACK })) return;
            buffer->pop_formula();
            return;
        }

        if (try_match({ Token::OR, Token::LBRACK })) {
            buffer->push_formula("Or");
            formula();
            if (!must_match({ Token::RBRACK })) return;
            buffer->pop_formula();
            return;
        }

        if (try_match({ Token::NOT, Token::LBRACK })) {
            buffer->push_formula("Not");
            formula_single();
            if (!must_match({ Token::RBRACK })) return;
            buffer->pop_formula();
            return;
        }

        if (try_match({ Token::TOP })) {
            buffer->push_pop_formula("TOP");
            return;
        }

        if (try_match({ Token::BELIEVES, Token::LBRACK, Token::NAME })) {
            auto agent = get_svalue(2);
            buffer->push_formula("Believes");
            formula_single();
            if (!must_match({ Token::RBRACK })) return;
            buffer->pop_formula(agent);
            return formula();
        }

        return;
    }

    void Custom_Parser::proposition_instances() {
        if (try_match({ Token::NAME, Token::LBRACK })) {
            auto name = get_svalue(0);
            ordered_variables();
            buffer->push_proposition_instance(name);
            if (!must_match({ Token::RBRACK })) return;
            return proposition_instances();
        }
        return;
    }

    void Custom_Parser::propositions() {
        if (try_match({ Token::NAME, Token::LBRACK })) {
            auto name = get_svalue(0);
            buffer->clear_inputs();
            input();
            domain->add_proposition(name, buffer->get_inputs());
            if (!must_match({ Token::RBRACK })) return;
            return propositions();
        }
        return;
    }

    void Custom_Parser::actions() {
        if (try_match({ Token::ACTION_DEF, Token::NAME, Token::LBRACK })) {
            buffer->clear_proposition_instances();
            buffer->clear_seen_atoms();
            buffer->clear_inputs();
            domain->new_action(get_svalue(1));
            input();
            domain->set_action_input(buffer->get_inputs());
            if (!must_match({ Token::RBRACK, Token::LBRACK })) return;
            action_body();
            domain->finish_action(buffer->get_clear_instance_to_proposition());
            buffer->clear_proposition_instances();
            buffer->clear_seen_atoms();
            if (!must_match({ Token::RBRACK })) return;
            return actions();
        }
        return;
    }

    void Custom_Parser::variables() {
        if (try_match({ Token::NAME })) {
            buffer->add_variable(get_svalue(0));
            return variables();
        }
        return;
    }

    void Custom_Parser::ordered_variables() {
        if (try_match({ Token::NAME })) {
            buffer->add_ordered_variable(get_svalue(0));
            return ordered_variables();
        }
        if (try_match({ Token::REST_DEF })) {
            buffer->add_ordered_variable(get_svalue(0));
            return ordered_variables();
        }
        return;
    }
}
