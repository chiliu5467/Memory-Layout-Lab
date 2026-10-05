#include "MemoryLayout.h"
#include "LayoutTypes.h"

#include <iostream>

int main()
{
    PrintTypeInfo<char>("char");
    PrintTypeInfo<short>("short");
    PrintTypeInfo<int>("int");
    PrintTypeInfo<long long>("long long");
    PrintTypeInfo<float>("float");
    PrintTypeInfo<double>("double");

    PrintTypeInfo<LayoutA>("LayoutA");
    PrintTypeInfo<LayoutB>("LayoutB");

    LayoutA value{};

    std::cout << "Object: "
        << static_cast<void*>(&value) << '\n';

    std::cout << "a: "
        << static_cast<void*>(&value.a) << '\n';

    std::cout << "b: "
        << &value.b << '\n';

    std::cout << "c: "
        << static_cast<void*>(&value.c) << '\n';

    return 0;
}