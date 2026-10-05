#pragma once

#include <iostream>
#include <string>

template <typename T>
void PrintTypeInfo(const std::string& name)
{
    std::cout
        << name
        << "\nsizeof: " << sizeof(T)
        << "\nalignof: " << alignof(T)
        << "\n\n";
}