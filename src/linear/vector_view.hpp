#pragma once
#include <cassert>
#include "type.hpp"
namespace sanity::linear
{
class VectorConstView
{
    const Real* _data;
    uint _inc;
    uint _size;

public:
    VectorConstView(const Real* data, uint inc, uint size)
        : _data(data), _inc(inc), _size(size)
    {
    }
    uint inc() const { return _inc; };
    uint size() const { return _size; }
    const Real& operator()(uint i) const
    {
        assert(i < size());
        return _data[i * inc()];
    }
};

class VectorMutableView
{
    Real* _data;
    uint _inc;
    uint _size;

public:
    VectorMutableView(Real* data, uint inc, uint size)
        : _data(data), _inc(inc), _size(size)
    {
    }
    uint inc() const { return _inc; };
    uint size() const { return _size; }
    Real& operator()(uint i) const
    {
        assert(i < size());
        return _data[i * inc()];
    }
    operator VectorConstView() const
    {
        return VectorConstView(_data, _inc, _size);
    }
};

inline VectorConstView constView(VectorMutableView v)
{
    return VectorConstView(&v(0), 1, v.size());
}
inline VectorMutableView mutableView(VectorMutableView v) { return v; }
VectorConstView blockView(VectorConstView v, uint i, uint size);
VectorMutableView blockView(VectorMutableView v, uint i, uint size);

class CVectorConstView
{
    const Complex* _data;
    uint _inc;
    uint _size;

public:
    CVectorConstView(const Complex* data, uint inc, uint size)
        : _data(data), _inc(inc), _size(size)
    {
    }
    uint inc() const { return _inc; };
    uint size() const { return _size; }
    const Complex& operator()(uint i) const
    {
        assert(i < size());
        return _data[i * inc()];
    }
};

class CVectorMutableView
{
    Complex* _data;
    uint _inc;
    uint _size;

public:
    CVectorMutableView(Complex* data, uint inc, uint size)
        : _data(data), _inc(inc), _size(size)
    {
    }
    uint inc() const { return _inc; };
    uint size() const { return _size; }
    Complex& operator()(uint i) const
    {
        assert(i < size());
        return _data[i * inc()];
    }
    operator CVectorConstView() const
    {
        return CVectorConstView(_data, _inc, _size);
    }
};

inline CVectorConstView constView(CVectorMutableView v)
{
    return CVectorConstView(&v(0), 1, v.size());
}
inline CVectorMutableView mutableView(CVectorMutableView v) { return v; }
CVectorConstView blockView(CVectorConstView v, uint i, uint size);
CVectorMutableView blockView(CVectorMutableView v, uint i, uint size);

}  // namespace sanity::linear
