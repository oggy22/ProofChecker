#include "stdafx.h"

char const * const file_name = "test.thr";

void create_file(const std::initializer_list<wchar_t*> &lines, char const * const filename)
{
	// Put the content into the file
	std::wofstream fout(filename);
	fout.imbue(std::locale(std::locale::empty(), new std::codecvt_utf8<wchar_t>));
	for (auto line : lines)
	{
		fout << line << std::endl;
	}
	fout.close();
}
