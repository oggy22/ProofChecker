#include "stdafx.h"

#include "..\ProofChecker\lexer.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace ProofCheckerTest
{
	TEST_CLASS(UnitTest1)
	{
	public:

		void test(wchar_t* file_content, const std::vector<std::pair<token_enum, std::wstring>> &tokens)
		{
			// Put the content into the file
			wofstream fout("test.thr");
			fout.imbue(std::locale(std::locale::empty(), new std::codecvt_utf8<wchar_t>));
			fout << file_content;
			fout.close();

			// Exercise the lexer and compare it agains expected tokens
			lexer_t lexer("test.thr");
			std::pair<token_enum, std::wstring> token;
			for (const auto& token : tokens)
			{
				auto actual_token = lexer.next_token();
				Assert::AreNotEqual(int(token_enum::END_OF_FILE), int(token.first));

				// Check token_enum and wstring
				Assert::AreEqual(int(actual_token.first), int(token.first));
				Assert::AreEqual(token.second, actual_token.second);
			}

			// Make sure the END_OF_FILE is retrieved here
			Assert::AreEqual(int(lexer.next_token().first), int(token_enum::END_OF_FILE));
		}

		TEST_METHOD(lexer_test)
		{
			int x = 5;
			Assert::AreEqual(5, x);
			using token_vector = std::vector < std::pair<token_enum, std::wstring> >;
			test(L"(α,*,e)", token_vector
			{
				{ token_enum::L_BRACE, L"(" },
				{ token_enum::ID, L"α" },
				{ token_enum::COMMA, L"," },
				{ token_enum::BIN_OPERATION, L"*" },
				{ token_enum::COMMA, L"," },
				{ token_enum::ID, L"e" },
				{ token_enum::R_BRACE, L")" }
			});
		}

		TEST_METHOD(filereader_test)
		{
			wstring text(L"G=(G,*,e)");

			// Put the content into the file
			wofstream fout("test.txt");
			fout << text;
			fout.close();

			file_reader reader("test.txt");
			for (const auto& w : text)
			{
				reader.get();
			}
		}
	};
}