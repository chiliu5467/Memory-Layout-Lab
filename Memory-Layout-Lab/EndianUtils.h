#pragma once

#include <iostream>
#include <bit>
#include <array>


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
