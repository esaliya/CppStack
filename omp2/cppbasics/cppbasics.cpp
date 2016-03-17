// cppbasics.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
using namespace std;
void test(char*** ref_argv)
{
	cout << "200 --" << &ref_argv << '\n';
	cout << "100 --" << ref_argv << '\n';
	cout << "105 --" << *ref_argv << '\n';
	cout << "*(105 + '1') --" << *((*ref_argv) + 1) << '\n';
	cout << " --" << *((*ref_argv) + 2) << '\n';
	cout << " --" << *((*ref_argv) + 3) << '\n';
	cout << " --" << *((*ref_argv) + 4) << '\n';
	cout << " --" << *((*ref_argv) + 5) << '\n';

}
int main(int argc, char* argv[])
{
	/* See pointer_notes in Resources */
	/* Input arguments are: hello hi how are you */
	cout << "100 --" << &argv << '\n';
	cout << "105 --" << argv << '\n';
	cout << "105 + '1' --" << argv + 1 << '\n';
	cout << "*(105) --" << *argv << '\n';
	cout << "*(105 + '1') --" << *(argv + 1) << '\n';
	cout << "(*(105 + '1'))[0] --" << (*(argv + 1))[0] << '\n';
	cout << "(*(105 + '1')+'1') --" << ((*(argv + 1)) + 1) << '\n'; // theoretically this is an address, but seems given that it's a string, c++ will print it
	cout << "*(*(105 + '1')+'1') --" << *((*(argv + 1)) + 1) << '\n'; // querying the above address for its value returns correctly a single character
	cout << '\n';
	test(&argv);
	system("PAUSE");
	return 0;
}
