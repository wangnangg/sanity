#pragma once
#include <complex>
#include <cstddef>
#include <cstdint>
#ifndef NDEBUG
#include <iostream>
#endif
using Real = double;
using Complex = std::complex<double>;
enum Mutability
{
    Const,
    Mutable
};
