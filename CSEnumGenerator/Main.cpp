#include <iostream>
#include <fstream>
#include <string>
#include <locale>

#define ENDL "\r\n"
using namespace std;

void flush_file_content(ofstream& fout, char* file_name)
{
	ifstream fin;
	fin.open(file_name, ios::binary);
	if (!fin.is_open())
		return;
	for (std::string line; getline(fin, line); )
	{
		if (line[line.length() - 1] == '\r')
			line.resize(line.length() - 1);
		fout << "\t\t" << line << endl;
		cout << line << endl;
	}
}

int main(int argc, char** argv)
{
	if (argc != 3)
	{
		cout << "Usage:" << endl;
		cout << "CSEnumGenerator FULL_ENM_NAME FILENAME" << endl;
		cout << "*FULL_ENUM_NAME - Qulified name for C Sharp enum including namespaces e.g. ProofCheckerEditor.Tokens" << endl;
		cout << "*FILENAME - The file containing the enum values" << endl;
		return 1;
	}

	char *enumfullname = argv[1];
	char *filename = argv[2];
	char* enumname = strchr(enumfullname, '.');
	if (enumname == nullptr)
	{
		enumname = enumfullname;
	}
	else
		enumname++;


	string name_space(enumfullname, enumname - enumfullname - 1);

	string enumfile = enumname;
	enumfile += ".cs";

	ofstream of;
	of.open(enumfile, ofstream::out);
	of << "namespace " << name_space << endl;
	of << "{" << endl;
	of << "\tenum " << enumname << endl;
	of << "\t{" << endl;
	flush_file_content(of, filename);
	of << endl << endl;
	of << "\t}" << endl;
	of << "}" << endl;
	return 0;
}