#pragma once
#include <cassert>
#include <type_traits>
#include <vector>
#include "type.hpp"
#include "vector_view.hpp"
namespace sanity::linear
{
template <typename DataT = Real>
class Vector;

template <typename DataT>
DataT get(Vector<DataT>& v, int i)
{
    return *(v.data() + i);
}

template <typename DataT>
const DataT& get(const Vector<DataT>& v, int i)
{
    return *(v.data() + i);
}

template <typename DataT>
VectorView<DataT> constView(const Vector<DataT>& v)
{
    return VectorView<DataT>(v.data(), v.inc(), v.size(), false);
}

template <typename DataT>
VectorMutView<DataT> mutView(Vector<DataT>& v)
{
    return VectorMutView<DataT>(v.data(), v.inc(), v.size());
}

template <typename DataT>
VectorView<DataT> blockView(const Vector<DataT>& v, int st, int ed = -1)
{
    if (ed < 0)
    {
        ed += v.size() + 1;
    }
    assert(st <= ed);
    assert(ed <= v.size());
    return VectorView<DataT>(v.data() + st, 1, ed - st);
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
    int inc() const { return 1; }
    int size() const { return _data.size(); }

    operator VectorView<DataT>() const { return constView(*this); }

    DataT operator()(int i) { return get(*this, i); }
    const DataT& operator()(int i) const { return get(*this, i); }
};
}
