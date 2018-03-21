#pragma once
#include <cassert>
#include "type.hpp"
namespace sanity::linear
{
template <typename DataT>
class VectorView;
template <typename DataT>
class VectorMutView;

template <typename DataT>
DataT get(VectorView<DataT> v, int i);

template <typename DataT>
VectorView<DataT> constView(VectorMutView<DataT>& v)
{
    return VectorView<DataT>(v.data(), v.inc(), v.size(), false);
}

template <typename DataT>
VectorView<DataT> blockView(VectorView<DataT> v, int st, int ed = -1)
{
    if (ed < 0)
    {
        ed += v.size() + 1;
    }
    assert(st <= ed);
    assert(ed <= v.size());
    return VectorView<DataT>(v.data() + st * v.inc(), v.inc(), ed - st);
}

template <typename DataT>
VectorMutView<DataT> blockView(VectorMutView<DataT> v, int st, int ed = -1)
{
    if (ed < 0)
    {
        ed += v.size() + 1;
    }
    assert(st <= ed);
    assert(ed <= v.size());
    return VectorMutView<DataT>(v.data() + st * v.inc(), v.inc(), ed - st);
}

template <typename DataT>
class VectorView
{
public:
    static const bool Mutable = false;

private:
    const DataT* _data;
    int _inc;
    int _size;
    bool _conj;

public:
    VectorView(const DataT* data, int inc, int size, bool conj)
        : _data(data), _inc(inc), _size(size), _conj(conj)
    {
    }
    const DataT* data() const { return _data; }
    int inc() const { return _inc; };
    int size() const { return _size; }
    bool conjugated() const { return _conj; }

    DataT operator()(int i) { return get(*this, i); }
};

template <typename DataT>
class VectorMutView
{
public:
    static const bool Mutable = true;

private:
    DataT* _data;
    int _inc;
    int _size;

public:
    VectorMutView(DataT* data, int inc, int size)
        : _data(data), _inc(inc), _size(size)
    {
    }
    DataT* data() const { return _data; }
    int inc() const { return _inc; };
    int size() const { return _size; }

    DataT& operator()(int i) const { return get(*this, i); }
    operator VectorView<DataT>() const
    {
        return VectorView<DataT>(data(), inc(), size(), false);
    }
};

template <>
inline Real get<Real>(VectorView<Real> v, int i)
{
    return *(v.data() + i * v.inc());
}

template <>
inline Complex get<Complex>(VectorView<Complex> v, int i)
{
    if (v.conjugated())
    {
        return std::conj(*(v.data() + i * v.inc()));
    }
    else
    {
        return *(v.data() + i * v.inc());
    }
}

template <typename DataT>
DataT& get(VectorMutView<DataT> v, int i)
{
    return *(v.data() + i * v.inc());
}
}
