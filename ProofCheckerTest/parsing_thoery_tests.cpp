#include "stdafx.h"

#include "..\ProofChecker\recursive_descent_parser.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace ProofCheckerTest
{
	TEST_CLASS(parsing_tests)
	{
		char const * const file_name = "test.thr";

		void create_file(const std::initializer_list<wchar_t*> &lines)
		{
			// Put the content into the file
			wofstream fout(file_name);
			fout.imbue(std::locale(std::locale::empty(), new std::codecvt_utf8<wchar_t>));
			for (auto line : lines)
			{
				fout << line << endl;
			}
			fout.close();
		}

		void positive_test(const std::initializer_list<wchar_t*> &lines)
		{
			create_file(lines);

			recursive_descent_parser_t parser(file_name);
			try
			{
				parser.parse();
			}
			catch (const parsing_exception& exc)
			{
				Assert::Fail(exc.get_message().c_str());
			}
		}

		void negative_test(const std::initializer_list<wchar_t*> &lines, int line = 0, int column = 0)
		{
			create_file(lines);

			recursive_descent_parser_t parser(file_name);
			try
			{
				parser.parse();
				Assert::Fail(L"Parser was expected to fail");
			}
			catch (const parsing_exception& exc)
			{
				if (line != 0)
				{
					Assert::AreEqual(line, exc.get_line(), L"Lines do not match");
					if (column != 0)
					{
						Assert::AreEqual(column, exc.get_column(), L"Columns do not match");
					}
				}
			}
			catch (...)
			{
				Assert::Fail(L"Unknown exception caught");
			}
		}

	public:
		TEST_METHOD(positive_parsing_test)
		{
			positive_test({
				L"(G,f) is something",
				L"G is set",
				L"f:G->G"
			});
			positive_test({
				L"(G,*,e) is Group",
				L"G is set",
				L"*:G×G->G",
				L"e∈G",
			});
			positive_test({
				L"(F,*,+,e,n) is Field",
				L"F is set",
				L"*:F×F->F",
				L"+:F×F->F",
				L"e∈F",
				L"n∈F",
			});

			// With Axioms
			positive_test({
				L"(G,*,e) is Group",
				L"G is set",
				L"*:G×G->G",
				L"e∈G"//,
				//L"(∀x∈G)(x=x)",
			});
		}

		TEST_METHOD(negative_parsing_tests)
		{
			negative_test({	// expecting name
				L"(G,f)"
			}, 1, 6);
			negative_test({	// unknown + operation
				L"(G,*) is Group",
				L"G is set",
				L"+:G×G->G",
			}, 3, 1);
			negative_test({	// f already defined
				L"(A,f) is something",
				L"A is set",
				L"f:A->A",
				L"f:A->A"
			}/*, 1*/);
			negative_test({ // type of 'e' is not defined
				L"(G,*,e) is Group",
				L"G is set",
				L"*:G×G->G",
			}, 3);
			negative_test({ // D is not defined
				L"(G,e) is something",
				L"G is set",
				L"e:D->G",
			}, 3, 3);
		}
	};
}