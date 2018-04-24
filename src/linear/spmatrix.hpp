#pragma once
#include <vector>
#include "type.hpp"

class SparseMatrix
{
    std::vector<int> _ptr;
    std::vector<int> _idx;
    std::vector<Real> _val;
    int _nrow;
    int _ncol;

public:
    SparseMatrix(int nrow, int ncol, std::vector<int> ptr,
                 std::vector<int> idx, std::vector<Real> val)
        : _ptr(std::move(ptr)),
          _idx(std::move(idx)),
          _val(std::move(val)),
          _nrow(nrow),
          _ncol(ncol)
    {
    }
};
