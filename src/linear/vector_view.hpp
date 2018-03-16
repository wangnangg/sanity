#pragma once
#include "type.hpp"
namespace sanity::linear
{
template <typename DataT>
class VectorView;
template <typename DataT>
class VectorMutView;

template <typename DataT>
const DataT& get(const VectorView<DataT>& v, int i)
{
    return *(v.data() + i);
}

template <typename DataT>
DataT& get(const VectorMutView<DataT>& v, int i)
{
    return *(v.data() + i);
}

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
    const DataT& operator()(int i) { return get(*this, i); }
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
    DataT& operator()(int i) const { return get(*this, i); }
    operator VectorView<DataT>() const { return *this; }
};
}
