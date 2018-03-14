#pragma once
#include <vector>
#include "type.hpp"
namespace sanity::linear
{
template <typename DataT>
class VectorView
{
protected:
    DataT* _data;
    int _inc;
    int _size;

public:
    VectorView(const DataT* data, int inc, int size)
        : _data(const_cast<DataT*>(data)), _inc(inc), _size(size)
    {
    }
    const DataT* data() const { return _data; }
    int inc() const { return _inc; };
    int size() const { return _size; }
};

template <typename DataT>
class VectorMutView : public VectorView<DataT>
{
public:
    VectorMutView(DataT* data, int inc, int size)
        : VectorView<DataT>(data, inc, size)
    {
    }
    DataT* data() const { return VectorView<DataT>::_data; }
    VectorView<DataT> constView() const { return *this; }
};

template <typename DataT>
class Vector
{
    std::vector<DataT> _data;

public:
    Vector(int size, int val = DataT())
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
};
}
