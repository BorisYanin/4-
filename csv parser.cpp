#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>
#include <sstream>
#include <climits>
#include <csv.h>
using namespace std;



int main()
{
    ifstream file("test.csv");
    CSVParser<long int, string> parser(file, 0);
    for (auto rs : parser)
    {
        cout << rs << endl;
    }
}
