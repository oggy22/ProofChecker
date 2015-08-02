#include <utility>
#include <algorithm>
#include "file_reader.h"
#include "common.h"

enum token_enum
{
#include "tokens.def"
};

inline bool is_object_type(token_enum token)
{
	return token == token_enum::ID || token == token_enum::BIN_OPERATION;
}

class lexer_t
{
	file_reader reader;
	ofstream ftokens;
	int line, column;
	bool is_peeked;
	std::pair<token_enum, wstring> current, peeked;

public:
	lexer_t(char const * const filename, const char * const tokens_filename=nullptr)
		: reader(filename),
		line(1),
		column(1),
		is_peeked(false)
	{
		if (tokens_filename)
			ftokens.open(tokens_filename);
	}

	void change_input(const char* const filename) // for testing only
	{
		reader.change_input(filename);
		is_peeked = false;
	}

private:
	void read_while_not(wchar_t c)
	{
		while (reader.get() != c);
	}

	void read_while(wchar_t c)
	{
		while (reader.peek() == L' ')
		{
			reader.get();
		}
	}

	void skip_spaces_and_comments()
	{
		read_while(L' ');
		if (reader.peek() == L'/')
		{
			reader.get();
			if (reader.get() != L'/')
				throw parsing_exception(get_line(), get_column(), L"unproper comment");

			read_while_not(L'\r');
			if (reader.get() != '\n')
				throw parsing_exception(get_line(), get_column(), L"\\r is not followed by \\n");
			
			skip_spaces_and_comments();
		}
	}

public:
	int get_column() const
	{
		return column;
	}

	int get_line() const
	{
		return line;
	}

	bool isgreek(wchar_t c)
	{
		return c >= L'α' && c <= L'Ω';
	}

	std::pair<token_enum, std::wstring> curr_token()
	{
		return current;
	}

	std::pair<token_enum, std::wstring> peek_token()
	{
		if (!is_peeked)
		{
			peeked = _next_token();
			is_peeked = true;
		}
		return peeked;
	}

	std::pair<token_enum, std::wstring> next_token()
	{
		if (is_peeked)
		{
			is_peeked = false;
			current = peeked;
		}
		else
		{
			line = reader.get_line();
			column = reader.get_column();
			current = _next_token();
		}
		return current;
	}
	
	std::pair<token_enum, std::wstring> _next_token()
	{
		skip_spaces_and_comments();

		wchar_t curr_char = reader.get();


#define RETURN(a, b)	\
		{											\
			if (ftokens.is_open())			\
				ftokens << a << " " << std::wstring(b).length() << endl;		\
			return std::make_pair(a, b);	\
		}

#define CHECK(c, tok)							\
		if (curr_char == c)						\
		{												\
			RETURN(tok, (std::wstring()+c));	\
		}

		//∀∃∈∧∨⊆×αβ∪∩≠
		CHECK(L'∪', token_enum::UNION);
		CHECK(L'∩', token_enum::INTERSECTION);
		CHECK(L'×', token_enum::CARTESIAN_PRODUCT);
		CHECK(L'∀', token_enum::FOR_EACH);
		CHECK(L'∃', token_enum::EXISTS);
		CHECK(L'=', token_enum::EQUAL);
		CHECK(L'=', token_enum::NEQUAL);
		CHECK(L'∧', token_enum::AND);
		CHECK(L'∨', token_enum::OR);
		CHECK(L'∈', token_enum::ELEMENT);
		CHECK(L'(', token_enum::L_BRACE);
		CHECK(L')', token_enum::R_BRACE);
		CHECK(L'{', token_enum::L_CURLY_BRACE);
		CHECK(L'}', token_enum::R_CURLY_BRACE);
		CHECK(L'\'', token_enum::COMMA);
		CHECK(L',', token_enum::COMMA);
		CHECK(L':', token_enum::COLUMN);
		CHECK(L'\n', token_enum::NEW_LINE);
		CHECK(wchar_t(EOF), token_enum::END_OF_FILE);
#undef CHECK

#define CHECK(s, tok)												\
		for (wchar_t *ptr = s; *ptr!=0; ptr++)						\
		{															\
			if (curr_char == *ptr)									\
			{														\
				RETURN(tok, (std::wstring() + *ptr));				\
			}														\
		}
		CHECK(L"*+", token_enum::BIN_OPERATION);
#undef CHECK

		if (curr_char == L'i' && reader.peek() == L's')
		{
			reader.get();
			RETURN(token_enum::IS, L"is");
		}

		if (iswalpha(curr_char) && iswalpha(reader.peek()))
		{
			std::wstring name;
			name += curr_char;
			name += reader.get();

			while (iswalpha(reader.peek()))
			{
				name += reader.get();
			}

			if (name == L"set")
				RETURN(token_enum::SET, name);

			RETURN(token_enum::NAME, name);
		}

		// Identifier
		if (iswalpha(curr_char) || isgreek(curr_char))
		{
			std::wstring id; id += curr_char;
			if (iswdigit(reader.peek()))
				id += reader.get();
			RETURN(token_enum::ID, id);
		}

		// Arrow ->
		if (curr_char == L'-')
		{
			if (reader.get() != L'>')
				throw parsing_exception(get_line(), get_column(), L"Incomplete arrow");
			RETURN(token_enum::ARROW, L"->");
		}

		std::wstring msg(L"Unrecognized token: '%'");
		std::replace(msg.begin(), msg.end(), L'%', curr_char);
		error(reader.get_line(), reader.get_column(), msg);
	}
};

//ID_LIST -> ID 					// x
//ID_LIST -> ID_LIST, COMMA, ID			// x,y,+
//TUPLE   -> L_BRACE, ID_LIST, R_BRACE		// (x,y,z)
//BELONGING -> ID_LIST, ELEMENT, SET		// x e A
//BELONGING -> ID_LIST, COLUMN, SET, ARROW, SET	// f : A -> B
//BELONGING -> ID_LIST, IS, NAME			// G is Group
//SET	-> ID
//SET	-> L_BRACKET, SET, R_BRACKET
//SET	-> SET, CARTESIAN, SET
//LOG	-> ID EQ ID
//LOG	-> ID NEQ ID
//LOG	-> ID ELEMENT ID
//LOG	-> ID NELEMENT ID
//LOG	-> LOG AND LOG
//LOG	-> LOG OR LOG
//LOG	-> LOG THEN LOG
//LOG	-> NEG LOG
//LOG	-> Q_VAR LOG
//
 