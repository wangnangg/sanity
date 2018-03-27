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
    Vector(int size, int val = 0.0)
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
    VectorMutableView mut()
    {
        return VectorMutableView(&_data.front(), 1, _size);
    }
};
}
