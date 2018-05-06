#pragma once
#include <complex>
#ifndef NDEBUG
#include <iostream>
#endif
using Real = double;
using Complex = std::complex<double>;
using uint = unsigned int;

struct IterationResult
{
    Real error;
    uint nIter;
};
