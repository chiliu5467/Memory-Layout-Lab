#include "MemoryLayout.h"
#include "LayoutTypes.h"

#include <iostream>
#include <cstddef>

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

    std::cout << "\nMember offsets:\n";

    std::cout << "a offset: "
        << offsetof(LayoutA, a) << '\n';

    std::cout << "b offset: "
        << offsetof(LayoutA, b) << '\n';

    std::cout << "c offset: "
        << offsetof(LayoutA, c) << '\n';

    std::cout << "\nLayoutB offsets:\n";

    std::cout << "b offset: "
        << offsetof(LayoutB, b) << '\n';

    std::cout << "a offset: "
        << offsetof(LayoutB, a) << '\n';

    std::cout << "c offset: "
        << offsetof(LayoutB, c) << '\n';

    return 0;
}