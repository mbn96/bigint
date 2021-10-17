#include <iostream>
#include "bigint.h"
// #include "ArrayList.hpp"

using namespace std;
using MBN::Bigint;
// using MBN::ArrayList;
// using m_bytes = ArrayList<int>;

// m_bytes get_ints()
// {
//     m_bytes ints;
//     // ints.append(1);
//     // ints.append(2);
//     // ints.append(3);
//     return ints;
// }

int main(int argc, char const *argv[])
{
    Bigint a(450, false);
    Bigint b(140, 0);
    Bigint c = a + b;
    Bigint d = a * b;
    Bigint e = a - b;
    Bigint f = b - a;

    cout << a << endl;
    cout << b << endl;
    cout << c << endl;
    cout << d << endl;
    cout << e << endl;
    cout << f << endl;

    cout << (a > b) << endl;

    // auto list = get_ints();

    return 0;
}
