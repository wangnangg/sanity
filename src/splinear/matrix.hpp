#pragma once
#include <cassert>
#include <vector>
#include "type.hpp"

namespace sanity::splinear
{
// idx is sorted.
struct Spmatrix
{
    enum Format
    {
        RowCompressed,
        ColCompressed
    } format;
    std::vector<uint> ptr;
    std::vector<uint> idx;
    std::vector<Real> val;
    uint nrow;
    uint ncol;
    Spmatrix(uint nrow, uint ncol, Format format)
        : format(format), nrow(nrow), ncol(ncol)
    {
    }
};

struct SpmatrixTriple
{
    struct Triple
    {
        uint row;
        uint col;
        Real val;
    };
    std::vector<Triple> triples;
    uint nrow;
    uint ncol;
    SpmatrixTriple(uint nrow, uint ncol) : nrow(nrow), ncol(ncol) {}
};

void sortTriples(SpmatrixTriple& spmat, bool rowFirst);
Spmatrix triple2compressed(SpmatrixTriple& trimat,
                           Spmatrix::Format target_format);

class SpmatrixCreator
{
    SpmatrixTriple _trmat;

public:
    SpmatrixCreator(uint nrow, uint ncol) : _trmat(nrow, ncol) {}
    void reserve(uint nEntry) { _trmat.triples.reserve(nEntry); }
    void addEntry(uint row, uint col, Real val)
    {
        _trmat.triples.push_back({row, col, val});
    }
    Spmatrix create(Spmatrix::Format target_format)
    {
        return triple2compressed(_trmat, target_format);
    }
};

Real SpmatrixGet(const Spmatrix& spmat, size_t row, size_t col);

class SpmatrixRowConstIterator
{
    const uint* _col;
    const uint* _col_end;
    const Real* _val;

public:
    SpmatrixRowConstIterator(const uint* col, const uint* col_end,
                             const Real* val)
        : _col(col), _col_end(col_end), _val(val)
    {
    }
    Real val() const { return *_val; }
    uint col() const { return *_col; }
    bool end() const { return _col == _col_end; }
    void nextNonzero()
    {
        _col++;
        _val++;
    }
};

class SpmatrixColConstIterator
{
    const uint* _col;
    const uint* _col_end;
    const Real* _val;

public:
    SpmatrixColConstIterator(const uint* col, const uint* col_end,
                             const Real* val)
        : _col(col), _col_end(col_end), _val(val)
    {
    }
    Real val() const { return *_val; }
    uint row() const { return *_col; }
    bool end() const { return _col == _col_end; }
    bool nextNonzero()
    {
        if (end())
        {
            return false;
        }
        else
        {
            _col++;
            _val++;
            return true;
        }
    }
};

inline SpmatrixRowConstIterator initRowIter(const Spmatrix& spmat, uint row)
{
    assert(spmat.format == Spmatrix::RowCompressed);
    return SpmatrixRowConstIterator(&spmat.idx[spmat.ptr[row]],
                                    &spmat.idx[spmat.ptr[row + 1]],
                                    &spmat.val[spmat.ptr[row]]);
}

inline SpmatrixColConstIterator initColIter(const Spmatrix& spmat, uint col)
{
    assert(spmat.format == Spmatrix::RowCompressed);
    return SpmatrixColConstIterator(&spmat.idx[spmat.ptr[col]],
                                    &spmat.idx[spmat.ptr[col + 1]],
                                    &spmat.val[spmat.ptr[col]]);
}

}  // namespace sanity::splinear
