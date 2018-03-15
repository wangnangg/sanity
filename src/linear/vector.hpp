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
DataT& get(Vector<DataT>& v, int i)
{
    return *(v.data() + i);
}

template <typename DataT>
const DataT& get(const Vector<DataT>& v, int i)
{
    return *(v.data() + i);
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

    operator VectorView<DataT>() const
    {
        return VectorView<DataT>(data(), inc(), size());
    }

    VectorMutView<DataT> mutView()
    {
        return VectorMutView<DataT>(data(), inc(), size());
    }
    VectorView<DataT> constView() const
    {
        return VectorView<DataT>(data(), inc(), size());
    }

    VectorView<DataT> blockView(int st, int ed = -1) const
    {
        if (ed < 0)
        {
            ed += size() + 1;
        }
        assert(st <= ed);
        assert(ed <= size());
        return VectorView<DataT>(&get(*this, st), 1, ed - st);
    }

    DataT& operator()(int i) { return get(*this, i); }
    const DataT& operator()(int i) const { return get(*this, i); }
};
}
