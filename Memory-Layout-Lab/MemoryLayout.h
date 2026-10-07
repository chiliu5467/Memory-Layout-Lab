#pragma once

#include <iostream>
#include <string>
#include <span>
#include <iomanip>
#include <cstddef>
#include <bit>
#include <array>

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