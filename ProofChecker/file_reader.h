#include <string>
#include <fstream>
#include <iostream>
#include <codecvt>

using namespace std;

class file_reader
{
	std::wifstream file;
	int position;
	int line, column;
	wchar_t peek_char;
	bool is_peeked;

#ifdef _DEBUG
	std::wstring file_content;
#endif

	wchar_t _get()
	{
		wchar_t ret = file.get();
		position++;
		column++;
		if (ret == '\n')
		{
			column = 1;
			line++;
		}

#ifdef _DEBUG
		file_content += ret;
#endif
		return ret;
	}

public:
	file_reader(char const * const filename) : file(filename), position(0), line(1), column(1), is_peeked(false)
	{
		file.imbue(std::locale(std::locale::empty(), new std::codecvt_utf8<wchar_t>));
	}

	void change_input(const char* const filename) // for testing only
	{
		file.close();
		file.open(filename);
		file.imbue(std::locale(std::locale::empty(), new std::codecvt_utf8<wchar_t>));
		position = 0;
		line = column = 1;
		is_peeked = false;
	}

	inline int get_column()
	{
		return column;
	}

	inline int get_line()
	{
		return line;
	}

	bool eof()
	{
		return file.eof();
	}

	wchar_t peek()
	{
		if (!is_peeked)
		{
			peek_char = _get();
			is_peeked = true;
		}
		return peek_char;
	}

	wchar_t get()
	{
		if (is_peeked)
		{
			is_peeked = false;
			return peek_char;
		}
		return _get();
	}

	const wchar_t * regular_chars = L" =<>(){}.'";
	bool regular_character(wchar_t c)
	{
		if (c >= L'0' && c <= L'9') return true;
		if (c >= L'A' && c <= L'Z') return true;
		if (c >= L'a' && c <= L'z') return true;
		if (c >= L'α' && c <= L'ώ') return true;
		
		return false;
	}
};