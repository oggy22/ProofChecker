#include <vector>
#include <algorithm>
#include <unordered_map>
#include <functional>
#include <set>

#ifdef TEST_PROJECT
#define  FUNC_DECL //__declspec(dllimport)
#else
#define  FUNC_DECL __declspec(dllexport)
#endif

#include "lexer.h"
#include "types.h"
#include "log_expressions.h"
#include "theory.h"

class recursive_descent_parser_t
{
	lexer_t lexer;

	void expect(token_enum token)
	{
		if (lexer.next_token().first != token)
			report_error(L"Unexpected token '" + lexer.curr_token().second + L"'");
	}

	void expect(token_enum token1, token_enum token2)
	{
		auto token = lexer.next_token().first;
		if (token != token1 && token != token2)
			report_error(L"Unexpected token '" + lexer.curr_token().second + L"'");
	}

	void report_error(const std::wstring message) const
	{
		throw parsing_exception(lexer.get_line(), lexer.get_column(), message);
	}

	static void report_error2(const std::wstring message)
	{

	}

	class variable_manager
	{
		std::unordered_map<std::wstring, base_type*> map;
		const recursive_descent_parser_t& parser;

	public:
		variable_manager(const recursive_descent_parser_t& parser) : parser(parser)
		{ }

		auto begin() const -> decltype(map.begin())
		{
			return map.begin();
		}

		auto end() const -> decltype(map.end())
		{
			return map.end();
		}

		template <class L>
		void for_each_var(L lambda) const
		{
			for (auto ref : map)
			{
				lambda(ref);
			}
		}

		void add(const std::wstring &var, base_type* p_type)
		{
			map[var] = p_type;
		}

		base_type& get(const std::wstring &var)
		{
			auto it = map.find(var);
			if (it == map.end())
				parser.report_error(var + L" not found");

			return *(it->second);
		}

		void remove(const std::wstring &var)
		{
			map.erase(var);
		}

		bool contains(const std::wstring &var)
		{
			return map.find(var) != map.end();
		}
	};

	friend class variable_manager;

	variable_manager var_manager;

public:
	recursive_descent_parser_t(char const * const filename, char *tokens_filename = nullptr) :
		lexer(filename, tokens_filename),
		var_manager(*this)
	{
		//p_singletone = this;
	}

	//static recursive_descent_parser_t* p_singletone;

	void change_input(const char* const filename) // for testing only
	{
		lexer.~lexer();
		lexer.lexer_t::lexer_t(filename);
		//lexer.lexer(filename);
		//lexer.change_input(filename);
	}

	void parse()
	{
		parse_theory();
	}

	void parse_theory()
	{
		if (lexer.peek_token().first == token_enum::END_OF_FILE)
			return;

		theory *p_thr = tuple();
		theory &thr = *p_thr;

		std::set<wstring> set_list;	// set of variables which are declared as sets

		// Parse varialbe definitions
		while (is_object_type(lexer.peek_token().first))
		{
			var_definition(true, set_list);
			expect(token_enum::NEW_LINE, token_enum::END_OF_FILE);
		}
		check_all_vars_defined(thr, set_list);

		if (lexer.peek_token().first == token_enum::END_OF_FILE)
			return;

		// Parse Axioms
		//log_expression* p_le = parse_log_expression();
	}

	void check_all_vars_defined(const theory& thr, const std::set<wstring>& set_list)
	{
		var_manager.for_each_var([&](std::pair<const std::wstring, base_type*>& p)
		{
			if (p.second->get_type() == nullptr && set_list.find(p.first) == set_list.end())
				report_error(L"Variable " + p.first + L" is not defined in definition section");
		});
	}

	theory *tuple()
	{
		std::vector<wstring> vars;

		expect(token_enum::L_BRACE);
		expect(token_enum::ID);
		vars.push_back(lexer.curr_token().second);

		while (lexer.peek_token().first == token_enum::COMMA)
		{
			lexer.next_token();
			expect(token_enum::ID, token_enum::BIN_OPERATION);
			vars.push_back(lexer.curr_token().second);
		}

		expect(token_enum::R_BRACE);

		expect(token_enum::IS);

		expect(token_enum::NAME);
		const wstring &wname = lexer.curr_token().second;
		theory *p_theory = new theory(vars.size(), string(wname.begin(), wname.end()));
		int i = 0;
		std::for_each(vars.begin(), vars.end(), [&vars, p_theory, this](std::wstring &ws)
		{
			int i = (&ws) - (&vars[0]);
			base_type& bt = p_theory->operator[](i);
			this->var_manager.add(ws, &bt);
		});

		expect(token_enum::NEW_LINE);

		return p_theory;
	}

	void var_definition(bool only_type_definition = false, std::set<std::wstring> &set_set = std::set<std::wstring>())
	{
		expect(token_enum::ID, token_enum::BIN_OPERATION);
		std::wstring var = lexer.curr_token().second;

		// Check if the variable is in the tuple
		if (only_type_definition && !var_manager.contains(var))
			report_error(L"Variable " + var + L" undefined");

		// Check if already defined
		base_type &bt = var_manager.get(var);
		if (only_type_definition && (bt.get_type() != nullptr || set_set.find(var) != set_set.end()))
			report_error(L"Varialbe " + var + L" already defined");

		switch (lexer.next_token().first)
		{
		case token_enum::IS:
			expect(token_enum::SET);
			bt.set_type(nullptr);
			set_set.insert(var);
			return;

		case token_enum::ELEMENT:
			bt.set_type(type_exp());
			return;

		case token_enum::SUBSET:
			bt.set_type(new power_set_type(type_exp()));
			return;

		case token_enum::COLUMN:
			type *from, *to;
			from = type_exp();
			expect(token_enum::ARROW);
			to = type_exp();
			bt.set_type(new function_type(from, to));
			return;
		}
		
		report_error(L"Definition of type expected");
	}

#pragma warning( push )
#pragma warning( disable : 4715 )
	type* type_exp()
	{
		switch (lexer.next_token().first)
		{
			case token_enum::PARTIAL:
			{
				expect(token_enum::L_BRACE);
				type* p_inner_type = type_exp();
				expect(token_enum::R_BRACE);
				return new power_set_type(p_inner_type);
			}
			case token_enum::L_BRACE:
			{
				type* p_type = type_exp();
				expect(token_enum::R_BRACE);
				return p_type;
			}
			case token_enum::ID:
			{
				type *pt1 = &var_manager.get(lexer.curr_token().second);
				if (lexer.peek_token().first == token_enum::CARTESIAN_PRODUCT)
				{
					lexer.next_token();
					expect(token_enum::ID);
					type *pt2 = &var_manager.get(lexer.curr_token().second);
					return new cartesian_product_type(pt1, pt2);
				}
				return pt1;
			}
		}

		report_error(L"Definition of type expected");
	}

	expression* parse_expression()
	{
		// Check for brackets
		if (lexer.peek_token().first == token_enum::L_BRACE)
		{
			lexer.next_token();
			return parse_expression();
			expect(token_enum::R_BRACE);
		}

		// Parse an ID
		expect(token_enum::ID);
		base_type *p_object = &var_manager.get(lexer.curr_token().second);

		// Function
		if (p_object->get_type()->is_function() && lexer.peek_token().first == token_enum::L_BRACE)
		{
			lexer.next_token();
			expression *p_expression = parse_expression();
			expect(token_enum::R_BRACE);
			function_application *p_fa = new function_application(p_object, p_expression);
			if (p_fa->get_type() != p_fa->p_argument->get_type())
			{
				report_error(L"Function argument does not match the formal parameter");
			}
			return p_fa;
		}

		// Binary operation
		if (lexer.peek_token().first == token_enum::BIN_OPERATION)
		{
			base_type *p_bin_op = &var_manager.get(lexer.next_token().second);
			base_type *p_object2 = &var_manager.get(lexer.curr_token().second);

			expression *p_arg1 = new identifier_expression(p_object);
			expression *p_arg2 = new identifier_expression(p_object2);
			
			//if (dynamic_cast< p_bin_op->get_type()

			//return new binary_operation_application();
		}

		return new identifier_expression(p_object);
	}
#pragma warning( pop )

	log_expression* parse_log_expression()
	{
		expect(token_enum::L_BRACE);
		token_enum quantifier_token = lexer.next_token().first;
		log_expression *p_log = nullptr;
		switch (quantifier_token)
		{
		case token_enum::FOR_EACH:
			p_log = new forall_expression(nullptr);
			break;
		case token_enum::EXISTS:
			p_log = new exists_expression(nullptr);
			break;
		default:
			report_error(L"Expected either ∀ or ∃");
		}

		var_definition();

		expect(token_enum::R_BRACE);

		return p_log;
	}
};