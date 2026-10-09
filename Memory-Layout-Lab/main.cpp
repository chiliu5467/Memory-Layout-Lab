#include "MemoryLayout.h"
#include "LayoutTypes.h"
#include "EndianUtils.h"
#include "BinaryRecord.h"

#include <iostream>
#include <cstddef>
#include <cassert>
#include <stdexcept>
#include <string>

void Check(bool condition, const std::string& message)
{
    if (!condition)
    {
        throw std::runtime_error(message);
    }
}

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

void TestEncodeRecord()
{
    std::cout << "\nEecodeRecord Test Start!\n";
    Record record{
        2,
        0x12345678,
        {
            std::byte{0xAA},
            std::byte{0xBB},
            std::byte{0xCC}
        }
    };

    auto result = EncodeRecord(record);

    Check(result.has_value(), "Encoding failed");

    std::cout << "Encoded size: "
        << result->size() << '\n';

    std::cout << "Encoded bytes:\n";

    for (auto byte : *result)
    {
        std::cout
            << std::hex
            << std::setw(2)
            << std::setfill('0')
            << std::to_integer<int>(byte)
            << ' ';
    }
	
    const std::vector<std::byte> expected{
    std::byte{0x4D}, std::byte{0x4C},
    std::byte{0x01}, std::byte{0x02},
    std::byte{0x12}, std::byte{0x34},
    std::byte{0x56}, std::byte{0x78},
    std::byte{0x00}, std::byte{0x03},
    std::byte{0xAA}, std::byte{0xBB},
    std::byte{0xCC}
    };

    Check(*result == expected,
        "Encoded bytes do not match protocol specification");

    std::cout << std::dec << '\n';
}

void TestDecodeRecord()
{
    std::cout << "\nDecodeRecord Test Start!\n";
    Record original{
        2,
        0x12345678,
        {
            std::byte{0xAA},
            std::byte{0xBB},
            std::byte{0xCC}
        }
    };

    // Step 2: Encode
    auto encoded = EncodeRecord(original);

    Check(encoded.has_value(), "Encoding failed");

    // Step 3: Decode
    auto decoded = DecodeRecord(*encoded);

    Check(decoded.has_value(), "Decoding failed");

    Check(decoded->type == original.type,
        "Type mismatch");

    Check(decoded->sequence == original.sequence,
        "Sequence mismatch");

    Check(decoded->payload == original.payload,
        "Payload mismatch");

    std::cout << "DecodeRecord Test Passed!\n";
}

void TestInvalidHeader()
{
    std::cout << "\nInvalid Header Test Start!\n";
    std::vector<std::byte> bytes{
        std::byte{0x4D},
        std::byte{0x4C},
        std::byte{0x01}
    };

    auto decoded = DecodeRecord(bytes);

    Check(!decoded.has_value(),
        "Invalid header should be rejected");

    std::cout << "Invalid Header Test Passed!\n";
}

void TestWrongMagic()
{
    std::vector<std::byte> bytes{
        std::byte{0x00}, std::byte{0x4C},
        std::byte{0x01}, std::byte{0x02},
        std::byte{0x12}, std::byte{0x34},
        std::byte{0x56}, std::byte{0x78},
        std::byte{0x00}, std::byte{0x00}
    };

    auto decoded = DecodeRecord(bytes);

    Check(!decoded.has_value(),
        "Wrong magic bytes should be rejected");
    std::cout << "\nWrong Magic Test Passed!\n";
}

void TestInvalidPayloadLength()
{
    std::vector<std::byte> bytes{
        std::byte{0x4D}, std::byte{0x4C},
        std::byte{0x01}, std::byte{0x02},
        std::byte{0x12}, std::byte{0x34},
        std::byte{0x56}, std::byte{0x78},
        std::byte{0x00}, std::byte{0x05},
        std::byte{0xAA}, std::byte{0xBB}
    };

    auto decoded = DecodeRecord(bytes);

    Check(!decoded.has_value(),
        "Invalid payload length should be rejected");
    std::cout << "\nInvalid Payload Length Test Passed!\n";
}

void TestEmptyPayload()
{
    Record original{
        1,
        123,
        {}
    };

    auto encoded = EncodeRecord(original);

    Check(encoded.has_value(), "Encoding failed");

    auto decoded = DecodeRecord(*encoded);

    Check(decoded.has_value(), "Decoding failed");

    Check(decoded->type == original.type,
        "Type mismatch");

    Check(decoded->sequence == original.sequence,
        "Sequence mismatch");

    Check(decoded->payload == original.payload,
        "Payload mismatch");

    std::cout << "\nEmpty Payload Test Passed!\n";
}

void TestUnsupportedVersion()
{
    Record record{ 2, 0x12345678, {} };

    auto encoded = EncodeRecord(record);

    Check(encoded.has_value(), "Encoding failed");

    // Corrupt the version byte
    (*encoded)[2] = std::byte{ 0x02 };

    auto decoded = DecodeRecord(*encoded);

    Check(!decoded.has_value(), "Unsupported version should be rejected");
}

void TestTrailingBytes()
{
    Record record{ 2, 0x12345678, { std::byte{0xAA}, std::byte{0xBB} } };
    auto encoded = EncodeRecord(record);
    Check(encoded.has_value(), "Encoding failed");
    
    encoded->push_back(std::byte{ 0xCC });
    encoded->push_back(std::byte{ 0xDD });
    auto decoded = DecodeRecord(*encoded);
	Check(!decoded.has_value(), "Trailing bytes should cause decoding to fail");
}

void TestOversizedPayload()
{
    Record record{ 2, 0x12345678, std::vector<std::byte>(65536, std::byte{0xAA}) };
    auto encoded = EncodeRecord(record);
    Check(!encoded.has_value(), "Encoding should fail for oversized payload");
}

int main()
{
    try
    {
        TestPrintTypeInfo();
        TestPrintBytes();
        PrintNativeEndian();
        TestEncodeBigEndian();

        TestEncodeRecord();
        TestDecodeRecord();
        TestInvalidHeader();
        TestWrongMagic();
        TestInvalidPayloadLength();
        TestEmptyPayload();

        TestUnsupportedVersion();
        std::cout << "TestUnsupportedVersion PASSED\n";

        TestTrailingBytes();
        std::cout << "TestTrailingBytes PASSED\n";

        TestOversizedPayload();
        std::cout << "TestOversizedPayload PASSED\n";

        std::cout << "\nAll tests passed!\n";
    }
    catch (const std::exception& e)
    {
        std::cerr << "TEST FAILED: "
            << e.what() << '\n';

        return 1;
    }

    return 0;
}