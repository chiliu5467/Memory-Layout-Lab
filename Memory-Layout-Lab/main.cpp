#include "MemoryLayout.h"
#include "LayoutTypes.h"

#include <iostream>
#include <cstddef>

void TestPrintTypeInfo()
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

    std::cout << "\n==========================\n";
}

void TestPrintBytes()
{
    std::cout << "\nPrint Bytes:\n";
    int a = 1;
    int b = 256;
    int c = 0x12345678;

    PrintBytes(a);
    PrintBytes(b);
    PrintBytes(c);

    std::cout << "\nPrint Layout A Bytes:\n";
    LayoutA layout{
    'A',
    0x12345678,
    'B'
    };

    PrintBytes(layout);

    auto bytes = std::as_bytes(
        std::span<const LayoutA>{&layout, 1});

    if (bytes.size() == sizeof(layout))
    {
        std::cout << "The size of 'bytes' is equal to the size of 'layout'\n";
    }
}

int main()
{
    TestPrintTypeInfo();
    TestPrintBytes();

    return 0;
}