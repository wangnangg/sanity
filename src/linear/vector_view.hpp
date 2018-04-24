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

inline VectorConstView constView(VectorMutableView v)
{
    return VectorConstView(&v(0), 1, v.size());
}
inline VectorMutableView mutableView(VectorMutableView v) { return v; }
VectorConstView blockView(VectorConstView v, int i, int size);
VectorMutableView blockView(VectorMutableView v, int i, int size);

class CVectorConstView
{
    const Complex* _data;
    int _inc;
    int _size;

public:
    CVectorConstView(const Complex* data, int inc, int size)
        : _data(data), _inc(inc), _size(size)
    {
    }
    int inc() const { return _inc; };
    int size() const { return _size; }
    const Complex& operator()(int i) const
    {
        assert(i < size());
        assert(i >= 0);
        return _data[i * inc()];
    }
};

class CVectorMutableView
{
    Complex* _data;
    int _inc;
    int _size;

public:
    CVectorMutableView(Complex* data, int inc, int size)
        : _data(data), _inc(inc), _size(size)
    {
    }
    int inc() const { return _inc; };
    int size() const { return _size; }
    Complex& operator()(int i) const
    {
        assert(i < size());
        assert(i >= 0);
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
CVectorConstView blockView(CVectorConstView v, int i, int size);
CVectorMutableView blockView(CVectorMutableView v, int i, int size);

}  // namespace sanity::linear
