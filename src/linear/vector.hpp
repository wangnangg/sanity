#pragma once
#include <cassert>
#include <type_traits>
#include <vector>
#include "type.hpp"
#include "vector_view.hpp"
namespace sanity::linear
{
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
};


template <typename DataT>
VectorView<DataT, NoConj, Const> constView(const Vector<DataT>& v)
{
    return VectorView<DataT, NoConj, Const>(v.data(), v.inc(), v.size());
}

template <typename DataT>
VectorView<DataT, NoConj, Mutable> mutView(Vector<DataT>& v)
{
    return VectorView<DataT, NoConj, Mutable>(v.data(), v.inc(), v.size());
}


}
