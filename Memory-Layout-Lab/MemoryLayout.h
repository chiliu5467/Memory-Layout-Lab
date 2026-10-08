#pragma once

#include <iostream>
#include <string>
#include <span>
#include <iomanip>
#include <cstddef>
#include <bit>
#include <array>
#include <vector>
#include <limits>
#include <optional>
#include <cstdint>
#include <span>

template <typename T>
void PrintTypeInfo(const std::string& name)
{
    std::cout
        << name
        << "\nsizeof: " << sizeof(T)
        << "\nalignof: " << alignof(T)
        << "\n\n";
}

template <typename T>
void PrintBytes(const T& value)
{
    std::span<const T> object{ &value, 1 };

    auto bytes = std::as_bytes(object);

    for (std::byte byte : bytes)
    {
        std::cout
            << std::hex
            << std::setw(2)
            << std::setfill('0')
            << std::to_integer<int>(byte)
            << ' ';
    }

    std::cout << std::dec << '\n';
}

void PrintNativeEndian()
{
    if constexpr (std::endian::native == std::endian::little)
    {
        std::cout << "\nNative Endian: Little Endian\n";
    }
    else if constexpr (std::endian::native == std::endian::big)
    {
        std::cout << "\native Endian: Big Endian\n";
    }
    else
    {
		std::cout << "\nNative Endian: Mixed Endian\n";
    }
}

std::array<std::byte, 4> EncodeBigEndian(std::uint32_t value)
{
	return std::array<std::byte, 4>{
        std::byte((value >> 24) & 0xFF),
        std::byte((value >> 16) & 0xFF),
        std::byte((value >> 8) & 0xFF),
        std::byte(value & 0xFF)
	};
}

std::uint32_t DecodeBigEndian(
    const std::array<std::byte, 4>& bytes)
{
    return (std::to_integer<std::uint32_t>(bytes[0]) << 24) |
           (std::to_integer<std::uint32_t>(bytes[1]) << 16) |
           (std::to_integer<std::uint32_t>(bytes[2]) << 8) |
		    std::to_integer<std::uint32_t>(bytes[3]);
}

struct Record
{
    std::uint8_t type;
    std::uint32_t sequence;
    std::vector<std::byte> payload;
};

inline constexpr std::size_t HEADER_SIZE = 10;

inline std::optional<std::vector<std::byte>>
EncodeRecord(const Record& record)
{
    if (record.payload.size() >
        std::numeric_limits<std::uint16_t>::max())
    {
        return std::nullopt;
    }

    std::vector<std::byte> result;
    result.reserve(HEADER_SIZE + record.payload.size());

    result.push_back(std::byte{ 0x4D });
    result.push_back(std::byte{ 0x4C });

	std::byte version = std::byte{ 0x01 };
	result.push_back(version);

    std::byte type = static_cast<std::byte>(record.type);
	result.push_back(type);

    auto sequenceBytes = EncodeBigEndian(record.sequence);
	result.insert(result.end(), sequenceBytes.begin(), sequenceBytes.end());

    std::array<std::byte, 2> payloadLengthBytes
    {
        std::byte((record.payload.size() >> 8) & 0xFF),
        std::byte(record.payload.size() & 0xFF)
    };

	result.insert(result.end(), payloadLengthBytes.begin(), payloadLengthBytes.end());
    result.insert(result.end(), record.payload.begin(), record.payload.end());

    return result;
}

inline std::optional<Record>
DecodeRecord(std::span<const std::byte> bytes)
{
    Record result{};

    if (bytes.size() < HEADER_SIZE)
    {
		return std::nullopt;
    }

    if (bytes[0] != std::byte{ 0x4D } || 
        bytes[1] != std::byte{ 0x4C } ||
        bytes[2] != std::byte{ 0x01 })
    {
		return std::nullopt;
    }

    result.type = std::to_integer<std::uint8_t>(bytes[3]);

    auto sequenceBytes =
        DecodeBigEndian(std::array<std::byte, 4>{
        bytes[4], bytes[5], bytes[6], bytes[7]});

	result.sequence = sequenceBytes;

    std::uint16_t payloadLength =
        (std::to_integer<std::uint16_t>(bytes[8]) << 8) |
        std::to_integer<std::uint16_t>(bytes[9]);

    auto remaining = bytes.size() - HEADER_SIZE;
    if (payloadLength != remaining)
    {
        return std::nullopt;
    }

    auto payloadBytes =
        bytes.subspan(HEADER_SIZE, payloadLength);

	std::vector<std::byte> payload(payloadBytes.begin(), payloadBytes.end());

	result.payload.insert(result.payload.end(), payload.begin(), payload.end());

    return result;
}