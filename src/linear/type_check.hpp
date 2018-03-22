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
}
