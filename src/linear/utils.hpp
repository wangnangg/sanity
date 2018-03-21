#pragma once
#include <algorithm>
#include <cassert>
#include <iomanip>
#include "matrix.hpp"
#include "matrix_view.hpp"

namespace sanity::linear
{
void copy(MatrixView<Real, General> src, MatrixMutView<Real, General> dst);
void copy(MatrixView<Real, Upper> src, MatrixMutView<Real, Upper> dst);
void copy(MatrixView<Real, Lower> src, MatrixMutView<Real, Lower> dst);
void copy(MatrixView<Real, StrictUpper> src,
          MatrixMutView<Real, StrictUpper> dst);
void copy(MatrixView<Real, StrictLower> src,
          MatrixMutView<Real, StrictLower> dst);
void copy(MatrixView<Real, Diagonal> src, MatrixMutView<Real, Diagonal> dst);
void copy(MatrixView<Complex, General> src,
          MatrixMutView<Complex, General> dst);
void copy(MatrixView<Complex, Upper> src, MatrixMutView<Complex, Upper> dst);
void copy(MatrixView<Complex, Lower> src, MatrixMutView<Complex, Lower> dst);
void copy(MatrixView<Complex, StrictUpper> src,
          MatrixMutView<Complex, StrictUpper> dst);
void copy(MatrixView<Complex, StrictLower> src,
          MatrixMutView<Complex, StrictLower> dst);
void copy(MatrixView<Complex, Diagonal> src,
          MatrixMutView<Complex, Diagonal> dst);

constexpr MatrixViewport transViewport(MatrixViewport vt)
{
    switch (vt)
    {
        case Upper:
            return Lower;
        case Lower:
            return Upper;
        case StrictUpper:
            return StrictLower;
        case StrictLower:
            return StrictUpper;
        default:
            return vt;
    }
}

template <typename DataT, MatrixViewport vt>
MatrixView<DataT, transViewport(vt)> transpose(MatrixView<DataT, vt> mat)
{
    return MatrixView<DataT, transViewport(vt)>(
        mat.data(), mat.nCol(), mat.nRow(), mat.lDim(), !mat.rowMajor(),
        mat.conjugated());
}

template <typename DataT>
MatrixView<DataT, General> transpose(const Matrix<DataT>& mat)
{
    return MatrixView<DataT, General>(mat.data(), mat.nCol(), mat.nRow(),
                                      mat.nCol(), true, false);
}

template <typename DataT>
std::ostream& operator<<(std::ostream& os, const Matrix<DataT>& mat)
{
    for (int i = 0; i < mat.nRow(); i++)
    {
        for (int j = 0; j < mat.nCol(); j++)
        {
            os << std::fixed << std::setw(8) << std::setprecision(3)
               << std::setfill(' ') << mat(i, j);
        }
        os << std::endl;
    }
    return os;
}

template <typename DataT, MatrixViewport vt>
std::ostream& operator<<(std::ostream& os, MatrixView<DataT, vt> mat)
{
    for (int i = 0; i < mat.nRow(); i++)
    {
        for (int j = 0; j < mat.nCol(); j++)
        {
            os << std::fixed << std::setw(8) << std::setprecision(3)
               << std::setfill(' ') << mat(i, j);
        }
        os << std::endl;
    }
    return os;
}

template <typename DataT, MatrixViewport vt>
std::ostream& operator<<(std::ostream& os, MatrixMutView<DataT, vt> mat)
{
    for (int i = 0; i < mat.nRow(); i++)
    {
        for (int j = 0; j < mat.nCol(); j++)
        {
            os << std::fixed << std::setw(8) << std::setprecision(3)
               << std::setfill(' ') << mat(i, j);
        }
        os << std::endl;
    }
    return os;
}
}
