#include <iostream>

extern "C"
{
#include "mylib.h"
}
using namespace std;

int main()
{
	int i = myAdd(1, 2);
	cout << "from main: i = " << i << endl;
	return 0;
}