#pragma once
#include <cassert>
#include <type_traits>
#include <vector>
#include "type.hpp"
#include "vector_view.hpp"
namespace sanity::linear
{
template <typename DataT>
class Vector;

template <typename DataT>
const DataT* addressOf(const Vector<DataT>& v, int i)
{
    return v.data() + i;
}

template <typename DataT>
DataT* addressOf(Vector<DataT>& v, int i)
{
    return v.data() + i;
}

template <typename DataT>
const DataT& get(const Vector<DataT>& v, int i)
{
    return *addressOf(v, i);
}

template <typename DataT>
DataT& get(Vector<DataT>& v, int i)
{
    return *addressOf(v, i);
}

template <typename DataT>
VectorView<DataT, Const> constView(const Vector<DataT>& v)
{
    return VectorView<DataT, Const>(v.data(), 1, v.size());
}

template <typename DataT>
VectorView<DataT, Mutable> mutView(Vector<DataT>& v)
{
    return VectorView<DataT, Mutable>(v.data(), 1, v.size());
}

template <typename DataT, typename DataT2>
void set(Vector<DataT>& v, int i, DataT2 val)
{
    *addressOf(v, i) = DataT(val);
}

template <typename DataT>
class Vector
{
    std::vector<DataT> _data;

public:
    Vector(int size, std::vector<DataT> data) : _data(std::move(data))
    {
        assert(size == (int)_data.size());
    }
    Vector(int size, DataT val = DataT())
        : _data(static_cast<unsigned int>(size), val)
    {
    }
    const DataT* data() const { return &_data.front(); }
    DataT* data() { return &_data.front(); }
    int size() const { return _data.size(); }
    DataT& operator()(int i) { return get(*this, i); }
    const DataT& operator()(int i) const { return get(*this, i); }

    operator VectorView<DataT, Const>() const { return constView(*this); }
};
}
