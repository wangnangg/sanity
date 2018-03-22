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

enum Conjugation
{
    NoConj,
    Conj
};

constexpr Conjugation invertConj(Conjugation ct)
{
    if (ct == Conj)
    {
        return NoConj;
    }
    else
    {
        return Conj;
    }
}
