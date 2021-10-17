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
    Bigint a(45511515, false);
    Bigint b(1958442, 0);
    Bigint c = a + b;

    cout << a << endl;
    cout << b << endl;
    cout << c << endl;

    // auto list = get_ints();

    return 0;
}
