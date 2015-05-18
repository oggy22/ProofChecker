//#include <iostream>
//#include <functional>
//#include <string>
//
//using namespace std;
//
//void static_report_error(const string s)
//{
//	cout << s;
//}
//
//class parser
//{
//	class var_manager
//	{
//		std::function<void(const string)> re;
//	public:
//		var_manager(std::function<void(const string)> re)
//		{
//			string s;
//			report(s);
//			//re = report_error;
//		}
//
//		void report(const string s)
//		{
//			re(s);
//		}
//
//	};
//
//	var_manager vm;
//
//public:
//	void report_error(const string s)
//	{
//		cout << s;
//	}
//
//	parser() : vm(report_error)
//	{
//
//	}
//};
//
//void main()
//{
//	std::function<void(int)> f_display;
//	std::function<void(const string)> re;
//	parser p;
//	//var_manager vm(report_error);
//	//vm.report("Hello\n");
//}