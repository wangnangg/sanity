#pragma once
#include <complex>
#ifndef NDEBUG
#include <iostream>
#endif
#include <exception>
using Real = double;
using Complex = std::complex<double>;
using uint = unsigned int;

struct IterationResult
{
    Real error;
    uint nIter;
};
