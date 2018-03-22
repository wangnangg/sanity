#pragma once
#include <cassert>
#include "type.hpp"
namespace sanity::linear
{
template <typename DataT, Conjugation ct, Mutability mt>
class VectorView;

template <typename DataT, Conjugation ct>
class VectorView<DataT, ct, Const>
{
public:
    using DataType = DataT;
    static const Conjugation conjugation = ct;
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
};

template <typename DataT, Conjugation ct>
class VectorView<DataT, ct, Mutable>
{
public:
    using DataType = DataT;
    static const Conjugation conjugation = ct;
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
};

template <typename DataT, Conjugation ct, Mutability mt>
decltype(auto) addressOf(VectorView<DataT, ct, mt> v, int i)
{
    return v.data() + v.inc() * i;
}

template <typename DataT, Conjugation ct, Mutability mt>
DataT get(VectorView<DataT, ct, mt> v, int i);

template <typename DataT, Mutability mt>
DataT get(VectorView<DataT, NoConj, mt> v, int i)
{
    return *addressOf(v, i);
}

template <typename DataT, Mutability mt>
DataT get(VectorView<DataT, Conj, mt> v, int i)
{
    return std::conj(*addressOf(v, i));
}

template <typename DataT, Conjugation ct>
VectorView<DataT, ct, Const> constView(VectorView<DataT, ct, Mutable> v)
{
    return VectorView<DataT, ct, Const>(v.data(), v.inc(), v.size());
}

template <typename DataT, Conjugation ct, Mutability mt>
VectorView<DataT, ct, mt> blockView(VectorView<DataT, ct, mt> v, int st,
                                    int ed = -1)
{
    if (ed < 0)
    {
        ed += v.size() + 1;
    }
    assert(st <= ed);
    assert(ed <= v.size());
    return VectorView<DataT, ct, mt>(addressOf(v, st), v.inc(), ed - st);
}

template <typename DataT, Conjugation ct, Mutability mt>
auto conjugate(VectorView<DataT, ct, mt> v);

template <Conjugation ct, Mutability mt>
auto conjugate(VectorView<Real, ct, mt> v)
{
    return v;
}

template <Conjugation ct, Mutability mt>
auto conjugate(VectorView<Complex, ct, mt> v)
{
    return VectorView<Complex, invertConj(ct), mt>(v.data(), v.inc().v.size());
}
}
