#pragma once
#include "matrix.hpp"
#include "matrix_view.hpp"
#include "vector.hpp"
#include "vector_view.hpp"

namespace sanity::linear
{
template <typename...>
struct oneOf
{
    static constexpr bool value = false;
};

template <typename F, typename S, typename... T>
struct oneOf<F, S, T...>
{
    static constexpr bool value =
        std::is_same<F, S>::value || oneOf<F, T...>::value;
};

template <typename T, typename Enable = void>
struct isMutableS
{
    static constexpr bool value = false;
};

template <typename T>
struct isMutableS<T, typename std::enable_if<T::Mutable>::type>
{
    static constexpr bool value = true;
};

template <typename T>
inline constexpr bool isMutable = isMutableS<T>::value;

template <typename F>
struct isVectorTypeS
{
    static constexpr bool value = false;
};

template <>
struct isVectorTypeS<Vector<Real>>
{
    static constexpr bool value = true;
};

template <>
struct isVectorTypeS<Vector<Complex>>
{
    static constexpr bool value = true;
};

template <>
struct isVectorTypeS<VectorView<Real>>
{
    static constexpr bool value = true;
};

template <>
struct isVectorTypeS<VectorView<Complex>>
{
    static constexpr bool value = true;
};

template <>
struct isVectorTypeS<VectorMutView<Real>>
{
    static constexpr bool value = true;
};

template <>
struct isVectorTypeS<VectorMutView<Complex>>
{
    static constexpr bool value = true;
};

template <typename T>
inline constexpr bool isVectorType = isVectorTypeS<T>::value;

template <typename T, typename Enable = void>
struct isMatrixTypeS
{
    static constexpr bool value = false;
};

template <typename T>
struct isMatrixTypeS<
    T, typename std::enable_if<std::is_same<decltype(T::Viewport),
                                            const MatrixViewport>::value>::type>
{
    static constexpr bool value = true;
};

template <typename T>
inline constexpr bool isMatrixType = isMatrixTypeS<T>::value;
}
