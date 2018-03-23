#pragma once
#include <cassert>
#include "type.hpp"
namespace sanity::linear
{
template <typename DataT, Mutability mt>
class VectorView;

template <typename DataT, Mutability mt>
decltype(auto) addressOf(VectorView<DataT, mt> v, int i)
{
    return v.data() + v.inc() * i;
}

template <typename DataT>
const DataT& get(VectorView<DataT, Const> v, int i)
{
    return *addressOf(v, i);
}

template <typename DataT>
DataT& get(VectorView<DataT, Mutable> v, int i)
{
    return *addressOf(v, i);
}

template <typename DataT, typename DataT2>
void set(VectorView<DataT, Mutable>& v, int i, DataT2 val)
{
    *addressOf(v, i) = DataT(val);
}

template <typename DataT>
auto constView(VectorView<DataT, Mutable> v)
{
    return VectorView<DataT, Const>(v.data(), v.inc(), v.size());
}

template <typename DataT, Mutability mt>
auto blockView(VectorView<DataT, mt> v, int st, int ed = -1)
{
    if (ed < 0)
    {
        ed += v.size() + 1;
    }
    assert(st <= ed);
    assert(ed <= v.size());
    return VectorView<DataT, mt>(addressOf(v, st), v.inc(), ed - st);
}

template <typename DataT>
class VectorView<DataT, Const>
{
public:
    using DataType = DataT;
    static const Mutability mutability = Const;

private:
    const DataT* _data;
    int _inc;
    int _size;

public:
    VectorView(const DataT* data, int inc, int size)
        : _data(data), _inc(inc), _size(size)
    {
    }
    const DataT* data() const { return _data; }
    int inc() const { return _inc; };
    int size() const { return _size; }
    const DataT& operator()(int i) const { return get(*this, i); }
};

template <typename DataT>
class VectorView<DataT, Mutable>
{
public:
    using DataType = DataT;
    static const Mutability mutability = Mutable;

private:
    DataT* _data;
    int _inc;
    int _size;

public:
    VectorView(DataT* data, int inc, int size)
        : _data(data), _inc(inc), _size(size)
    {
    }
    DataT* data() const { return _data; }
    int inc() const { return _inc; };
    int size() const { return _size; }
    DataT& operator()(int i) const { return get(*this, i); }
    operator VectorView<DataT, Const>() const { return constView(*this); }
};
}
