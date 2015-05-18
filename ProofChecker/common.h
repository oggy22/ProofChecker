#pragma once
#include <sstream>
class parsing_exception
{
	int line, column;
	const std::wstring message;
public:

	parsing_exception(int line, int column, const wchar_t* const message) : line(line), column(column), message(message)
	{ }

	parsing_exception(int line, int column, const std::wstring message) : line(line), column(column), message(message)
	{ }
	
	std::wstring get_message() const
	{
		std::wstringstream ss;
		ss << L"(" << line << L"," << column << L")" << message;
		return ss.str();
	}

	int get_line() const	{ return line; }
	int get_column() const	{ return column; }
};

#define error(line, column, message) throw parsing_exception(line, column, message)