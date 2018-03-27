#pragma once
#include <cassert>
#include "type.hpp"
namespace sanity::linear
{
class VectorConstView
{
    const Real* _data;
    int _inc;
    int _size;

public:
    VectorConstView(const Real* data, int inc, int size)
        : _data(data), _inc(inc), _size(size)
    {
    }
    int inc() const { return _inc; };
    int size() const { return _size; }
    const Real& operator()(int i) const
    {
        assert(i < size());
        assert(i >= 0);
        return _data[i * inc()];
    }
};

class VectorMutableView
{
    Real* _data;
    int _inc;
    int _size;

public:
    VectorMutableView(Real* data, int inc, int size)
        : _data(data), _inc(inc), _size(size)
    {
    }
    int inc() const { return _inc; };
    int size() const { return _size; }
    Real& operator()(int i) const
    {
        assert(i < size());
        assert(i >= 0);
        return _data[i * inc()];
    }
    operator VectorConstView() const
    {
        return VectorConstView(_data, _inc, _size);
    }
};

VectorConstView blockView(VectorConstView v, int i, int size);
VectorMutableView blockView(VectorMutableView v, int i, int size);
}
