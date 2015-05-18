#include "lexer.h"

enum class terminals
{
#include "tokens.def"
,
LOGICAL_EXPRESSION,		//e.g. A and B
QUANTIFIED_VARIABLES,	// (A x,y e X)
};

class parser_t
{
	lexer_t lexer;

public:
	parser_t(char *filename) : lexer(filename)
	{
	}

	void parse()
	{
		token_enum tok;
		while ((tok = lexer.next_token().first) != EOF)
		{
			wcout << int(tok) << endl;
		}
	}
};