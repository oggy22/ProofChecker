#include "stdafx.h"

#include "..\ProofChecker\recursive_descent_parser.h"
//#ifdef _DEBUG
//#pragma comment(lib, "../bin/Debug/ProofChecker.lib")
//#else
//#pragma comment(lib, "../bin/Release/ProofChecker.lib")
//#endif

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace ProofCheckerTest
{
	TEST_CLASS(parsing_expression_tests)
	{
		char const * const test_expression = "test_expression.thr";

		void test_positive(recursive_descent_parser_t& parser, wchar_t* line)
		{
			create_file({
				line,
			}, test_expression);
			parser.change_input(test_expression);
			parser.parse_expression();
		}

		void test_negative(recursive_descent_parser_t& parser, wchar_t* line)
		{
			create_file({
				line,
			}, test_expression);
			parser.change_input(test_expression);
			try
			{
				parser.parse_expression();
				Assert::Fail(L"The parse_expression() was expected to fail");
			}
			catch (const parsing_exception&)
			{

			}
			catch (...)
			{
				Assert::Fail(L"Unknown exception thrown");
			}
		}
	public:

		TEST_METHOD(positive_expression_parsing_test)
		{
			create_file({
				L"(A,*,+,s,e) is Group",
				L"A is set",
				L"+:A×A->A",
				L"*:A×A->A",
				L"s:A->A",
				L"e∈A"
			});
			recursive_descent_parser_t parser(file_name);
			parser.parse();

			test_positive(parser, L"s(e)");
			test_positive(parser, L"s(s(e))");
		}

		TEST_METHOD(negative_expression_parsing_tests)
		{
			create_file({
				L"(A,*,+,s,e) is Group",
				L"A is set",
				L"+:A×A->A",
				L"*:A×A->A",
				L"s:A->A",
				L"e∈A"
			});
			recursive_descent_parser_t parser(file_name);
			parser.parse();

			// Syntax errors
			test_negative(parser, L"s(");
			test_negative(parser, L"s()");

			// Missmatch types errors
			//test_negative(parser, L"e(e)");
		}
	};
}