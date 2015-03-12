#include "stdafx.h"

#include "..\ProofChecker\lexer.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace ProofCheckerTest
{
	TEST_CLASS(files_test)
	{
	public:
		void test(char* file_name)
		{

		}

		TEST_METHOD(file_group_test)
		{
			test("group.thr");

		}
	};
}