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

public:
    Vector() : _data() {}
    Vector(VectorConstView v) : _data(v.size())
    {
        for (uint i = 0; i < v.size(); i++)
        {
            _data[i] = v(i);
        }
    }
    Vector(uint size, Real val = 0.0) : _data(size, val) {}
    uint size() const { return _data.size(); }
    Real& operator()(uint i)
    {
        assert(i < size());
        return _data[i];
    }
    const Real& operator()(uint i) const
    {
        assert(i < size());
        return _data[i];
    }
    operator VectorConstView() const
    {
        return VectorConstView(&_data.front(), 1, size());
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

public:
    CVector() : _data() {}
    CVector(CVectorConstView v) : _data(v.size())
    {
        for (uint i = 0; i < v.size(); i++)
        {
            _data[i] = v(i);
        }
    }
    CVector(uint size, Complex val = Complex()) : _data(size, val) {}
    uint size() const { return _data.size(); }
    Complex& operator()(uint i)
    {
        assert(i < size());
        return _data[(unsigned int)i];
    }
    const Complex& operator()(uint i) const
    {
        assert(i < size());
        return _data[i];
    }
    operator CVectorConstView() const
    {
        return CVectorConstView(&_data.front(), 1, size());
    }
    CVectorMutableView mut()
    {
        return CVectorMutableView(&_data.front(), 1, size());
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
