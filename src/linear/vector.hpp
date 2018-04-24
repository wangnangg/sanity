#pragma once
#include <cassert>
#include <vector>
#include "type.hpp"
#include "vector_view.hpp"
namespace sanity::linear
{
class Vector
{
    std::vector<Real> _data;
    int _size;

public:
    Vector() : _data(), _size(0) {}
    Vector(int size, Real val = 0.0)
        : _data((unsigned int)size, val), _size(size)
    {
    }
    int size() const { return _size; }
    Real& operator()(int i)
    {
        assert(i < size());
        assert(i >= 0);
        return _data[(unsigned int)i];
    }
    const Real& operator()(int i) const
    {
        assert(i < size());
        assert(i >= 0);
        return _data[(unsigned int)i];
    }
    operator VectorConstView() const
    {
        return VectorConstView(&_data.front(), 1, _size);
    }
};

inline VectorConstView constView(const Vector& v)
{
    return VectorConstView(&v(0), 1, v.size());
}
inline VectorMutableView mutableView(Vector& v)
{
    return VectorMutableView(&v(0), 1, v.size());
}

class CVector
{
    std::vector<Complex> _data;
    int _size;

public:
    CVector() : _data(), _size(0) {}
    CVector(int size, Complex val = Complex())
        : _data((unsigned int)size, val), _size(size)
    {
    }
    int size() const { return _size; }
    Complex& operator()(int i)
    {
        assert(i < size());
        assert(i >= 0);
        return _data[(unsigned int)i];
    }
    const Complex& operator()(int i) const
    {
        assert(i < size());
        assert(i >= 0);
        return _data[(unsigned int)i];
    }
    operator CVectorConstView() const
    {
        return CVectorConstView(&_data.front(), 1, _size);
    }
    CVectorMutableView mut()
    {
        return CVectorMutableView(&_data.front(), 1, _size);
    }
};

inline CVectorConstView constView(const CVector& v)
{
    return CVectorConstView(&v(0), 1, v.size());
}
inline CVectorMutableView mutableView(CVector& v)
{
    return CVectorMutableView(&v(0), 1, v.size());
}
}  // namespace sanity::linear
