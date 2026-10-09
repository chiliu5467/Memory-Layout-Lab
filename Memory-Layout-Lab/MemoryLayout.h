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