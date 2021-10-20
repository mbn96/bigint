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

Bigint factor(Bigint num)
{
    static const Bigint one(1, 0);

    if (num == one)
    {
        return one;
    }
    Bigint f = factor(num - one);
    return num * f;
}

int main(int argc, char const *argv[])
{
    // cout << "hey" << endl;

    Bigint a(455, false);
    Bigint b(121, 0);
    Bigint b1(48, 0);
    Bigint b2(3, 0);
    Bigint b3(793, 0);

    // Bigint c = a / b;
    // Bigint d = a / b1;
    // Bigint e = a / b2;
    // Bigint f = a / b3;
    // Bigint g = b1 / b2;
    // Bigint h = b3 / b1;

    Bigint c = a + b;
    Bigint d = a * b;
    d = d * c * a;
    Bigint e = a - b;
    Bigint f = b - a;
    Bigint g = a / b;
    Bigint h = a % b;

    cout << a << endl;
    cout << b << endl;
    cout << b1 << endl;
    cout << b2 << endl;
    cout << c << endl;
    cout << d << endl;
    cout << e << endl;
    cout << f << endl;
    cout << g << endl;
    cout << h << endl;

    // cout << (a > b) << endl;

    Bigint fac_num(120, 0);
    Bigint fact(1, 0);
    fact = factor(fac_num);

    cout << fact << endl;

    // auto list = get_ints();

    return 0;
}
