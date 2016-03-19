#include <iostream>
#include "recursive_descent_parser.h"

using namespace std;

void read_file(char* file_name)
{
	file_reader file(file_name);
	
	while (!file.eof())
	{
		wchar_t c = file.get();
		cout << (int)(c) << " " << char(c) << endl;
	}
}

void asci_read_file(char *file_name)
{
	std::ifstream fin(file_name/*, ios::binary*/);

	while (fin)
	{
		char c;
		fin >> c;
		std::cout << c;
	}
}

void lex_file(char* file_name)
{
	lexer_t lexer(file_name, nullptr);

	do
	{
		lexer.peek_token();
		auto t = lexer.next_token();
		cout << t.first << " ";
		wcout << t.second << endl;
	} while (lexer.curr_token().first != token_enum::END_OF_FILE);
}

void lexing_only(char *)
{
	lexer_t lexer();
}

int main(int argc, char** argv)
{
	cout << "Proof Checker 1.0" << endl;
	cout << "Parsing " << argv[1] << endl;

	if (argc < 2)
	{
		cout << "Proof Checker: Needs source file" << endl;
		return 1;
	}

	char *tokens_file_name = nullptr;
	if (argc >= 3 && strncmp(argv[2], "/lex:", 5) == 0)
	{
		tokens_file_name = argv[2] + 5;
	}

	recursive_descent_parser_t parser(argv[1], tokens_file_name);
	try
	{
		parser.parse();
		wcout << "Ok!" << endl;
	}
	catch (const char *s)
	{
		cout << "Error message: " << s << endl;
	}
	catch (const parsing_exception& exp)
	{
		wstring message = exp.get_message();
		auto pos = message.find(L'\n');
		if (pos != std::wstring::npos)
			message.replace(pos, 1, L"\\n");
		wcout << message << endl;
	}
	//lex_file(argv[1]);
	//asci_read_file(argv[1]);

	return 0;
}