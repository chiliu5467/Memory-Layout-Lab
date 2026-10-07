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

    std::cout << "1:          ";
    PrintBytes(a);

    std::cout << "256:        ";
    PrintBytes(b);

    std::cout << "0x12345678: ";
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

    std::cout << "bytes.size(): " << bytes.size() << '\n';
    std::cout << "sizeof(layout): " << sizeof(layout) << '\n';
}

void TestEncodeBigEndian()
{
    std::cout << "\nEncode Big Endian:\n";
    int a = 1;
    int b = 256;
    int c = 0x12345678;
    std::cout << "1:          ";
	std::array<std::byte, sizeof(int)> encodedA = EncodeBigEndian(a);
    PrintBytes(encodedA);

    std::cout << "256:        ";
    std::array<std::byte, sizeof(int)> encodedB = EncodeBigEndian(b);
    PrintBytes(encodedB);

    std::cout << "0x12345678: ";
    std::array<std::byte, sizeof(int)> encodedC = EncodeBigEndian(c);
    PrintBytes(encodedC);

    std::cout << "\nDecode Big Endian:\n";
	std::cout << "a: " << DecodeBigEndian(encodedA) << "\n";
    std::cout << "b: " << DecodeBigEndian(encodedB) << "\n";
    std::cout << "c: " << DecodeBigEndian(encodedC) << "\n";
}


int main()
{
    TestPrintTypeInfo();
    TestPrintBytes();
    PrintNativeEndian();
    TestEncodeBigEndian();

    return 0;
}